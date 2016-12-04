/**
 * @file    usbd_vendor.c
 * @brief   Vendor Class driver, used for faster DAPlink
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "string.h"
 
#include "RTL.h"
#include "rl_usb.h"
#include "usb_for_lib.h"
#include "util.h"
#include "macro.h"

U32 BulkLen;    /* Bulk In/Out Length */

void usbd_cls_init()
{

}


void USBD_CLS_SOF_Event()
{
	// Start of Frame event. We don't need to do anything really.	
}

// # How USB works in this crazy macro-riddled library.
//
// Ok so somebody *loves* macros so deciphering this was a pain but anyway, here's how it works.
//
// There are actually two systems, depending on whether or not RTX is enabled (in which case __RTX is #defined).
//
// In any case, the USB interrupt handler - USBD_Handler() can be found in usbd_LPC11Uxx.c:619 (or equivalent for other chips).
// In that function you can see lots of #ifdef __RTX's for each event. If RTX is enabled then events
// trigger isr_evt_set(...), and they are picked up from another task. Otherwise the callback functions
// for each event are called directly. For example for reset events, USBD_P_Reset_Event() is called.
//
// USBD_P_Reset_Event() is redirected to USBD_Reset_Event() which is defined in usb_lib.c:964. It simply calls
// a reset handler event for each of the interfaces, e.g. USBD_MSC_Reset_Event() for mass storage, which is
// defined in usb_msc.c.
//
// The actual IN and OUT packets are handled slightly differently. Looking in USBD_Handler() you can see that
// (without RTX) it calls the function pointers in USBD_P_EP[]. There is one for each endpoint. That array
// is defined in usb_lib.c:1063 as containing USBD_EndPoint0 - USBD_EndPoint15 (the maximum number of endpoints is 16).
//
// Except for USBD_EndPoint0 (which is the special control endpoint), these are macros defined with a mental macro
// system in usb_lib.c to go to the relevant handler function, e.g. USBD_MSC_EP_BULK_Event()
// 
// Note that there are seperate handlers USBD_MSC_EP_BULKIN_Event and USBD_MSC_EP_BULKOUT_Event if the IN and OUT
// endpoints have a different endpoint address. But since endpoints are uniquely identified by their address and direction
// you can have two endpoints with the same address, and that is how it is done here.
// 
// USBD_MSC_EP_BULK_Event() is defined in usb_msc.c and it redirects to USBD_MSC_BulkIn() and USBD_MSC_BulkOut() where
// the real work starts.
//
// Now if RTX is enabled it works differently. Instead of this callback chain an ISR event is set with isr_evt_set().
// This event is picked up from a task (I guess this is like a thread; RTX is some kind of operating system). The tasks are
// all started from USBD_RTX_TaskInit() in usb_lib.c:1456. The task function pointers are stored in an array
// called USBD_RTX_P_EP[]. Like USBD_P_EP[] this has one task per endpoint, and they are similarly directed using an
// unsanely verbose macro system.
//
// With RTX we get to USBD_RTX_MSC_EP_BULK_Event() which is defined near USBD_MSC_EP_BULK_Event() in usb_msc.c.
// This task just loops infinitely with usbd_os_evt_wait_or(0xFFFF, 0xFFFF) and then directly calls the original
// non-RTX callback with the event USBD_MSC_EP_BULK_Event(usbd_os_evt_get()). In this way the RTX and non-RTX versions
// can share their implementation.
//
//
// With our bulk CMSIS-DAP/DAPlink interface we need to call DAP_ProcessCommand() with OUT packets and then send
// the responses with IN packets. We can call main_blink_hi_led(MAIN_LED_OFF) to blink the led.


// CMSIS-DAP task
/*__task void cls_process(void *argv)
{
	while (1) {
		// Process DAP Command
		os_sem_wait(&proc_sem, 0xFFFF);
		DAP_ProcessCommand(USB_Request[proc_idx], temp_buf);
		memcpy(USB_Request[proc_idx], temp_buf, DAP_PACKET_SIZE);
		proc_idx = (proc_idx + 1) % DAP_PACKET_COUNT;
		os_sem_send(&send_sem);
		// Send input report if USB is idle
		os_mut_wait(&hid_mutex, 0xFFFF);

		if (USB_ResponseIdle) {
			USB_ResponseIdle = 0;
			os_sem_wait(&send_sem, 0xFFFF);
			usbd_hid_get_report_trigger(0, USB_Request[send_idx], DAP_PACKET_SIZE);
			send_idx = (send_idx + 1) % DAP_PACKET_COUNT;
			os_sem_send(&free_sem);
		}

		os_mut_release(&hid_mutex);
		main_blink_hid_led(MAIN_LED_OFF);
	}
}*/





/*
 *  USB Device CLS Bulk In Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_CLS_BulkIn(void)
{

}


/*
 *  USB Device CLS Bulk Out Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_CLS_BulkOut(void)
{

}

/*
 *  USB Device MSC Bulk In Endpoint Event Callback
 *    Parameters:      event: not used (just for compatibility)
 *    Return Value:    None
 */

void USBD_CLS_EP_BULKIN_Event(U32 event)
{
	USBD_CLS_BulkIn();
}


/*
 *  USB Device MSC Bulk Out Endpoint Event Callback
 *    Parameters:      event: not used (just for compatibility)
 *    Return Value:    None
 */

void USBD_CLS_EP_BULKOUT_Event(U32 event)
{
	BulkLen = USBD_ReadEP(usbd_cls_ep_bulkout, USBD_CLS_BulkBuf, USBD_CLS_BulkBufSize);
	USBD_CLS_BulkOut();
}


/*
 *  USB Device MSC Bulk In/Out Endpoint Event Callback
 *    Parameters:      event: USB Device Event
 *                       USBD_EVT_OUT: Output Event
 *                       USBD_EVT_IN:  Input Event
 *    Return Value:    None
 */

void USBD_CLS_EP_BULK_Event(U32 event)
{
	if (event & USBD_EVT_OUT) {
		USBD_CLS_EP_BULKOUT_Event(0);
	}

	if (event & USBD_EVT_IN) {
		USBD_CLS_EP_BULKIN_Event(0);
	}
}


#ifdef __RTX                            /* RTX tasks for handling events */

/*
 *  USB Device MSC Bulk In Endpoint Event Handler Task
 *    Parameters:      None
 *    Return Value:    None
 */

__task void USBD_RTX_CLS_EP_BULKIN_Event(void)
{
	for (;;) {
		usbd_os_evt_wait_or(0xFFFF, 0xFFFF);

		if (usbd_os_evt_get() & USBD_EVT_IN) {
			USBD_CLS_EP_BULKIN_Event(0);
		}
	}
}


/*
 *  USB Device MSC Bulk Out Endpoint Event Handler Task
 *    Parameters:      None
 *    Return Value:    None
 */

__task void USBD_RTX_MSC_EP_BULKOUT_Event(void)
{
	for (;;) {
		usbd_os_evt_wait_or(0xFFFF, 0xFFFF);

		if (usbd_os_evt_get() & USBD_EVT_OUT) {
			USBD_CLS_EP_BULKOUT_Event(0);
		}
	}
}


/*
 *  USB Device MSC Bulk In/Out Endpoint Event Handler Task
 *    Parameters:      None
 *    Return Value:    None
 */

__task void USBD_RTX_CLS_EP_BULK_Event(void)
{
	for (;;) {
		usbd_os_evt_wait_or(0xFFFF, 0xFFFF);
		USBD_CLS_EP_BULK_Event(usbd_os_evt_get());
	}
}
#endif

