# EV 4.11 Activating the Tractive System

* EV 4.11.1: The TS is active if any of the AIRs or the pre-charge relay is closed. 

* EV 4.11.2: The driver must be able to activate and deactivate the TS from within the cockpit without the assistance of any other person.

Note- Using the shutdown button in the cockpit.

* EV 4.11.3: The ASR(Autonomous System Responsible) must be able to activate the TS from outside the vehicle with an external TS activation button in proximity to the TSMS if and only if the ASMS(Autonomous System Master Switch) is in “On” position.

Note- HV RESET AS

* EV 4.11.4: Closing the SDC by any part defined in EV6.1.2 must not (re-)activate the TS. Additional action must be required.

Note- HV RESET is the additional logic used in our car.

* EV 4.11.5: The AS must not be able to (re-)activate the TS. 

* EV 4.11.6: The vehicle is in Ready-to-drive (R2D) mode as soon as the motors will respond to the input of the APPS. 

Note- After RFE & FRG signals are sent to the MCU.

* EV 4.11.7: After the TS has been activated, additional actions must be required by the driver to set the vehicle to R2D mode, e.g. pressing a dedicated start button. The transition to R2D mode must only be possible during the actuation of the mechanical brakes and a simultaneous dedicated additional action. 

Note- To get into R2D mode, driver must hold on the brakes and turn on the ignition button.

* EV 4.11.8: The R2D mode must be left immediately when the SDC is opened.
