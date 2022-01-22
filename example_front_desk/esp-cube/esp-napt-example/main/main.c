


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <arpa/inet.h>
#include "lwip/opt.h"

#if IP_NAPT
#include "lwip/lwip_napt.h"
#endif

#include "lwip/err.h"
#include "lwip/sys.h"

// WIFI CONFIGURATION
#define EXAMPLE_AP_SSID            CONFIG_AP_SSID
#define EXAMPLE_AP_PASS            CONFIG_AP_PASSWORD
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_STA_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_STA_PASSWORD

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group = NULL;

/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "esp_repeater";

/* Event handler for catching system events */
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        /* Signal main application to continue execution */
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        ESP_LOGI(TAG, "STA Connecting to the AP again...");
    }
}

esp_netif_t *app_wifi_init(wifi_mode_t mode)
{
    if (s_wifi_event_group) {
        return NULL;
    }

    esp_netif_t *wifi_netif = NULL;

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    if (mode & WIFI_MODE_STA) {
        wifi_netif = esp_netif_create_default_wifi_sta();
    }

    if (mode & WIFI_MODE_AP) {
        wifi_netif = esp_netif_create_default_wifi_ap();
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Register our event handler for Wi-Fi, IP and Provisioning related events */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(mode));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_start());

    return wifi_netif;
}

esp_err_t app_wifi_set(wifi_mode_t mode, const char *ssid, const char *password)
{
    wifi_config_t wifi_cfg = {0};

    if (mode & WIFI_MODE_STA) {
        strlcpy((char *)wifi_cfg.sta.ssid, ssid, sizeof(wifi_cfg.sta.ssid));
        strlcpy((char *)wifi_cfg.sta.password, password, sizeof(wifi_cfg.sta.password));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_cfg));

        ESP_LOGI(TAG, "sta ssid: %s password: %s", ssid, password);
    }

    if (mode & WIFI_MODE_AP) {
        wifi_cfg.ap.max_connection = 10;
        wifi_cfg.ap.authmode = strlen(password) < 8 ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
        strlcpy((char *)wifi_cfg.ap.ssid, ssid, sizeof(wifi_cfg.ap.ssid));
        strlcpy((char *)wifi_cfg.ap.password, password, sizeof(wifi_cfg.ap.password));

        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_cfg));

        ESP_LOGI(TAG, "softap ssid: %s password: %s", ssid, password);
    }

    return ESP_OK;
}

esp_err_t app_wifi_set_dhcps(esp_netif_t *netif, uint32_t addr)
{
    esp_netif_dns_info_t dns;
    dns.ip.u_addr.ip4.addr = addr;
    dns.ip.type = IPADDR_TYPE_V4;
    dhcps_offer_t dhcps_dns_value = OFFER_DNS;
    ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER, &dhcps_dns_value, sizeof(dhcps_dns_value)));
    ESP_ERROR_CHECK(esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns));
    return ESP_OK;
}

esp_err_t app_wifi_sta_connected(uint32_t wait_ms)
{
    esp_wifi_connect();
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT,
                        true, true, pdMS_TO_TICKS(wait_ms));
    return ESP_OK;
}

void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    // Setup WIFI
    ESP_LOGI(TAG, "============================");
    ESP_LOGI(TAG, "Wi-Fi Repeater");
    ESP_LOGI(TAG, "============================");

    /* Create STA netif */
    esp_netif_t *sta_wifi_netif = app_wifi_init(WIFI_MODE_STA);
    app_wifi_set(WIFI_MODE_STA, EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    app_wifi_sta_connected(portMAX_DELAY);

    /* Create AP netif  */
    esp_netif_t *ap_wifi_netif = esp_netif_create_default_wifi_ap();

    /* Config dns info for AP */
    esp_netif_dns_info_t dns;
    ESP_ERROR_CHECK(esp_netif_get_dns_info(sta_wifi_netif, ESP_NETIF_DNS_MAIN, &dns));
    ESP_LOGI(TAG, "Main DNS: " IPSTR, IP2STR(&dns.ip.u_addr.ip4));
#if CONFIG_MANUAL_DNS
    ESP_ERROR_CHECK(app_wifi_set_dhcps(ap_wifi_netif, inet_addr(CONFIG_MANUAL_DNS_ADDR)));
    ESP_LOGI(TAG, "Set AP DNS addr(manual): %s", CONFIG_MANUAL_DNS_ADDR);
#else /* using dns from ppp */
    ESP_ERROR_CHECK(app_wifi_set_dhcps(ap_wifi_netif, dns.ip.u_addr.ip4.addr));
    ESP_LOGI(TAG, "Set AP DNS addr(auto): " IPSTR, IP2STR(&dns.ip.u_addr.ip4));
#endif

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    app_wifi_set(WIFI_MODE_AP, EXAMPLE_AP_SSID, EXAMPLE_AP_PASS);

#if IP_NAPT
    u32_t napt_netif_ip = 0xC0A80401; // Set to ip address of softAP netif (Default is 192.168.4.1)
    ip_napt_enable(htonl(napt_netif_ip), 1);
    ESP_LOGI(TAG, "NAT is enabled");
#endif

    while (1) {
        vTaskDelay(1000);
    }
}
