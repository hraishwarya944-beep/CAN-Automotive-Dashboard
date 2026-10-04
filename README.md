# CAN-Based Automotive Dashboard using PIC18F4580

## 📌 Project Overview

The **CAN-Based Automotive Dashboard** is a multi-node embedded system developed using **three PIC18F4580 microcontrollers**.

The project demonstrates communication between multiple Electronic Control Units (ECUs) using the **Controller Area Network (CAN) protocol**.

Two ECUs collect vehicle-related information such as:

* Vehicle Speed
* Gear Position
* Engine RPM
* Left/Right Turn Indicator

The collected information is transmitted through the **CAN bus** to a third ECU, which works as the **instrument cluster/dashboard**.

The dashboard ECU receives the CAN messages and displays the vehicle information on a **16x2 Character LCD (CLCD)**.

The firmware is developed using **Embedded C, MPLAB X IDE and MPLAB XC8 compiler**.

---

## 🎯 Objective

The main objective of this project is to understand and implement **CAN-based communication between multiple microcontrollers**.

The project demonstrates how different ECUs can collect individual vehicle parameters and communicate them to a central dashboard.

### Main objectives

* Implement CAN communication between multiple PIC18F4580 nodes
* Read analog values using ADC
* Interface a matrix keypad
* Interface digital switches
* Control seven-segment displays
* Interface a 16x2 CLCD
* Transmit and receive CAN messages
* Design message identifiers for different vehicle parameters
* Implement a multi-ECU automotive architecture

---

# 🚗 System Architecture

The system consists of three ECUs.

```text
                  CAN BUS
        CANH ============================= CANH
        CANL ============================= CANL
             |                    |
             |                    |
             v                    v

     +---------------+     +---------------+
     |     ECU1      |     |     ECU2      |
     | Speed + Gear  |     | RPM +         |
     |               |     | Indicator     |
     +-------+-------+     +-------+-------+
             |                     |
             | CAN                 | CAN
             |                     |
             +----------+----------+
                        |
                        v
                +---------------+
                |     ECU3      |
                |   Dashboard   |
                |               |
                |     CLCD      |
                +---------------+
```

### ECU1

Responsible for:

* Speed
* Gear selection
* ADC
* Matrix keypad
* CAN transmission

### ECU2

Responsible for:

* Engine RPM
* Turn indicators
* ADC
* Seven-segment display
* Digital keypad
* CAN transmission

### ECU3

Responsible for:

* CAN reception
* Processing received messages
* Displaying vehicle information on CLCD

The three-node architecture and the corresponding CAN TX/RX message flow are defined in the project documentation.

---

# 🧩 ECU1 – Speed and Gear

ECU1 is responsible for measuring vehicle speed and selecting the gear.

## Speed

A potentiometer connected to **ADC channel AN4** is used as an analog input.

The 10-bit ADC value is converted into a speed value between:

```text
0 – 99
```

The speed is then converted into ASCII characters and transmitted through CAN.

### Speed CAN ID

```text
0x10
```

---

## Gear

A matrix keypad is used for gear selection.

The available gear states are:

```text
ON
GN
G1
G2
G3
G4
G5
GR
```

The keypad uses state-change detection.

```text
SW1 → Gear Up
SW2 → Gear Down
```

The gear value is transmitted through CAN using:

```text
CAN ID = 0x20
```

---

# 🧩 ECU2 – RPM and Indicators

ECU2 is responsible for engine RPM and turn indicators.

## Engine RPM

A potentiometer connected to **AN4** is used as the analog input.

The ADC value is mapped to:

```text
0 – 6000 RPM
```

The RPM value is displayed locally using a **4-digit seven-segment display**.

The RPM information is also transmitted through CAN.

```text
CAN ID = 0x30
```

---

## Turn Indicators

Digital switches are used to select:

```text
Left
Right
OFF
```

The indicator states are represented as:

```text
0 → OFF
1 → LEFT
2 → RIGHT
```

The selected indicator is transmitted using:

```text
CAN ID = 0x50
```

LEDs are used to represent the indicator status.

---

# 🖥️ ECU3 – Dashboard / Instrument Cluster

ECU3 acts as the main dashboard.

It receives CAN frames from ECU1 and ECU2.

The received CAN message is identified using its CAN ID.

The corresponding information is then displayed on the 16x2 CLCD.

### Dashboard display

Example:

```text
Speed Gear RPM  In
42    G3   3150 ->
```

The dashboard displays:

* Speed
* Gear
* RPM
* Turn indicator

---

# 📡 CAN Message Map

The project uses standard **11-bit CAN identifiers**.

| Message            | CAN ID | Sender   | Receiver | Data               |
| ------------------ | -----: | -------- | -------- | ------------------ |
| Speed              | `0x10` | ECU1     | ECU3     | 2 ASCII digits     |
| Gear               | `0x20` | ECU1     | ECU3     | 2 ASCII characters |
| RPM                | `0x30` | ECU2     | ECU3     | 4 bytes            |
| Engine Temperature | `0x40` | Reserved | Reserved | Future use         |
| Indicator          | `0x50` | ECU2     | ECU3     | 0/1/2              |

The project uses a common `msg_id.h` so that all three ECUs use the same message identifiers.

---

# ⚙️ CAN Configuration

The PIC18F4580's built-in **ECAN module** is used for CAN communication.

### CAN configuration

| Parameter       | Configuration     |
| --------------- | ----------------- |
| Microcontroller | PIC18F4580        |
| CAN Module      | ECAN              |
| ECAN Mode       | Mode 0            |
| Identifier      | Standard 11-bit   |
| CAN TX          | RB2               |
| CAN RX          | RB3               |
| Receive Filter  | Accept all        |
| CAN Transceiver | MCP2551 / TJA1050 |

The CAN driver provides the following main APIs:

```c
init_can()
can_transmit()
can_receive()
```

All three nodes use the same CAN bit-timing configuration so they can communicate correctly on the same bus.

---

# 🔌 Hardware Requirements

### Microcontrollers

* 3 × PIC18F4580

### CAN

* 3 × CAN transceiver
* MCP2551 / TJA1050 or equivalent
* CANH
* CANL
* 120 Ω termination resistors

### Displays

* 16x2 CLCD
* 4-digit seven-segment display

### Inputs

* Potentiometers
* Matrix keypad
* Digital push buttons

### Outputs

* Indicator LEDs

### Clock

* 20 MHz crystal oscillator

---

# 🔗 CAN Bus Connection

The three ECUs are connected through a common CAN bus.

```text
ECU1                  ECU2                  ECU3
 |                      |                      |
 |                      |                      |
CANH ------------------ CANH ---------------- CANH
CANL ------------------ CANL ---------------- CANL
 |                      |                      |
GND ------------------- GND ----------------- GND
```

A **120 Ω termination resistor** is used at each end of the CAN bus.

---

# 📍 Pin Mapping

## CAN

| Function | ECU1 | ECU2 | ECU3 |
| -------- | ---- | ---- | ---- |
| CAN TX   | RB2  | RB2  | RB2  |
| CAN RX   | RB3  | RB3  | RB3  |

## ADC

| Function     | ECU1 | ECU2 |
| ------------ | ---- | ---- |
| Analog input | AN4  | AN4  |

## CLCD

| CLCD       | ECU1  | ECU3  |
| ---------- | ----- | ----- |
| Data D0-D7 | PORTD | PORTD |
| RS         | RC1   | RC1   |
| RW         | RC0   | RC0   |
| EN         | RC2   | RC2   |

## Keypads

| Function | ECU1          | ECU2           |
| -------- | ------------- | -------------- |
| Keypad   | Matrix keypad | Digital keypad |

## Display

| Function         | ECU2  |
| ---------------- | ----- |
| SSD data         | PORTD |
| SSD digit select | PORTA |

---

# 🔄 Complete Working Flow

```text
                  ECU1
                   |
          +--------+--------+
          |                 |
       ADC AN4        Matrix Keypad
          |                 |
        Speed              Gear
          |                 |
          +--------+--------+
                   |
                   v
              CAN TX
            ID 0x10/0x20
                   |
                   |
================ CAN BUS ================
                   |
                   v
                  ECU3
                   |
             CAN Reception
                   |
                   v
                CLCD
```

```text
                  ECU2
                   |
          +--------+--------+
          |                 |
       ADC AN4        Digital Keypad
          |                 |
         RPM             Indicator
          |                 |
        SSD LED
          |                 |
          +--------+--------+
                   |
                   v
              CAN TX
            ID 0x30/0x50
                   |
                   |
================ CAN BUS ================
                   |
                   v
                  ECU3
                   |
             CAN Reception
                   |
                   v
                CLCD
```

---

# 🧠 Important Embedded Concepts

## 1. CAN Communication

CAN is a multi-master communication protocol commonly used in automotive systems.

Multiple ECUs can communicate over the same CAN bus.

---

## 2. CAN Identifier

Each CAN message has an identifier.

For example:

```text
0x10 → Speed
0x20 → Gear
0x30 → RPM
0x50 → Indicator
```

ECU3 checks the received identifier and determines which vehicle parameter the message contains.

---

## 3. ADC

The ADC converts the analog voltage from the potentiometer into a digital value.

For example:

```text
Analog Voltage
      ↓
     ADC
      ↓
Digital Value
      ↓
Speed / RPM
```

---

## 4. Matrix Keypad

The matrix keypad is used to select the gear.

State-change detection is used so that one button press produces one gear change instead of continuously changing while the button is held.

---

## 5. Seven-Segment Display

ECU2 uses a four-digit seven-segment display to show RPM.

Digit multiplexing is used to control multiple digits using shared segment lines.

---

## 6. CLCD

ECU3 uses the 16x2 CLCD as the vehicle instrument cluster.

It displays:

```text
Speed
Gear
RPM
Indicator
```

---

# 📂 Repository Structure

```text
CAN-Automotive-Dashboard/
│
├── ECU1.X/
│   ├── main.c
│   ├── adc.c / adc.h
│   ├── can.c / can.h
│   ├── clcd.c / clcd.h
│   ├── matrix_keypad.c / matrix_keypad.h
│   ├── msg_id.h
│   └── nbproject/
│
├── ECU2.X/
│   ├── main.c
│   ├── adc.c / adc.h
│   ├── can.c / can.h
│   ├── digital_keypad.c / digital_keypad.h
│   ├── ssd_display.c / ssd_display.h
│   ├── msg_id.h
│   └── nbproject/
│
├── ECU3.X/
│   ├── main.c
│   ├── can.c / can.h
│   ├── clcd.c / clcd.h
│   ├── msg_id.h
│   └── nbproject/
│
├── firmware/
│   ├── ECU1_Speed_Gear.hex
│   ├── ECU2_RPM_Indicator.hex
│   └── ECU3_Dashboard.hex
│
├── README.md
├── LICENSE
└── .gitignore
```

Each `.X` directory represents an independent MPLAB X project.

---

# 🛠️ Software Requirements

* MPLAB X IDE
* MPLAB XC8 v4.00
* PIC18F4580 Device Family Pack
* Embedded C

---

# 🚀 Build and Flash

### Step 1

Open MPLAB X IDE.

### Step 2

Open:

```text
ECU1.X
ECU2.X
ECU3.X
```

### Step 3

Build each project.

```text
Production → Build Project
```

### Step 4

Program each PIC18F4580 using a suitable programmer.

For example:

```text
PICkit 3
PICkit 4
ICD
```

Each ECU should be programmed with its corresponding firmware.

---

# ▶️ Running the Demo

### Step 1

Connect the three PIC18F4580 boards to the CAN bus.

```text
CANH → CANH
CANL → CANL
GND  → GND
```

Use 120 Ω termination at both ends of the bus.

### Step 2

Program:

```text
ECU1 → Speed + Gear firmware
ECU2 → RPM + Indicator firmware
ECU3 → Dashboard firmware
```

### Step 3

Change the ECU1 potentiometer.

The speed value changes.

### Step 4

Press the ECU1 matrix keypad switches.

The gear changes.

### Step 5

Change the ECU2 potentiometer.

The RPM value changes.

### Step 6

Press the indicator buttons.

The corresponding indicator LEDs blink.

### Step 7

ECU3 receives the CAN messages and displays the information on the CLCD.

---

# 📊 Example

### ECU1

```text
Speed = 42
Gear  = G3
```

ECU1 transmits:

```text
0x10 → "42"
0x20 → "G3"
```

### ECU2

```text
RPM = 3150
Indicator = Right
```

ECU2 transmits:

```text
0x30 → RPM
0x50 → 2
```

### ECU3

Receives the CAN messages and displays:

```text
Speed Gear RPM  In
42    G3   3150 ->
```

---

# ⚠️ Known Issues / Future Improvements

The current project has some areas that can be improved.

### 1. ECU2 CAN Loopback

The current ECU2 CAN configuration uses loopback mode.

For actual multi-node CAN communication, it should use:

```text
Normal CAN Mode
```

instead of loopback mode.

---

### 2. RPM Data Format

The current implementation has a difference between the RPM data representation transmitted by ECU2 and how ECU3 interprets it.

A cleaner implementation would transmit the RPM as ASCII digits or a defined binary format and decode it consistently on ECU3.

---

### 3. Indicator Display

The dashboard can be improved to distinguish:

```text
Left  → <-
Right → ->
OFF   → blank
```

---

### 4. CAN Receive Interrupt

Instead of continuously polling:

```c
can_receive()
```

the project can be improved by implementing **interrupt-based CAN reception**.

---

### 5. CAN Acceptance Filters

Hardware CAN acceptance filters can be configured so that ECU3 accepts only the required message IDs.

---

### 6. Engine Temperature

CAN ID:

```text
0x40
```

can be used in the future for engine temperature.

---

# 📚 Concepts Demonstrated

This project demonstrates practical knowledge of:

* Embedded C
* PIC18F4580
* ECAN module
* CAN protocol
* CAN message identifiers
* CAN data frames
* Multi-ECU architecture
* ADC
* Matrix keypad
* Digital keypad
* CLCD
* Seven-segment display
* GPIO
* State-change detection
* Display multiplexing
* Modular driver development
* Automotive embedded systems

The source project specifically identifies CAN fundamentals, ECAN configuration, multi-ECU message design, ADC scaling, keypad scanning, SSD multiplexing, CLCD interfacing, and modular Embedded C as demonstrated concepts.

---

# 🎓 Learning Outcome

Through this project, I gained practical experience in:

* Designing a multi-node embedded system
* Understanding CAN bus communication
* Configuring the PIC18F4580 ECAN module
* Designing CAN message IDs
* Transmitting and receiving CAN frames
* Reading analog inputs using ADC
* Interfacing matrix and digital keypads
* Driving a seven-segment display
* Interfacing a 16x2 CLCD
* Developing reusable embedded C drivers
* Understanding an automotive ECU architecture



