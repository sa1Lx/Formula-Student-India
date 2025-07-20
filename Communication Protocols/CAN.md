# Controller Area Network (CAN)

CAN is a robust vehicle bus standard designed to facilitate communication among various electronic control units (ECUs) in vehicles. It allows multiple microcontrollers to communicate with each other without a host computer, making it ideal for automotive applications.

CAN bus is a **Shared** and **Asynchronous** line.

## Structure of CAN Bus

Typically the CAN Bus is made up two wires, CAN-H (CAN High) and CAN-L (CAN Low) which connect to all the devices in the network. The signals on the two CAN lines have the same sequence of data, but their amplitudes are opposite.

So if a pulse on the CAN-H line goes from 2.5V to 3.75V then the corresponding pulse on the CAN-L line goes from 2.5V to 1.25V (opposite than CAN-H). By sending the data in equal and opposite ways like this allows for greater noise immunity and therefore less chance of the data being corrupted.

| State     | CAN-H   | CAN-L   | Difference      | Bit |
|-----------|---------|---------|-----------------|-----|
| Idle      | 1/2     | 1/2     | 0               | 1   |
| Dominant  | HIGH    | LOW     | CAN-H > CAN-L   | 0   |
| Recessive | LOW     | HIGH    | CAN-H ≤ CAN-L   | 1   |

More on Noise immunity can be found in the [CAN Bus Noise Immunity](https://www.ni.com/en-us/innovations/white-papers/06/can-bus-noise-immunity.html) article.

### 1. CAN Controller

The CAN Controller handles data communication between the microcontroller (inside the control unit or CAN node) and the CAN Transceiver.

* It receives data from the microcontroller and passes it to the transceiver.
* It also receives data from the transceiver, processes it, and passes it to the microcontroller.

### 2. CAN Transceiver

The CAN Transceiver acts as both a transmitter and receiver.
* It converts digital data from the CAN controller into electrical signals for the CAN bus lines.
* It also receives electrical signals from the bus and converts them back into data for the controller.

### 3. CAN Data Bus Terminal Resistor

At both ends of the CAN bus, there is a 120-ohm resistor.
* This prevents signal reflection (which would act like an echo) and ensures signal integrity across the bus.
* These are called termination resistors and are critical for proper communication.

More on the CAN Bus structure can be found in the [CAN Bus Structure](https://www.ni.com/en-us/innovations/white-papers/06/can-bus-structure.html) article.

## CAN Bus Data Transfer Process

1. **Supplying Data**: The CAN Node provides data to the CAN controller for transfer.
2. **Sending Data**: The CAN transceiver receives data from the CAN controller, converts it into electrical signals and sends them back into the network.
3. **Receiving Data**: All other CAN Nodes networked with the CAN data bus become receivers.
4. **Checking Data**: The CAN Node checks whether they require the data they have received for their functions or not.
5. **Accepting Data**: If the received data is important, it is accepted and processed. If not, the received data is ignored.

## About Bit Timing

Since there is no dedicated clock line used in the CAN bus, all nodes must agree on using the same nominal bit rate in order to communicate through the same bus. But since each node will have separate oscillators of different tolerances for generating their base clock signals, we can not expect all of the frequencies to be the same. This is where synchronization comes into play. The nodes can recover the clock from the transmitted signals themselves and adjust it dynamically. All CAN nodes must be able to do this.

When the bus is idle, it's in a recessive state (logic 1). A **hard synchronization** occurs on the first recessive to dominant transition (1 → 0) after a period of bus idle (the start bit). 

Resynchronization occurs on every recessive to dominant transition during the frame. The CAN controller expects the transition to occur at a multiple of the nominal bit time. If the transition does not occur at the exact time the controller expects it, the controller adjusts the nominal bit time accordingly.

## CAN Data Frame Structure

### 1. SOF bit (Start of Frame)

Marks the start of a CAN message using a dominant bit. (logic 0)

### 2. Message Identifier

The identifier is a unique number that identifies the message. It can be either 11 bits (standard frame) or 29 bits (extended frame). Lower values have higher priority.

Since the IDs are used for unique messages and nodes, with a longer ID we can have more number of unique messages and devices with more priority levels. The standard frame allows a maximum of $2^{11}$ = 2048 IDs and the extended frame allows $2^{29}$ = 536870912 IDs. That’s a huge difference. Cool thing is that both standard and extended frames can be used on the same bus. But as we have seen before, the lower IDs with more numbers of 0s on the MSB side will always win the arbitration. So standard frames get priority over extended frames.


### 3. Control Field (Check Field)

Tells how many data bytes are present in the message.
Helps the receiver verify if the full message has been received.

### 4. Data Field

This is the actual information being sent to other CAN nodes.
Can carry 0 to 8 bytes of data in classical CAN (up to 64 bytes in CAN-FD).

### 5. CRC (Cyclic Redundancy Check)
Also called the Safety Field.
Includes a 15-bit checksum to detect errors in transmission.
Ends with a delimiter bit to separate this section.

### 6. ACK (Acknowledge Field)
After receiving the message, nodes use this field to confirm it was received correctly.
If there's an error, receivers stay silent, and the transmitter resends the message.

### 7. EF (End Field)
Signals the end of the CAN message.
Also a last chance to report errors, triggering a retransmission if needed.


