#include "espnow_simple.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <string.h>
#include "sdkconfig.h"

static const char *TAG = "espnow_simple";
static espnow_recv_cb_t user_recv_cb = NULL;

static void recv_cb(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (user_recv_cb) {
        user_recv_cb(info->src_addr, data, len);
    }
}

// Internal callback registered with esp_now
static void internal_recv_cb(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (!info || !data || len <= 0) {
        ESP_LOGW(TAG, "Received invalid data");
        return;
    }

    if (user_recv_cb) {
        user_recv_cb(info->src_addr, data, len);  // Forward to user callback
    } else {
        ESP_LOGW(TAG, "No user callback registered");
    }
}

esp_err_t espnow_simple_init(void) {
    // Init NVS (Non-Volatile Storage), mandatory for Wi-Fi
    ESP_ERROR_CHECK(nvs_flash_init());

    // Initialize network interface and event loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // Initialize Wi-Fi with default configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Init ESP-NOW and register internal receive callback
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(internal_recv_cb));

    // Add broadcast peer (FF:FF:FF:FF:FF:FF)
    esp_now_peer_info_t peer = {
        .channel = 0,
        .ifidx = ESP_IF_WIFI_STA,
        .encrypt = false,
    };
    memset(peer.peer_addr, 0xFF, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));

    ESP_LOGI(TAG, "ESP-NOW initialized");
    return ESP_OK;
}

esp_err_t espnow_simple_send(const uint8_t *mac_addr, const uint8_t *data, size_t len) {
    if (!mac_addr || !data || len == 0) {
        ESP_LOGE(TAG, "Invalid arguments to espnow_simple_send");
        return ESP_ERR_INVALID_ARG;
    }
    return esp_now_send(mac_addr, data, len);
}

void espnow_simple_set_recv_cb(espnow_recv_cb_t cb) {
    if (cb == NULL) {
        ESP_LOGE(TAG, "Callback function cannot be NULL");
        return;
    }

    user_recv_cb = cb;
    ESP_LOGI(TAG, "Receive callback set");
}