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
