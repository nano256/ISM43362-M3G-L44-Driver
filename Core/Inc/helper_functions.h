/*
 * helper_functions.h
 *
 *  Created on: 23 Mar 2020
 *      Author: Federico Villani
 */

#ifndef SRC_HELPER_FUNCTIONS_H_
#define SRC_HELPER_FUNCTIONS_H_

#include "stm32l4xx_hal.h"
#include <stdio.h>

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int getc(FILE *f)
#endif /* __GNUC__ */

/**
 * Hacks
 */
#define SCANF_INIT_BUF() setvbuf(stdin, NULL, _IONBF, 0)  	//set input  buffer to 0
#define PRINTF_INIT_BUF() setvbuf(stdout, NULL, _IONBF, 0)	//set output buffer to 0

/**
 * Cycle counter macros.
 * Can be used to count the number of cycles that a function takes to run
 */
//internal cycle counter functions
__STATIC_INLINE void __DWT_ResetTimer(void){
	//disable counter
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
	// reset cycle counter
	DWT->CYCCNT = 0;
	//enable trace and debug blocks (DWT, ITM, ETM, TPIU)
	CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk;
}
#define __DWT_START_TIMER() DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk
#define __DWT_STOP_TIMER()  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk

// Returns the current number of cycles according to the internal cycle counter
#define __DWT_GET_CYCLES() 	(uint32_t) DWT->CYCCNT


#endif /* SRC_HELPER_FUNCTIONS_H_ */
