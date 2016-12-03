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

/* Dummy Weak Functions that need to be provided by user */
void usbd_cls_init()
{

}


void USBD_CLS_SOF_Event()
{
	// Start of Frame event? This should be called once every millisecond on Full Speed USB.
}
