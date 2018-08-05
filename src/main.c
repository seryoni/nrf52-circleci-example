/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

////////////
// NORDIC //
////////////

#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "app_uart.h"
#include "nm_common.h"
#include "app_timer.h"
#include "m2m_wifi.h"
#include "socket.h"
#include "bsp.h"
#include "main.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "AWS_SDK/aws_iot_src/protocol/mqtt/aws_iot_mqtt_interface.h"
#include "bme280.h"
#include "nrf_drv_clock.h"
#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#define UART_TX_BUF_SIZE 256                                                        /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256								                         /**< UART RX buffer size. */


void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}

/**
 * @brief BSP events callback.
 */
void bsp_event_callback(bsp_event_t event)
{
    switch (event)
    {
        case BSP_EVENT_KEY_0:
            button_callback_callad = true;
            break;
        default :
            //Do nothing.
            break;
    }
}

/**@brief Function for initializing bsp module.
 */
void bsp_configuration()
{
    uint32_t err_code;
    err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS, bsp_event_callback);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing low frequency clock.
 */
void clock_initialization()
{
    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        // Do nothing.
    }
}

#include <string.h>
#include "main.h"
#include "m2m_wifi.h"
#include "spi_flash.h"

/**@brief Function for initializing bsp module.
 */
void button_configuration() {

    clock_initialization();

    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

//    NRF_LOG_INFO("BSP example started.");
    bsp_configuration();

    err_code = bsp_buttons_enable();
    APP_ERROR_CHECK(err_code);
}

/** Message format definitions. */
typedef struct s_msg_wifi_product {
	uint8_t name[9];
} t_msg_wifi_product;

/** Message format declarations. */
static t_msg_wifi_product msg_wifi_product = {
	.name = MAIN_WIFI_M2M_PRODUCT_NAME,
};

/** Message format declarations. */
static t_msg_wifi_product msg_no_ecoh = {
      .name = NO_ECHO,
};

/** Receive buffer definition. */
static uint8_t tcpRecvBuffer[MAIN_WIFI_M2M_BUFFER_SIZE];
static uint8_t udpRecvBuffer[MAIN_WIFI_M2M_BUFFER_SIZE];

/** Socket for client */
static SOCKET tcp_client_socket = -1;

/** Socket for server */
static SOCKET tcp_server_socket = -1;

/** Wi-Fi connection state */
static uint8_t wifi_connected;

volatile uint32_t ms_ticks = 0;

static void close_socket(SOCKET* socket, char* error_msg){
    printf(error_msg);
    close(*socket);
    *socket = -1;
}

static void socket_cb_tcp_client_socket(uint8_t u8Msg, void *pvMsg){
    switch (u8Msg) {

        /* Socket connected */
        case SOCKET_MSG_CONNECT: {
            tstrSocketConnectMsg *pstrConnect = (tstrSocketConnectMsg *)pvMsg;
            if (pstrConnect && pstrConnect->s8Error >= 0) {
                printf("TCP client: Connection successful!\r\n");
                printf("TCP client: Send : '%s' , to echo TCP server\r\n", msg_wifi_product.name);
                send(tcp_client_socket, &msg_wifi_product, sizeof(t_msg_wifi_product), 0);
            } else {
                close_socket(&tcp_client_socket, "TCP client: Connection error!\r\n");
            }
            break;
        }

            /* Message send */
        case SOCKET_MSG_SEND: {
            printf("TCP client: Send successful!\r\n");
            recv(tcp_client_socket, tcpRecvBuffer, sizeof(tcpRecvBuffer), 1);
            break;
        }

            /* Message receive */
        case SOCKET_MSG_RECV: {
            tstrSocketRecvMsg *pstrRecv = (tstrSocketRecvMsg *)pvMsg;
            if (pstrRecv && pstrRecv->s16BufferSize > 0) {
                printf("TCP client: Data received from echo TCP server: ");
                for (int i=0; i<pstrRecv->s16BufferSize; i++) {
                    int char_data = tcpRecvBuffer[i];
                    printf("%c", char_data);
                }
                printf("\r\n");

                printf("TCP client: Send again: '%s' , to echo TCP server\r\n", msg_wifi_product.name);
                send(tcp_client_socket, &msg_wifi_product, sizeof(t_msg_wifi_product), 0);
            } else if(pstrRecv && pstrRecv->s16BufferSize == SOCK_ERR_TIMEOUT) {
                printf("TCP client: Timeout waiting for TCP response\r\n");
            }
            else {
                close_socket(&tcp_client_socket, "TCP client: Error on receiving from TCP server!\r\n");
            }
            break;
        }

        default: break;
    }
}

static void socket_cb_tcp_server_socket(uint8_t u8Msg, void *pvMsg){
    switch (u8Msg) {

        /* Socket bind */
        case SOCKET_MSG_BIND: {
            tstrSocketBindMsg *pstrBind = (tstrSocketBindMsg *)pvMsg;
            if (pstrBind && pstrBind->status == 0) {
                printf("TCP server: Bind successful!\r\n");
                listen(tcp_server_socket, 0);
            } else {
                close_socket(&tcp_server_socket, "TCP server: bind error!\r\n");
            }
            break;
        }

            /* Socket listen */
        case SOCKET_MSG_LISTEN: {
            tstrSocketListenMsg *pstrListen = (tstrSocketListenMsg *)pvMsg;
            if (pstrListen && pstrListen->status == 0) {
                printf("TCP server: listen success!\r\n");
                accept(tcp_server_socket, NULL, NULL);
            } else {
                close_socket(&tcp_server_socket, "TCP server: listen error!\r\n");
            }
            break;
        }

            /* Connect accept */
        case SOCKET_MSG_ACCEPT: {
            tstrSocketAcceptMsg *pstrAccept = (tstrSocketAcceptMsg *)pvMsg;
            if (pstrAccept) {
                printf("TCP server: Accept successful!\r\n");
                accept(tcp_server_socket, NULL, NULL);
                tcp_server_socket = pstrAccept->sock;
                recv(tcp_server_socket, tcpRecvBuffer, sizeof(tcpRecvBuffer), 1);
            } else {
                close_socket(&tcp_server_socket, "TCP server: Accept error!\r\n");
            }
            break;
        }

            /* Message send */
        case SOCKET_MSG_SEND: {
            printf("TCP server: Send successful!\r\n");
            recv(tcp_client_socket, tcpRecvBuffer, sizeof(tcpRecvBuffer), 1);
            break;
        }

            /* Message receive */
        case SOCKET_MSG_RECV: {
            tstrSocketRecvMsg *pstrRecv = (tstrSocketRecvMsg *)pvMsg;
            if (pstrRecv && pstrRecv->s16BufferSize > 0) {
                printf("TCP server: Data received from echo TCP server: ");
                for (int i=0; i<pstrRecv->s16BufferSize; i++) {
                    int char_data = tcpRecvBuffer[i];
                    printf("%c", char_data);
                }
                printf("\r\n");

            } else if(pstrRecv && pstrRecv->s16BufferSize == SOCK_ERR_TIMEOUT) {
                printf("TCP server: Timeout waiting for TCP response\r\n");
            }
            else {
                close_socket(&tcp_server_socket, "TCP server: Error on receiving from TCP client!\r\n");
            }
            break;
        }

        default:break;
    }
}

/**
 * \brief Callback to get the Data from socket.
 *
 * \param[in] sock socket handler.
 * \param[in] u8Msg socket event type. Possible values are:
 *  - SOCKET_MSG_BIND
 *  - SOCKET_MSG_LISTEN
 *  - SOCKET_MSG_ACCEPT
 *  - SOCKET_MSG_CONNECT
 *  - SOCKET_MSG_RECV
 *  - SOCKET_MSG_SEND
 *  - SOCKET_MSG_SENDTO
 *  - SOCKET_MSG_RECVFROM
 * \param[in] pvMsg is a pointer to message structure. Existing types are:
 *  - tstrSocketBindMsg
 *  - tstrSocketListenMsg
 *  - tstrSocketAcceptMsg
 *  - tstrSocketConnectMsg
 *  - tstrSocketRecvMsg
 */
static void socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
    printf("\r\n");

    /* TCP client and server callback */
    if (sock == tcp_client_socket) {
        socket_cb_tcp_client_socket(u8Msg, pvMsg);
    }

    else if (sock == tcp_server_socket) {
        socket_cb_tcp_server_socket(u8Msg, pvMsg);
    }
}

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_RESP_CURRENT_RSSI](@ref M2M_WIFI_RESP_CURRENT_RSSI)
 *  - [M2M_WIFI_RESP_CON_STATE_CHANGED](@ref M2M_WIFI_RESP_CON_STATE_CHANGED)
 *  - [M2M_WIFI_RESP_CONNTION_STATE](@ref M2M_WIFI_RESP_CONNTION_STATE)
 *  - [M2M_WIFI_RESP_SCAN_DONE](@ref M2M_WIFI_RESP_SCAN_DONE)
 *  - [M2M_WIFI_RESP_SCAN_RESULT](@ref M2M_WIFI_RESP_SCAN_RESULT)
 *  - [M2M_WIFI_REQ_WPS](@ref M2M_WIFI_REQ_WPS)
 *  - [M2M_WIFI_RESP_IP_CONFIGURED](@ref M2M_WIFI_RESP_IP_CONFIGURED)
 *  - [M2M_WIFI_RESP_IP_CONFLICT](@ref M2M_WIFI_RESP_IP_CONFLICT)
 *  - [M2M_WIFI_RESP_P2P](@ref M2M_WIFI_RESP_P2P)
 *  - [M2M_WIFI_RESP_AP](@ref M2M_WIFI_RESP_AP)
 *  - [M2M_WIFI_RESP_CLIENT_INFO](@ref M2M_WIFI_RESP_CLIENT_INFO)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type. Existing types are:
 *  - tstrM2mWifiStateChanged
 *  - tstrM2MWPSInfo
 *  - tstrM2MP2pResp
 *  - tstrM2MAPResp
 *  - tstrM2mScanDone
 *  - tstrM2mWifiscanResult
 */
static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
        case M2M_WIFI_RESP_CON_STATE_CHANGED:
        {
            tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
            if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
                printf("wifi_cb: M2M_WIFI_RESP_CON_STATE_CHANGED: CONNECTED\r\n");
                m2m_wifi_request_dhcp_client();
            } else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
                printf("wifi_cb: M2M_WIFI_RESP_CON_STATE_CHANGED: DISCONNECTED\r\n");
                wifi_connected = 0;
                m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
            }

            break;
        }

        case M2M_WIFI_REQ_DHCP_CONF:
        {
            uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
            wifi_connected = 1;
            printf("wifi_cb: M2M_WIFI_REQ_DHCP_CONF: IP is %u.%u.%u.%u\r\n",
                    pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
            break;
        }

        default:
        {
            break;
        }
	}
}


void wifi_configuration() {
    // APP
    tstrWifiInitParam param;
    // APP_ERROR_CHECK(nm_bsp_init());

    /* Initialize Wi-Fi parameters structure. */
    memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));

    /* Initialize Wi-Fi driver with data and status callbacks. */
    param.pfAppWifiCb = wifi_cb;
    int8_t ret = m2m_wifi_init(&param);
    if (M2M_SUCCESS != ret) {
    //  printf("main: m2m_wifi_init call error!(%d)\n", ret);
        while (1) {
        }
    }

    /* Request scan. */
    /* Initialize socket module */
    socketInit();
    registerSocketCallback(socket_cb, NULL);

    uint32 flash_size = spi_flash_get_size();
    printf("main: flash size %d\n", (int) flash_size);
    /* Connect to router. */
    m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
}


////////////////
// END OF APP //
////////////////

int main(void)
{
	uint32_t err_code;
    button_configuration();


    MQTTConnectParams connectParams = MQTTConnectParamsDefault;
//    connectParams.port = 1883;
    IoT_Error_t rc = NONE_ERROR;

    connectParams.KeepAliveInterval_sec = 10;
	connectParams.isCleansession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = (char*)"arn:aws:iot:us-east-1:617413614608:thing/TemperatureSensor";
	connectParams.pHostURL = "a1rxr3meyz7der.iot.us-east-1.amazonaws.com";
	connectParams.port = 8883;
	connectParams.isWillMsgPresent = false;
	connectParams.pRootCALocation = "root-CA.crt";
	connectParams.pDeviceCertLocation = "TemperatureSensor.cert.pem";
	connectParams.pDevicePrivateKeyLocation = "TemperatureSensor.private.key";
	connectParams.mqttCommandTimeout_ms = 5000;
	connectParams.tlsHandshakeTimeout_ms = 5000;
	connectParams.isSSLHostnameVerify = true; // ensure this is set to true for production
//	connectParams.disconnectHandler = disconnectCallbackHandler;

//    const app_uart_comm_params_t comm_params =
//      {
//          RX_PIN_NUMBER,
//          TX_PIN_NUMBER,
//          RTS_PIN_NUMBER,
//          CTS_PIN_NUMBER,
//          APP_UART_FLOW_CONTROL_DISABLED,
//          false,
//          NRF_UARTE_BAUDRATE_115200
//      };
//
//    APP_UART_FIFmO_INIT(&comm_params,
//                         UART_RX_BUF_SIZE,
//                         UART_TX_BUF_SIZE,
//                         uart_error_handle,
//                         APP_IRQ_PRIORITY_LOWEST,
//                         err_code);
//
//    APP_ERROR_CHECK(err_code);

    bme_start();
//    wifi_configuration();

    while (1) {
        // /* Handle pending events from network controller. */
        // m2m_wifi_handle_events(NULL);
        // if (wifi_connected == M2M_WIFI_CONNECTED) {
        //     printf("Connecting...");
        //     rc = aws_iot_mqtt_connect(&connectParams);
        //     if (NONE_ERROR != rc) {
        //         printf("Error(%d) connecting to %s:%d", rc, connectParams.pHostURL, connectParams.port);
        //         break;
        //     }
        // }
        nrf_delay_ms(50);
        if(button_callback_callad) {
            /* Delay while the sensor completes a measurement */
            printf("Temperature: %d\r\n", bme280_get_temperature());
            button_callback_callad = false;
        }
    }
}
