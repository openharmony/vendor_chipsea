/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "softbus_config_adapter.h"

void SoftbusConfigAdapterInit(const ConfigSetProc *ConfigSets)
{
    // Set ifname.
    unsigned char *ifName = "0:eth0,1:wl1\0";
    int len = 13;
    ConfigSets->SetConfig(SOFTBUS_STR_LNN_NET_IF_NAME, ifName, len);

    // Set max bytes length.
    int32_t max_bytes_length = 4194304;
    ConfigSets->SetConfig(SOFTBUS_INT_MAX_BYTES_LENGTH,
        (unsigned char*)&max_bytes_length, sizeof(max_bytes_length));

    // Set ability collection.
    int32_t ability_collection = 0x1;
    ConfigSets->SetConfig(SOFTBUS_INT_AUTH_ABILITY_COLLECTION,
        (unsigned char*)&ability_collection, sizeof(ability_collection));

    // Set log level.
    int32_t log_level = 3;
    ConfigSets->SetConfig(SOFTBUS_INT_ADAPTER_LOG_LEVEL, (unsigned char*)&log_level, sizeof(log_level));

    // Set networking switch.
    int32_t networking_switch = 0;
    ConfigSets->SetConfig(SOFTBUS_INT_AUTO_NETWORKING_SWITCH,
        (unsigned char*)&networking_switch, sizeof(networking_switch));
}
