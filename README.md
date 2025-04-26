# Water Level Monitoring System - Raspberry Pi

## Description  
A precise water level monitoring system using Raspberry Pi with:  
- **MS5837 Pressure Sensor**  
- **I2C LCD Display**  
- **Visual LED Indicators**  
- **Audible Alarm System**  

## Features  
✔ Real-time water level measurement  
✔ 3-color LED status indication (Red/Yellow/Green)  
✔ Programmable alarm threshold  
✔ Single-button control interface  

## Hardware Components  
| Component | Quantity |  
|-----------|----------|  
| Raspberry Pi | 1 |  
| MS5837 Sensor | 1 |  
| 16x2 I2C LCD | 1 |  
| LEDs (R/Y/G) | 3 |  
| Buzzer | 1 |  
| Push Button | 1 |  
| 220Ω Resistors | 3 |  
| 10KΩ Resistor | 1 |  

## Installation  
### Prerequisites  
```bash  
sudo apt-get update  
sudo apt-get install wiringpi
```
## Library Setup
```
git clone https://AmlBanna/YourRepo/water-level-system.git
cd water-level-system
make
```
## 🚀 Usage
```
sudo ./water_level_monitor
```
## 🛠️ Configuration

Modify these parameters in config.h:

```
#define TANK_HEIGHT 50.0    // Tank height in cm
#define ALARM_THRESHOLD 90  // Alarm trigger percentage
#define LCD_ADDRESS 0x27    // I2C address for LCD
```
## 📊 System Architecture


![System Architecture Diagram](images/diagram_water_lvl.png)

## 📜 License
MIT

## 🔍 Detailed Technical Specifications

### Pin Configuration
| Raspberry Pi Pin | Component      | Connection               |
|------------------|----------------|--------------------------|
| 3.3V             | MS5837         | VCC                      |
| GND              | MS5837         | GND                      |
| GPIO2 (SDA)      | MS5837         | SDA                      |
| GPIO3 (SCL)      | MS5837         | SCL                      |
| GPIO4            | Green LED      | Through 220Ω resistor    |
| GPIO5            | Yellow LED     | Through 220Ω resistor    |
| GPIO6            | Red LED        | Through 220Ω resistor    |
| GPIO7            | Push Button    | With 10KΩ pull-down      |
| GPIO8            | Buzzer         | Positive terminal        |




## Calibration Procedure
Power on the system with empty tank

Run calibration script:

```
sudo ./calibrate_sensor
```
Follow on-screen instructions

## Troubleshooting Guide
Issue	Solution
LCD not displaying	Check I2C address and connections
Sensor not detected	Verify I2C is enabled in raspi-config
Buzzer not working	Check polarity and GPIO configuration


## 📈 Future Enhancements
Mobile app integration

Cloud data logging

Automated valve control

Low-power mode
