## ESP NAPT Example

This example demonstrates ESP32/S2/C3/S3 works as a Wi-Fi repeater, which connects to the `Router`, then shares the internet through its SoftAP hotspot.

## How to use

1. Please change your `Router's` SSID to `router`, password to `12345678`
2. After ESP32 connected to the `Router`, the hotspot SSID name `esp_repeater` with password `12345678` will be created.
3. Other devices can connect `esp_repeater` to use the internet

## Config example

You can use `menuconfig` to config the Wi-Fi station or softap `SSID` and `password`

## How to build

1. ESP-IDF v4.3 and above versions is suggested, you can refer: https://docs.espressif.com/projects/esp-idf/en/v4.3.1/esp32/get-started/index.html
2. Set the IDF build target to `esp32` `esp32c3` `esp32s2` or `esp32s3`

    ```bash
    idf.py set-target esp32c3
    ```

3. Build, download, check log output

    ```bash
    idf.py build flash monitor
    ```

## Example output

```
I (342) esp_repeater: ============================
I (342) esp_repeater: Wi-Fi Repeater
I (342) esp_repeater: ============================
I (352) pp: pp rom version: 9387209
I (352) net80211: net80211 rom version: 9387209
I (372) wifi:wifi driver task: 3fc9d518, prio:23, stack:6656, core=0
I (372) system_api: Base MAC address is not set
I (372) system_api: read default base MAC address from EFUSE
I (382) wifi:wifi firmware version: f84e709
I (382) wifi:wifi certification version: v7.0
I (392) wifi:config NVS flash: enabled
I (392) wifi:config nano formating: disabled
I (392) wifi:Init data frame dynamic rx buffer num: 32
I (402) wifi:Init management frame dynamic rx buffer num: 32
I (402) wifi:Init management short buffer num: 32
I (412) wifi:Init dynamic tx buffer num: 32
I (412) wifi:Init static tx FG buffer num: 2
I (422) wifi:Init static rx buffer size: 1600
I (422) wifi:Init static rx buffer num: 10
I (422) wifi:Init dynamic rx buffer num: 32
I (432) wifi_init: rx ba win: 6
I (432) wifi_init: tcpip mbox: 32
I (432) wifi_init: udp mbox: 6
I (442) wifi_init: tcp mbox: 6
I (442) wifi_init: tcp tx win: 5744
I (452) wifi_init: tcp rx win: 5744
I (452) wifi_init: tcp mss: 1440
I (452) wifi_init: WiFi IRAM OP enabled
I (462) wifi_init: WiFi RX IRAM OP enabled
I (462) phy_init: phy_version 906,ed5370c,Nov  8 2021,18:01:10
I (512) wifi:set rx active PTI: 0, rx ack PTI: 0, and default PTI: 0
I (512) wifi:mode : sta (7c:df:a1:61:bb:04)
I (512) wifi:enable tsf
I (512) esp_repeater: sta ssid: router password: 12345678
I (2272) wifi:new:<13,0>, old:<1,0>, ap:<255,255>, sta:<13,0>, prof:1
I (2842) wifi:state: init -> auth (b0)
I (2852) wifi:state: auth -> assoc (0)
I (3052) wifi:state: assoc -> run (10)
I (3102) wifi:connected with router, aid = 1, channel 13, BW20, bssid = be:2b:42:5f:72:36
I (3102) wifi:security: WPA2-PSK, phy: bgn, rssi: -8
I (3102) wifi:pm start, type: 1

I (3102) wifi:set rx beacon pti, rx_bcn_pti: 0, bcn_timeout: 0, mt_pti: 25000, mt_time: 10000
W (3132) wifi:<ba-add>idx:0 (ifx:0, be:2b:42:5f:72:36), tid:0, ssn:0, winSize:64
I (3192) wifi:BcnInt:102400, DTIM:2
I (3852) esp_netif_handlers: sta ip: 192.168.159.28, mask: 255.255.255.0, gw: 192.168.159.72
I (3852) esp_repeater: Connected with IP Address:192.168.159.28
I (3852) esp_repeater: Main DNS: 192.168.159.72
I (3862) esp_repeater: Set AP DNS addr(manual): 114.114.114.114
I (3862) wifi:mode : sta (7c:df:a1:61:bb:04) + softAP (7c:df:a1:61:bb:05)
I (3872) wifi:Total power save buffer number: 16
I (3872) wifi:Init max length of beacon: 752/752
I (3882) wifi:Init max length of beacon: 752/752
I (4452) wifi:Total power save buffer number: 16
I (4462) esp_repeater: softap ssid: esp_repeater password: 12345678
I (4462) esp_repeater: NAT is enabled
```