/* Includes ------------------------------------------------------------------*/
#include "wifi.h"
#include "helper_functions.h"



/**
  * @brief  Receives data over the defined SPI interface and writes
  * 		it in buffer.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @param  buffer: A char buffer, where the received data will be saved in.
  * @param  size: Buffer size
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_SPI_Receive(WIFI_HandleTypeDef* hwifi, char* buffer, uint16_t size){

	uint16_t cnt = 0;
	memset(buffer, '\0', size); // Erase buffer

	while (WIFI_IS_CMDDATA_READY())
	{
		// Fill buffer as long there is still space
		if ( (cnt > (size - 2)) || (HAL_SPI_Receive(hwifi->handle , (uint8_t*) buffer + cnt, 1, WIFI_TIMEOUT) != HAL_OK) )
		  {
			Error_Handler();
		  }
		cnt+=2;
	}

	// Trim padding chars from data
	trimstr(buffer, size, (char) WIFI_RX_PADDING);

	return WIFI_OK;
}


/**
  * @brief  Sends data over the defined SPI interface which it
  * 		reads from buffer.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @param  buffer: A char buffer, where the data to be sent is saved in.
  * @param  size: Buffer size (including \0, so it is compatible with sizeof())
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_SPI_Transmit(WIFI_HandleTypeDef* hwifi, char* buffer, uint16_t size){

	char bTx[(size/2)*2 + 1]; // Make a buffer that has an even amount of bytes (even is meant for the chars excluding \0)
	snprintf( bTx, size, buffer ); // Copy buffer in bTx

	if ( !(size % 2) ) strcat(bTx, (char) WIFI_TX_PADDING); // If buffer had an odd amount of bytes, append a filler char to bTx

	if (HAL_SPI_Transmit(hwifi->handle, (uint8_t*)bTx, size/2, WIFI_TIMEOUT) != HAL_OK) // size must be halved since 16bits are sent via SPI
	  {
		Error_Handler();
	  }

	return WIFI_OK;
}


/**
  * @brief  Resets and initialises the Wifi module.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_Init(WIFI_HandleTypeDef* hwifi){

	int msgLength = 0;

	WIFI_RESET_MODULE();
	WIFI_ENABLE_NSS();

	while(!WIFI_IS_CMDDATA_READY());

	if(WIFI_SPI_Receive(hwifi, wifiRxBuffer, WIFI_RX_BUFFER_SIZE) != WIFI_OK) Error_Handler();

	if( strcmp(wifiRxBuffer, WIFI_MSG_POWERUP) ) Error_Handler();

	WIFI_DISABLE_NSS();


	msgLength = sprintf(wifiTxBuffer, "Z3=0\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, WIFI_TX_BUFFER_SIZE, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	msgLength = sprintf(wifiTxBuffer, "Z0\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, WIFI_TX_BUFFER_SIZE, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);
	printf("Answer reset:\n %s", wifiRxBuffer);


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

WIFI_StatusTypeDef WIFI_SendATCommand(WIFI_HandleTypeDef* hwifi, char* bCmd, uint16_t sizeCmd, char* bRx, uint16_t sizeRx){

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

	int msgLength = 0;
	char* ipStart;
	char* ipEnd;

	// The msgLength+1 is because sprintf only returns string length without counting \0

	// Activate the soft access point
	msgLength = sprintf(wifiTxBuffer, "A1=%d\r", hwifi->securityType);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set AP security key
	msgLength = sprintf(wifiTxBuffer, "A2=%s\r", hwifi->passphrase);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set AP SSID
	msgLength = sprintf(wifiTxBuffer, "AS=0,%s\r", hwifi->ssid);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Activate AP direct mode
	msgLength = sprintf(wifiTxBuffer, "AD\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Get AP info
	msgLength = sprintf(wifiTxBuffer, "A?\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Get the position of the IP address
	ipStart = strstr(wifiRxBuffer, ",") + 1;
	ipEnd = strstr(ipStart, ",");

	// Save IP address in the Wifi handle
	memset(hwifi->ipAddress, '\0', sizeof(hwifi->ipAddress));
	snprintf(hwifi->ipAddress, ipEnd - ipStart + 1, ipStart);

	return WIFI_OK;
}


/**
  * @brief  Starts up web server on Wifi module
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_WebServerInit(WIFI_HandleTypeDef* hwifi){

	int msgLength = 0;


	// Set TCP keep alive
	msgLength = sprintf(wifiTxBuffer, "PK=1,3000\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set communication socket
	msgLength = sprintf(wifiTxBuffer, "P0=0\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set transport protocol
	msgLength = sprintf(wifiTxBuffer, "P1=%d\r", hwifi->transportProtocol);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set port
	msgLength = sprintf(wifiTxBuffer, "P2=%u\r", hwifi->port);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	return WIFI_OK;
}


/**
  * @brief  Checks web server for incoming connections and calls the request
  * 		handler when a request was received.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_WebServerListen(WIFI_HandleTypeDef* hwifi){

	int msgLength = 0;

	// Start web server
	msgLength = sprintf(wifiTxBuffer, "P5=1\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set read packet size
	msgLength = sprintf(wifiTxBuffer, "R1=%d\r", WIFI_READ_PACKET_SIZE);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set read timeout
	msgLength = sprintf(wifiTxBuffer, "R2=%d\r", WIFI_READ_TIMEOUT);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Poll as long until a transport request arrives
	while(1){
		do{
			// delay so the Wifi module is not blocked by the polling
			WIFI_DELAY(WIFI_POLLING_DELAY);
			// Read messages
			msgLength = sprintf(wifiTxBuffer, "MR\r");
			WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);
		}while(!strcmp(WIFI_MSG_OK, wifiRxBuffer) || !strcmp(WIFI_MSG_EMPTY, wifiRxBuffer));

		// Check the received message
		if(strstr(wifiRxBuffer, "Accepted") != 0){
			break;
		}
		else if(strstr(wifiRxBuffer, "ERROR") != 0){
			Error_Handler();
		}
		else{
			continue;
		}
	}

	// Read received data
	msgLength = sprintf(wifiTxBuffer, "R0\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Call request handler
	strcpy(wifiTxBuffer,wifiRxBuffer);
	WIFI_WebServerHandleRequest(hwifi, wifiTxBuffer, WIFI_TX_BUFFER_SIZE, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Send response
	msgLength = sprintf(wifiTxBuffer, "S3=%d\r%s", strlen(wifiRxBuffer), wifiRxBuffer);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Stop web server
	msgLength = sprintf(wifiTxBuffer, "P5=0\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	return WIFI_OK;
}

/**
  * @brief  Checks web server for incoming connections
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @param  req: A char buffer, where the HTTP request to the server is contained.
  * @param  sizeReq: Request buffer size
  * @param  res: A char buffer, where the response to the request is written in.
  * @param  sizeRes: Response buffer size
  * @retval WIFI_StatusTypeDef
  */

__weak WIFI_StatusTypeDef WIFI_WebServerHandleRequest(WIFI_HandleTypeDef* hwifi, char* req, uint16_t sizeReq, char* res, uint16_t sizeRes){
	return WIFI_OK;
}

/**
  * @brief  Joins an existing Network using the network configuration in
  * 		the Wifi handle.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_JoinNetwork(WIFI_HandleTypeDef* hwifi){

	int msgLength = 0;

	// Set SSID
	msgLength = sprintf(wifiTxBuffer, "C1=%s\r", hwifi->ssid);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set passphrase
	msgLength = sprintf(wifiTxBuffer, "C2=%s\r", hwifi->passphrase);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set security type
	msgLength = sprintf(wifiTxBuffer, "C3=%d\r", hwifi->securityType);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set if IP is requested via DHCP
	msgLength = sprintf(wifiTxBuffer, "C4=%d\r", hwifi->DHCP);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// If DHCP is not used, set the additionally needed configurations
	if(hwifi->DHCP != SET){

		// Set module's IP address
		msgLength = sprintf(wifiTxBuffer, "C6=%s\r", hwifi->ipAddress);
		WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

		// Set module's network mask
		msgLength = sprintf(wifiTxBuffer, "C7=%s\r", hwifi->networkMask);
		WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

		// Set module's default gateway
		msgLength = sprintf(wifiTxBuffer, "C8=%s\r", hwifi->defaultGateway);
		WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

		// Set module's primary DNS server
		msgLength = sprintf(wifiTxBuffer, "C9=%s\r", hwifi->primaryDNSServer);
		WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	}

	// Join the network
	msgLength = sprintf(wifiTxBuffer, "C0\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// If there was an error, call the error handler
	if(strstr(wifiRxBuffer, "ERROR") != NULL) Error_Handler();

	// If the module's IP address was assigned by DHCP, then parse it
	// from the response and save it in the Wifi handle.
	if(hwifi->DHCP == SET){
		// The IP address is between the first and second comma
		char* startPos = strstr(wifiRxBuffer, ",");
		char* endPos = strstr(startPos+1, ",");

		// Check whether the commas have been found
		if(startPos == NULL || endPos == NULL) Error_Handler();

		// Copy the IP address from the response buffer into the Wifi handle
		// For n set IP_length+1, because the ending char \0 must be considered
		snprintf(hwifi->ipAddress, endPos - startPos, startPos+1);
	}

	return WIFI_OK;
}

/**
  * @brief  Initialises the module for using the MQTT Protocol
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_MQTTClientInit(WIFI_HandleTypeDef* hwifi){

	int msgLength = 0;

	// Set publish topic
	msgLength = sprintf(wifiTxBuffer, "PM=0,%s\r", hwifi->mqtt.publishTopic);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set subscribe topic
	msgLength = sprintf(wifiTxBuffer, "PM=1,%s\r", hwifi->mqtt.subscribeTopic);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set security mode
	msgLength = sprintf(wifiTxBuffer, "PM=2,%d\r", hwifi->mqtt.securityMode);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	if(hwifi->mqtt.securityMode == WIFI_MQTT_SECURITY_USER_PW){

		// Set user name
		msgLength = sprintf(wifiTxBuffer, "PM=3,%s\r", hwifi->mqtt.userName);
		WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

		// Set password
		msgLength = sprintf(wifiTxBuffer, "PM=4,%s\r", hwifi->mqtt.password);
		WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	} else if(hwifi->mqtt.securityMode == WIFI_MQTT_SECURITY_CERT) {
		// TODO: Add certificate functionality
	}

	// Set keep alive time
	msgLength = sprintf(wifiTxBuffer, "PM=6,%u\r", hwifi->mqtt.keepAlive);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set communication socket
	msgLength = sprintf(wifiTxBuffer, "P0=0\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set transport protocol
	msgLength = sprintf(wifiTxBuffer, "P1=%d\r", WIFI_MQTT_PROTOCOL);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set port
	msgLength = sprintf(wifiTxBuffer, "P2=%u\r", hwifi->port);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set remote IP
	msgLength = sprintf(wifiTxBuffer, "D0=%s\r", hwifi->remoteAddress);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set read packet size
	msgLength = sprintf(wifiTxBuffer, "R1=%d\r", WIFI_READ_PACKET_SIZE);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Set read timeout
	msgLength = sprintf(wifiTxBuffer, "R2=%d\r", WIFI_READ_TIMEOUT);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	return WIFI_OK;
}

/**
  * @brief  Builds up a connection with the MQTT server and publishes a message.
  * @param  hwifi: Wifi handle, which decides which Wifi instance is used.
  * @param  message: A char buffer, where the message is contained.
  * @param  sizeMessage: Message buffer size.
  * @retval WIFI_StatusTypeDef
  */

WIFI_StatusTypeDef WIFI_MQTTPublish(WIFI_HandleTypeDef* hwifi, char* message, uint16_t sizeMessage){

	int msgLength = 0;

	// Start client connection
	msgLength = sprintf(wifiTxBuffer, "P6=1\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Send response
	msgLength = sprintf(wifiTxBuffer, "S3=%d\r%s", strlen(message), message);
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	// Stop client connection
	msgLength = sprintf(wifiTxBuffer, "P6=0\r");
	WIFI_SendATCommand(hwifi, wifiTxBuffer, msgLength+1, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

	return WIFI_OK;
}

/**
  * @brief  Trims a given character from beginning and end of a c string.
  * @param  str: C string
  * @param  strSize: C string size
  * @param  size: Character to trim
  * @retval None
  */

void trimstr(char* str, uint32_t strSize, char c){

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
