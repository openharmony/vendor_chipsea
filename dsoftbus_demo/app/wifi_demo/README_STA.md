# CST85_WBLink开发板Wifi编程开发——热点连接

本文基于CST85_WBLink开发板，演示了如何使用鸿蒙API进行热点连接功能开发。

## API介绍：

### 1.EnableWifi

```c
WifiErrorCode EnableWifi (void)
```

**描述**：使能WIFI设备的STA模式。

**返回值**：接口执行结果

### 2.IsWifiActive

```c
int IsWifiActive(void);
```

**描述**：检查WIFI设备STA模式是否已启用。

**返回值**：接口执行结果

### 3.RegisterWifiEvent

```c
WifiErrorCode RegisterWifiEvent (WifiEvent *event)
```

**描述**：设定wifi设备行为的回调函数。例如：热点扫描结果，连接结果回调等。

**参数**：

| 参数名 | 描述                   |
| ------ | ---------------------- |
| event  | 需要注册的行为回调函数 |

**返回值**：接口执行结果

### 4.Scan

```c
WifiErrorCode Scan(void);
```

**描述**：启动设备扫描热点。

**返回值**：接口执行结果

### 5.GetScanInfoList

```c
WifiErrorCode GetScanInfoList(WifiScanInfo *result, unsigned int *size);
```

**描述**：获取热点扫描结果。

**参数**：

| 参数名 | 描述             |
| ------ | ---------------- |
| result | 扫描返回结果列表 |
| size   | 返回热点个数     |

**返回值**：接口执行结果

### 6.AddDeviceConfig

```c
WifiErrorCode AddDeviceConfig(const WifiDeviceConfig *config, int *result);
```

**描述**：添加用于连接所需的热点设备信息。

**参数**：

| 参数名 | 描述                                            |
| ------ | ----------------------------------------------- |
| config | 热点配置信息                                    |
| result | 系统为该配置项生成的用于后续连接使用的networkid |

**返回值**：接口执行结果

### 7.ConnectTo

```c
WifiErrorCode ConnectTo(int networkId);
```

**描述**：获取热点扫描结果。

**参数**：

| 参数名    | 描述                                       |
| --------- | ------------------------------------------ |
| networkId | 由AddDeviceConfig接口返回的分配的networkId |

**返回值**：接口执行结果

## 流程简介

1. 调用接口EnableWifi使能wifi设备。
2. 通过IsWifiActive检查设备是否工作正常。
3. 将扫描结果回调函数注册到系统中，并且在回调函数中释放等待扫描结果的信号量。
4. 调用Scan接口开始扫描，并且等待信号量。
5. 信号量正常返回则继续调用GetScanInfoList查询扫描结果，否则异常超时退出。
6. 调用接口AddDeviceConfig配置一个测试用AP信息，并获取到分配的network ID。
7. 配置成功返回后ConnectTo到这个net ID。
8. 测试完成

```c
static void OnWifiScanStateChangedHandler(int state, int size)

{

  (void)state;

  if (state == WIFI_STATE_AVAILABLE) {

​    // 接收到扫描结果，释放信号量

​    rtos_semaphore_signal(g_scanSemap, false);

​    dbg("get wifi scan result %d\r\n", size);

  } else {

​    dbg("wifi scan failed\r\n");

  }

}



static void RegistEvent(void)

{

  static WifiEvent wifiEventHandler = {

​    // 扫描结果回调函数

​    .OnWifiScanStateChanged = OnWifiScanStateChangedHandler,

  };



  // 注册回调函数

  WifiErrorCode error = RegisterWifiEvent(&wifiEventHandler);

  if (error != WIFI_SUCCESS) {

​    dbg("register wifi event fail!\r\n");

  } else {

​    dbg("register wifi event succeed!\r\n");

  }

}



static int32_t ScanTest()

{

  unsigned int rstSize = WIFI_SCAN_HOTSPOT_LIMIT;



  dbg("start scan test!\r\n");

  // 启动设备扫描

  if (Scan() != WIFI_SUCCESS) {

​    dbg("StartScan err!\r\n");

​    return -1;

  }



  dbg("wait scan result...!\r\n");

  // 等待扫描结果

  if (rtos_semaphore_wait(g_scanSemap, 10000) != 0) {

​    dbg("StartScan timeout!\r\n");

​    return -1;

  }



  WifiScanInfo *info = (WifiScanInfo *)rtos_malloc(sizeof(WifiScanInfo) * WIFI_SCAN_HOTSPOT_LIMIT);

  if (info == NULL) {

​    dbg("malloc failed\r\n");

​    return -1;

  }



  // 获取扫描结果

  if (GetScanInfoList(info, &rstSize) != WIFI_SUCCESS) {

​    dbg("GetScanInfoList failed\r\n");

​    rtos_free(info);

​    return -1;

  }

  for (unsigned int i = 0; i < rstSize; i++) {

​    dbg("no:%03u, ssid:%-30s, rssi:%5d\r\n", i + 1, info[i].ssid, info[i].rssi);

  }

  rtos_free(info);

  return 0;

}



int32_t ConnectTest()

{

  int netId;

  // 设定连接热点配置信息

  WifiDeviceConfig demoConfig = {

​    .ssid = DEMO_WIFI_SSID,

​    .preSharedKey = DEMO_WIFI_PASSWORD,

​    .securityType = DEMO_WIFI_SECURITYTYPE,

  };



  dbg("start connect test!\r\n");

  // 添加热点配置信息，获取配置对应的networkId

  if (AddDeviceConfig(&demoConfig, &netId) != WIFI_SUCCESS) {

​    printf("AddDeviceConfig failed!\r\n");

​    return -1;

  }

  dbg("Connecting to ssid:[%s],netId:[%d]...\r\n", DEMO_WIFI_SSID, netId);



  // 连接热点

  return ConnectTo(netId);

}



VOID WifiEntry(VOID)

{

  // 延时2s后开始测试

  osDelay(2000);

  unsigned char mac[WIFI_MAC_LEN] = {0};



  // 使能wifi设备

  if (EnableWifi() != WIFI_SUCCESS) {

​    dbg("EnableWifi failed\r\n");

​    goto Exit;

  }



  // 检查wifi设备工作是否正常

  if (IsWifiActive() == WIFI_STA_NOT_ACTIVE) {

​    dbg("Wifi station is not actived.\n");

​    goto Exit;

  }



  // 注册回调事件

  RegistEvent();



  // 获取设备的MAC地址

  if (GetDeviceMacAddress(mac) != 0) {

​    dbg("get mac addr\r\n");

​    goto Exit;

  }

  dbg("MAC ADDRESS:%02x:%02x:%02x:%02x:%02x:%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);



  // 扫描测试

  if (ScanTest() != 0) {

​    dbg("Wifi scan test err.\n");

​    goto Exit;

  }

  dbg("scan test success!\r\n");



  // 连接测试

  if (ConnectTest() != 0) {

​    dbg("Wifi connect test err.\n");

​    goto Exit;

  }

  dbg("connect test success!\r\n");



Exit:

  // 去使能wifi

  (void)DisableWifi();

  while(1);

}
```

## 编译选项

修改热点信息：

```c
#define DEMO_WIFI_SSID "test"

#define DEMO_WIFI_PASSWORD  "12345678"

#define DEMO_WIFI_SECURITYTYPE WIFI_SEC_TYPE_PSK
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
start scan test!
vif_name AIC_CC69
_scanu_start:0,0,0
wait scan result...!
_scanu_confirm:0,0,0
get wifi scan result 5
wifiDevice:(-53 dBm) CH=  1 BSSID=d4:76:a0:3e:16:c0 SSID=chipsea-PC
wifiDevice:(-87 dBm) CH=  2 BSSID=08:01:0f:c4:52:19 SSID=ChinaNet-KuSai
wifiDevice:(-79 dBm) CH=  4 BSSID=50:33:f0:3e:64:a6 SSID=VPS
wifiDevice:(-42 dBm) CH=  6 BSSID=aa:48:2b:61:fb:e5 SSID=Muffin
wifiDevice:(-89 dBm) CH= 11 BSSID=7c:b5:9b:56:44:0f SSID=TP-LINK_440F
no:001, ssid:chipsea-PC                    , rssi:  -53
no:002, ssid:ChinaNet-KuSai                , rssi:  -87
no:003, ssid:VPS                           , rssi:  -79
no:004, ssid:Muffin                        , rssi:  -42
no:005, ssid:TP-LINK_440F                  , rssi:  -89
scan test success!
start connect test!
wifiDevice:AddDeviceConfig success netid = 0
Connecting to ssid:[Muffin],netId:[0]...
Connect Wi-Fi: Muffin, 12345678
vif_name AIC_CC69
WPA task started for interface {FVIF-0}
WPA network 0: created and configured
_scanu_start:1,0,0
_scanu_confirm:1,0,0
_scanu_start:1,1,0
_scanu_confirm:1,1,0
_ps_disable_cfm:800000
_sm_auth_send
_tx cfm:5,80800000
_auth_handler:0
_sm_assoc_req_send
_tx cfm:7,80800000
_assoc_rsp_handler:0
_vif state active:0,1,0,0
_ps_upm_enter:0,94
_connect:1,0,aid=0
Connect 0 48aa:612b:e5fb, sta_index 1
_curr=0, bss=2
_add ptk:0
_add gtk:1
WPA enter FHOST_WPA_STATE_CONNECTED
_ps_enable_cfm:800000
WPA network 0: connected
_curr=2, bss=0
_handle addba_rsp:tid=0, idx=5
DHCP completed: ip=172.20.10.6 gw=172.20.10.1
fhost_reconnect_dhcp_register
connect test success!
_disconnct:1,3
_delete reources
_vif state inactive:0,1
_ps_upm_exit
Disconnect 0 48aa:612b:e5fb
WPA network 0: disconnected
wpa_supplicant_main end
WPA enter FHOST_WPA_STATE_STOPPED
fhost_reconnect_dhcp_registe
```

