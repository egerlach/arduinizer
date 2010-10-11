/* -*- tab-width: 4 -*-

/*
 * program.c
 * =========
 * Place your Arduino-based code in this file.
 *
 *  Created on: Sep 2, 2010
 *      Author: amackie
 *
 *  Modified on: Oct 11, 2010
 *      By: Jonathan Lamothe <jonathan@jlamothe.net>
 */

// Use the Arduino & SOONCon libraries.
#include "SoonCon2010Badge.h"

byte redTime;	// The red colour intensity of upper LEDs.
byte greenTime;	// The green colour intensity of upper LEDs.
byte blueTime;	// The blue colour intensity of upper LEDs.

byte colourMode;	// Which colour(s) are lit on lower LEDs.
unsigned int colourChange;	// Keeps track of when the lower LED colour should change.

// Software PWM pulse which sets the shade of each of
// red, green and blue.  Call repeatedly to maintain the colour needed.
// If redTime is 0, the LED will have no red, whereas a value of 255 is full red.
// Basically, this lights each colour for a percentage of its value for a
// certain amount of time, then blanks it for the rest, but faster than the
// eye can see (unless you're Superman, I guess)
// Same for greenTime and blueTime.
void pulse()
{
  int pulse = 0;				// Beginning of pulse.
  for(pulse = 0; pulse <= 255; pulse++)
	{
	  digitalWrite(LIGHT_1_RED,   redTime   >= pulse ? HIGH : LOW);
	  digitalWrite(LIGHT_1_GREEN, greenTime >= pulse ? HIGH : LOW);
	  digitalWrite(LIGHT_1_BLUE,  blueTime  >= pulse ? HIGH : LOW);
	  delayMicroseconds(10);
	}
}

// Cycles between black->red->green->yellow->blue->purple->cyan->white
// by incrementing a value between 0 and 7 and checking the flag of each.
void setColourMode()
{
	if(millis() / 1000 != colourChange)	// Do this once per second.
	{	// 1 second has passed
		colourChange = millis() / 1000;	// Record our current time so we don't do this too often.
		colourMode = (colourMode+1) % 8; // Change the colour.
	}

	// Display the colour based on the value of colourMode.
	digitalWrite(LIGHT_2_RED,   colourMode & 1 ? HIGH : LOW);
	digitalWrite(LIGHT_2_GREEN, colourMode & 2 ? HIGH : LOW);
	digitalWrite(LIGHT_2_BLUE,  colourMode & 4 ? HIGH : LOW);
}

// Arduino-emulating function, called once at startup.
void setup()
{
	// Tell the microprocessor we want to use these pins as output.
	pinMode(LIGHT_1_RED,   OUTPUT);
	pinMode(LIGHT_1_GREEN, OUTPUT);
	pinMode(LIGHT_1_BLUE,  OUTPUT);

	pinMode(LIGHT_2_RED,   OUTPUT);
	pinMode(LIGHT_2_GREEN, OUTPUT);
	pinMode(LIGHT_2_BLUE,  OUTPUT);

	// Init our variables.
	// Keep the red/green/blue pulse times evenly out of sync.
	redTime = 170;
	greenTime = 85;
	blueTime = 0;

	colourMode = 0;
	colourChange = millis() / 1000; // Set the change-time to the current time.
}
// Arduino-emulating function, called repeatedly.
void loop()
{
	// Shift the colour of the top LEDs.
	redTime++;
	greenTime++;
	blueTime++;

	// Pulse 10 times (~100 microseconds)
	byte lightTime=0;
	for(lightTime=0;lightTime<10;lightTime++)
	{
		pulse();
		setColourMode();	// Check if the bottom LED colour needs updating.
	}
}
