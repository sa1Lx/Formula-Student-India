# EV 1 Definitions

## EV 1.1 Tractive System

* EV 1.1.1: Tractive System (TS) – every part that is electrically connected to the motors and TS
accumulators. The LVS may be supplied by the TS if a galvanic isolation between both
systems is ensured.

Note- Our vehicle has separate LV battery, the TS doesnt power the LVS.

* EV 1.1.2: TS enclosures – every housing or enclosure that contains parts of the TS.

Note- TS enclosures includes the accumulator enclosure, MCU enclosure, motor enclosure, HVD interlock.

## EV 1.2 Electrical

* EV 1.2.1: Galvanic Isolation – two electric circuits are defined as galvanically isolated if all of the
following conditions are true:
    - The resistance between both circuits is ≥500 Ω/V, related to the maximum TS voltage
of the vehicle, at a test voltage of maximum TS voltage or 250 V, whichever is higher.
    - The isolation test voltage RMS, AC for 1 min, between both circuits is higher than
three times the maximum TS voltage or 750 V, whichever is higher.
    - The working voltage of the isolation barrier, if specified in the datasheet, is higher than
the maximum TS voltage.
Capacitors that bridge galvanic isolation must be class-Y capacitors.

Note- Since the max TS voltage of our vehicle is 403.2V, the resistance to maintain galvanic isolation is 201.6 kΩ. The isolation barrier must withstand an isolation test voltage of 1209.6V without breakdown. The working voltage of the isolation barrier must be higher than 403.2V.

* EV 1.2.2: High Current Path – any path of a circuitry that, during normal operation, carries more than
1 A.
