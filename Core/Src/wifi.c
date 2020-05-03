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
	return WIFI_OK;
}


/**
  * @brief  Resets and initialises the Wifi module.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_Init(WIFI_HandleTypeDef* hwifi){
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
