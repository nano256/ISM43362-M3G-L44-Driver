#ifndef INC_WIFI_H_
#define INC_WIFI_H_

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "stm32l4xx_hal.h"
#include "main.h"


/* Defines -------------------------------------------------------------------*/
#define WIFI_TIMEOUT_TIME 5000
#define WIFI_TX_BUFFER_SIZE 1024
#define WIFI_RX_BUFFER_SIZE 1024
#define WIFI_MAX_READ_PACKET_SIZE 1200
#define WIFI_READ_PACKET_SIZE ( WIFI_MAX_READ_PACKET_SIZE > WIFI_RX_BUFFER_SIZE ? WIFI_RX_BUFFER_SIZE : WIFI_MAX_READ_PACKET_SIZE )
#define WIFI_READ_TIMEOUT 2000
#define WIFI_POLLING_DELAY 200

#define WIFI_TX_PADDING 0x0A
#define WIFI_RX_PADDING 0x15
#define WIFI_MSG_POWERUP "\r\n> "
#define WIFI_MSG_OK "\r\n\r\nOK\r\n> "
#define WIFI_MSG_START "\r\n[SOMA]"
#define WIFI_MSG_END "[EOMA]\r\nOK\r\n>"
#define WIFI_MSG_EMPTY "\r\n[SOMA][EOMA]\r\nOK\r\n> "

/* Macros --------------------------------------------------------------------*/
#define WIFI_RESET_MODULE()                 HAL_GPIO_WritePin(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin, GPIO_PIN_RESET );\
                                            HAL_Delay(10);\
                                            HAL_GPIO_WritePin( WIFI_RESET_GPIO_Port, WIFI_RESET_Pin, GPIO_PIN_SET );\
                                            HAL_Delay(500);


#define WIFI_ENABLE_NSS()                   HAL_GPIO_WritePin( WIFI_NSS_GPIO_Port, WIFI_NSS_Pin, GPIO_PIN_RESET );\
                                            HAL_Delay(10);


#define WIFI_DISABLE_NSS()                  HAL_GPIO_WritePin( WIFI_NSS_GPIO_Port, WIFI_NSS_Pin, GPIO_PIN_SET );\
                                            HAL_Delay(10);


#define WIFI_IS_CMDDATA_READY()             (HAL_GPIO_ReadPin(WIFI_CMD_DATA_READY_GPIO_Port, WIFI_CMD_DATA_READY_Pin) == GPIO_PIN_SET)

#define WIFI_DELAY(ms)						HAL_Delay(ms);


/* Variables -----------------------------------------------------------------*/
char wifiTxBuffer[WIFI_TX_BUFFER_SIZE];
char wifiRxBuffer[WIFI_RX_BUFFER_SIZE];


/* Structs and Enums ---------------------------------------------------------*/
typedef enum
{
  WIFI_OK		= HAL_OK,
  WIFI_ERROR	= HAL_ERROR,
  WIFI_BUSY		= HAL_BUSY,
  WIFI_TIMEOUT  = HAL_TIMEOUT,
} WIFI_StatusTypeDef;

typedef enum
{
  OPEN = 0,
  WEP,
  WPA2_AES,
  WPA_MIXED

} WIFI_SecurityTypeTypeDef;

typedef enum
{
  IP_V4 = 0,
  IP_V6

} WIFI_IPVersionTypeDef;

typedef enum {
  WIFI_TCP_PROTOCOL = 0,
  WIFI_UDP_PROTOCOL,
  WIFI_UDP_LITE_PROTOCOL,
  WIFI_TCP_SSL_PROTOCOL,
  WIFI_MQTT_PROTOCOL
}WIFI_TransportProtocolTypeDef;

typedef enum {
  WIFI_MQTT_SECURITY_NONE = 0,
  WIFI_MQTT_SECURITY_USER_PW,
  WIFI_MQTT_SECURITY_CERT
}WIFI_MQTTSecurityTypeDef;

typedef struct{
	char publishTopic[64];
	char subscribeTopic[64];
	WIFI_MQTTSecurityTypeDef securityMode;
	char userName[32];
	char password[32];
	char clientId[24];
	uint16_t keepAlive;
} WIFI_MQTTTypeDef;

typedef struct
{
  SPI_HandleTypeDef* handle;
  char* ssid;
  char* passphrase;
  WIFI_SecurityTypeTypeDef securityType;
  FlagStatus DHCP;
  WIFI_IPVersionTypeDef ipStatus;
  WIFI_TransportProtocolTypeDef transportProtocol;
  uint16_t port;
  uint16_t remotePort;
  char ipAddress[17];
  char remoteIpAddress[17];
  char networkMask[17];
  char defaultGateway[17];
  char primaryDNSServer[17];
  WIFI_MQTTTypeDef mqtt;
} WIFI_HandleTypeDef;

/* Prototypes ----------------------------------------------------------------*/
WIFI_StatusTypeDef WIFI_SPI_Receive(WIFI_HandleTypeDef* hwifi, char* buffer, uint16_t size);
WIFI_StatusTypeDef WIFI_SPI_Transmit(WIFI_HandleTypeDef* hwifi, char* buffer, uint16_t size);
WIFI_StatusTypeDef WIFI_Init(WIFI_HandleTypeDef* hwifi);
WIFI_StatusTypeDef WIFI_SendATCommand(WIFI_HandleTypeDef* hwifi, char* hCmd, uint16_t sizeCmd, char* hRx, uint16_t sizeRx);
WIFI_StatusTypeDef WIFI_CreateNewNetwork(WIFI_HandleTypeDef* hwifi);
WIFI_StatusTypeDef WIFI_WebServerInit(WIFI_HandleTypeDef* hwifi);
WIFI_StatusTypeDef WIFI_WebServerListen(WIFI_HandleTypeDef* hwifi);
WIFI_StatusTypeDef WIFI_WebServerHandleRequest(WIFI_HandleTypeDef* hwifi, char* req, uint16_t sizeReq, char* res, uint16_t sizeRes);
WIFI_StatusTypeDef WIFI_JoinNetwork(WIFI_HandleTypeDef* hwifi);
WIFI_StatusTypeDef WIFI_MQTTClientInit(WIFI_HandleTypeDef* hwifi);

void trimstr(char* str, uint32_t strSize, char c);


#endif /* INC_WIFI_H_ */
