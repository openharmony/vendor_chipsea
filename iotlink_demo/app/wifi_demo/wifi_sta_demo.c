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
#include "rtos_al.h"
#include "dbg.h"
#include "wifi_device.h"
#include "wifi_device_config.h"
#include "ohos_init.h"

#define DEMO_WIFI_SSID "test"
#define DEMO_WIFI_PASSWORD  "12345678"
#define DEMO_WIFI_SECURITYTYPE WIFI_SEC_TYPE_PSK
#define TWO_SECONDS (2000)
#define TEN_SECONDS (10000)

rtos_semaphore g_scanSemap;

static void OnWifiScanStateChangedHandler(int state, int size)
{
    (void)state;
    if (state == WIFI_STATE_AVAILABLE) {
        // 接收到扫描结果，释放信号量
        rtos_semaphore_signal(g_scanSemap, false);
        dbg("get wifi scan result %d\r\n", size);
    } else {
        dbg("wifi scan failed\r\n");
    }
}

static void RegistEvent(void)
{
    static WifiEvent wifiEventHandler = {
        // 扫描结果回调函数
        .OnWifiScanStateChanged = OnWifiScanStateChangedHandler,
    };

    // 注册回调函数
    WifiErrorCode error = RegisterWifiEvent(&wifiEventHandler);
    if (error != WIFI_SUCCESS) {
        dbg("register wifi event fail!\r\n");
    } else {
        dbg("register wifi event succeed!\r\n");
    }
}

static int32_t ScanTest(void)
{
    unsigned int rstSize = WIFI_SCAN_HOTSPOT_LIMIT;

    dbg("start scan test!\r\n");
    // 启动设备扫描
    if (Scan() != WIFI_SUCCESS) {
        dbg("StartScan err!\r\n");
        return -1;
    }

    dbg("wait scan result...!\r\n");
    // 等待扫描结果
    if (rtos_semaphore_wait(g_scanSemap, TEN_SECONDS) != 0) {
        dbg("StartScan timeout!\r\n");
        return -1;
    }

    WifiScanInfo *info = (WifiScanInfo *)rtos_malloc(sizeof(WifiScanInfo) * WIFI_SCAN_HOTSPOT_LIMIT);
    if (info == NULL) {
        dbg("malloc failed\r\n");
        return -1;
    }

    // 获取扫描结果
    if (GetScanInfoList(info, &rstSize) != WIFI_SUCCESS) {
        dbg("GetScanInfoList failed\r\n");
        rtos_free(info);
        return -1;
    }
    for (unsigned int i = 0; i < rstSize; i++) {
        dbg("no:%03u, ssid:%-30s, rssi:%5d\r\n", i + 1, info[i].ssid, info[i].rssi);
    }
    rtos_free(info);
    return 0;
}

static int32_t AdvanceScanTest(void)
{
    unsigned int rstSize = WIFI_SCAN_HOTSPOT_LIMIT;
    WifiScanParams params = {
        .scanType = WIFI_SSID_SCAN,
        .ssid = "Muffin",
    };
    params.ssidLen = strlen("Muffin");
    dbg("start scan test!\r\n");
    // 启动设备扫描
    if (AdvanceScan(&params) != WIFI_SUCCESS) {
        dbg("StartScan advance err!\r\n");
        return -1;
    }

    dbg("wait scan result...!\r\n");
    // 等待扫描结果
    if (rtos_semaphore_wait(g_scanSemap, TEN_SECONDS) != 0) {
        dbg("StartScan timeout!\r\n");
        return -1;
    }

    WifiScanInfo *info = (WifiScanInfo *)rtos_malloc(sizeof(WifiScanInfo) * WIFI_SCAN_HOTSPOT_LIMIT);
    if (info == NULL) {
        dbg("malloc failed\r\n");
        return -1;
    }

    // 获取扫描结果
    if (GetScanInfoList(info, &rstSize) != WIFI_SUCCESS) {
        dbg("GetScanInfoList failed\r\n");
        rtos_free(info);
        return -1;
    }
    for (unsigned int i = 0; i < rstSize; i++) {
        dbg("no:%03u, ssid:%-30s, rssi:%5d\r\n", i + 1, info[i].ssid, info[i].rssi);
    }
    rtos_free(info);
    return 0;
}

int32_t ConnectTest(void)
{
    int netId;
    // 设定连接热点配置信息
    WifiDeviceConfig demoConfig = {
        .ssid = DEMO_WIFI_SSID,
        .preSharedKey = DEMO_WIFI_PASSWORD,
        .securityType = DEMO_WIFI_SECURITYTYPE,
    };

    dbg("start connect test!\r\n");
    // 添加热点配置信息，获取配置对应的networkId
    if (AddDeviceConfig(&demoConfig, &netId) != WIFI_SUCCESS) {
        printf("AddDeviceConfig failed!\r\n");
        return -1;
    }
    dbg("Connecting to ssid:[%s],netId:[%d]...\r\n", DEMO_WIFI_SSID, netId);

    // 连接热点
    return ConnectTo(netId);
}

VOID WifiEntry(VOID)
{
    // 延时2s后开始测试
    osDelay(TWO_SECONDS);
    unsigned char mac[WIFI_MAC_LEN] = {0};

    // 使能wifi设备
    if (EnableWifi() != WIFI_SUCCESS) {
        dbg("EnableWifi failed\r\n");
        return;
    }

    // 检查wifi设备工作是否正常
    if (IsWifiActive() == WIFI_STA_NOT_ACTIVE) {
        dbg("Wifi station is not activated.\n");

        (void)DisableWifi();
        return;
    }

    // 注册回调事件
    RegistEvent();

    // 获取设备的MAC地址
    if (GetDeviceMacAddress(mac) != 0) {
        dbg("get mac addr\r\n");

        (void)DisableWifi();
        return;
    }

    // 扫描测试
    if (ScanTest() != 0) {
        dbg("Wifi scan test err.\n");

        (void)DisableWifi();
        return;
    }
    dbg("scan test success!\r\n");

    if (AdvanceScanTest() != 0) {
        dbg("Wifi advance scan test err.\n");

        (void)DisableWifi();
        return;
    }
    dbg("Advance scan test success!\r\n");

    // 连接测试
    if (ConnectTest() != 0) {
        dbg("Wifi connect test err.\n");

        (void)DisableWifi();
        return;
    }
    dbg("connect test success!\r\n");
    osDelay(TEN_SECONDS);

    Disconnect();
    // 去使能wifi
    (void)DisableWifi();
}

int WifiStaDemo(void)
{
    if (rtos_semaphore_create(&g_scanSemap, 1, 0) != 0) {
        return -1;
    }

    if (rtos_task_create(WifiEntry, "Example task", 0, 0x500, NULL, RTOS_TASK_PRIORITY(1), NULL)) {
        return -1;
    }

    return 0;
}

SYS_RUN(WifiStaDemo);
