# CST85_WBLink开发板Wifi编程开发——启用热点

本文基于CST85_WBLink开发板，演示了如何使用鸿蒙API进行热点功能开发。

### 1.RegisterWifiEvent

```c
WifiErrorCode RegisterWifiEvent (WifiEvent *event)
```

**描述**：设定wifi设备行为的回调函数。例如：热点扫描结果，连接结果回调等。

**参数**：

| 参数名 | 描述                   |
| ------ | ---------------------- |
| event  | 需要注册的行为回调函数 |

**返回值**：接口执行结果

### 2.SetHotspotConfig

```c
WifiErrorCode SetHotspotConfig(const HotspotConfig* config)
```

**描述**：设置热点配置信息。

**参数**：

| 参数名 | 描述         |
| ------ | ------------ |
| config | 热点配置信息 |

**返回值**：接口执行结果

### 3.EnableHotspot

```c
WifiErrorCode EnableHotspot(void)
```

**描述**：打开热点。

**返回值**：接口执行结果

### 4.IsHotspotActive

```c
int IsHotspotActive(void);
```

**描述**：检查热点是否开启。

**返回值**：接口执行结果

### 4.DisableHotspot

```c
WifiErrorCode DisableHotspot(void);
```

**描述**：关闭热点。

**返回值**：接口执行结果

## 流程简介

1. 注册AP回调函数接口，包括sta连接，sta离开以及热点打开三个接口。
2. 调用SetHotspotConfig配置热点信息。
3. 通过IsHotspotActive检查热点是否正常打开。
4. 测试热点加入场景，回调OnHotspotStaJoinHandler函数，在该函数中通过GetStationList打印当前sta列表。
5. 测试热点离开场景，回调OnHotspotStaLeaveHandler，在该函数中打印离开热点信息。
6. 测试完成

```c
// sta加入回调函数

static void OnHotspotStaJoinHandler(StationInfo *info)

{

  if (info == NULL) {

​    dbg("HotspotStaJoin:info is null.\r\n");

​    return;

  }

  StationInfo stainfo[WIFI_MAX_STA_NUM] = {0};

  StationInfo *sta_list_node = NULL;

  unsigned int size = WIFI_MAX_STA_NUM;



  // 获取当前热点sta列表

  int error = GetStationList(stainfo, &size);

  if (error != WIFI_SUCCESS) {

​    dbg("HotspotStaJoin:get list fail, error is %d.\r\n", error);

​    return;

  }

  sta_list_node = stainfo;

  for (uint32_t i = 0; i < size; i++, sta_list_node++) {

​    unsigned char *mac = sta_list_node->macAddress;

​    dbg("HotspotSta[%u]: macAddress=%02X:%02X:%02X:%02X:%02X:%02X\r\n", i,

​        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  }

}



// sta离开回调函数

static void OnHotspotStaLeaveHandler(StationInfo *info)

{

  if (info == NULL) {

​    dbg("HotspotStaLeave:info is null.\r\n");

​    return;

  }

  unsigned char *mac = info->macAddress;

  dbg("HotspotStaLeave: macAddress=%02X:%02X:%02X:%02X:%02X:%02X, reason=%d.\r\n",

​      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],

​      info->disconnectedReason);

}



// 热点状态变化回调函数

static void OnHotspotStateChangedHandler(int state)

{

  dbg("HotspotStateChanged:state is %d.\r\n", state);

  if (state == WIFI_HOTSPOT_ACTIVE) {

​    dbg("wifi hotspot active.\r\n");

  } else {

​    dbg("wifi hotspot noactive.\r\n");

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

​    dbg("RegisterWifiEvent failed, error = %d.\r\n", error);

​    return;

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

​    dbg("SetHotspotConfig failed, error = %d.\r\n", error);

​    return;

  }

  dbg("SetHotspotConfig succeed!\r\n");



  // 打开热点

  error = EnableHotspot();

  if (error != WIFI_SUCCESS) {

​    dbg("EnableHotspot failed, error = %d.\r\n", error);

​    return;

  }

  dbg("EnableHotspot succeed!\r\n");



  //检查热点是否已开启

  if (IsHotspotActive() == WIFI_HOTSPOT_NOT_ACTIVE) {

​    dbg("Wifi Hotspot is not actived.\r\n");

​    return;

  }

  dbg("Wifi Hotspot is actived!\r\n");

}
```

## 编译选项

修改热点信息：

```c
#define AP_SSID "chipsea_test"
#define AP_PSK "12345678"
#define CHANNEL_NUM 7
```

打开编译选项

将<u>vendor/chipsea/weblink_demo/app/BUILD.gn</u>文件下的false修改为true以打开编译

```makefile
declare_args() {

 wifi_sta_demo = false

 wifi_ap_demo = false

}
```

## 运行结果

```
wifiDevice: RegisterWifiEvent!
RegisterWifiEvent succeed!
WifiAp:set akm =10
SetHotspotConfig succeed!
WPA task started for interface {FVIF-0}
WPA network 0: created and configured
_ps_disable_cfm:800000
_add gtk:1
WPA enter FHOST_WPA_STATE_CONNECTED
WPA network 0: connected
dhcpServerStart 0 0, 0
DHCPS init: ip=192.168.88.1
HotspotStateChanged:state is 1.
wifi hotspot active.
EnableHotspot succeed!
Wifi Hotspot is actived!
_add gtk:1
_add ptk:0
station join: mac->C2:67:83:F6:B6:9B ip-> 192.168.88.10,cnt= 1
HotspotSta[0]: macAddress=C2:67:83:F6:B6:9B
dhcps: ack C2:67:83:F6:B6:9B -> 192.168.88.10
```

