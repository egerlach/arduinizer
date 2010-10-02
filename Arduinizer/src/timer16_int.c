//*****************************************************************************
//   +--+
//   | ++----+
//   +-++    |
//     |     |
//   +-+--+  |
//   | +--+--+
//   +----+    Copyright (c) 2009 Code Red Technologies Ltd.
//
// timer16_int.c - Interrupt handler code for timer16 peripherals of LPC13xx
//
// Software License Agreement
//
// The software is owned by Code Red Technologies and/or its suppliers, and is
// protected under applicable copyright laws.  All rights are reserved.  Any
// use in violation of the foregoing restrictions may subject the user to criminal
// sanctions under applicable laws, as well as to civil liability for the breach
// of the terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD.
//
//*****************************************************************************

// Code based on content of NXP timer16.c, to be found in LPC13xx_Lib project.
// Interrupts handlers have been extracted to this file so that the library
// project is then application independent.

#include "LPC13xx.h"
#include "timer16.h"

volatile uint32_t timer16_0_counter = 0;
volatile uint32_t timer16_1_counter = 0;



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
void TIMER16_0_IRQHandler(void)
{
  LPC_TMR16B0->IR = 1;			/* clear interrupt flag */
  timer16_0_counter++;
  return;
}

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
void TIMER16_1_IRQHandler(void)
{
  LPC_TMR16B1->IR = 1;			/* clear interrupt flag */
  timer16_1_counter++;
  return;
}
