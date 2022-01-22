## Introduction
use gpio to send espnow

chip : esp32s3
button input : GPIO 19

## To Flash

```
idf.py set-target esp32s3
idf flash -p /dev/ttyUSBx monitor
```

## Output log

```
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fcd0108,len:0x1780
load:0x403b6000,len:0x8c8
load:0x403ba000,len:0x2ecc
entry 0x403b61c0
I (25) boot: ESP-IDF v5.0-dev-401-g451ce8a7ed 2nd stage bootloader
I (25) boot: compile time 15:20:57
I (25) boot: chip revision: 0
I (28) boot.esp32s3: Boot SPI Speed : 80MHz
I (33) boot.esp32s3: SPI Mode       : DIO
I (37) boot.esp32s3: SPI Flash Size : 2MB
I (42) boot: Enabling RNG early entropy source...
W (47) bootloader_random: RNG for ESP32-S3 not currently supported
I (54) boot: Partition Table:
I (58) boot: ## Label            Usage          Type ST Offset   Length
I (65) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (73) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (80) boot:  2 factory          factory app      00 00 00010000 00100000
I (88) boot: End of partition table
I (92) esp_image: segment 0: paddr=00010020 vaddr=3c070020 size=140fch ( 82172) map
I (115) esp_image: segment 1: paddr=00024124 vaddr=3fc940f0 size=040e0h ( 16608) load
I (119) esp_image: segment 2: paddr=0002820c vaddr=40374000 size=07e0ch ( 32268) load
I (128) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=6aa18h (436760) map
I (208) esp_image: segment 4: paddr=0009aa40 vaddr=4037be0c size=082e4h ( 33508) load
I (215) esp_image: segment 5: paddr=000a2d2c vaddr=50000000 size=00010h (    16) load
I (222) boot: Loaded app from partition at offset 0x10000
I (222) boot: Disabling RNG early entropy source...
W (225) bootloader_random: RNG for ESP32-S3 not currently supported
I (243) cpu_start: Pro cpu up.
I (243) cpu_start: Starting app cpu, entry point is 0x403751e4
0x403751e4: call_start_cpu1 at /home/lijiaiq/esp/esp-idf/components/esp_system/port/cpu_start.c:164

I (0) cpu_start: App cpu up.
I (257) cpu_start: Pro cpu start user code
I (257) cpu_start: cpu freq: 160000000 Hz
I (258) cpu_start: Application information:
I (260) cpu_start: Project name:     espnow_cube_test
I (266) cpu_start: App version:      1414964
I (271) cpu_start: Compile time:     Nov 24 2021 15:20:49
I (277) cpu_start: ELF file SHA256:  33bfa08f971a2926...
I (283) cpu_start: ESP-IDF:          v5.0-dev-401-g451ce8a7ed
I (290) heap_init: Initializing. RAM available for dynamic allocation:
I (297) heap_init: At 3FC9BCB8 len 00044348 (272 KiB): D/IRAM
I (303) heap_init: At 3FCE0000 len 0000EE34 (59 KiB): STACK/DRAM
I (310) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (316) heap_init: At 600FE000 len 00002000 (8 KiB): RTCRAM
I (323) spi_flash: detected chip: generic
I (327) spi_flash: flash io: dio
W (331) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (345) sleep: Configure to isolate all GPIO pins in sleep state
I (351) sleep: Enable automatic switching of GPIO sleep configuration
I (358) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (401) pp: pp rom version: e7ae62f
I (401) net80211: net80211 rom version: e7ae62f
I (411) wifi:wifi driver task: 3fce55c8, prio:23, stack:6656, core=0
I (411) system_api: Base MAC address is not set
I (411) system_api: read default base MAC address from EFUSE
I (421) wifi:wifi firmware version: 7cb53e4
I (421) wifi:wifi certification version: v7.0
I (421) wifi:config NVS flash: enabled
I (431) wifi:config nano formating: disabled
I (431) wifi:Init data frame dynamic rx buffer num: 32
I (441) wifi:Init management frame dynamic rx buffer num: 32
I (441) wifi:Init management short buffer num: 32
I (441) wifi:Init dynamic tx buffer num: 32
I (451) wifi:Init static tx FG buffer num: 2
I (451) wifi:Init static rx buffer size: 1600
I (461) wifi:Init static rx buffer num: 10
I (461) wifi:Init dynamic rx buffer num: 32
I (461) wifi_init: rx ba win: 6
I (471) wifi_init: tcpip mbox: 32
I (471) wifi_init: udp mbox: 6
I (481) wifi_init: tcp mbox: 6
I (481) wifi_init: tcp tx win: 5744
I (481) wifi_init: tcp rx win: 5744
I (491) wifi_init: tcp mss: 1440
I (491) wifi_init: WiFi IRAM OP enabled
I (501) wifi_init: WiFi RX IRAM OP enabled
I (501) phy_init: phy_version 302,ede2b68,Oct 20 2021,19:34:11
I (561) wifi:mode : softAP (7c:df:a1:e0:39:bd)
I (561) wifi:Total power save buffer number: 16
I (561) wifi:Init max length of beacon: 752/752
I (561) wifi:Init max length of beacon: 752/752
I (571) ESPNOW: espnow [version: 1.0] init
I (571) gpio: GPIO[19]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:2 
I (5571) espnow_example: Start sending broadcast data
```

After pressed the input button:

```
turn on light
I (204731) espnow_example: Turn on cmd
00 00 00 00 01 
I (204731) espnow_example: Send dest addr
ff ff ff ff ff ff 
turn off light
I (204911) espnow_example: Turn off cmd
00 00 00 00 10 
I (204911) espnow_example: Send dest addr
ff ff ff ff ff ff 
```
