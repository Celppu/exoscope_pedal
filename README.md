# Exoscope pedal
Pedal copy of existing exoscope pedal. Used for surgeon training simulator


# Hardware

Uses esp32-s3 for BLE keyboard. Basic buttons and joystick

## Button Mapping
Gpio pins 5 - 18

## Joystick Mapping
first pins for four directions

Gpio 5 - Forward - Q
Gpio 6 - Backward - W
Gpio 7 - Left - E
Gpio 8 - Right - R

| Gpio | Direction | Key |
| --- | --- | --- |
| 5 | Forward | Q |
| 6 | Backward | W |
| 7 | Left | E |
| 8 | Right | R |


# Software

Requirements
- PlatformIO
- ESP32-S3
- BLE Keyboard

