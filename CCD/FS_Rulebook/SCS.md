# T 11.9 System Critical Signal

* T 11.9.1: SCS are defined as all electrical signals which
    - Influence actions on the SDC, see CV 4.1 and EV 6.1.
    - Influence the wheel torque.
    - [EV ONLY] Influence indicators according to EV 5.8.9, EV 4.10 or EV 6.3.8.

Note- SCS include, signals which influence actions of SDC, influence the wheel torque (APPS, BPPS), and signals influencing the red indicators for AMS or IMD (indicating AMS or IMD has opened the SDC), & TSAL.

* T 11.9.2: Any of the following SCS single failures must result in a safe state of all connected systems:

a. Failures of signals transmitted by cable:<br>
    - Open circuit
    - Short circuit to ground

b. Failures of analog sensor signals transmitted by cable:<br>
    - Short circuit to supply voltage

c. Failures of sensor signals used in programmable devices:<br>
    - Implausibility due to out of range signals, e.g. mechanically impossible angle of
    an angle sensor.

d. Failures of digitally transmitted signals by cable or wireless:<br>
    - Data corruption (e.g. checked by a checksum)
    - Loss and delay of messages (e.g. checked by transmission time outs)

Signals might be a member of multiple signal classes, e.g. analog signals transmitted by
cable might be a member of T 11.9.2.a, T 11.9.2.b and T 11.9.2.c.

Note- For eg., APPS belongs to the first 3 classes, so all of them must be checked to declare a SCS failure.

* T 11.9.3: If a signal failure is correctable, e.g. due to redundancy or worst case values, the safe state
must be entered as soon as an additional non correctable failure occurs.

Note- Safe state maybe avoided, in cases of failures of signals which are correctable, due to redundancy (eg. if 1 out of 3 APPS fails, then the rest 2 are enough to provide valid APPS value) or worst case values (eg. ?)

* T 11.9.4: The maximum allowed delay of messages according to T 11.9.2.d must be chosen depending
on the impact of delayed messages to the connected system, but must not exceed 500 ms.

Note- A maximum delay of 500ms is tolerable for messages carried by digitally transmitted signals. (wireless signals arent included as of now)

* T 11.9.5: Safe state is defined depending on the signals as follows:
    - signals only influencing indicators – Indicating a failure of its own function or of the
connected system
    - LV battery signals – At least one pole is electrically disconnected from the rest of the
vehicle
    - ASSI, see T 14.9 – indicating “AS Off”
    - [EV ONLY] For all others signals – opened SDC and opened AIRs
    - [CV ONLY] For all others signals – opened SDC and stopped engine

* T 11.9.6: Indicators according to T 11.9.1 with safe state “illuminated” (e.g. absence of failures is not
actively indicated) must be illuminated for 1 s to 3 s for visible check after power cycling the
LVMS.

Note- When the LVMS is powered up once (LV Supply is connected to the LV battery), IMD and AMS indicators must be illuminated for 1 s to 3 s for visible check, indicating that the communication is intact. 