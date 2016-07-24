# Fan-Controller
Fan controller system for watercooled computer. The board currently monitors the inlet/outlet temperature of each radiator in the system, and then sets the desired fan speed based on the difference between the average loop temperature and the external temperature.
The board also monitors the flow rate in the system, and can set the pump to match a desired flow rate.
The board is also capable of driving a RGB LED strip along with a single color LED strp (UV LEDs in my case).

Lastly, the board has WiFi capabilities through an ESP8266 board. Currently the code is able to interfarce with MSI Afterburner Remote Server and read the CPU & GPU temperatures.
I haven't developed this part much (mostly due to lack of time), but my idea is to have the board send out information such as fan speeds and have the user read out the information on a webserver or an app. This would also allow the user to change the LED colors and brightness.

The board is powered via a single molex connector (AMP MATE-N-LOK 1-480424-0). Obviously, the power used depends on how many fans/which fans are used and the LEDs used. The 12V trace powering the fans/LEDs is 75mils wide, so it is not recommended pulling more than 6Amps through the 12V for a 1oz copper board.
For reference, a Corsar SP120 High-performance PWM fan draws around 0.13 Amps at 100% duty cycle (and 0.2 Amps during power up). A 5050 LED strip with 18 SMDs/ft draws around 0.2 Amps per feet.

### Capabilities ###
* Control and monitor up to 7 PWM fans (can control more if using 4-pin fan cable splitters)
* Control and monitor 1 Pump
* Monitor up to 7 10k Ohms temperature sensors (6 inline sensors and 1 external sensor)
* Monitor 1 flow sensor (the code is desined to use a Koolance INS-FM17N flow meter, but can be easily modified to support different sensors)
* Brightness and Color control of an RGB LED strip and a single color LED strip.
* WiFi (needs to be developed though)

### Configuration ###
Sadly, this project is (currently) not simply "plug-and-play". The user needs to modify a few things to get the code working. 
Most importantly, one needs to modify the first few lines of the esp_main.ino code to add their SSID and MSI afterburner information.
The user also needs to modify the wc_board.ino code so it knows how many fans are being connected and how many radiators/temp sensors the system has.


### To-Do List ###
* Control/monitor throught wifi
* Warn the user if the pump has stopped or if the temperature gets really high. Since I don't have a buzzer, one idea was to have the LEDs flash.
* Add some nice light effects, like fade-in, or rainbow cycle.
* Fan calibration (i.e. find slowest operating speed)
* Detect number of fans and sensors connected, and automatically calibrate the system.
* Fan RPM control. The MAX31790 chips support controlling the fans by setting the desired RPM rate (as opposed to setting the desired duty cycle). I tried this before but for some reason it wasn't working.
* Figure out how to have Windows recognize the board so monitoring softwares (like HWMonitor, Corsair Link, or MSI Afterburner) could report fan speeds and loop temperatures.
* Improve this readme file.

If anyone is interested in helping out with the code, feel free to push commits.

### Repository structure ###

* ./esp_main

Includes the code for the ESP8266 board. For more information about the ESP8266 board and flashing it, please refer to https://github.com/esp8266/Arduino

* ./libraries

Includes the necessary libraries to run the Teensy code. The I2Cdev library included is probably out-of-date, but guaranteed to work with the code in this repo.

* ./PCB

Includes all the schematic files necessary to modify the pcb board along with gerber files for manufacturing. The pcb was generated using Altium Designer 16, however, feel free to email me asking for files for difference programs. I believe Altium allows me to generate source files for many different programs.

* ./wc_board

Includes the main code. I'm using a Teensy 3.2.

### Third-Party Software ###
I've used the MSI Afterburner GUI & Remote Server application for my code. Both are available on the [MSI Afterburner website](http://event.msi.com/vga/afterburner/overview.htm). This code is currently capable of interfacing with MSI Afterburner versions :
* MSI Afterburner GUI Version 4.1.1.7625
* MSI Afterburner Remote Server v1.2.0.0
