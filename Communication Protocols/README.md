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

