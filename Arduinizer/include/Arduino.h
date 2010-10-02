#ifndef __ARDUINO_H__
#define __ARDUINO_H__

#define INPUT 0
#define OUTPUT 1

#define HIGH 1
#define LOW 0

#define MSBFIRST 1
#define MSBLAST 0

#define byte unsigned char
#define word unsigned short

void pinMode(byte pin, byte mode);

void digitalWrite(byte pin, byte value);
byte digitalRead(byte pin);
/* String - object
    * analogReference()
    * analogRead()
    * analogWrite() - PWM

    * tone()
    * noTone()
    * shiftOut()
    * pulseIn()

    * millis()
    * micros()
    * delay()
    * delayMicroseconds()

    * min()
    * max()
    * abs()
    * constrain()
    * map()
    * pow()
    * sqrt()

    * sin()
    * cos()
    * tan()

    * randomSeed()
    * random()

    * lowByte()
    * highByte()
    * bitRead()
    * bitWrite()
    * bitSet()
    * bitClear()
    * bit()

    * attachInterrupt()
    * detachInterrupt()

    * interrupts()
    * noInterrupts()

    * Serial */

// void setup();
// void loop();

#endif // __ARDUINO_H__
