# Smart Bobbin Thread Monitoring System

A low-cost embedded system designed to detect interruptions in sewing-machine lower-thread delivery and alert the tailor before unnoticed stitching failure continues.

> **Project Status:** Software logic and simulation testing completed. Physical sensing prototype in development.

## The Problem

While sewing, the lower/bobbin thread can stop feeding without the tailor noticing immediately. This may happen because:

- the bobbin becomes empty;
- the lower thread breaks;
- the thread becomes tangled or jammed;
- the bobbin contains thread but fails to feed correctly;
- an improperly wound or overfilled bobbin interferes with thread delivery.

The machine may continue running even though proper stitches are no longer being formed.

As a tailor, I have experienced this problem personally. This led me to explore whether an embedded system could detect the problem automatically.

## The Idea

Instead of simply asking:

**“Is there thread remaining on the bobbin?”**

this project asks:

**“While the machine is sewing, is lower thread actually being delivered?”**

This distinction allows the system to target more than an empty bobbin.

The proposed system independently monitors:

1. **Sewing-machine movement**
2. **Lower-thread activity**

If the machine continues operating while lower-thread activity disappears, the system begins a 2-second confirmation period.

If thread activity returns, the suspected fault is cancelled.

If the machine stops, the suspected fault is also cancelled.

If the machine continues sewing with no detected lower-thread activity for the full confirmation period, the system enters an alert state and warns the tailor to:

**CHECK YOUR BOBBIN.**

## System Logic

The system uses four states:

```text
IDLE
   │
   │ Machine starts
   ▼
MONITORING
   │
   │ Lower-thread activity stops
   ▼
CONFIRMING
   │
   ├── Thread resumes ──────────> MONITORING
   │
   ├── Machine stops ───────────> IDLE
   │
   └── No thread for ≥2 seconds
                  │
                  ▼
                ALERT
                  │
                  │ Deliberate reset
                  ▼
          MONITORING / IDLE
```

Once an alert has been triggered, thread movement alone does not clear it. The tailor must deliberately acknowledge/reset the warning.

## Software

The system logic is currently implemented in **C** using a state-machine architecture.

```c
enum SystemState {
    IDLE,
    MONITORING,
    CONFIRMING,
    ALERT
};
```

The desktop implementation currently simulates three inputs:

```c
machineRunning
threadActivity
resetEvent
```

These will later be replaced with physical sensor inputs on the microcontroller.

## Testing

A separate C test program was created to verify the state-machine behavior.

| Test | Scenario | Result |
|---|---|---|
| 1 | Temporary thread interruption | PASS |
| 2 | No lower thread for ≥2 seconds while sewing | PASS |
| 3 | Machine stops during confirmation | PASS |
| 4 | Thread returns after alert without reset | PASS |
| 5 | Reset while machine is running | PASS |
| 6 | Reset while machine is stopped | PASS |

**Current result: 6/6 software-behavior tests passed.**

These tests validate the decision logic under simulated conditions. Physical sensor reliability has not yet been validated.

## Proposed Hardware

The V1 prototype is currently designed around:

- ESP32-WROOM-32 DevKit V1
- A3144 Hall-effect sensor
- Neodymium magnet
- H2010 optical photointerrupter
- IR LED + phototransistor for alternative optical experiments
- Active buzzer
- Reset push button
- Breadboard and supporting components

### Proposed Architecture

```text
Handwheel movement                  Lower-thread movement
       │                                    │
       ▼                                    ▼
Hall-effect sensor                  Optical/IR sensor
       │                                    │
       └──────────────┬─────────────────────┘
                      ▼
                    ESP32
                      │
               State-machine logic
                      │
               ┌──────┴──────┐
               ▼             ▼
             Buzzer      Reset button
```

## Why Optical Thread Sensing?

Several approaches were considered, including:

- bobbin rotation sensing;
- remaining-thread detection;
- mechanical thread sensing;
- tension sensing;
- optical thread sensing.

For V1, optical/IR sensing was selected as the primary experimental direction because it could potentially detect actual lower-thread movement without physically interfering with the thread.

This still needs to be experimentally validated against factors such as thread colour, thickness, sewing speed, ambient light, vibration and lint.

## Repository Structure

```text
smart-bobbin-monitor/
│
├── README.md
├── src/
│   └── bobbin_project.c
├── tests/
│   └── bobbin_test.c
├── docs/
└── images/
```

## Current Progress

- [x] Problem definition
- [x] V1 requirements
- [x] State-machine design
- [x] C implementation
- [x] Desktop simulation
- [x] Six software test cases
- [x] Sensor/mechanism research
- [x] Preliminary hardware architecture
- [ ] ESP32 implementation
- [ ] Hall-sensor testing
- [ ] Optical thread-sensing experiment
- [ ] Buzzer/reset integration
- [ ] Sewing-machine prototype
- [ ] Physical fault testing

## Next Steps

The next development phase focuses on hardware prototyping.

The first major experiment will determine whether optical sensing can reliably distinguish **moving lower thread from absent/stationary lower thread**.

If successful, the simulated software inputs will be replaced with actual sensor readings on the ESP32 before testing the complete system on a sewing machine.

## Project Goal

The goal is not simply to build an empty-bobbin detector.

The goal is to develop an affordable system capable of identifying **loss of lower-thread delivery while sewing**, regardless of whether the underlying cause is an empty bobbin, breakage, tangling or another feeding problem.
