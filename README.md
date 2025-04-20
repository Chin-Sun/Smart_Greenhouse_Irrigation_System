[![Smart Greenhouse Irrigration System](https://res.cloudinary.com/marcomontalbano/image/upload/v1654682824/video_to_markdown/images/youtube--n6UJMdB_ueU-c05b58ac6eb4c4700831b2b3070cd403.jpg)](https://youtu.be/n6UJMdB_ueU "Smart Greenhouse Irrigration System")

# 🌿 Smart Greenhouse Irrigation System

📅 **Project Duration:** May 2020 – September 2020  
🔗 **GitHub Repository:** [Smart_Greenhouse_Irrigation_System](https://github.com/Chin-Sun/Smart_Greenhouse_Irrigation_System)

## 📌 Overview

This project presents a smart irrigation control system designed for greenhouse environments. The system monitors environmental data in real time and automates irrigation decisions accordingly. It integrates hardware design, embedded system programming, and user interface development to deliver a fully functional IoT-based solution.

---

## 🧩 Key Features

- **Automated irrigation** based on soil moisture, temperature, and humidity.
- **Real-time monitoring** and visualization of environmental data.
- **User-friendly interface** for manual control and parameter adjustment.
- Modular design enabling **easy scalability and maintenance**.

---

## 🔧 Hardware Design

- **Microcontroller & Components:**
  - Microcontroller (e.g., STC or STM32 series)
  - Soil moisture sensors
  - Temperature and humidity sensors (e.g., DHT11/DHT22)
  - Solenoid valves and relay modules for irrigation control
  - Power modules and signal converters

- **PCB Design:**
  - Designed and fabricated custom PCBs to connect sensors, relays, and display units.
  - Ensured stability and electrical isolation for safe operation.

- **Signal Flow:**
  - Sensors → MCU ADC/IO pins → Data Processing → Relay Activation (Water Pump/Valve)

---

## 💻 Software Development

### Embedded System

- **Language:** C  
- **Tools:** Keil uVision  
- **Functionality Implemented:**
  - Sensor data acquisition and filtering
  - Threshold-based decision logic for automatic irrigation
  - Signal conditioning for control signals
  - UART communication with the PC interface

### User Interface

- **Platform:** Kingview SCADA + C# Integration  
- **Key Functions:**
  - Real-time display of temperature, humidity, and soil moisture
  - Manual control of irrigation via GUI
  - Logging and visual alerts for abnormal conditions
  - Serial communication with microcontroller (RS232/USB)

---

## 🛠️ Development Process

1. **Requirement Analysis:** Defined environmental parameters and system response logic  
2. **Circuit Design:** Created schematic and PCB layout; selected components  
3. **Sensor Testing:** Validated sensor accuracy and signal reliability  
4. **Firmware Development:** Programmed sensor readings, control logic, and serial communication  
5. **Interface Design:** Built GUI for user interaction, data logging, and remote control  
6. **System Integration:** Connected all modules, tested full workflow, and optimized performance  
7. **Final Testing:** Simulated different greenhouse conditions to verify reliability

---

## 📚 Core Skills Demonstrated

- **Embedded systems programming** in C  
- **Hardware system design** including PCB layout and electrical interface  
- **Real-time data processing** and automation  
- **HMI (Human-Machine Interface)** development with Kingview and C#  
- **Serial communication** protocol integration and debugging  
- **IoT-oriented design thinking** with scalability and maintainability in mind

---
