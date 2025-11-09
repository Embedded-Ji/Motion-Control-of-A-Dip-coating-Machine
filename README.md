# Motion Control of a Dip-Coating Machine

An Arduino-based 2-axis controller for a dip-coating system, communicating with a LabVIEW host via TCP/IP.  
The firmware is implemented in `DipCoater.ino`.

---

## Features
- Dual-axis stepper motion (Enable/Dir/Step)
- Limit switch debouncing and safe bounce-back
- Built-in TCP server (default `192.168.0.1:49152`)
- Command set: `HomeX`, `HomeY`, `Move`, `Submit`, `Dip`, `Stop`
- Parameterized dip-coating sequence with configurable speeds and dwell times

---

## Hardware Setup
- **MCU:** Arduino MKR WiFi 1010 + MKR ETH Shield  
- **Motor Drivers:** TB6600 (Microstep = 32, 10 mm lead screw, 200 steps/rev)  
- **Limit switches:** A1–A4 (active LOW)

| Axis | Enable | Dir | Step | Notes |
|------|--------|-----|------|-------|
| Horizontal | D6 | D7 | D14 | TB6600 EN/DIR/CLK |
| Vertical | D3 | D4 | D13 | TB6600 EN/DIR/CLK |

![Wiring Diagram](docs/images/fig-wiring.png)

---

## Network Configuration
Edit the header of `DipCoater.ino`:
```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 1);
const uint16_t kPort = 49152;
---

Build & Upload

Open DipCoater.ino in the Arduino IDE.

Install the Ethernet library if missing.

Adjust MAC/IP/Port as needed.

Verify → Upload. Open Serial Monitor to confirm the TCP server started.

TCP Command Protocol

Directions: 0=left, 1=up, 2=right, 3=down

Command	Description
HomeX	Home the X/Horizontal axis and back off safely.
HomeY	Home the Y/Vertical axis and back off safely.
Move,<dir>,<dist_mm>,<speed_mm_s>	Move in direction <dir> by <dist_mm> at <speed_mm_s>.
Submit,<12 params>	Submit the current recipe parameters (see below).
Dip	Execute the full dip sequence using the last submitted parameters.
Stop	Safely stop motion/tones; if mid-dip, exit at the next safe point.

Submit parameters (12 total, order matters):

moveSpeed (mm/s)

dipSpeed (mm/s)

withdrawSpeed (mm/s)

dipX (mm) – target X for dipping

dipY1 (mm) – upper Y reference

dipY2 (mm) – lower Y (dip depth)

curX (mm) – current X (for resume)

curY (mm) – current Y (for resume)

dipTimes (count) – number of cycles

curingTime_ms (ms) – curing/hold time

dippingTime_ms (ms) – dip dwell time

liftHeight_mm (mm) – lift height after dip

Dip process (placeholder)
