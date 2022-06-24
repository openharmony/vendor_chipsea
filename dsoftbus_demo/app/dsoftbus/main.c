/*
 * Copyright (c) 2021 Chipsea Technologies (Shenzhen) Corp., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <string.h>
#include "cmsis_os2.h"
#include "ohos_init.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "wifi_device.h"

#include "softbus_server_frame.h"

#define STACK_SIZE (65536)
#define TASK_PRIORITY (24)

static void DSoftBus(void)
{
    printf("[XXX-XXX %s:%d]: %s\n", __FILE__, __LINE__, __func__);

    osThreadAttr_t attr;
    attr.name = "dsoftbus task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = STACK_SIZE;
    attr.priority = TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t) InitSoftBusServer, NULL, &attr) == NULL) {
        printf("Failed to create WifiSTATask!\n");
    }
}

APP_FEATURE_INIT(DSoftBus);
