# quickstart
* Buy materials listed on the Bill of Materials
* Follow the HelicopterWorkshopHandout.pdf
* Make an RC helicopter fly in a box-shape automatically

# Introduction
This was a four-hour workshop designed for high school students to go
from not knowing how a breadboard works to coding open-loop control
of RC helicopters using Arduino.

During the summer of 2014 we partnered with Envision for their National
Youth Leadership Forum (NYLF): Careers in Engineering & Technology to
run this helicopter workshop for roughly 1500 students across the US.

# Bill of Materials
Listed by product name, followed by example US source and cost in USD

1. Arduino microprocessor (Uno) *
( http://www.robotmesh.com/arduino-uno-rev3 )
$25
2. USB cable 
(http://www.robotmesh.com/sparkfun-cerberus-usb-cable-6ft)
$2.55
1. Helicopter and controller
( http://www.banggood.com/Wholesale-Genuine-Syma-S107-S107G-3-Channel-Infrared-Remote-Control-Helicopter-With-GYRO-p-36759.html?currency=USD )
!! Make sure to select US warehouse, which is $22, instead of China warehouse, which is $20, if you are short on time !!
$22 
2. Breadboard (mini style)
( http://www.robotmesh.com/400-tie-point-interlocking-solderless-breadboard)
$2.90
3. IR LED array 
??
4. Amplifier 
5. LED light and resistor (between 499 ohms and 1000 ohms inclusive should be
fine) 
(http://www.robotmesh.com/5mm-led-pack-6-pcs)
$0.50
6. AA Battery holder (holds six batteries)
7. 9V battery snap 
9. AA Batteries
10. Small pushbutton (2 lead)
11. A small fan 
12. A few male-male breadboard jumper wires 

* We selected the Uno as it is the default chosen in the Arduino IDE so
we do not have to have students select the correct board in Arduino.
However any other Arduino microcontroller should work well.

# Lesson Learned, Variations Tried
Don't skip the helicopter flying steps -- they are spaced so that the
helicopter batteries have time to charge

Yes, they should be missing a battery and a controller at the beginning

Try to explain to students their is a big difference between working
together as a group with one person at doing the physical work and
everyone actually sitting in front of the computer or breadboard and
understanding where to hit upload etc. 

The advanced groups may actually have a harder time because they will
try to skip steps

Make sure that students get keyboard flying working before trying to
finish the box-shape program. This way they can check their circuit works.
Make sure they are able to get the IR LED blinking before that. That way
they can make sure their circuit is wired up correctly.

## Operationally
Make sure to have extras of everything. Things will break.
* Perhaps create a crash kit for each room?

# Debugging 
## Common Problems
Are Arduino and battery pack grounds connected?

If the serial monitor says the button is pressed in an infinite loop,
that is because something is shorting pin 8 to ground all the time.

For the IR, an older dumb phone will show the IR LEDs brightly.
Additionally, you can plug and unplug the IR LEDs while staring at it to
verify that they are on.

## Trickier Problems
Check if the battery pack is dead by connecting the fan directly across
the leads

If they are having problems uploading,
* Check if "Arduino Uno" is selected in Tools > Boards
* Is the serial port greyed out? If so 
** Disconnect the circuit from the Arduino and try uploading again. If
this works, the problem is a circuit that is shorting the Arduino out
(drawing so much current the Arduino cannot power itself)
** Disconnect and reconnect the Arduino and try uploading again. If this
works, it was a serial port issue.
** In windows, double-check that the Arduino shows up in Start > Devices
and Printers
** Grab a fresh Arduino and try reuploading
** Grab a fresh cable and try reuploading 

Windows 8 appears to occasionally have unsolvable driver issues.

# Todo
## Add instructor notes and techniques
Remind instructors to let students debug their own
circuits.

* First, determine and tell them there is something wrong with their circuits
* If you have time, sit down with them and with a pen check off each connection against the handout
* If they are still not finding it, gently ask them "what is __
  connected to?"

For the PWM example,
* "Half speed all the time sounds like..."
* "Full speed... half the time"
* This should only be done if you have at least five minutes to sit with
  the group and guide them

For the alternately blinking LED example
* "What is the LED in the circuit connected to?"
* "What is next to that?"
* "Perhaps you want to use more than just pin 13 on the Arduino"


## Add optional exercises for students to complete

While on the LED blinking step
* Make the LED blink faster and faster, then slower and slower, then
  speed up again, slow down again, repeatedly

While the helicopter is
charging and they are working on the box or figure-eight shape:
* Make the button blink faster each time the pushbutton is pressed
* Add a key to do something different with the helicopter (just as the
  WASD UJ and 0 keys do things)
* Make the button blink a delay specified by what you type into the
  serial monitor

## Handout changes
Include a link on the handout to this repository

Explain transistors as electrical switches

Explain setup() and loop()

Give an estimated amount of time for each step

## Longer-term
Create website and starter kit pack for students or instructors to
continue playing / learning at home

Partner with distributor to distribute starter kit
