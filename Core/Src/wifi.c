/* Includes ------------------------------------------------------------------*/
#include "wifi.h"



/**
  * @brief  Receives data over the defined SPI interface and writes
  * 		it in buffer.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @param  buffer: A char buffer, where the received data will be saved in.
  * @param  size: Buffer size
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_SPI_Receive(WIFI_HandleTypeDef* hwifi, uint8_t* buffer, uint16_t size){

	uint16_t cnt = 0;
	memset(buffer, '\0', size); // Erase buffer

	while (WIFI_IS_CMDDATA_READY())
	{
		// Fill buffer as long there is still space
		if ( (cnt > (size - 2)) || (HAL_SPI_Receive(hwifi->handle , buffer + cnt, 1, WIFI_TIMEOUT) != HAL_OK) )
		  {
			WIFI_DISABLE_NSS();
			Error_Handler();
		  }
		cnt+=2;
	}

	// Trim padding chars from data
	trimstr(buffer, size, WIFI_RX_PADDING);

	return WIFI_OK;
}


/**
  * @brief  Sends data over the defined SPI interface which it
  * 		reads from buffer.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @param  buffer: A char buffer, where the data to be sent is saved in.
  * @param  size: Buffer size
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_SPI_Transmit(WIFI_HandleTypeDef* hwifi, uint8_t* buffer, uint16_t size){

	uint8_t bTx[((size)/2)*2 + 1]; // Make a buffer that has an even amount of bytes
	snprintf( bTx, size, buffer ); // Copy buffer in bTx

	if ( !(size % 2) ) strcat(bTx, WIFI_TX_PADDING); // If buffer had an odd amount of bytes, append a filler char to bTx

	if (HAL_SPI_Transmit(hwifi->handle, bTx, size/2, WIFI_TIMEOUT) != HAL_OK) // size must be halved since 16bits are sent via SPI
	  {
		Error_Handler();
	  }

	return HAL_OK;
}


/**
  * @brief  Resets and initialises the Wifi module.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_Init(WIFI_HandleTypeDef* hwifi){

	WIFI_RESET_MODULE();
	WIFI_ENABLE_NSS();

	while(!WIFI_IS_CMDDATA_READY());

	WIFI_SPI_Receive(hwifi, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	if( strcmp(wifiRxBuffer, WIFI_MSG_POWERUP) ) Error_Handler();

	WIFI_ENABLE_NSS();

	return WIFI_OK;
}


/**
  * @brief  Sends an AT command to the Wifi module and write the response
  * 		in a buffer.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @param  bCmd: Char buffer that contains command.
  * @param  sizeCmd: Command buffer size
  * @param  bRx: Response buffer
  * @param  sizeCmd: Response buffer size
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_SendATCommand(WIFI_HandleTypeDef* hwifi, uint8_t* bCmd, uint16_t sizeCmd, uint8_t* bRx, uint16_t sizeRx){

	while(!WIFI_IS_CMDDATA_READY());

	WIFI_ENABLE_NSS();

	if(WIFI_SPI_Transmit(hwifi, bCmd, sizeCmd) != WIFI_OK) Error_Handler();

	WIFI_DISABLE_NSS();

	while(!WIFI_IS_CMDDATA_READY());

	WIFI_ENABLE_NSS();

	if(WIFI_SPI_Receive(hwifi, bRx, sizeRx) != WIFI_OK) Error_Handler();

	if(WIFI_IS_CMDDATA_READY()) Error_Handler(); // If CMDDATA_READY is still high, then the buffer is too small for the data

	WIFI_DISABLE_NSS();

	return WIFI_OK;
}


/**
  * @brief  Creates Wifi access point on Wifi module
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @retval WIFI_StatusTypeDef
  */


WIFI_StatusTypeDef WIFI_CreateNewNetwork(WIFI_HandleTypeDef* hwifi){
	return WIFI_OK;
}


/**
  * @brief  Starts up web server on Wifi module
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_WebServerInit(WIFI_HandleTypeDef* hwifi){
	return WIFI_OK;
}


/**
  * @brief  Checks web server for incoming connections
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @param  buffer: A char buffer, where the received data will be saved in.
  * @param  size: Buffer size
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_WebServerListen(WIFI_HandleTypeDef* hwifi, uint8_t* buffer, uint16_t size){
	return WIFI_OK;
}

/**
  * @brief  Checks web server for incoming connections
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @param  buffer: A char buffer, where the data to be sent is saved in.
  * @param  size: Buffer size
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_WebServerSend(WIFI_HandleTypeDef* hwifi, uint8_t* buffer, uint16_t size){
	return WIFI_OK;
}


/**
  * @brief  Trims a given character from beginning and end of a c string.
  * @param  str: C string
  * @param  strSize: C string size
  * @param  size: Character to trim
  * @retval None
  */

void trimstr(uint8_t* str, uint32_t strSize, uint8_t c){

	uint32_t trimPos = 0;
	uint32_t endPos = 0;

	// Find end of string a.k.a. first occurrence of '\0'
	for(uint32_t i = 0; i < strSize; i++){
		if( str[i] != '\0' ) continue;
		else{
			endPos = i;
			break;
		}
	}

	/**
	 * If c is at the end of the string, replace it with '\0'.
	 * Repeat until a char emerges that is not c.
	 */
	for(uint32_t i = endPos -1; i > 0; i--){
		if( str[i] == c ){
			str[i] = '\0';
			endPos = i;
		}
		else break;
	}

	// Find the position of the first char in the string that is not c.
	for(uint32_t i = 0; i < strSize; i++){
		if(str[i] == c){
			trimPos = i + 1;
		}else break;
	}
	// Trim leading c
	snprintf( str, endPos + 1 - trimPos, &str[trimPos] );
}
