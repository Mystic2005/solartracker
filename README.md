# Solar Tracker – Arduino UNO + ESP8266

Project for the ED course: an automatic light-tracking system with live data display on a web page.

## Description:

The Arduino UNO scans four angles {15°, 35°, 75°, 110°}, reads the light intensity at each position, and stops at the angle with the highest intensity for 10 seconds before restarting the cycle. Concurrently, it reads the temperature using a DS18B20 sensor. A button connected to pin D2 starts and stops the system. Data is transmitted via serial communication to an ESP8266, which creates its own Wi-Fi network and hosts a web page at 192.168.4.1 displaying real-time values.

## Components and Connections

### Arduino UNO

* LDR (photoresistor) on A0 – voltage divider with 10k resistor to GND
* DS18B20 (temperature sensor) on D4 – 4.7k pull-up resistor to 5V
* SG90 Servo on D9 – direct signal, 5V power from UNO
* Button on D2 – connected between D2 and GND, using INPUT_PULLUP
* TX line to ESP via 1k/2.2k voltage divider – required because UNO operates at 5V and ESP at 3.3V

### ESP8266 NodeMCU 12E

* D5 receives serial data from UNO via the aforementioned voltage divider
* Common GND with UNO
* Creates a "SolarTracker" WiFi Access Point; hosts a web page on port 80

## Bonus Features

* 4 interactive components: LDR (photoresistor) + DS18B20 (temperature sensor) + servo + button (digital input)
* Live web page with automatic refresh every 2 seconds

`delayCuButon()` replaces standard `delay()` calls to ensure button presses aren't missed during scanning

Power optimization suggestion: replace `delayCuButon(10000)` with `LowPower.powerDown(SLEEP_8S)`

Decision: left unimplemented so the system can still be shut down via the button during the pause between scans

Smooth servo movement: 1 degree per 10ms

Practical application: orienting a real photovoltaic panel throughout the day without manual intervention

## Improvements

The project is a prototype; it could easily be expanded with additional hardware

Positioning two LDRs at a 90-degree angle to each other would determine light direction more precisely than scanning at fixed angles; alternatively, adding a second servo on the vertical axis would enable full 2D tracking.

## Attached files

* `uno.ino` - Arduino UNO code
* `esp.ino` - ESP8266 code
* `pozacircuit_sus.jpeg`, `pozacircuit_frontal.jpeg` - photos of the assembly
* `solartracker.mp4` - video demonstrating functionality and explaining the circuit

## VIDEO LINK : https://youtu.be/cfiTsZvg8xs?si=jbbps0h_lCL57tfR
