/*
 * interrupts.h
 *
 *  Created on: 05.11.2014
 *      Author: Дима
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include "../ecu_global_defines.h"
#include "../drv/can_if.h"
#include "../drv/diesel_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

// прерывания АЦП
__interrupt void adca1_isr(void);
__interrupt void adcd1_isr(void);

// внешние прерывания
__interrupt void xint1_isr(void);
__interrupt void xint2_isr(void);

interrupt void canHwIsr();

// таймер
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);

//Uint16 cylToCode(int nCyl);

#ifdef __cplusplus
}
#endif

#endif /* INTERRUPTS_H_ */
