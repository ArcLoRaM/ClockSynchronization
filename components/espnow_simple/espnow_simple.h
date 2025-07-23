#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*espnow_recv_cb_t)(const uint8_t *mac_addr, const uint8_t *data, int len);

// Initialize Wi-Fi + ESP-NOW
esp_err_t espnow_simple_init(void);

// Send data to another ESP32
esp_err_t espnow_simple_send(const uint8_t *mac_addr, const uint8_t *data, size_t len);

void espnow_simple_set_recv_cb(espnow_recv_cb_t cb);


#ifdef __cplusplus
}
#endif
