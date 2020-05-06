/* helper_functions.c
 *
 *  Created on: 23 Mar 2020
 *      Author: Federico Villani
 */

#include "helper_functions.h"
 /**
  * external variables and defines
  */

#define UART_HANDLE huart1 //Copy your handle name here



extern UART_HandleTypeDef UART_HANDLE;
volatile int32_t ITM_RxBuffer=ITM_RXBUFFER_EMPTY;


//Uncomment next line to use ITM
//#define USE_ITM
/**
 * Transmission and reception macros
 */

#ifndef USE_ITM
	static void transmit_char(char ch){
		HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	}
	static char receive_char(){
		char ch;
		HAL_UART_Receive(&UART_HANDLE, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
		return ch;
	}
#else
	static void transmit_char(char ch){
		ITM_SendChar((uint32_t)ch);
	}
	static char receive_char(){
		char ch = 0;
		ch = (char)ITM_ReceiveChar();
		return ch;
	}
#endif
/**
 * @brief PUTCHAR_PROTOTYPE function, called from printf
 * @param ch 	: Char to be written to console
 * @return
 */
PUTCHAR_PROTOTYPE{

	transmit_char((char) ch);

	return 0;
}
/**
 * @brief GETCHAR_PROTOTYPE function, called from scanf
 * @return read out character
 */
GETCHAR_PROTOTYPE{
	char ch;

	ch = receive_char();
	transmit_char(ch);

	return (int)ch;
}



