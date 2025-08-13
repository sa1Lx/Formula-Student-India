#include <Arduino.h>
#include "EasyNextionLibrary.h"  // Include EasyNextionLibrary
#include <FlexCAN_T4.h> // Include the FlexCAN library
#include <ArduinoJson.h>
#include <unordered_map>
#include <string>
#include <cstdint>

#include "SharedLib.h"

C_CAN_PARSER::CANParser c_can_parser;
D_CAN_PARSER::CANParser d_can_parser;

// Color Definitions
#define RED 59456
#define GREEN 2016

// CAN IDs
#define AMS_ID
#define ECU
#define TO_LEFT_MC_ID 0x202
#define TO_RIGHT_MC_ID 0x201
#define FROM_LEFT_MC_ID 0x182
#define FROM_RIGHT_MC_ID 0x181
#define APPS_ID 0x104
#define DASH2ECU 0xBE
#define ECU_ERROR_ID 0x100
#define YAW_ID1 0x70
#define YAW_ID2 0x80

// Create Nextion display object on Serial5 (pins depend on wiring)
EasyNex myNex(Serial5);

// Create CAN objects for two interfaces
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> c_can;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> d_can;

// Global time variables
int hours = 0, mins = 0, sec = 0;
int actual_millis = 0;
bool timeInitialized = false;

// Variables for right MC
float i_right = 0, p_right = 0, v_right = 0, enable_right = 0;
// Variables for left MC
float i_left = 0.0, p_left = 0, v_left = 0, enable_left = 0;

int16_t rpm_right_raw = 0, rpm_left_raw = 0;
int16_t i_left_raw = 0, i_right_raw = 0;
int16_t v_left_raw = 0, v_right_raw = 0;

float rpm_right, torque_right, motor_temp_right, mc_temp_right;
float rpm_left, torque_left, motor_temp_left, mc_temp_left;


int bps_min = 104, bps_max = 240;
int apps1_min = 30, apps1_max = 700;
int apps2_min = 100, apps2_max = 900;
uint8_t percent_95 = 0;
float APPS_norm = 0;
float APPS1_norm = 0.0, APPS2_norm = 0.0;
uint16_t APPS1_clip = 0, APPS2_clip = 0;

template <typename CAN_Type>
  void sendRequest(const uint8_t *request, uint32_t id, CAN_Type& can_bus) {
      CAN_message_t msg;
      msg.id = id;
      // msg.len = sizeof(request);
      std::memcpy(msg.buf, request, msg.len);

      while (true) {
          if (can_bus.write(msg) == 1) {
              Serial.println("Message Request sent successfully!");
              
              break;  // Exit the loop on success
          }
          Serial.println("Error: Failed to send Request. Retrying...");
          delay(20);  // Short delay before retrying to avoid bus overload
      }
  }

uint8_t* createTransmissionRequest(MC::Register reg, uint8_t time) {
    // const MC::RegisterInfo* info = MC::getRegisterInfo(reg);
    static uint8_t request[3];
    request[0] = 0x3D;
    request[1] = static_cast<uint8_t>(reg);  // Register ID
    request[2] = time;
    return request;
}

// Returns a descriptive error or warning message based on ID and level
String update_err_msg(int id, int level){
  String err_msg;
  if(level == 0){ // errors
    switch(id){
      case 0: err_msg = "Eprom Read Error"; break;
      case 1: err_msg = "Critical Hardware Error"; break;
      case 2: err_msg = "RFE input not present with FRG present"; break;
      case 3: err_msg = "CAN timeout error"; break;
      case 4: err_msg = "Feedback Signal Error"; break;
      case 5: err_msg = "Mains Voltage Min Limit"; break;
      case 6: err_msg = "Motor Temp Max Limit"; break;
      case 7: err_msg = "IGBT Temp Max Limit"; break;
      case 8: err_msg = "Mains Voltage Max Limit"; break;
      case 9: err_msg = "Overcurrent or oscillating current detected"; break;
      case 10: err_msg = "Race Away Detected (wrong direction)"; break;
      case 11: err_msg = "Ecode Timeout error"; break;
      case 12: err_msg = "Watchdog Reset"; break;
      case 13: err_msg = "AC Current offset detection fault"; break;
      case 14: err_msg = "Internal HW voltage problem"; break;
      case 15: err_msg = "Bleed Resistor overload"; break;
      default: err_msg = "No error or No MC input"; break;
    }
  }
  else if(level == 1){ // warnings
    switch(id){
      case 0: err_msg = "Parameter conflict detected"; break;
      case 1: err_msg = "FRG signal disturbed, EMI"; break;
      case 2: err_msg = "RFE input inactive (active without FRG input)"; break;
      case 3: err_msg = "Auxiliary Voltage Min Limit"; break;
      case 4: err_msg = "Feedback Signal Problem"; break;
      case 6: err_msg = "Motor Temperature > (I-red TM)"; break;
      case 7: err_msg = "IGBT Temp > 87% Vom limit"; break;
      case 8: err_msg = "Vout saturation max Limit"; break;
      case 10: err_msg = "Speed Actual Resolution Limit"; break;      
      case 11: err_msg = "Check ECode ID: 0x94"; break;
      case 12: err_msg = "Tripzone Glitch detected"; break;
      case 13: err_msg = "ADC Sequencer problem"; break;
      case 14: err_msg = "ADC Measurement problem"; break;
      case 15: err_msg = "Bleeder resistor load (>87%)"; break;
      case 5:
      case 9: err_msg = "No warning";
      default: err_msg = "No error/No MC input"; break;
    }
  }
  else {
    err_msg = "No error/No MC input";
  }
  return err_msg;
}

// Consolidated function to update MC error information
void update_mc_error(uint16_t err_code, bool isRightMC) {
  
  // uint32_t errors = (msg.buf[4] << 24) | (msg.buf[3] << 16) | (msg.buf[2] << 8) | msg.buf[1];
  uint16_t errors = err_code;
  int errorId = -1;
  // int warnId = -1;
  
  // Check error bits (0-15)
  for (int i = 0; i < 16; i++) {
    if (errors & (1UL << i)) { 
      errorId = i;
      break;
    }
  }
  // If no error, check warning bits (16-31)
  // if (errorId == -1) {
  //   for (int i = 16; i < 32; i++) {
  //     if (errors & (1UL << i)) { 
  //       warnId = i - 16;
  //       break;
  //     }
  //   }
  // }
  
  String err_msg = update_err_msg(errorId, 0);
  String err_id = String(errorId, HEX);
  if (errorId > -1) {
    if(isRightMC) {
      myNex.writeStr("mc_err_right.txt", err_id);
      myNex.writeNum("warn_led_R.val", 1);
      myNex.writeStr("err_desc_right.txt", err_msg);
    }
    else if(!isRightMC) {
      myNex.writeStr("mc_err_left.txt", err_id);
      myNex.writeNum("warn_led_L.val", 1);
      myNex.writeStr("err_desc_left.txt", err_msg);
    }
  } 
  else {
    if(isRightMC) {
      myNex.writeStr("mc_err_right.txt", "-1");
      myNex.writeNum("led_R_tm.en", 0);
      myNex.writeStr("err_desc_left.txt", "No error/No MC input");
    }
    else if(!isRightMC) {
      myNex.writeStr("mc_err_left.txt", "-1");
      myNex.writeNum("led_L_tm.en", 0);
      myNex.writeStr("err_desc_right.txt", "No error/No MC input");
    }
  }
}

int c_can_transmission_status = -1, d_can_transmission_status = -1;
int APPS1_raw = 0, APPS2_raw = 0, BPS2_raw = 0, STEER_raw = 0;
float rpm_max_right = 0.0, rpm_max_left = 0.0;
int16_t torque_left_raw = 0, torque_right_raw = 0;
int16_t motor_temp_left_raw = 0, motor_temp_right_raw = 0;
int16_t mc_temp_left_raw = 0, mc_temp_right_raw = 0;
int16_t error_left_receive = 0, error_right_receive = 0;

inline std::array<int16_t, 0xFF> registerValuesLeft{};
inline std::array<int16_t, 0xFF> registerValuesRight{};

void storeDecodedValue(const CAN_message_t& msg, std::array<int16_t, 0xFF>& storage) {
    auto reg = static_cast<MC::Register>(msg.buf[0]);
    int16_t decodedValue = 0;
    c_can_parser.decodeBamocarMessage(msg.id, &decodedValue, msg);
    storage[static_cast<size_t>(reg)] = decodedValue;
}

void c_canISR(const CAN_message_t &msg){
    // Serial.print("Interrupt Callback C CAN - ID:");Serial.print(msg.id);Serial.print(" BUFO:");Serial.println(msg.buf[0]);
    c_can_transmission_status = 1;
    switch(msg.id) {
        case FROM_LEFT_MC_ID:
          storeDecodedValue(msg, registerValuesLeft);
          break;

        case FROM_RIGHT_MC_ID:
          storeDecodedValue(msg, registerValuesRight);
          break;

        case APPS_ID:
        {
          // Serial.print("APPS Message Received - ID: ");
          std::unordered_map<std::string, float> actuator_signals;
          c_can_parser.decodeMessage(msg, actuator_signals);
          APPS1_raw = actuator_signals["APPS1"];
          APPS2_raw = actuator_signals["APPS2"];
          BPS2_raw = actuator_signals["BPS2"];
          STEER_raw = actuator_signals["STR0"];
          
          // Normalize APPS values
          APPS1_norm = (APPS1_raw - apps1_min) /(1.0 * (apps1_max - apps1_min));
          APPS2_norm = (APPS2_raw - apps2_min) / (1.0 * (apps2_max - apps2_min));
          // Serial.print("APPS1: "); Serial.print(APPS1_raw);
          // Serial.print(" APPS2: "); Serial.print(APPS2_raw);
          // Serial.print(" BPS2: "); Serial.print(BPS2_raw);
          // Serial.print(" STEER: "); Serial.println(STEER_raw);

          break;
        }
        default:
            break;
    }
}

bool con_src_il = false, to_ams_relay = false, pre_plaus = false, c_plus = false, c_minus = false;
bool con_src = false, c_plus_plaus = false, c_minus_plaus = false, gt_60v_plaus = false, pre_mech = false;

float yaw_rate = 0.0, accY = 0.0, yaw_ang_acc = 0.0, accX = 0.0;
float TSC=0.0, TSV=0.0, SOC=0.0;

void d_canISR(const CAN_message_t &msg){
    // Serial.print("Interrupt Callback D CAN - ID:");Serial.println(msg.id, HEX);
    d_can_transmission_status = 1;
    // uint16_t packedData = 0;
    switch(msg.id) {
        case 0x125:
        {
          std::unordered_map<std::string, float> ams_signals;
          d_can_parser.decodeMessage(msg, ams_signals);
          // Serial.println("AMS_FLT Message Received - ID: ");
          TSC = ams_signals["TSC"];
          TSV = ams_signals["TSV"];
          SOC = ams_signals["SOC"];
          con_src = ams_signals["CON_SRC"];
          con_src_il = ams_signals["CON_SRC_IL"];
          to_ams_relay = ams_signals["TO_AMS_RELAY"];
          pre_plaus = ams_signals["PRE_PLAUS"];
          c_plus = ams_signals["C_PLUS"];
          c_minus = ams_signals["C_MINUS"];
          c_plus_plaus = ams_signals["C_PLUS_PLAUS"];
          c_minus_plaus = ams_signals["C_MINUS_PLAUS"];
          gt_60v_plaus = ams_signals["GT_60V_PLAUS"];
          pre_mech = ams_signals["PRE_MECH"];
          // Serial.print("TSC: "); Serial.print(TSC);
          // Serial.print(" TSV: "); Serial.println(TSV);
          // Serial.print(" SOC: "); Serial.print(SOC);
          break;
        }
        case YAW_ID1:
        {
          std::unordered_map<std::string, float> yaw_signals1;
          d_can_parser.decodeMessage(msg, yaw_signals1);
          yaw_rate = yaw_signals1["YAW_RATE"];
          accY = yaw_signals1["ACC_Y"];
        }
        case YAW_ID2:
        {
          std::unordered_map<std::string, float> yaw_signals2;
          d_can_parser.decodeMessage(msg, yaw_signals2);
          yaw_ang_acc = yaw_signals2["YAW_ANG_ACC"];
          accX = yaw_signals2["ACC_X"];
        }
        default:
            break;
    }
}

void setup() {
  // Initialize serial ports
  Serial.begin(9600);       // Debug output
  Serial2.begin(115200);     // LTE and time data

  // Initialize CAN objects
  c_can.begin();
  c_can.setBaudRate(500000);
  c_can.enableFIFO();
  c_can.enableFIFOInterrupt();
  c_can.onReceive(c_canISR);
  
  d_can.begin();
  d_can.setBaudRate(1000000);
  d_can.enableFIFO();
  d_can.enableFIFOInterrupt();
  d_can.onReceive(d_canISR);

  // Initialize Nextion display
  myNex.begin(115200);
  myNex.writeStr("page 0");
  myNex.writeNum("page5.bt0.val", 0); // Disable button if max_torque is not set
  myNex.writeNum("page5.bt1.val", 0); // Disable button if max_torque is not set
  myNex.writeNum("page5.bt4.val", 0); // Disable button if max_rpm is not set
  myNex.writeNum("page5.bt5.val", 0); // Disable button if max_rpm is not set
  myNex.writeNum("soc_val.aph", 0); // Hiding SOC value text
  

  // Setup pins
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(17, INPUT_PULLDOWN);
  pinMode(19, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(2, LOW); // Set pin 2 to LOW, CAN TRANSCEIVER RS PIN
  digitalWrite(3,LOW);

  delay(1000);
}

int max_rpm = 0, actual_max_rpm = 0;
float max_torque = 0, actual_max_torque = 0;
std::unordered_map<std::string, float> dash_signals;

void sendDash2Ecu()
{
  CAN_message_t dash_to_ecu_msg;
  dash_signals["TORQ"] = max_torque;
  dash_signals["RPM"] = max_rpm;
  c_can_parser.encodeMessage(DASH2ECU, dash_signals, dash_to_ecu_msg);
  int dash2ecu_status = c_can.write(dash_to_ecu_msg);
  Serial.print("Dash2ECU Status: ");
  Serial.println(dash2ecu_status);
  if(dash2ecu_status == 1) {
    actual_max_rpm = max_rpm;
    actual_max_torque = max_torque;
  }
  delay(10);
}

int pg = 0; // Current page number

// These trigger functions are inbuilt functions in the EasyNextionLibrary
// Trigger Functions are called mirocontroller receives 23 02 54 xx on the serial port
// where xx is the trigger number, 01 for trigger0, 02 for trigger1, 0A for trigger10 etc.

void trigger0(){
  pg = 0;
}

void trigger1(){
  pg = 1;
}

void trigger2(){
  pg = 2;
}

void trigger3(){
  pg = 3;
}

void trigger4(){
  pg = 4;
}

void trigger5(){
  pg = 5;
}

void trigger10(){
  max_torque = 10;
  sendDash2Ecu();
}

void trigger11(){
  max_torque = 20;
  sendDash2Ecu();
}

void trigger12(){
  if(max_torque == 67.5){
    max_torque = 65;
  }
  else{
    max_torque = max(max_torque - 5, 0);
  }

  if(max_torque == 10){
    myNex.writeNum("bt0.val", 1); // Enable button if max_torque is 10
    myNex.writeNum("bt1.val",0);
  }
  else if(max_torque == 20){
    myNex.writeNum("bt1.val", 1); // Enable button if max_torque is 20
    myNex.writeNum("bt0.val",0);
  }
  else{
    myNex.writeNum("bt0.val", 0);
    myNex.writeNum("bt1.val", 0);
  }

  sendDash2Ecu();
}

void trigger13(){
  max_torque = min(max_torque + 5, 67.5);
  if(max_torque == 10){
    myNex.writeNum("bt0.val", 1); // Enable button if max_torque is 10
    myNex.writeNum("bt1.val",0);
  }
  else if(max_torque == 20){
    myNex.writeNum("bt1.val", 1); // Enable button if max_torque is 20
    myNex.writeNum("bt0.val",0);
  }
  else{
    myNex.writeNum("bt0.val", 0);
    myNex.writeNum("bt1.val", 0);
  }
  sendDash2Ecu();
}

void trigger14(){
  max_rpm = 500;
  sendDash2Ecu(); 
}

void trigger15(){
  max_rpm = 2000;
  sendDash2Ecu();
}

void trigger16(){
  max_rpm = max(0,max_rpm - 100);
  
  if(max_rpm == 500){
    myNex.writeNum("bt4.val", 1); // Enable button if max_rpm is 500
    myNex.writeNum("bt5.val",0);
  }
  else if(max_rpm == 2000){
    myNex.writeNum("bt5.val", 1); // Enable button if max_rpm is 2000
    myNex.writeNum("bt4.val",0);
  }
  else{
    myNex.writeNum("bt4.val", 0);
    myNex.writeNum("bt5.val", 0);
  }
  
  sendDash2Ecu();
}

void trigger17(){
  max_rpm = min(max_rpm + 100,6000);
  if(max_rpm == 500){
    myNex.writeNum("bt4.val", 1); // Enable button if max_rpm is 500
    myNex.writeNum("bt5.val",0);
  }
  else if(max_rpm == 2000){
    myNex.writeNum("bt5.val", 1); // Enable button if max_rpm is 2000
    myNex.writeNum("bt4.val",0);
  }
  else{
    myNex.writeNum("bt4.val", 0);
    myNex.writeNum("bt5.val", 0);
  }
  sendDash2Ecu();
}

float speed = 0;
int apps = 0;
float lv_bat = 0.0, hv_bat = 0.0;
int bps = 0;
bool apps_err = false, tc = false, tv = false, ts_off = false;
float motor_rpm_avg = 0.0, wheel_rpm_avg = 0.0, rpm_val = 0.0;
float wheel_radius = 0.22; // in m
float gear_ratio = 4.55; //motor to wheel ratio

void soc_update(){
  myNex.writeNum("soc_bar.val", (int)SOC); // Update SOC value on the display
  // myNex.writeStr("soc_val.txt", String((int)SOC)); // Update SOC text on the display
  if(SOC<20)
  {
    myNex.writeNum("soc_bar.pco", 63488); // Turn on LED for SOC < 20%
  }else if(SOC>=20&&SOC<40)
  {
    myNex.writeNum("soc_bar.pco", 64520);
  }else if(SOC>=40&&SOC<60)
  {
    myNex.writeNum("soc_bar.pco", 60836);
  }else if(SOC>=60&&SOC<80)
  {
    myNex.writeNum("soc_bar.pco", 9545);
  }else
  {
    myNex.writeNum("soc_bar.pco", 1024);
  }
}

void updatePg0(){
  // Serial.println("Page 0 Update");
  // int t1 = micros();

  APPS_norm = (APPS1_norm + APPS2_norm) / 2;
  if (APPS_norm < 0) {
    APPS_norm = 0;
  }

  rpm_left_raw = registerValuesLeft[static_cast<size_t>(MC::Register::SPEED_ACTUAL)];
  rpm_right_raw = registerValuesRight[static_cast<size_t>(MC::Register::SPEED_ACTUAL)];
  rpm_left = rpm_left_raw *6000/32767.0;
  rpm_right = rpm_right_raw *6000/32767.0;

  lv_bat = analogRead(19); // Read LV battery voltage from pin 19
  lv_bat = (lv_bat *3.3/ 1023.0) * (33000+10000)/10000; // Convert to voltage

  motor_rpm_avg = (abs(rpm_left) + abs(rpm_right))/2.0;
  wheel_rpm_avg = motor_rpm_avg / gear_ratio;
  speed = ((wheel_rpm_avg * 2.0 * 3.1416 * wheel_radius) / (60.0))*(18.0/5.0); // km/hr
  // rpm_val = map(wheel_rpm_avg, 0, 10000, 0, 360); // Assuming wheel_rpm_avg is a value between 0 and 10000
  Serial.print("Wheel RPM Avg: ");
  Serial.println(wheel_rpm_avg);
  
  apps = (int)map(APPS_norm, 0, 1, 0, 100);

  bps = (int)map(BPS2_raw, bps_min, bps_max, 0, 100);
  
  // soc = (hv_bat/403.2) * 100;

  myNex.writeStr("page0.max_rpm_var.txt", String(actual_max_rpm));
  myNex.writeStr("page0.max_torq_var.txt", String(actual_max_torque));

  torque_left_raw = registerValuesLeft[static_cast<size_t>(MC::Register::TORQUE_OUT)];
  torque_right_raw = registerValuesRight[static_cast<size_t>(MC::Register::TORQUE_OUT)];

  myNex.writeStr("speed.txt", String(speed));
  Serial.print("Speed: ");
  Serial.println(speed);
  
  float avg_torque = (abs(torque_left_raw) + abs(torque_right_raw)) / 2.0;
  avg_torque = (avg_torque * 67.5) / 32767.0;
  myNex.writeStr("torq.txt", String(avg_torque));
  
  // hv_bat = TSV*100;
  // Update Nextion display values
  myNex.writeStr("rpm.txt", String(motor_rpm_avg));
  myNex.writeNum("apps_bar.val", apps);
  myNex.writeNum("bps_bar.val", bps);
  myNex.writeStr("lv_bat.txt", lv_bat);
  myNex.writeStr("hv_bat.txt", String(TSV));
  myNex.writeStr("tsc.txt", String(TSC));
  soc_update();

  // Serial.print("Pg 0: ");
  // Serial.print(micros() - t1);
}

void updatePg1(){
  // Serial.println("Page 1 Update");
  // int t1 = micros();
  APPS_norm = (APPS1_norm + APPS2_norm) / 2;
  if (APPS_norm < 0) {
    APPS_norm = 0;
  }
  apps = (int)map(APPS_norm, 0, 1, 0, 100);
  myNex.writeNum("apps_bar.val", apps);

  rpm_left_raw = registerValuesLeft[static_cast<size_t>(MC::Register::SPEED_ACTUAL)];
  rpm_right_raw = registerValuesRight[static_cast<size_t>(MC::Register::SPEED_ACTUAL)];
  rpm_left = rpm_left_raw *6000/32767.0;
  rpm_right = rpm_right_raw *6000/32767.0;
  motor_rpm_avg = (abs(rpm_left) + abs(rpm_right))/2.0;

  myNex.writeStr("rpm.txt", String(motor_rpm_avg));

  torque_left_raw = registerValuesLeft[static_cast<size_t>(MC::Register::TORQUE_OUT)];
  torque_right_raw = registerValuesRight[static_cast<size_t>(MC::Register::TORQUE_OUT)];
  float avg_torque = (abs(torque_left_raw) + abs(torque_right_raw)) / 2.0;
  avg_torque = (avg_torque * 67.5) / 32767.0;
  myNex.writeStr("torq.txt", String(avg_torque));

  float torque_cmd_avg = (abs(registerValuesRight[static_cast<size_t>(MC::Register::TORQUE_CMD)]) + 
                         abs(registerValuesLeft[static_cast<size_t>(MC::Register::TORQUE_CMD)]))/2.0;

  torque_cmd_avg = (torque_cmd_avg * 67.5) / 32767.0;
  myNex.writeStr("torq_cmd.txt", String(torque_cmd_avg));
  
  // float power_avg = (abs(registerValuesLeft[static_cast<size_t>(MC::Register::POWER_ACTUAL)]) + 
  //                           abs(registerValuesRight[static_cast<size_t>(MC::Register::POWER_ACTUAL)])) / 2.0;
  

  int16_t v_dc_raw = registerValuesLeft[static_cast<size_t>(MC::Register::DC_BUS)];
  float v_dc = v_dc_raw/15.43;
  
  // if(v_dc < 0) {
  //   v_dc = 0; // Prevent negative voltage
  // }

  myNex.writeStr("vdc.txt", String(v_dc));

  int16_t i_ac_raw = (abs(registerValuesLeft[static_cast<size_t>(MC::Register::I_IST)]) + 
                            abs(registerValuesRight[static_cast<size_t>(MC::Register::I_IST)]));
  float i_ac = 2.0*i_ac_raw*80.0/(10.0 * 1153.0);
  float v_ph = (v_dc/sqrt(2)) * (registerValuesRight[static_cast<size_t>(MC::Register::VOUT)]/4096.0) * 0.92;
  float p_ac = sqrt(3) * v_ph * i_ac;
  float p_dc = p_ac;
  float i_dc = p_dc/ (1.0*v_dc);
  
  myNex.writeStr("idc.txt", String(i_dc));
  myNex.writeStr("power.txt", String(p_dc));

  myNex.writeStr("page1.max_rpm_var.txt", String(actual_max_rpm));
  myNex.writeStr("page1.max_torq_var.txt", String(actual_max_torque));

  
  Serial.print("RPM Left: ");
  Serial.print(rpm_left);
  Serial.print(" RPM Right: ");
  Serial.print(rpm_right);
  Serial.print(" Torque Left: ");
  Serial.print(torque_left_raw);
  Serial.print(" Torque Right: ");
  Serial.print(torque_right_raw);
  Serial.print(" Torque Left (Nm): ");
  Serial.print((torque_left_raw * 67.5) / 32767.0);
  Serial.print(" Torque Right (Nm): ");
  Serial.print((torque_right_raw * 67.5) / 32767.0);
  Serial.print(" VDC: ");
  Serial.print(v_dc);
  Serial.print(" IDC: ");
  Serial.println(i_dc);

  // Serial.print("Pg1: ");
  // Serial.print(micros() - t1);
}

// Page 2: Motor Controller error updates
void updatePg2(){
  // int t1 = micros();
  // Serial.print(" Pg2:");

  // Serial.print(micros() - t1);

}


// Page 3: MC data display
void updatePg3(){
  // int t1 = micros();
  // Update Nextion display elements with MC data
  myNex.writeStr("v_right.txt", v_right);
  myNex.writeStr("p_right.txt", p_right);
  myNex.writeStr("i_right.txt", i_right_raw);
  myNex.writeStr("enable_right.txt", enable_right);
  
  myNex.writeStr("v_left.txt", v_left);
  myNex.writeStr("p_left.txt", p_left);
  myNex.writeStr("i_left.txt", i_left_raw);
  myNex.writeStr("enable_left.txt", enable_left);

  // Serial.print(" Pg3: ");
  // Serial.print(micros() - t1);
}

// Page 4: AMS_FLT
void updatePg4(){
  // int t1 = micros();
  
  // Update Nextion display colors based on signals
  myNex.writeNum("con_src_2.bco", con_src ? GREEN : RED);
  myNex.writeNum("con_src_il.bco", con_src_il ? GREEN : RED);
  myNex.writeNum("to_ams_relay.bco", to_ams_relay ? GREEN : RED);
  myNex.writeNum("pre_plaus.bco", pre_plaus ? GREEN : RED);
  myNex.writeNum("c_plus_mech.bco", c_plus ? GREEN : RED);
  myNex.writeNum("c_minus_mech.bco", c_minus ? GREEN : RED);
  myNex.writeNum("con_plus_plaus.bco", c_plus_plaus ? GREEN : RED);
  myNex.writeNum("con_neg_plaus.bco", c_minus_plaus ? GREEN : RED);
  myNex.writeNum("g60_plaus.bco", gt_60v_plaus ? GREEN : RED);
  myNex.writeNum("pre_mech.bco", pre_mech ? GREEN : RED);

  // Serial.print(" Pg4: ");
  // Serial.print(micros() - t1);
}

void updatePg5(){
  // int t1 = micros();
  
  myNex.writeStr("page5.max_rpm_var.txt", String(actual_max_rpm));
  myNex.writeStr("page5.max_torq_var.txt", String(actual_max_torque));

  // Serial.print(" Pg5: ");
  // Serial.print(micros() - t1);
}

// Placeholder booleans – replace with decoded values
int error_code = 0;

// Declare variables for ECU message decoding
uint8_t pre_charge = 0, CON_MINUS = 0, CON_PLUS = 0, apps_error = 0;


// uint16_t sg_lr1 = 0, sg_lr2 = 0, sg_rr1 = 0, sg_rr2 = 0;
// uint16_t lc_rr = 0, lc_rl = 0, sus_rl = 0, sus_rr = 0;
// uint8_t temp[18] = {0};

//=================================================================
// Time and JSON Transmission Functions
//=================================================================

String timeStr = "00:00:00";

// Returns an updated time string using Serial2 and system millis
String getTimeString(){
  
  // Read time from Serial2 once if available
  if(Serial2.available() > 0 && !timeInitialized){
    timeStr = Serial2.readStringUntil('\n');
    Serial.println(timeStr);
    // Assume format "HH:MM:SS"
    hours = timeStr.substring(0,2).toInt();
    mins  = timeStr.substring(3,5).toInt();
    sec   = timeStr.substring(6,8).toInt();
    actual_millis = millis();
    timeInitialized = true;
    digitalWrite(LED_BUILTIN, HIGH);
  }

  // int t1 = micros();
  
  // Calculate elapsed time using millis
  unsigned long elapsed = (millis()-actual_millis) / 1000;

  int totalSec = sec + elapsed;
  int newSec = totalSec % 60;
  int totalMins = mins + (totalSec / 60);
  int newMin = totalMins % 60;
  int newHour = hours + (totalMins / 60);
  
  char buffer[20];
  sprintf(buffer, "%02d:%02d:%02d", newHour, newMin, newSec);
  // Serial.print("getTime time: ");
  // Serial.print(String(buffer));
  // Serial.println();
  // Serial.println(micros()-t1);
  return String(buffer);
}

// // Transmits JSON data (for LTE) over Serial2
void transmit(const String &timeStr){
  int t1 = micros();
  // Allocate the JSON document
  JsonDocument payload;
  JsonDocument ams_doc;
  JsonDocument motor_doc;
  JsonDocument left_motor_doc;
  JsonDocument right_motor_doc;
  JsonDocument sensor_doc;

  // Add values in the document
  ams_doc["time"] = timeStr;

  // Add an array
  JsonArray ams_data = ams_doc["data"].to<JsonArray>();
  // ams_data.add(random(400));
  // ams_data.add(random(5));
  // /*0=Con_SRC 1=CON_SRC_IL 2= TO_AMS_RELAY 3=PRE_Plaus 4= C+ 5=C- 6=C+_plaus 7=C-_plaus 8=>60V_plaus 9=Pre_Mech */
  // ams_data.add(random(2));
  // ams_data.add(random(2));
  // ams_data.add(random(2));
  // ams_data.add(random(2));
  // ams_data.add(random(2));
  // ams_data.add(random(2));
  // ams_data.add(random(2));
  // ams_data.add(random(2));
  // ams_data.add(random(2));
  // ams_data.add(random(2));
  ams_data.add(TSV); // Tractive System Voltage
  ams_data.add(TSC); // Tractive System Current
  ams_data.add(con_src);
  ams_data.add(con_src_il);
  ams_data.add(to_ams_relay);
  ams_data.add(pre_plaus);
  ams_data.add(c_plus);
  ams_data.add(c_minus);
  ams_data.add(c_plus_plaus);
  ams_data.add(c_minus_plaus);
  ams_data.add(gt_60v_plaus);
  ams_data.add(pre_mech);

  JsonArray left_motor_data = left_motor_doc["data"].to<JsonArray>();
  left_motor_data.add(registerValuesLeft[static_cast<size_t>(MC::Register::TORQUE_OUT)]*67.5/32767.0); // torque output
  left_motor_data.add(registerValuesLeft[static_cast<size_t>(MC::Register::TORQUE_CMD)]*67.5/32767.0); // torque command
  left_motor_data.add(registerValuesLeft[static_cast<size_t>(MC::Register::SPEED_ACTUAL)]*6000/32767.0); //unfiltered rpm
  left_motor_data.add(registerValuesLeft[static_cast<size_t>(MC::Register::SPEED_ACTUAL_)*6000/32767.0]); //filtered rpm
  left_motor_data.add(2.0*80.0*registerValuesLeft[static_cast<size_t>(MC::Register::I_IST)]/(10.0*1153.0)); // Idc
  left_motor_data.add(registerValuesLeft[static_cast<size_t>(MC::Register::DC_BUS)]); // v_dc

  // left_motor_data.add(random(40)); // torque output
  // left_motor_data.add(random(40)); // torque command
  // left_motor_data.add(random(4000)); //filtered rpm
  // left_motor_data.add(random(40)); // Idc
  // left_motor_data.add(random(400)); // v_dc

  JsonArray right_motor_data = right_motor_doc["data"].to<JsonArray>();
  right_motor_data.add(registerValuesRight[static_cast<size_t>(MC::Register::TORQUE_OUT)]*67.5/32767.0); // torque output
  right_motor_data.add(registerValuesRight[static_cast<size_t>(MC::Register::TORQUE_CMD)]*67.5/32767.0); // torque command
  right_motor_data.add(registerValuesRight[static_cast<size_t>(MC::Register::SPEED_ACTUAL)]*6000/32767.0); //unfiltered rpm
  right_motor_data.add(registerValuesRight[static_cast<size_t>(MC::Register::SPEED_ACTUAL_)]*6000/32767.0); //filtered rpm
  right_motor_data.add(2.0*80.0*registerValuesRight[static_cast<size_t>(MC::Register::I_IST)]/(10.0*1153.0)); // Idc
  right_motor_data.add(registerValuesRight[static_cast<size_t>(MC::Register::DC_BUS)]); // v_dc

  // right_motor_data.add(random(40)); // torque output
  // right_motor_data.add(random(40)); // torque command
  // right_motor_data.add(random(4000)); //filtered rpm
  // right_motor_data.add(random(100)); // Idc
  // right_motor_data.add(random(400)); // v_dc

  motor_doc["leftMotor"] = left_motor_doc;
  motor_doc["rightMotor"] = right_motor_doc;

  JsonArray sensor_data = sensor_doc["data"].to<JsonArray>();
  sensor_data.add(APPS1_raw);
  sensor_data.add(APPS2_raw);
  sensor_data.add(BPS2_raw);
  sensor_data.add(STEER_raw);
  sensor_data.add(yaw_rate);
  sensor_data.add(accY);
  sensor_data.add(yaw_ang_acc);
  sensor_data.add(accX);

  // sensor_data.add(random(1000)); // APPS1_raw
  // sensor_data.add(random(1000)); // APPS2_raw
  // sensor_data.add(random(1000)); // BPS2_raw
  // sensor_data.add(random(1000)); // STEER_raw
  // sensor_data.add(random(100)); // yaw_rate
  // sensor_data.add(random(100)); // accY
  // sensor_data.add(random(100)); // yaw_ang_acc
  // sensor_data.add(random(100)); // accX

  // Add the data to the payload
  payload["ams"] = ams_doc;
  payload["motor"] = motor_doc;
  payload["sensor"] = sensor_doc;

  // Generate the minified JSON and send it to the Serial port
  Serial2.print("<");
  serializeJson(payload, Serial2);
  Serial2.println(">");

  // Serial.print("Sent JSON: ");
  // // // serializeJsonPretty(doc, Serial);
  // serializeJson(payload, Serial);
  // Serial.println();
  // Serial.print("Transmit time ");
  // Serial.print(micros()-t1);
  // Serial.println();
}

String readFramedJson(Stream &serial) {
  static String buffer = "";
  static bool inFrame = false;
  // Serial.println("Reading framed JSON...");

  while (serial.available()) {
    char c = serial.read();

    if (c == '<') {
      buffer = "";        // Start new frame
      inFrame = true;
    } else if (c == '>' && inFrame) {
      inFrame = false;    // End of frame
      return buffer;
    } else if (inFrame) {
      buffer += c;        // Add to buffer
      if (buffer.length() > 2048) {  // Optional safeguard
        buffer = "";
        inFrame = false;
      }
    }
  }
  return "";  // No complete frame yet
}

std::unordered_map<std::string, float> ECU_error;

void send_ECU_error(bool ecu_error = false) {
  CAN_message_t ecu_msg;
  ECU_error["ERROR"] = ecu_error;
  c_can_parser.encodeMessage(ECU_ERROR_ID, ECU_error, ecu_msg);
  int ecu_status = c_can.write(ecu_msg);
  // Serial.print("ECU Error Message Status: ");
  // Serial.println(ecu_status);
}

void loop() {
  int loopTime = micros();
  String jsonBuffer = readFramedJson(Serial2);
  // Serial.print("Received trigger: ");
  // Serial.println(jsonBuffer);
  if(jsonBuffer == "true") {
    Serial.println("Trigger received: " + jsonBuffer);
    send_ECU_error(true);
    delay(1000);
  }
  else{
    send_ECU_error(false);
  }

  switch(pg) {
    case 0:
      updatePg0();
      break;
    case 1:
      updatePg1();
      break;
    case 2:
      updatePg2();
      break;
    case 3:
      updatePg3();
      break;
    case 5:
      updatePg5();
      break;
    default:
      break;
  }

  updatePg4();
  myNex.NextionListen();

  // Transmit JSON with updated time over Serial2 (LTE)
  // if(!c_plus){
  //   transmit(getTimeString());
  //   delay(10);
  // }



  transmit(getTimeString());
  delay(10);
  
  // Serial.println();
  // Serial.print("Page: ");
  // Serial.print(pg);
  // Serial.print(" RPM");
  // Serial.print(" Left: ");
  // Serial.print(rpm_left);
  // Serial.print(" Right: ");
  // Serial.print(rpm_right);
  // Serial.print(" APPS1: ");
  // Serial.print(APPS1_raw);
  // Serial.print(" APPS2: ");
  // Serial.print(APPS2_raw);
  // Serial.print(" I LEFT: ");
  // Serial.print(i_left);
  // Serial.print(" RIGHT: ");
  // Serial.print(i_right);
  // Serial.print(" V LEFT: ");
  // Serial.print(v_left);
  // Serial.print(" RIGHT: ");
  // Serial.print(v_right);
  Serial.print(" Looptime: ");
  Serial.print(micros()-loopTime);
  Serial.println();
  // Serial.print("MAX RPM: ");
  // Serial.print(max_rpm);
  // Serial.print(" MAX TORQUE: ");
  // Serial.print(max_torque);
  // Serial.println();

  // Serial.print("YAW RATE: "); Serial.print(yaw_rate); 
  // Serial.print(" deg/s ACC Y: "); Serial.print(accY);
  // Serial.print("g YAW ANG ACC: "); Serial.print(yaw_ang_acc);
  // Serial.print(" deg/s^2 ACC X: "); Serial.print(accX);
  // Serial.println("g");
}