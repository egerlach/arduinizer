/*****************************************************************************
 *   timer16.h:  Header file for NXP LPC13xx Family Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.08.20  ver 1.00    Preliminary version, first Release
 *   2010.03.24      1.01    Minor tweaks by Code Red
 *
******************************************************************************/
#ifndef __TIMER16_H 
#define __TIMER16_H

void delay_microseconds(uint8_t timer_num, uint32_t delayIn_us);
void TIMER16_0_IRQHandler(void);
void TIMER16_1_IRQHandler(void);
void enable_timer16(uint8_t timer_num);
void disable_timer16(uint8_t timer_num);
void reset_timer16(uint8_t timer_num);
void init_timer16(uint8_t timer_num, uint32_t timerInterval);

#endif /* end __TIMER16_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
