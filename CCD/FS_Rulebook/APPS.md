# T 11.8 Accelerator Pedal Position Sensor

* T 11.8.1: T 11.8 only applies for EVs or CVs using ETC.

* T 11.8.2: The APPS must be actuated by a foot pedal.

* T 11.8.3: Pedal travel is defined as percentage of travel from fully released position to a fully applied position where 0% is fully released and 100% is fully applied. The foot pedal must return to the 0% position when not actuated.

Note- When the accelerator is released, it must return to its relaxed position, which gives 0% pedal travel.

* T 11.8.4: The foot pedal must have a positive stop preventing the mounted sensors from being damaged or overstressed. Two springs must be used to return the foot pedal to the 0% position and each spring must work when the other is disconnected. Springs in the APPS are not accepted as return springs.

Note- "Positive Stop" refers to a mechanical limit that prevents the pedal from being pushed beyond its intended range of motion, protecting the sensors from damage. There should be 2 mechanical springs, which are not part of the APPS itself, which bring back the accelerator pedal to its relaxed position. 

* T 11.8.5: At least two separate sensors must be used as APPSs. The sensors may share the housing.

Note- Atleast 2 sensors should be present such that if one of them fails, the other can still provide a valid signal. // housing?

* T 11.8.6: If analog sensors are used, they must have different, non-intersecting transfer functions. A short circuit between the signal lines must always result in an implausibility according to T11.8.9.

Note- We do use analog sensors, i.e, potentiometers. These sensors must be caliberated such that their transfer functions dont meet for any given value of pedal travel. This is so that a short circuit between the two sensor lines will indicate a short circuit.

* T 11.8.7: The APPS signals are SCSs, see T11.9.

Note- APPS signals are System Critical Signals.

* T 11.8.8: If an implausibility occurs between the values of the APPSs and persists for more than 100ms
    - [EV ONLY] The power to the motor(s) must be immediately shut down completely. It is not necessary to completely deactivate the TS, the motor controller(s) shutting down the power to the motor(s) is sufficient.
    - [CV ONLY] The power to the electronic throttle must be immediately shut down.

Note- If there is an implausibility (defined at T11.8.9) and it persists more than 100 ms, then the system shall go into safe state, which here is defined by giving 0 torque command to the motors.

* T 11.8.9: Implausibility is defined as a deviation of more than ten percentage points pedal travel between any of the used APPSs or any failure according to T11.9. 

Note- If there is more than 10% difference between the pedal travel of APPS1 and APPS2, it should trigger 0 torque to the motors.

* T 11.8.10: If three sensors are used, then in the case of an APPS implausibility, any two sensors that are plausible may be used to define the torque target and the 3rd APPS may be ignored. 

Note- If three sensors are used, and one fails, the remaining two are enough to provide a valid APPS signal without causing to go into safe mode.

* T 11.8.11: It must be possible to separately disconnect each APPS signal wire to check all functionalities. 

Note- For scrutineering.

* T 11.8.12: A fully released accelerator pedal in manual mode must result in: 
    - [EV ONLY] Awheel torque of ≤0Nm 
    - [CV ONLY] Anidle position or lower throttle set-point. This may only be exceeded during a gearshift for a maximum of 500ms.

Note- A fully released accelerator pedal shall result 0 torque commands to the motor.
