#include <Arduino.h>
#include <unordered_map>
#include <string>
#include <cstdint>

#include "SharedLib.h"
C_CAN_PARSER::CANParser c_can_parser;

#include <FlexCAN_T4.h>
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> c_can; // Define CAN object for CAN1 interface

#define APPS_ID 0x104
#define apps1 22 
#define apps2 20
//#define bps1  
#define bps2 21 
#define steer0 23

//Actuator signals
uint16_t apps1_raw = 0;
uint16_t apps2_raw = 0;
// uint16_t bps1_raw = 0;
uint16_t bps2_raw = 0;
uint16_t steer_raw = 0;

bool can_transmission_status = 0;

void actuator_read_raw_values() {
    apps1_raw = analogRead(apps1);
    apps2_raw = analogRead(apps2);
    // bps1_raw = analogRead(bps1);
    bps2_raw = analogRead(bps2);
    steer_raw = analogRead(steer0);
  }

void setup() {

  Serial.begin(115200); 
  c_can.begin();
  c_can.setBaudRate(500000);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(apps2,INPUT);
  pinMode(apps1,INPUT);
  pinMode(bps2,INPUT);
  //pinMode(bps1,INPUT);
  pinMode(steer0,INPUT);
}

CAN_message_t msg;  // Define a message object to store incoming CAN values

void loop() {

  actuator_read_raw_values();

  std::unordered_map<std::string, float> actuator_signals;
  actuator_signals["APPS1"] = apps1_raw;
  actuator_signals["APPS2"] = apps2_raw;
  //actuator_signals["BPS1"] = bps1_raw;
  actuator_signals["BPS2"] = bps2_raw;
  actuator_signals["STR0"] = steer_raw;

  c_can_parser.encodeMessage(APPS_ID, actuator_signals, msg);
  can_transmission_status = c_can.write(msg);

  Serial.print("APPS1_raw:");Serial.print(apps1_raw);
  Serial.print("  APPS2_raw:");Serial.print(apps2_raw);
  // Serial.print("  BPS1_raw:");Serial.print(bps1_raw);
  Serial.print("  BPS2_raw:");Serial.print(bps2_raw);
  Serial.print("  Steer_raw:");Serial.print(steer_raw);
  Serial.println();

  if (can_transmission_status) digitalToggle(LED_BUILTIN);

      
  delay(50);

}

