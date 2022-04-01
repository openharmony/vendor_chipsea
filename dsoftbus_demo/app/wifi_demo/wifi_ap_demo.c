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
#include "wifi_device.h"
#include "wifi_error_code.h"
#include "wifi_hotspot.h"
#include "rtos_al.h"
#include "securec.h"
#include "dbg.h"
#include "ohos_init.h"

#define AP_SSID "chipsea_test"
#define AP_PSK "12345678"
#define CHANNEL_NUM 7

static void OnHotspotStaJoinHandler(StationInfo *info)
{
    if (info == NULL) {
        dbg("HotspotStaJoin:info is null.\r\n");
        return;
    }
    StationInfo stainfo[WIFI_MAX_STA_NUM] = {0};
    StationInfo *sta_list_node = NULL;
    unsigned int size = WIFI_MAX_STA_NUM;

    // 获取当前热点sta列表
    int error = GetStationList(stainfo, &size);
    if (error != WIFI_SUCCESS) {
        dbg("HotspotStaJoin:get list fail, error is %d.\r\n", error);
        return;
    }
    sta_list_node = stainfo;
    for (uint32_t i = 0; i < size; i++, sta_list_node++) {
        unsigned char *mac = sta_list_node->macAddress;
        dbg("HotspotSta[%u]: macAddress=%02X:%02X:%02X:%02X:%02X:%02X\r\n", i,
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
}

// sta离开回调函数
static void OnHotspotStaLeaveHandler(StationInfo *info)
{
    if (info == NULL) {
        dbg("HotspotStaLeave:info is null.\r\n");
        return;
    }
    unsigned char *mac = info->macAddress;
    dbg("HotspotStaLeave: macAddress=%02X:%02X:%02X:%02X:%02X:%02X, reason=%d.\r\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
           info->disconnectedReason);
}

// 热点状态变化回调函数
static void OnHotspotStateChangedHandler(int state)
{
    dbg("HotspotStateChanged:state is %d.\r\n", state);
    if (state == WIFI_HOTSPOT_ACTIVE) {
        dbg("wifi hotspot active.\r\n");
    } else {
        dbg("wifi hotspot noactive.\r\n");
    }
}

WifiEvent g_wifiEventHandler = {
    .OnHotspotStaJoin = OnHotspotStaJoinHandler,
    .OnHotspotStaLeave = OnHotspotStaLeaveHandler,
    .OnHotspotStateChanged = OnHotspotStateChangedHandler,
};

static void WifiApEntry(void)
{
    WifiErrorCode error;
    osDelay(2000);

    //注册wifi事件的回调函数
    error = RegisterWifiEvent(&g_wifiEventHandler);
    if (error != WIFI_SUCCESS) {
        dbg("RegisterWifiEvent failed, error = %d.\r\n", error);
        return;
    }
    dbg("RegisterWifiEvent succeed!\r\n");

    HotspotConfig config = {0};
    strcpy_s(config.ssid, WIFI_MAX_SSID_LEN, AP_SSID);
    strcpy_s(config.preSharedKey, WIFI_MAX_KEY_LEN, AP_PSK);
    config.securityType = WIFI_SEC_TYPE_PSK;
    config.band = HOTSPOT_BAND_TYPE_2G;
    config.channelNum = CHANNEL_NUM;

    // 设置热点配置信息
    error = SetHotspotConfig(&config);
    if (error != WIFI_SUCCESS) {
        dbg("SetHotspotConfig failed, error = %d.\r\n", error);
        return;
    }
    dbg("SetHotspotConfig succeed!\r\n");

    // 打开热点
    error = EnableHotspot();
    if (error != WIFI_SUCCESS) {
        dbg("EnableHotspot failed, error = %d.\r\n", error);
        return;
    }
    dbg("EnableHotspot succeed!\r\n");

    //检查热点是否已开启
    if (IsHotspotActive() == WIFI_HOTSPOT_NOT_ACTIVE) {
        dbg("Wifi Hotspot is not actived.\r\n");
        return;
    }

    while(1) {
        dbg("Wifi Hotspot is actived!\r\n");
        osDelay(5000);
    }

}

void WifiApDemo(void)
{
    dbg("[%s:%d]: %s\n", __FILE__, __LINE__, __func__);

    if (rtos_task_create(WifiApEntry, "wifi ap demo task", 0, 0x500, NULL, RTOS_TASK_PRIORITY(1), NULL)) {
        return -1;
    }
}

SYS_RUN(WifiApDemo);
