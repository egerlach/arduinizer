/* -*- tab-width: 4 -*- */

//*****************************************************************************
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD.
//
//*****************************************************************************

#include "LPC13xx.h"                        /* LPC13xx definitions */
#include "timer16.h"
#include "clkconfig.h"
#include "gpio.h"
#include "Arduino.h"
#include "uart.h"

// ADC code from http://ics.nxp.com "Sample Code Bundle for LPC13xx Peripheral's using Keil's MDK-ARM"
// todo: Just import the library instead of pasting code.
#define ADC_INTERRUPT_FLAG  1	/* 1 is interrupt driven, 0 is polling */
#define BURST_MODE          0   /* Burst mode works in interrupt driven mode only. */
#define ADC_DEBUG           0

#define ADC_OFFSET          0x10
#define ADC_INDEX           4

#define ADC_DONE            0x80000000
#define ADC_OVERRUN         0x40000000
#define ADC_ADINT           0x00010000

#define ADC_NUM             8			/* for LPC13xx */
#define ADC_CLK             1000000		/* set to 1Mhz */

volatile uint32_t ADCValue[ADC_NUM];
volatile uint32_t ADCIntDone = 0;

/*****************************************************************************
** Function name:		ADCInit
**
** Descriptions:		initialize ADC channel
**
** parameters:			ADC clock rate
** Returned value:		None
**
*****************************************************************************/
void ADCInit( uint32_t ADC_Clk )
{
  uint32_t i;

  /* Disable Power down bit to the ADC block. */
  LPC_SYSCON->PDRUNCFG &= ~(0x1<<4);

  /* Enable AHB clock to the ADC. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<13);

  for ( i = 0; i < ADC_NUM; i++ )
  {
	ADCValue[i] = 0x0;
  }
  /* Unlike some other pings, for ADC test, all the pins need
  to set to analog mode. Bit 7 needs to be cleared according
  to design team. */
#ifdef __JTAG_DISABLED
  LPC_IOCON->R_PIO0_11 &= ~0x8F; /*  ADC I/O config */
  LPC_IOCON->R_PIO0_11 |= 0x02;  /* ADC IN0 */
  LPC_IOCON->R_PIO1_0  &= ~0x8F;
  LPC_IOCON->R_PIO1_0  |= 0x02;  /* ADC IN1 */
  LPC_IOCON->R_PIO1_1  &= ~0x8F;
  LPC_IOCON->R_PIO1_1  |= 0x02;  /* ADC IN2 */
  LPC_IOCON->R_PIO1_2  &= ~0x8F;
  LPC_IOCON->R_PIO1_2  |= 0x02;  /* ADC IN3 */
#ifdef __SWD_DISABLED
  LPC_IOCON->SWDIO_PIO1_3   &= ~0x8F;
  LPC_IOCON->SWDIO_PIO1_3   |= 0x02;  /* ADC IN4 */
#endif
#endif
  LPC_IOCON->PIO1_4    &= ~0x8F; /* Clear bit7, change to analog mode. */
  LPC_IOCON->PIO1_4    |= 0x01;  /* ADC IN5 */
  LPC_IOCON->PIO1_10   &= ~0x8F; /* Clear bit7, change to analog mode. */
  LPC_IOCON->PIO1_10   |= 0x01;  /* ADC IN6 */
  LPC_IOCON->PIO1_11   &= ~0x8F; /* Clear bit7, change to analog mode. */
  LPC_IOCON->PIO1_11   |= 0x01;  /* ADC IN7 */

  LPC_ADC->CR = ( 0x01 << 0 ) |  /* SEL=1,select channel 0~7 on ADC0 */
	(((ADC_CLK/LPC_SYSCON->SYSAHBCLKDIV)/SystemCoreClock-1)<<8) |  /* CLKDIV = Fpclk / 1000000 - 1 */
	( 0 << 16 ) | 		/* BURST = 0, no BURST, software controlled */
	( 0 << 17 ) |  		/* CLKS = 0, 11 clocks/10 bits */
	( 1 << 21 ) |  		/* PDN = 1, normal operation */
	( 0 << 22 ) |  		/* TEST1:0 = 00 */
	( 0 << 24 ) |  		/* START = 0 A/D conversion stops */
	( 0 << 27 );		/* EDGE = 0 (CAP/MAT singal falling,trigger A/D conversion) */

  /* If POLLING, no need to do the following */
#if ADC_INTERRUPT_FLAG
  NVIC_EnableIRQ(ADC_IRQn);
  LPC_ADC->INTEN = 0x1FF;		/* Enable all interrupts */
#endif
  return;
}

/*****************************************************************************
** Function name:		ADCRead
**
** Descriptions:		Read ADC channel
**
** parameters:			Channel number
** Returned value:		Value read, if interrupt driven, return channel #
**
*****************************************************************************/
uint32_t ADCRead( uint8_t channelNum )
{
#if !ADC_INTERRUPT_FLAG
  uint32_t regVal, ADC_Data;
#endif

  /* channel number is 0 through 7 */
  if ( channelNum >= ADC_NUM )
  {
	channelNum = 0;		/* reset channel number to 0 */
  }
  LPC_ADC->CR &= 0xFFFFFF00;
  LPC_ADC->CR |= (1 << 24) | (1 << channelNum);
				/* switch channel,start A/D convert */
#if !ADC_INTERRUPT_FLAG
  while ( 1 )			/* wait until end of A/D convert */
  {
	regVal = *(volatile unsigned long *)(LPC_ADC_BASE
			+ ADC_OFFSET + ADC_INDEX * channelNum);
	/* read result of A/D conversion */
	if ( regVal & ADC_DONE )
	{
	  break;
	}
  }

  LPC_ADC->CR &= 0xF8FFFFFF;	/* stop ADC now */
  if ( regVal & ADC_OVERRUN )	/* save data when it's not overrun, otherwise, return zero */
  {
	return ( 0 );
  }
  ADC_Data = ( regVal >> 6 ) & 0x3FF;
  return ( ADC_Data );	/* return A/D conversion value */
#else
  return ( channelNum );	/* if it's interrupt driven, the ADC reading is
							done inside the handler. so, return channel number */
#endif
}

#define PWM_PERIOD 14399
// todo: pin is ignored.

// Code from knowledgebase.nxp.com, thread 593 - Brinkand
void pwm(byte pin, word frequency, byte duty)
{
	if(0 == frequency)
	{
		return;
	}
	if(21 == pin)	// 1.9
	{
	  float newF = (float)frequency / 50000.0;
	  int period = (int)((float)PWM_PERIOD / (newF));
	  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8);  // Enable clock to timer 1
	  LPC_IOCON->PIO1_9           &= ~0x1F; // - remove bits 0,1 & 2 and pull resistors
	  LPC_IOCON->PIO1_9           |= 0x01; /* Timer1_16 MAT0 */ //- set bit 1
	  LPC_TMR16B1->MCR = 0x0400;        /* Reset at MR3*/
	  LPC_TMR16B1->MR0 = (0x100 - duty) * period / 0x100;            /* Match register 0 - Turn on time: 2879 = 20% duty cycle */
	  LPC_TMR16B1->MR1 = 7000;            /* Match register 1 - no PWM - toggle test */
	  LPC_TMR16B1->MR3 = period;          /* Match register 3 - cycle length */
	  LPC_TMR16B1->EMR = 0xC2;            /* External Match register Settings for PWM channel have no effect.*/
	  LPC_TMR16B1->PWMC = 0x01;             /* PWMC register -MAT0 is PWM. */
	  GPIOSetDir( 1, 9, 1 );  // Set as output
	  LPC_TMR16B1->TCR = 1;   // Enable timer 1
	}
	else if(18 == pin) // 1.6
	{
	  float newF = (float)frequency / 50000.0;
	  int period = (int)((float)PWM_PERIOD / (newF));
	  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);  // Enable clock to timer 1
	  LPC_IOCON->PIO1_6         &= ~0x1F; // - remove bits 0,1 & 2 and pull resistors
	  LPC_IOCON->PIO1_6         |= 0x01; /* Timer0_32 MAT0 */ //- set bit 1
	  LPC_TMR32B0->MCR = 0x0400;        /* Reset at MR3*/
	  LPC_TMR32B0->MR0 = (0x100 - duty) * PWM_PERIOD / 0x100;            /* Match register 0 - Turn on time: 2879 = 20% duty cycle */
	  LPC_TMR32B0->MR1 = 7000;            /* Match register 1 - no PWM - toggle test */
	  LPC_TMR32B0->MR3 = period;          /* Match register 3 - cycle length */
	  LPC_TMR32B0->EMR = 0xC2;            /* External Match register Settings for PWM channel have no effect.*/
	  LPC_TMR32B0->PWMC = 0x01;             /* PWMC register -MAT0 is PWM. */
	  GPIOSetDir( 1, 6, 1 ); // Set as output
	  LPC_TMR32B0->TCR = 1;  // Enable timer 1
	}
	else if(8 == pin) // 0.8
	{
	  float newF = (float)frequency / 50000.0;
	  int period = (int)((float)PWM_PERIOD / (newF));
	  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);  // Enable clock to timer 16.0
	  LPC_IOCON->PIO0_8         &= ~0x1F; // - remove bits 0,1 & 2 and pull resistors
	  LPC_IOCON->PIO0_8         |= 0x01; /* Timer0_32 MAT0 */ //- set bit 1
	  LPC_TMR16B0->MCR = 0x0400;        /* Reset at MR3*/
	  LPC_TMR16B0->MR0 = (0x100 - duty) * period / 0x100;            /* Match register 0 - Turn on time: 2879 = 20% duty cycle */
	  LPC_TMR16B0->MR1 = 7000;            /* Match register 1 - no PWM - toggle test */
	  LPC_TMR16B0->MR3 = period;          /* Match register 3 - cycle length */
	  LPC_TMR16B0->EMR = 0xC2;            /* External Match register Settings for PWM channel have no effect.*/
	  LPC_TMR16B0->PWMC = 0x01;             /* PWMC register -MAT0 is PWM. */
	  GPIOSetDir( 0,  8, 1 ); // Set as output
	  LPC_TMR16B0->TCR = 1;   // Enable timer 1
	}
	else if(11 == pin) // 0.11
	{
      float newF = (float)frequency / 50000.0;
      int period = (int)((float)PWM_PERIOD / (newF));
	  LPC_SYSCON->SYSAHBCLKCTRL   |= (1<<10);  // Enable clock to timer 1
	  LPC_IOCON->JTAG_TDI_PIO0_11 &= ~0x1F; // - remove bits 0,1 & 2 and pull resistors
	  LPC_IOCON->JTAG_TDI_PIO0_11 |= 0x01; /* Timer0_32 MAT0 */ //- set bit 1
	  LPC_TMR32B1->MCR = 0x0400;        /* Reset at MR3*/
	  LPC_TMR32B1->MR0 = (0x100 - duty) * PWM_PERIOD / 0x100;            /* Match register 0 - Turn on time: 2879 = 20% duty cycle */
	  LPC_TMR32B1->MR1 = 7000;            /* Match register 1 - no PWM - toggle test */
	  LPC_TMR32B1->MR3 = period;          /* Match register 3 - cycle length */
	  LPC_TMR32B1->EMR = 0xC2;            /* External Match register Settings for PWM channel have no effect.*/
	  LPC_TMR32B1->PWMC = 0x01;             /* PWMC register -MAT0 is PWM. */
	  GPIOSetDir( 0, 11, 1 ); // Set as output
	  LPC_TMR32B1->TCR = 1;   // Enable timer 1
	}
}

// Pins and ports
typedef struct
{
	byte Port;
	byte Pin;
	byte Mode;
} AM_LCPXPin;

#define AM_UNSET 2

AM_LCPXPin AM_Pins[] =
{
	{0, 0, AM_UNSET}, {0, 1, AM_UNSET}, {0, 2, AM_UNSET}, {0, 3, AM_UNSET}, {0, 4, AM_UNSET}, {0, 5, AM_UNSET}, {0, 6, AM_UNSET}, {0, 7, AM_UNSET}, {0, 8, AM_UNSET}, {0, 9, AM_UNSET}, {0, 10, AM_UNSET}, {0, 11, AM_UNSET},
	{1, 0, AM_UNSET}, {1, 1, AM_UNSET}, {1, 2, AM_UNSET}, {1, 3, AM_UNSET}, {1, 4, AM_UNSET}, {1, 5, AM_UNSET}, {1, 6, AM_UNSET}, {1, 7, AM_UNSET}, {1, 8, AM_UNSET}, {1, 9, AM_UNSET}, {1, 10, AM_UNSET}, {1, 11, AM_UNSET},
	{2, 0, AM_UNSET}, {2, 1, AM_UNSET}, {2, 2, AM_UNSET}, {2, 3, AM_UNSET}, {2, 4, AM_UNSET}, {2, 5, AM_UNSET}, {2, 6, AM_UNSET}, {2, 7, AM_UNSET}, {2, 8, AM_UNSET}, {2, 9, AM_UNSET}, {2, 10, AM_UNSET}, {2, 11, AM_UNSET},
	{3, 0, AM_UNSET}, {3, 1, AM_UNSET}, {3, 2, AM_UNSET}, {3, 3, AM_UNSET}
};

volatile uint32_t g_systemMicroTicks; /* Counts microseconds. */
volatile uint32_t g_systemMacroTicks; /* Increments every 2^32
										 microseconds. */
volatile uint32_t g_systemMilliTicks; /* Counts milliseconds. */

extern volatile uint32_t timer16_0_counter;

// volatile uint32_t timer16_0_counter = 0;
// volatile uint32_t timer16_1_counter = 0;



/******************************************************************************
** Function name:		TIMER_0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
/*
void TIMER16_0_IRQHandler(void)
{
  LPC_TMR16B0->IR = 1;			// clear interrupt flag
  timer16_0_counter++;
  return;
}
*/
/******************************************************************************
** Function name:		TIMER_1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
/*
void TIMER16_1_IRQHandler(void)
{
  LPC_TMR16B1->IR = 1;			// clear interrupt flag
  timer16_1_counter++;
  return;
}
*/
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
  g_systemMicroTicks+=10;
  while(g_systemMicroTicks >= 1000)
    {
      g_systemMicroTicks -= 1000;
      g_systemMilliTicks++;
      if(g_systemMilliTicks == 0)
		g_systemMacroTicks++;
    }
}

long g_randomSeed;
void randomSeed(long seed)
{
	g_randomSeed = seed;
}
unsigned int random(unsigned int max)
{
	g_randomSeed >>= 3;
	g_randomSeed *= 0xF12CAC2D;
	return(g_randomSeed % max);
}


/////////////////////////////////////////////
// Public functions.
/////////////////////////////////////////////

// Arduino event functions.
void setup();	// Called once.
void loop();	// Called repeatedly between maintenance actions.

//Arduino API functions
void pinMode(byte pin, byte mode)
{
	if(5 == pin)
	{
		LPC_IOCON->PIO0_5 = 0;//(1 << 7) | (1 << 0);
	}
	else if(11 == pin)
	{
		LPC_IOCON->JTAG_TDI_PIO0_11 = (1 << 7) | (1 << 0);
	}
	else if(12 == pin)
	{
		LPC_IOCON->JTAG_TMS_PIO1_0 = (1 << 7) | (1 << 0);
	}
	else if(13 == pin)
	{
		LPC_IOCON->JTAG_TDO_PIO1_1 = (1 << 7) | (1 << 0);
	}
	else if(14 == pin)
	{
		LPC_IOCON->JTAG_nTRST_PIO1_2 = (1 << 7) | (1 << 0);
	}
	else if(15 == pin)
	{
		LPC_IOCON->ARM_SWDIO_PIO1_3 = (1 << 7) | (1 << 0);
	}
	else if(16 == pin)
	{
		LPC_IOCON->PIO1_4 = (1 << 7) | (1 << 0);
	}
	GPIOSetDir(AM_Pins[pin].Port, AM_Pins[pin].Pin, mode);
	AM_Pins[pin].Mode = mode;
}

void digitalWrite(byte pin, byte value)
{
	// todo: Check if pin is in INPUT mode, pullup resistor is enabled.
	if(AM_Pins[pin].Mode != OUTPUT)
	{
		pinMode(pin, OUTPUT);
	}
	GPIOSetValue(AM_Pins[pin].Port, AM_Pins[pin].Pin, value);
}

byte digitalRead(byte pin)
{
	// Check bitVal is a binary value - 0 or 1
	pin = pin % sizeof(AM_Pins); // todo: What does Arduino do for pins > max?
	if(AM_Pins[pin].Mode != INPUT)
	{
		pinMode(pin, INPUT);
	}

	AM_LCPXPin lcpxPin = AM_Pins[pin];
	uint32_t value = 0;
	switch ( lcpxPin.Port )
	{
	case PORT0:
		value = LPC_GPIO0->MASKED_ACCESS[(1<<lcpxPin.Pin)];
	break;
	case PORT1:
		value = LPC_GPIO1->MASKED_ACCESS[(1<<lcpxPin.Pin)];
	break;
	case PORT2:
		value = LPC_GPIO2->MASKED_ACCESS[(1<<lcpxPin.Pin)];
	break;
	case PORT3:
		value = LPC_GPIO3->MASKED_ACCESS[(1<<lcpxPin.Pin)];
	break;
	default:
	  break;
	}
  return(value ? HIGH : LOW);
}

word analogRead(byte pin)
{
	return((word)ADCRead(pin));
}

// todo: Only work on pin 1.9 (21).
void analogWrite(byte pin, byte value)
{
	pwm(pin, PWM_PERIOD, value);
}

 void tone(byte pin, word frequency)
{
	 pwm(pin, frequency, 0x80);
}

void noTone(byte pin)
{
	if(21 == pin)
	{
	  LPC_TMR16B1->TCR = 0;   // Enable timer 1
	}
	else if(1 == pin)
	{
	  LPC_TMR32B0->TCR = 0;   // Enable timer 1
	}
	else if(8 == pin)
	{
	  LPC_TMR16B0->TCR = 0;   // Enable timer 1
	}
	else if(11 == pin)
	{
	  LPC_TMR32B1->TCR = 0;   // Enable timer 1
	}
}

unsigned int micros()
{
  return g_systemMilliTicks * 1000 + g_systemMicroTicks;
}

unsigned int millis()
{
  return g_systemMilliTicks;
}

// todo: Check out "LPC1343 Code Base" which has SPI and ADC functions
// but SPI only for 1 port (?!).
#define SHIFTCLOCKMICROSECONDS 1000	// 1 MHz.

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
  Taken from example program "systick".
 *------------------------------------------------------------------------------*/
void sys_hold_until(uint32_t macro, uint32_t milli, uint32_t micro)
{
  while(micro >= 1000)
	{
	  micro -= 1000;
	  milli++;
	  if(milli == 0)
		macro++;
	}
  while(g_systemMacroTicks <= macro)
	{
	  if(g_systemMacroTicks < macro)
		continue;
	  if(g_systemMilliTicks > milli)
		break;
	  if(g_systemMilliTicks < milli)
		continue;
	  if(g_systemMicroTicks >= micro)
		break;
	}
}

void delay (uint32_t ms)
{
  uint32_t macro = g_systemMacroTicks, milli = g_systemMilliTicks;
  if(milli + ms < milli)
	macro++;
  milli += ms;
  sys_hold_until(macro, milli, g_systemMicroTicks);
}

void delayMicroseconds(unsigned int us)
{
  uint32_t macro = g_systemMacroTicks,
	milli = g_systemMilliTicks,
	micro = g_systemMicroTicks;
  micro += us % 1000;
  if(milli + us / 1000 < milli)
	macro++;
  milli += us / 1000;
  sys_hold_until(macro, milli, micro);
}

void shiftOut(byte dataPin, byte clockPin, byte bitOrder, byte value)
{
	byte clockOut = HIGH;
	byte i;
	if(MSBFIRST == bitOrder)
	{
		for(i=7;i!= 0xFF;i--)
		{
			digitalWrite(dataPin, (value >> i & 0x1) ? HIGH : LOW);
			digitalWrite(clockPin, LOW == clockOut ? HIGH : LOW);
			delayMicroseconds(SHIFTCLOCKMICROSECONDS);
			clockOut = (LOW == clockOut) ? HIGH : LOW;
		}
	}
	else if(MSBLAST == bitOrder)
	{
		for(i=0;i<8;i++)
		{
			digitalWrite(dataPin, (value >> i & 0x1) ? HIGH : LOW);
			digitalWrite(clockPin, LOW == clockOut ? HIGH : LOW);
			delayMicroseconds(SHIFTCLOCKMICROSECONDS);
			clockOut = (LOW == clockOut) ? HIGH : LOW;
		}
	}
}
unsigned long pulseIn(byte pin, byte value, unsigned long timeout)
{
	// Wait for pin to go to the required value.
	unsigned long startTime = micros();
	while(value != digitalRead(pin))
	{
		if(timeout)
		{
			if(micros()-startTime >= timeout)
			{
				return(0);	// Timed out.
			}
		}
	}

	unsigned long pulseStart = micros();
	while(value == digitalRead(pin))
	{
		if(timeout)
		{
			if(micros()-startTime >= timeout)
			{
				return(0);	// Timed out.
			}
		}
	}

	return(micros() - pulseStart);	// todo: Will this hold when it rolls over?
}

// Renamed because C does not support overrides or defaults.
unsigned long pulseIn2(byte pin, byte value)
{
	return(pulseIn(pin, value, 0));
}

long min(long x, long y)
{
	return(x < y ? x : y);
}

long max(long x, long y)
{
	return(x > y ? x : y);
}

long abs(long x)
{
	return(x > 0 ? x : -x);
}

long constrain(long x, long a, long b)
{
	if(x<a)
	{
		return(a);
	}
	if(x>b)
	{
		return(b);
	}
	return(x);
}

int map(int value, int fromLow, int fromHigh, int toLow, int toHigh)
{
	float position = (value - fromLow) / (fromHigh - fromLow);
	return((toHigh-toLow) * position + toLow);
}

unsigned int strlen(const char* string)
{
	int i=0;
	while(string[i])
	{
		i++;
	}
	return(i);
}

void serialBegin(unsigned int speed)
{
	UARTInit(speed);
}

void serialEnd()
{
}

// A simple 2-pointer data queue.
byte* g_serialStart;
byte* g_serialEnd;
byte g_serialBuffer[128];

byte serialAvailable()
{
	int size = g_serialEnd - g_serialStart;
	return(size >= 0 ? size : 128+size+1);
}

word serialRead()
{
	if(g_serialStart == g_serialEnd)
	{
		return(-1);
	}
	byte data = *g_serialStart;
	g_serialStart++;
	if(g_serialStart - g_serialBuffer == 128)
	{
		g_serialStart = g_serialBuffer;
	}
	return(data);
}

void serialFlush()
{
	g_serialStart = g_serialBuffer;
	g_serialEnd = g_serialBuffer;
}

// For now, unless there's a way to overload Serial.print(),
// this only works with numbers to be encoded as strings.
void serialPrint(int val, Format format)
{
	byte buffer[33];
	byte size = 0;
	unsigned int uval = val;
	switch(format)
	{
	case BYTE:
		buffer[0] = (byte)val;
		size=1;
		break;
	case BIN:
		do
		{
			buffer[size++] = uval % 2 + '0';
			uval >>= 1;
		} while(uval);
		break;
	case OCT:
		do
		{
			buffer[size++] = uval % 8 + '0';
			uval >>= 3;
		} while(uval);
		break;
	case DEC:
		if(val < 0) buffer[size++] = '-';
		do
		{
			buffer[size++] = val % 10 + '0';
			val /= 10;
		} while(val);
		break;
	case HEX:
		do
		{
			byte value = uval % 0x10;
			if(value <= 9)
			{
				buffer[size++] = value + '0';
			}
			else
			{
				buffer[size++] = value - 10 + 'A';
			}
			uval >>= 4;
		} while(uval);
		break;
	}
	UARTSend(buffer, size);
}

void serialWrite(const char* val)
{
	UARTSend((unsigned char*)val, strlen(val));
}

void serialPrintln(const char* val)
{
	serialWrite(val);
	serialWrite("\r\n");
}

/* String - object
    * analogReference()	// todo: Is this supportable on LPC1343?

    * attachInterrupt()
    * detachInterrupt()

    * interrupts()
    * noInterrupts()

   */

void initSerial()
{
	g_serialStart = g_serialBuffer;
	g_serialEnd = g_serialBuffer;
	// Point functions to Serial "class".
	Serial.begin 	   = serialBegin;
	Serial.end 	   = serialEnd;
	Serial.available = serialAvailable;
	Serial.read      = serialRead;
	Serial.flush     = serialFlush;
	Serial.print 	   = serialPrint;
	Serial.println   = serialPrintln;
	Serial.write     = serialWrite;
}

int main (void)
{

  GPIOInit();

  g_systemMicroTicks = 0;
  g_systemMilliTicks = 0;
  g_systemMacroTicks = 0;

  initSerial();

  g_randomSeed = 0;

  if (SysTick_Config(SystemCoreClock / 100000)) { // Setup SysTick Timer for 1 msec interrupts
    while (1);                                  // Capture error
  }
  {
	  if ( !(SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk) )
	  {
		// When external reference clock is used(CLKSOURCE in
		// Systick Control and register bit 2 is set to 0), the
		// SYSTICKCLKDIV must be a non-zero value and 2.5 times
		// faster than the reference clock.
		// When core clock, or system AHB clock, is used(CLKSOURCE
		// in Systick Control and register bit 2 is set to 1), the
		// SYSTICKCLKDIV has no effect to the SYSTICK frequency. See
		// more on Systick clock and status register in Cortex-M3
		// technical Reference Manual.
		LPC_SYSCON->SYSTICKCLKDIV = 0x08;
	  }
  }

  // Initialise Timer16_0 to tick at rate of 1/2000th of second.
	// Note that as this is a 16 bit timer, the maximum count we can
	// load into timer is 0xFFFF, or 65535. Default clock speed
	// set up by CMSIS SystemInit function - SystemCoreClock - is
	// 72MHz or 72000000 Hz. Dividing this by 2000 is 36000 which is
	// within appropriate timer16 maximum. This could be extended
	// if timer routine extended to make use of Prescale Counter register
	// Note by default LPC_SYSCON->SYSAHBCLKDIV is 1.
  // init_timer16(0, (SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/2000 );

  // Initialise counter that counts Timer16_0 ticks
  // timer16_0_counter = 0;

  //Enable Timer16_0
  // enable_timer16(0);

  // ADCInit(ADC_CLK);

  byte lowByte(int x)
  {
	return((byte)x);
  }

  byte highByte(int x)
  {
	  return((byte) x >> 8);
  }

  // todo: x is passed by ref in Arduino.
  void bitWrite(int* x, byte n, boolean b)
  {
	  int mask = b << n;
	  *x = b ? *x | mask : *x & ~mask;
  }

  // todo: x is passed by ref in Arduino.
  void bitSet(int* x, byte n)
  {
	  int mask = 1 << n;
	  *x |= mask;
  }

  // todo: x is passed by ref in Arduino.
  void bitClear(int* x, byte n, boolean b)
  {
	  int mask = 1 << n;
	  *x &= ~mask;
  }

  unsigned int bit(byte n)
  {
	  return(1 << n);
  }

  setup(); // Arduino setup(), called once.

    while (1)                                /* Loop forever */
  {
	  loop();	// Arduino loop(), called until cows come home.
  }
}
/*
// LPCXpresso processor card LED
#define LED_PORT 0		// Port for led
#define LED_BIT 7		// Bit on port for led
#define LED_ON 1		// Level to set port to turn on led
#define LED_OFF 0		// Level to set port to turn off led

extern volatile uint32_t timer16_0_counter;

int main (void) {

  GPIOInit();

  // Initialise Timer16_0 to tick at rate of 1/2000th of second.
	// Note that as this is a 16 bit timer, the maximum count we can
	// load into timer is 0xFFFF, or 65535. Default clock speed
	// set up by CMSIS SystemInit function - SystemCoreClock - is
	// 72MHz or 72000000 Hz. Dividing this by 2000 is 36000 which is
	// within appropriate timer16 maximum. This could be extended
	// if timer routine extended to make use of Prescale Counter register
	// Note by default LPC_SYSCON->SYSAHBCLKDIV is 1.
  init_timer16(0, (SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/2000 );

  // Initialise counter that counts Timer16_0 ticks
  timer16_0_counter = 0;

  //Enable Timer16_0
  enable_timer16(0);

  // Set port for LED to output
  GPIOSetDir( LED_PORT, LED_BIT, 1 );

  while (1)                                // Loop forever
  {

	// LED is on for 1st half-second
	if ( (timer16_0_counter > 0) && (timer16_0_counter <= 500) )
	{
	  GPIOSetValue( LED_PORT, LED_BIT, LED_OFF );
	}
	// LED is off for 2nd half-second
	if ( (timer16_0_counter > 500) && (timer16_0_counter <= 1000) )
	{
		GPIOSetValue( LED_PORT, LED_BIT, LED_ON );
	}
	// Reset counter
	else if ( timer16_0_counter > 1000 )
	{
	  timer16_0_counter = 0;
	}
  }
}
*/
