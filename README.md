# nfc-emu

![image](https://raw.githubusercontent.com/solosky/nfc-emu/main/assets/nfc-emu-revA.jpg)

这是一个可以模拟NTAG2xx和Mifare的模拟器。
可以用来模拟Amiibo和门禁卡。

## 功能

* 支持最多10张卡模拟
* 支持模拟Amiibo
* 4秒无响应自动断电


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


## 支持的卡

硬件上支持14443-3A的低速卡，比如NTAG2, Mifare等常见卡。
软件层面目前只支持模拟NTAG2。

## 功耗

目前使用中耗电量为22mA，CR2032勉强能使用，还需要进一步优化电路减少功耗。
因为设计了断电电路，不使用基本不耗电，使用时间取决于使用次数。

## 写入TAG

如果手机支持NFC，可以下载如下软件写入Amiibo:

iOS: AppleStore搜索AmiiBoss写入。

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

默认固件第一个标签已经自带了一个Amiibo标签，可以用Switch打开荒野之息测试。