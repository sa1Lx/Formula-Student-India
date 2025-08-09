# Communication Protocols

Communication involves exchange of data between 2 nodes. Communication protocols are those rules which are followed so that the receiving node can successfully decode and use the data.
**Nodes** refer to the whole PCB which contains the microcontroller and the hardware required for communication.

* What is an **Embedded System**?

An embedded system is a specialized computer system. It combines computer hardware and software to perform dedicated functions within a larger mechanical or electronic system. It is embedded as part of a complete device.

* How do Embedded Systems communicate?

In embedded systems, communication is the exchange of data between microcontrollers using bits. This follows specific rules called communication protocols.

When bits are sent one after another in sequence, it's called **Serial Communication**. It uses a single channel to transmit data, making it useful when fewer wires or longer distances are involved.

On the other hand, **Parallel Communication** sends multiple bits simultaneously over multiple channels. This allows for faster data transfer but requires more wires, making it less suitable for compact or long-distance applications.

* So how do the Reciever and Transmitter communicate in Serial Communication?

A. Simplex Mode
In simplex transmission, data flows in only one direction. Either the sender transmits or the receiver receives, but not both at the same time. It's a one-way communication system, like a radio or television where we only receive information.

B. Half-Duplex Mode
Half-duplex transmission allows data to flow in both directions, but not at the same time. This means that while one device is sending data, the other must wait until the transmission is complete before responding. An example of half-duplex communication is a walkie-talkie, where one person speaks while the other listens, and they must take turns to communicate.

C. Full-Duplex Mode
Full-duplex transmission enables simultaneous two-way communication. Both the sender and receiver can transmit and receive data at the same time. This is like a telephone conversation, where both parties can talk and listen simultaneously, allowing for more efficient communication.

## Clock Synchronization

In serial communication, different devices may use different methods for clock synchronization. This is mainly divided into two types: Synchronous Serial Interface and Asynchronous Serial Interface. These determine how the clock signal is managed between sender and receiver.

### 1. Synchronous Serial Interface

This interface is used for ***point-to-point communication*** between a master and one or more slave devices. In this setup, both data and clock are shared on the same CPU bus, making transmission faster and eliminating baud rate mismatches. On the transmitter side, data is shifted onto the serial line while a separate clock signal is provided. Since there are no start, stop, or parity bits, the transmission is more efficient. On the receiver side, the clock from the transmitter is used to extract and convert the serial data back to parallel form. I2C and SPI are common examples of synchronous serial communication.

### 2. Asynchronous Serial Interface

Unlike the synchronous type, asynchronous serial interfaces ***do not*** use an external clock signal. This makes them ideal for long-distance communication and stable transmission where precise timing isn’t shared between devices. Since there’s no common clock, these systems depend on various parameters like data flow control, error control, baud rate control, and transmission/reception control. On the transmitter side, parallel data is converted to serial using its own internal clock, and extra bits—start, stop, and sometimes parity—are added to ensure proper communication. The receiver, using its own clock, removes these bits and converts the serial stream back into parallel form. Common examples include CAN, RS-232, RS-422, and RS-485.

## Some More Terms:

### 1. Baud Rate

Baud rate refers to the speed of data transfer between the transmitter and receiver, measured in bits per second (bps). A common baud rate is 9600 bps, but other values like 1200, 2400, 4800, 57600, and 115200 are also used depending on the application. Higher baud rates allow faster data transmission. Both transmitter and receiver must use the same baud rate for proper communication.

### 2. Synchronization

Synchronization bits are used to identify the start and end of each data frame. The transmitter adds start and stop bits to help the receiver correctly detect the beginning and end of the incoming data. Without this, the receiver could easily misinterpret the stream of bits.

### 3. Error Bits (Parity Bits)

Serial communication can be affected by noise and errors, especially over long distances. To detect errors, parity bits are added. These can be set for even or odd parity. For example, if a data frame has an even number of 1s, the parity bit is set to 1 (even parity); if odd, it’s cleared (odd parity). This is helpful for basic error checking, though more advanced systems may use additional error correction methods since it will only check odd number of bit flipping.

### 4. Bit Stuffing

Some communication protocols use special bit patterns to mark the start or end of a frame. For example:

```
HDLC (High-Level Data Link Control) uses the frame delimiter:
01111110
```

If the actual data being sent contains this same pattern by chance, the receiver might think it's the end of the frame, even when it's not.

In HDLC-style protocols:

1. The sender watches for five consecutive 1s in the data.

2. If it sees five 1s in a row, it automatically inserts a 0 after them.

3. This prevents the creation of the 01111110 flag sequence inside the data.

To maintain synchronization, CAN bus requires that an opposite polarity bit is stuffed after every 5 consecutive bits of the same polarity, in a process called bit stuffing. So if we send 00000 consecutively, then the next bit should be a 1 regardless of what data bit comes next. If we send the bits 11111 consecutively, then we must send a 0 next. CAN controllers are smart enough to recognize these bits and remove them from the actual data you get. So all data remain the same regardless of the number or positions of the stuff bits. This process is called bit destuffing.