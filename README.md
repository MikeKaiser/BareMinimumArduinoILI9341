# BareMinimumArduinoILI9341
Bare minimum code to initialise a ILI9341 LCD panel with SD Card slot


The Adafruit LCD drivers are great to use but a mess under the hood.
I wanted to use an ILI9341 LCD panel with Touch screen and SD Card slot on an Arduino Mega as a small CNC controller as the RAMPS boards are very large.
The problem was the LCD panel was targeted at the Arduino Uno and didn't work on the Mega out of the box even after fiddling with the pin assignments.
I also was not happy at the number of dependencies I needed download to get the LCD to work (the Adafruit library has been split up into many sub projects) so I decided to go back to basics and initialise the LCD myself.

