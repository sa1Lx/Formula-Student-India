# EV 3 GENERAL REQUIREMENTS

## EV 3.1 Grounding

* EV 3.1.1: TS enclosures, see EV 1.1.2, must consist of either
    - a grounded solid layer made of at least 0.5 mm thick electrically conductive material,
aluminium or better, see EV 3.1.3 or
    - be fully made out of electrically insulating materials having an isolation resistance
of at least 2 MΩ, measured with a voltage of 500 V. The TS enclosure must be rigid
and must prevent possible mechanical penetrations. Protruding electrically conductive
parts, such as fasteners or connectors, must follow EV 3.1.2.
The TSAC might use at least 0.9 mm thick steel layer as the grounded layer.

* EV 3.1.2: Electrically conductive seat, driver harness, and firewall mounting points as well as the
aluminium layer of the TS firewall, see T 4.8, and the LVS ground measuring point, see
EV 4.7.8, must be grounded, see EV 3.1.3.

* EV 3.1.3: An electrically conductive part is grounded if its resistance to LVS ground is below 100 mΩ,
measured with a current of 1 A, and the grounding wire is able to continuously carry at least
10 % of the TS accumulator main fuse current rating.

* EV 3.1.4: Parts of the vehicle which are or may become electrically conductive within 100 mm of any
TS component must have a resistance below 100Ω to LVS ground.

* EV 3.1.5: The rotating part of the wheels does not need to be grounded.

## EV 3.2 Overcurrent Protection

* EV 3.2.1: All electric systems must have appropriate overcurrent protection.

* EV 3.2.2: The continuous current rating of the overcurrent protection must not be greater than the
continuous current rating of any electric component, for example, wire, busbar, or other
conductors that it protects. I.e. if multiple pins of a connector are used to carry currents in
parallel, each pin must be appropriately protected.

Note- The continuous current rating of an electrical device is the maximum current that can flow continuously(3+ hours) through the device without causing tripping or overheating.

* EV 3.2.3: All used fuses must have an interrupt current rating that is higher than the theoretical short
circuit current of the system that it protects.

Note- The interrupt current is the highest current that a fuse can safely disconnect without
causing damage to itself or the circuit.

* EV 3.2.4: All overcurrent protection devices must be rated for the highest voltage in the systems they
protect. All devices used must be rated for DC.

Note- To prevent arcing or isolation breakdown. DC is more dangerous since there's no natural 0 crossing so arc can persist much longer than AC arcs.

* EV 3.2.5: All overcurrent protection devices that are part of the TS must not rely on programmable
logic. The overcurrent protection function of motor controllers/inverters for the motor outputs
may rely on programmable logic.

* EV 3.2.6: The overcurrent protection must be designed for the expected surrounding temperature range
but at least for 0 ◦C to 85 ◦C.

* EV 3.2.7: The TS high current path, see EV 1.2.2, through the TS accumulator(s) must be fused.