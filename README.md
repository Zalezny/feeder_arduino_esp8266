![image](https://github.com/Zalezny/feeder_arduino_esp8266/assets/65240240/ffb208e6-5b48-4f92-b3fc-5efbf0e083b2)


# Automated Feeder

An Automated Feeder designed for guinea pigs (and adaptable for other animals). It operates using an ESP8266 microcontroller. As part of the project, a rotating screw mechanism was 3D printed to dispense the feed.

## Scheme (Eagle)

![black](https://github.com/Zalezny/feeder_arduino_esp8266/assets/65240240/5b772d63-75e4-44fe-bfaf-e5e9fa2c05c6)


## Board (Eagle)

![board](https://github.com/Zalezny/feeder_arduino_esp8266/assets/65240240/3195864f-eddc-44bf-b9d3-282b425752e4)


## Parts

- Processor: ESP8266 WiFi
- LCD 4x20 green LCD-01136
- Clock RTC: MSX RTC DS1307
- Stepper motor: 28BYJ-48
- Power: Impulse Power Supply 3-12V/1.2A (for this project 5V)
- Mechanism: Rotating Screw (Printer 3D)

## How it works 

When you connect the feeder to power, the program will start counting down, and when it reaches zero, the feeder will begin dispensing feed.

ESP8266 is configured to REST Server with endpoints like /setTimer - set hour, minute, second or /setState - set pause, resume or restart

<img src="https://github.com/Zalezny/feeder_arduino_esp8266/assets/65240240/17da5cd9-c66c-420f-b6f7-13a72638102a" alt="Pierwszy obraz" style="width: 400px; height: auto;"/>
<img src="https://github.com/Zalezny/feeder_arduino_esp8266/assets/65240240/45d54019-48e5-4958-9c4b-2f9915985044" alt="Drugi obraz" style="width: 400px; height: auto;"/>
<img src="https://github.com/Zalezny/feeder_arduino_esp8266/assets/65240240/edbf91a0-5b47-423f-b33f-0f3e47097460" alt="Drugi obraz" style="width: 400px; height: auto;"/>
