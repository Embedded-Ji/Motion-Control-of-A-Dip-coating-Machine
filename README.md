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

## Build & Upload

1. Open `DipCoater.ino` in the **Arduino IDE**.  
2. Install the **Ethernet** library if it is not already installed.  
3. Adjust **MAC**, **IP**, and **Port** in the code header if needed.  
4. Click **Verify → Upload**, then open the **Serial Monitor** to confirm that the TCP server starts successfully.

---

## TCP Command Protocol

| Command | Description |
|----------|--------------|
| `HomeX`, `HomeY` | Return to limit and back off safely |
| `Move,<dir>,<dist_mm>,<speed_mm_s>` | Move in direction {0:left, 1:up, 2:right, 3:down} |
| `Submit,<12 params>` | Send dip parameters (speeds, positions, times) |
| `Dip` | Execute a full dipping sequence |
| `Stop` | Safely stop any ongoing motion |

---

## LabVIEW Interface

Screenshots of the LabVIEW control panel:

![LV Connect](labview/screenshots/lv-connect.png)  
![LV Command](labview/screenshots/lv-send-cmd.png)  
![LV UI](labview/screenshots/lv-ui.png)

---

## Safety

- Limit switches are debounced and automatically retracted after triggering.  
- `Stop` halts all outputs and completes the current safe segment before stopping.  
- Always verify wiring, motor current limits, and safety connections before powering on.

---

