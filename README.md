# Auto reverse for Precious Plastic Shredder!

Hi all 🖐,
Documentation to allow shredder to detect jams and autoreverse.
More background info about the project can be found here: [Forum topic](https://davehakkens.nl/community/forums/topic/v4-shredder-arduino-code/)👈for now we have:
- Arduino code
- Flowchart
- Wiring diagram

## Principle of operation

Use a microncontroller and hall effect sensor to measure motor current consumption and control motor rotation through relays.
It uses a 3 position switch to enable shredding and manual reverse.

## Arduino
Install required libraries:

- LiquidCrystal I2C
- Wire

Both can be installed using Ardunio Library Manager (available from IDE version 1.6.2)
Sketch -> Include Library -> Manage Libraries ...

Load shredderControl/shredderControl.ino to microcontroler

## WIP

Things that are still on the TODO list:

- [ ] Circuit diagram & PCB
- [ ] Allow configuring parameters from GUI
- [ ] List of components
