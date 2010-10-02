#ifndef __ARDUINO_H__
#define __ARDUINO_H__

#include "LPC13xx.h"                        /* LPC13xx definitions */
#include "math.h"

// Arduino consts
// See arduino.cc
#define INPUT 0
#define OUTPUT 1

#define HIGH 1
#define LOW 0

#define MSBFIRST 1
#define MSBLAST 0

#define byte unsigned char
#define boolean byte
#define false 0
#define true 1
#define word unsigned short

// Serial.print formats.
// See arduino.cc
typedef enum
{
	BYTE,
	BIN,
	OCT,
	DEC,
	HEX
} Format;

// Standard I/O strlen() implementation
// todo: There's probably an STDIO library to pop in here,
// as Arduino has it.
unsigned int strlen(const char* string);

// Arduino functions.
// See arduino.cc
void pinMode(byte pin, byte mode);

void digitalWrite(byte pin, byte value);
byte digitalRead(byte pin);

void delay (uint32_t ms);

word analogRead(byte pin);
void analogWrite(byte pin, byte value);
void tone(byte pin, word frequency);
void noTone(byte pin);

unsigned int micros();
unsigned int millis();

void shiftOut(byte dataPin, byte clockPin, byte bitOrder, byte value);
unsigned long pulseIn(byte pin, byte value, unsigned long timeout);

// Renamed because C does not support overrides or defaults.
unsigned long pulseIn2(byte pin, byte value);
long min(long x, long y);
long max(long x, long y);
long abs(long x);

long constrain(long x, long a, long b);
int map(int value, int fromLow, int fromHigh, int toLow, int toHigh);

byte lowByte(int x);
byte highByte(int x);

// todo: x is passed by ref in Arduino.
void bitWrite(int* x, byte n, boolean b);
// todo: x is passed by ref in Arduino.
void bitSet(int* x, byte n);
// todo: x is passed by ref in Arduino.
void bitClear(int* x, byte n, boolean b);
unsigned int bit(byte n);

void randomSeed(long seed);
unsigned int random(unsigned int max);

typedef void (*_serialBegin)();
typedef void (*_serialEnd)();
typedef byte (*_serialAvailable)();
typedef word (*_serialRead)();
typedef void (*_serialFlush)();
typedef void (*_serialPrint)(int, Format);
typedef void (*_serialWrite)(const char*);
typedef void (*_serialPrintln)(const char*);

struct
{
    _serialBegin begin;
    _serialEnd end;
    _serialAvailable available;
    _serialRead read;
    _serialFlush flush;
	_serialPrint print;
    _serialPrintln println;
    _serialWrite write;
} Serial;

/* String - object
    * analogReference()
    * analogRead()

    * attachInterrupt()
    * detachInterrupt()

    * interrupts()
    * noInterrupts()

    * Serial */

// void setup();
// void loop();

#endif // __ARDUINO_H__
