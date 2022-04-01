/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gpio_if.h"
#include "rtos_al.h"
#include "dbg.h"
#include "ohos_init.h"

#define LED_GPIO 0
/**
 * @brief led task output high and low levels to turn on and off LED
 *
 */
static void LedTask(void)
{
    // set GPIO_2 is output mode
    uint16_t gpioVal;

    while (1) {
        if (GpioRead(LED_GPIO, &gpioVal) == HDF_SUCCESS) {
            if (gpioVal == GPIO_VAL_HIGH) {
                GpioWrite(LED_GPIO, GPIO_VAL_LOW);
            } else {
                GpioWrite(LED_GPIO, GPIO_VAL_HIGH);
            }
        }

        rtos_task_suspend(1000);
    }
}

/**
 * @brief Main Entry of the Led Example
 *
 */
void LedExampleEntry(void)
{
    dbg("[%s:%d]: %s\n", __FILE__, __LINE__, __func__);

    if (rtos_task_create(LedTask, "led demo task", 0, 0x500, NULL, RTOS_TASK_PRIORITY(1), NULL)) {
        return -1;
    }

}

SYS_RUN(LedExampleEntry);