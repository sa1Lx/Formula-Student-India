# EV 4.10 Tractive System Active Light

* EV 4.10.1: The vehicles must include a single TSAL that must indicate the TS status. The TSAL must
not perform any other functions. A TSAL with multiple LEDs in one housing is allowed.

* EV 4.10.2: The TSAL itself must have a red light, flashing continuously with a frequency between 2 Hz
and 5 Hz and a duty cycle of 50 %, active if and only if the LVS is active and the voltage
across any DC-link capacitor exceeds-
    - 60 V DC or 50 V AC RMS
    - Half the nominal TS voltage
whichever is lower.

Note- TSAL's red light glows contingent the LVS is active and the voltage across MC+ and MC- exceeds 60V DC or 50V AC RMS.

* EV 4.10.3: The TSAL itself must have a green light, continuously on, active if and only if the LVS is
active and ALL of the following conditions are true:
    - All AIRs are opened.
    - The pre-charge relay, see EV 5.7.2, is opened.
    - The voltage at the vehicle side of the AIRs inside the TSAC does not exceed 60 V DC
or 50 V AC RMS.

Note- TSAL's green light glows contingent the LVS is active, AIR & Pre-Charge relays are open, and the voltage at the vehicle side of the AIRs inside the TSAC doesnt exceed 60 V DC or 50 V AC RMS.

* EV 4.10.4: The mentioned voltage detection must be performed inside the respective TS enclosure.

Note- 

* EV 4.10.5: The mentioned states of the relays (opened/closed) are the actual mechanical states. The
mechanical state can differ from the intentional state, i.e. if a relay is stuck. Any circuitry
detecting the mechanical state must meet EV 5.6.2.

Note-

* EV 4.10.6: The voltage detection circuit of the red light and the relay state and voltage detection circuit
of the green light must be independent. Any plausibility check between both lights is not
allowed. A TSAL state with both lights simultaneously active might occur and must not be
prevented.

Note-

* EV 4.10.7: The TSAL must:
    - Be located lower than the highest point of the main hoop and including the mounting
within the rollover protection envelope, see T 1.1.16.
    - Be no lower than 75 mm from the highest point of the main hoop.
    - Not be able to contact the driver’s helmet in any circumstances.

Note-

* EV 4.10.8: The entire illuminated surface of the TSAL must be clearly visible:
    - Except for angles less than 10◦ on each side which are blocked by the main hoop.
    - From a point 1.60 m vertically from ground level, within 3 m horizontal radius from
the TSAL.
    - In direct sunlight.

Note- 

* EV 4.10.9: The TSAL and all needed circuitry must be hard-wired electronics. Software control is not
allowed.

Note-

* EV 4.10.10: A green indicator light in the cockpit that is easily visible even in bright sunlight and clearly
marked with “TS off” must light up if TSAL green light is on, see EV 4.10.3.

Note-

* EV 4.10.11: Signals influencing the TSAL and the indicator according to EV 4.10.10 are SCS, see T 11.9.
The individual safe state of each of the TSAL lights is off. The TSAL’s red light must not be
illuminated for a visible check, see T 11.9.6.

Note-

* EV 4.10.12: The TSAL’s red light voltage detection circuit, see EV 4.10.2, does not need to detect an
open circuit, as required by T 11.9. A plausibility check must not be implemented.

Note-

* EV 4.10.13: The TSAL’s green light relay state detection circuit, see EV 4.10.3, does not need to detect an
open circuit, as required by T 11.9, when the intentional state of the used (auxiliary) contact
is opened. A plausibility check against the intentional relay state must be implemented in a
way that the TSAL’s green light stays off after the open circuit is detectable.

Note-

* EV 4.10.14: The TSAL’s green light voltage detection circuit, see EV 4.10.3, does not need to detect an
open circuit, as required by T 11.9, when no voltage is present. A plausibility check against
the intentional relay states must be implemented in a way that the TSAL’s green light stays
off after the open circuit of the TS accumulator voltage detection circuit is detectable.

Note-

* EV 4.10.15: The latching required by EV 4.10.13 and EV 4.10.14 must not be triggered during normal
operation conditions and must only be reset by power cycling the LVS.

Note-