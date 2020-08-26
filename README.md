# OH3AA antenna rotator controller firmware

The OH3AA antenna rotator controller firmware provides a general-purpose text-based network protocol
for controlling an azimuth antenna rotator. The rotator position is determined using an absolute
magnetic shaft encoder, which is a very accurate and durable sensor.

## Hardware requirements

* Board: Arduino Due (32-bit)
* Network shield: WIZnet W5100 Ethernet shield

### W5100 Ethernet shield reset

The WIZnet W5100 Ethernet shield does not receive required signal in its RESET pin when Arduino Due
is powered on, so it is necessary to reset W5100 shield manually. However, W5100 RESET pins are
connected to the Arduino Due RESET pins both via the standard Arduino RESET pin and the extended SPI
RESET pin. To avoid making Arduino Due reset at the same time, these two pins need to be disconnected.

1. Bend W5100 RESET pin so that it is not connected to Arduino Due RESET pin
2. Bend Arduino Due extended SPI RESET pin so that it is not connected to W5100
3. Connect a jumper wire from pin 30 to W5100 RESET pin

### Sensors

* Rotator position input: PWM sensing via TC0 and channel 0 = pin 2
  * Sensor: US Digital MA3 - Miniature Absolute Magnetic Shaft Encoder
    * MA3 output needs 5V <-> 3.3V voltage level conversion for Arduino Due

## Rotator control

Connect all pins via opto-isolators and relays to protect Arduino.

* CW/CCW direction control: GPIO outputs, pins 24 and 25
* Azimuth < 0 or > 360 signals: GPIO inputs, pins 26 and 27
* Minimum/maximum azimuth signals: GPIO inputs, pins 28 and 29
* Speed control (optional): Analog voltage from 0.55V to 2.75V (100 steps) via DAC1 = pin 67

## Build

```bash
platformio run
```

## Flash

```bash
platformio run --target upload
```

## TODO

* PwmDataReader: Implement scale correctly according to MA3 sensor spec: 1 µs = 0 deg, 1023 µs = 359.65 deg
* PwmDataReader: Implement averaging over N values to reduce noise
* Possible filtering of interrupts from threshold/limit inputs

## Notes

Analog output control and voltage levels on Arduino Due:

* https://create.arduino.cc/projecthub/ArduPic/how-to-modify-analog-output-range-of-arduino-due-6edfb5
* https://arduino.stackexchange.com/questions/9479/arduino-due-analog-output-has-500mv-offset
* https://forum.arduino.cc/index.php?topic=509961.0
* https://forum.arduino.cc/index.php?topic=154965.0
