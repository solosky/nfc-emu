# nfc-emu


这是一个可以模拟NTAG215的模拟器。
可以用来模拟Amiibo和门禁卡。
硬件理论上支持Mifare的卡片，但目前固件不支持。

## 功能

* 支持最多10张卡模拟
* 支持模拟Amiibo
* 4秒无响应自动断电

## PCB 

| RevA | RevB |
| --- | --- |
| ![image](https://raw.githubusercontent.com/solosky/nfc-emu/main/assets/nfc-emu-revA.jpg) | ![image](https://raw.githubusercontent.com/solosky/nfc-emu/main/assets/nfc-emu-revB.jpg) | 
| 可用 | 和RevA相比新增了CH343P串口芯片，可以直接通过USB下载固件。注意：RevB未能完整测试，使用慎重！！|

## 演示

[![](https://bb-embed.herokuapp.com/embed?v=BV1D24y1Z7Rh)](https://player.bilibili.com/player.html?aid=688118575&bvid=BV1D24y1Z7Rh&cid=835047325&page=1)


## Arduino

因为用了非标准的13.56M的晶振，需要新建一个board。
在 C:\Users\{{用户名}}\.platformio\platforms\atmelavr\boards下面新建一个文件 ATmega328PNfcEmu.json， 内容如下：

```
{
  "build": {
    "core": "MiniCore",
    "extra_flags": "-DARDUINO_AVR_ATmega328P",
    "f_cpu": "13560000L",
    "mcu": "atmega328p",
    "variant": "standard"
  },
  "bootloader": {
    "led_pin": "B5"
  },
  "debug": {
    "simavr_target": "atmega328p",
    "avr-stub": {
      "speed": 115200
    }
  },
  "frameworks": [
    "arduino"
  ],
  "name": "ATmega328P/PA NFC EMU",
  "upload": {
    "maximum_ram_size": 2048,
    "maximum_size": 32768,
    "protocol": "arduino",
    "require_upload_port": true,
    "speed": 115200
  },
  "url": "https://www.microchip.com/wwwproducts/en/ATmega328P",
  "vendor": "Microchip"
}
```
修改platformio.ini里面的串口为实际使用的串口号。


## 固件编译

需要下载VScode，然后安装platformio插件。
下载安装好arduino framework。点击编译下载即可。


## 刷入Bootloader

需要的软件：progisp 
需要的硬件：USB ISP
* assets/optiboot_flash_atmega328p_UART0_115200_13560000L_B1.hex 刷入MCU中。
* 修改熔丝位：低位值：0xCE, 高位值：0xDE，扩展位：0xFF
![image](https://raw.githubusercontent.com/solosky/nfc-emu/main/assets/fuse.png)

连接左边标记为ISP的接口到USB ISP烧写器上，接线顺序如下：

CLK MOSI MISO RESET GND 

注意需要把电池安装上，切下载的时候需要一直按住中键打开电源。


刷入步骤：
 1. 先点击 编程熔丝 后面的框，弹出熔丝对话框，点击读出按钮读出当前值，然后依次输入 低位值：0xCE, 高位值：0xDE，扩展位：0xFF，点击写入执行写入操作
 2. 点击文件 -> 调入flash, 选择assets/optiboot_flash_atmega328p_UART0_115200_13560000L_B1.hex，点击自动按钮写入bootloader
 
## 刷入固件

### RevA 

需要额外一个带DTR信号的USB转串口的模块，从左到右按如下的顺序从右边的UART接口连接USB串口模块。

+5V TXD RXD DTR GND 

注意：TXD连串口模块的RXD，RXD连串口模块的TXD。


### RevB 

直接插上USB线，系统自动识别出串口设备。

### 刷入步骤

1. 下载vscode, 安装platformio插件，安装arduino框架
2. 修改platformio.ini中的 upload_port，monitor_port的值为设备的串口号
3. 点击编译和下载到设备中



## 支持的卡

硬件上支持14443-3A的低速卡，比如NTAG215, Mifare等常见卡。
软件层面目前只支持模拟NTAG215，Mifare卡暂时不支持。

## 功耗

目前使用中耗电量为22mA，CR2032勉强能使用，还需要进一步优化电路减少功耗。

因为设计了断电电路，不使用基本不耗电，使用时间取决于使用次数。

注意：一定要使用南孚的传应CR2032电池，其他电池不一定能提供这么大的放电电流！

## 写入TAG

如果手机支持NFC，可以下载如下软件写入Amiibo:

iOS: AppleStore搜索Umiibo写入。
Umiibo是收费软件，但只需要6元RMB，没有应用内购买，很良心的APP的，强烈推荐购买。

Andriod: https://github.com/HiddenRamblings/TagMo/releases 下载最新版本的TAGMO安装。


如果以上都没有，可以购买PN532模块，然后安装Linux虚拟机，将PN532 USB设备接入虚拟机后，执行如下命令写入标签。

### 安装libnfc 

```
sudo apt update
sudo apt install libnfc-dev
```

### Amiibo写入
```
nfc-mfultralight w "SSB Zelda.bin" --full
NFC device: pn532 opened
1 ISO14443A passive target(s) found:
	040402cb5d6480
Using MIFARE Ultralight card with UID: 040402cb5d6480
WARNING: Tag is EV1 or NTAG - PASSWORD may be required
NTAG Type: NTAG215 (504 user bytes)
Checking if UL badge is DirectWrite...
 Original Block 0 (Pages 0-2): 0404028acb5d648072480000
 Original UID: 040402cb5d6480
 Attempt to write Block 0 (pages 0-2) ...
  Writing Page 0: 04 04 02 8a
  Writing Page 1: cb 5d 64 80
  Writing Page 2: 72 48 00 00
 Block 0 written successfully
Card is DirectWrite
Writing 135 pages |.......................................................................................................................................|
Done, 135 of 135 pages written (0 pages skipped, 0 pages failed).

```

默认固件所有标签都是空白的，需要用APP写入标签才能使用。

## 操作说明

* 关机状态下短按中键开机，电源指示灯亮起
* 开机状态下短按上，下键切换标签
* 开机状态下长按中键2秒以上重置标签，uid也会随机生成
* 开机状态下靠近读卡器读取标签 

可以使用重置标签操作结合APP来无限刷Amiibo!

注意：写入和重置标签会产生Flash的写入操作，一般来说Flash的写入操作是有次数限制的，按ATMEGA328p的手册来说，每扇区至少有1000次写入寿命，所以每个标签的最多只能刷写500次左右，超过这个次数当前标签的数据可能永久无法写入。

后续可以考虑实现写均衡来充分利用剩余未使用的空间来提高标签的写入次数。
 
# 感谢 

此项目是移植 [Simple NFC](https://github.com/Nonannet/simple-nfc)，主要的模拟代码来源SimpleNFC，感谢 @Nonannet 提供的代码，非常棒的实现！

# 欢迎加群讨论 

QQ群：109761876
