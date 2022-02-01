# Stone Crushing Machine IOT CODE

## Developed by SDA

This project is an IOT based project, it can contol your stone crushing machine at a distance where internet is available.

Steps to Start a project:

1: Connect a wire of a Machine with 220V
2: Rename your wifi/Hotspot's configuration to default SSID and PWD (Given Below)
3: Intsall Application and Login with Google
4: Enter Device ID (Given Below) to attched them with system (you can set and rename your device names)
5: Wait to a machine to connect with wifi/Hotspot
6: Ready to control using GUI switches


To configure a project you need to change following things:

*Information:*
- System can operate upto 4 devices (Conveyour, IR Sensor 1, IR Sensor 2, Grinder)
- Each machine can attached with only one Application at a time otherwise you will get an Error (ERROR: Machine Already Attached)
- Node MCU requires 5v to operate in VIN pin
- Each machine has own unique ID given below:
  Machine one: 786786 (you can increment manually for more machines)


*For Hardware*
- Can operate between 200V to 260V (220V Recommended)
- Wifi or Hotspost with following default credentials:
  Wifi SSID: Stone
  Wifi PWD: Stone123
  
 
 *For Code Configuration:*
- Your Wifi SSID
- Your Wifi Password
- Device IDs


*Caution:*
- If you want re-upload code then must eject 5v vcc connector(bridge/fuse) from relay module
- Do not connect with more than 260 voltages

*FAQ and Solutions:*
- if restarting again and again: change power/wire or connect power to both Node MCU (5V 2A Charger) + 220V to Power Supply  
- if slow response: change your internet connection
