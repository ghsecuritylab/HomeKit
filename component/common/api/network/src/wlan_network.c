/*
 *  Hello World
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" 

#include "main.h"
#include "main_test.h"
#include "wifi_conf.h"
#include "wlan_intf.h"
#include "lwip_netconf.h"
#include "wifi_constants.h"

#include <platform/platform_stdlib.h>
#include "model.h"
#ifdef PHYTREX
#include "phytrex_homekit_flow.h"
#endif

#ifndef CONFIG_INIT_NET
#define CONFIG_INIT_NET             1
#endif
#ifndef CONFIG_INTERACTIVE_MODE
#define CONFIG_INTERACTIVE_MODE     1
#endif

#define STACKSIZE                   (512 + 768)

xSemaphoreHandle uart_rx_interrupt_sema = NULL;

#ifdef PHYTREX
extern HomekitFlow_t hk_flow;
#endif

void init_thread(void *param)
{

#if CONFIG_INIT_NET
#if CONFIG_LWIP_LAYER
	/* Initilaize the LwIP stack */
	LwIP_Init();
#endif
#endif

#if CONFIG_WLAN
	wifi_on(RTW_MODE_STA);
#if CONFIG_AUTO_RECONNECT
	//setup reconnection flag
	wifi_set_autoreconnect(1);
#endif
	printf("\n\r%s(%d), Available heap 0x%x", __FUNCTION__, __LINE__, xPortGetFreeHeapSize());	
#endif

#if CONFIG_INTERACTIVE_MODE
 	/* Initial uart rx swmaphore*/
	vSemaphoreCreateBinary(uart_rx_interrupt_sema);
	xSemaphoreTake(uart_rx_interrupt_sema, 1/portTICK_RATE_MS);
	start_interactive_mode();
#endif	

#if PHYTREX
#if CONFIG_HOMEKIT
        phytrex_homekit_wac(&hk_flow);
#endif
#endif
        
	/* Kill init thread after all init tasks done */
	vTaskDelete(NULL);
}

void wlan_network()
{
	if(xTaskCreate(init_thread, ((const char*)"init"), STACKSIZE, NULL, tskIDLE_PRIORITY + 3 + PRIORITIE_OFFSET, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}
