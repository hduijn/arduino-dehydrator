# arduino-dehydrator
Dehydrator project based on Arduino

## Author

Author Name <hugovanduijn@gmail.com>

## Synopsis

Dehydrator project using Arduino for creating custom dehydrators. 

## Description

Using a rotary encoder it is possible to set a temperature between 25-75 degrees ( this can be modified in code ). When the switch of the rotary encoder is pressed then the program wil go in: "on" mode, this turns on the relays for the fans and starts to check the temperature and moisture using a KY-015 Sensor ( DHT11). Temperature and Moisture is displayed on the LCD and when the temperature is lower than the set temperature then the relay for the Heater will be turned on. 

If the heater is turned of then there will be a counter which will make sure there is a timeout of about 10 seconds before the heater can be turned on again, this is to avoid quick switching. 

2 x 0.47uF Capacitors are used on the CLK and DT pins to the ground for the rotary encoder, this removes debouncing and makes it possible to smoothly set the temperature. 

Key components used: 
- Solid State Relay : Omron G3MB-202P 2A ( can be switched by arduino without external power )
- PTC (Air) Heater Element : 100-300W 
- Arduino Uno
- Perf / prototype board
- KY-040 Rotary encoder
- KY-015 Temp + Moist sensor
- 1602 16x2 HD44780 Character LCD /w IIC/I2C Serial Interface 
- 60mm fans for airflow
- skilled woodworker / case builder


## Code

Code of the project can be found in arduinocode folder, code is still a bit messy but working. 

## Fritzing

Schema with parts is created in Fritzing, sketch and parts can be found in Fritzing folder. In the Fritzing/exports folder the dehydrator schema can be found in png format.

## 3D models 

3D models of LCD case in STL_3D folder, Files are modified from http://www.thingiverse.com/thing:30073
I've printed them with .2mm layer thickness and the case was a bit to small to fit rotary encoder and display properly. 
My advise is to print is slightly larger or just don't use the files for printing but only for inspiration.

## Parts

Parts can be purchased from http://www.makerwebshop.com see http://www.makerwebshop.com/kits/components-required-for-dehydrator-arduino-project for a link with almost all related parts. 
