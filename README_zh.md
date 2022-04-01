# vendor_chipsea

## 介绍

该仓库托管芯海科技开发的产品示例代码，主要包括:
1. 连接示例代码。
2. XTS示例代码。
3. 软总线示例代码。

该仓库在整个Openharmony架构中主要用于配置产品信息(config.json)，比如：产品的名称、开发板的名称、公司名称以及产品包含的
子系统列表。Openharmony构建系统会根据该配置来生成构建使用的基础信息。

## 软件架构

支持基于芯海CST85F01的cst85_wblink单板进行开发的产品示例。

代码路径：

```
vendor/chipsea/         // vendor_chipsea 仓的仓库路径
├── dsoftbus_demo       // 软总线示例代码
├── iotlink_demo        // 连接示例代码
└── xts_demo            // XTS示例代码

```
* 软总线的示例主要用于演示验证软总线的服务启动功能，其依赖于软总线的框架。
* 连接示例主要用于演示WIFI的STATION和AP的功能。
* XTS示例主要用于进行XTS的测试验证。

## 安装教程

参考 [安装调试教程](https://gitee.com/openharmony-sig/device_soc_chipsea/blob/master/README_zh.md)

## 使用说明


## 贡献

[如何参与](https://gitee.com/openharmony/docs/blob/HEAD/zh-cn/contribute/%E5%8F%82%E4%B8%8E%E8%B4%A1%E7%8C%AE.md)

[Commit message规范](https://gitee.com/openharmony/device_qemu/wikis/Commit%20message%E8%A7%84%E8%8C%83?sort_id=4042860)

## 相关仓

[device_board_chipsea](https://gitee.com/openharmony-sig/device_board_chipsea)

[device_soc_chipsea](https://gitee.com/openharmony-sig/device_soc_chipsea)

