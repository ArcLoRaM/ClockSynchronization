/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "../components/gpio_control/gpio_control.h"
#include "esp_log.h"
#include "../components/espnow_simple/espnow_simple.h"
#include <string.h>



#define BLINK_DELAY_MS 2000  // 2 second
static uint8_t s_led_state = 0;
static const char *TAGGPIO = "GPIO";
static const char *TAGESPNOW = "ESP-NOW";
//GPIO
void gpio_blink_task(void* pvParameter)
{
    bool level = false;

    while (1) {
        gpio_control_set(level);
        level = !level;  // Toggle
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
        ESP_LOGI(TAGGPIO, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        s_led_state = !s_led_state;  // Toggle state
    }
}




//ESP-NOW
// Use the global on_data_recv function if needed


//Gateway
void gateway_on_data_recv(const uint8_t *mac, const uint8_t *data, int len)
{
    ESP_LOGI("gateway", "onDataRecv: MAC: " MACSTR ", Data: %.*s",
             MAC2STR(mac), len, data);
}

void esp_now_gateway_task(void *pvParameter)
{    bool level = false;
    espnow_simple_set_recv_cb(gateway_on_data_recv); // Set the callback to handle incoming data
    // This task can be used to handle incoming ESP-NOW messages
    // For now, it just logs the received data
    const uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAGESPNOW, "ESP-NOW Gateway Task started");
    const char *msg = "Hello from ESP32!";

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000)); // Adjust delay as needed
        esp_err_t err = espnow_simple_send(broadcast_mac, (const uint8_t *)msg, strlen(msg));
        if (err == ESP_OK)
        {
            ESP_LOGI(TAGESPNOW, "Message sent successfully");
            gpio_control_set(level);
            level = !level;  // Toggle GPIO state
        }
        else
        {
            ESP_LOGE(TAGESPNOW, "Failed to send message: %s", esp_err_to_name(err));
        }
    }
}

// Relay
    bool relayLevel = false;

void relay_on_data_recv(const uint8_t *mac, const uint8_t *data, int len)
{    relayLevel = !relayLevel;  // Toggle relay state

    gpio_control_set(relayLevel);
    ESP_LOGI(TAGESPNOW, "onDataRecv: MAC: " MACSTR ", Data: %.*s",
             MAC2STR(mac), len, data);
}

void esp_now_relay_task(void *pvParameter)
{
    espnow_simple_set_recv_cb(relay_on_data_recv); // Set the callback to handle incoming data
    vTaskDelay(pdMS_TO_TICKS(2000)); // wait for ESP-NOW to initialize

    ESP_LOGI(TAGESPNOW, "ESP-NOW Relay Task started");
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000)); // Adjust delay as needed
        // The on_data_recv function will handle incoming messages
        // No need to send anything here, just wait for incoming data
    }
}

void app_main(void)
{
    printf("Testing new component\n");
    espnow_simple_init();
    vTaskDelay(pdMS_TO_TICKS(2000)); // wait for ESP-NOW to initialize

    gpio_control_init();


    // xTaskCreate(
    //     esp_now_gateway_task, // Task function
    //     "ESP-NOW Gateway",     // Name
    //     2048,                 // Stack size
    //     NULL,                 // Parameters
    //     5,                    // Priority
    //     NULL                  // Task handle (optional)
    // );


    xTaskCreate(
        esp_now_relay_task,   // Task function
        "ESP-NOW Relay",      // Name
        2048,                 // Stack size
        NULL,                 // Parameters
        5,                    // Priority
        NULL                  // Task handle (optional)
    );

    // /* Print chip information */
    // esp_chip_info_t chip_info;
    // uint32_t flash_size;
    // esp_chip_info(&chip_info);
    // printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
    //        CONFIG_IDF_TARGET,
    //        chip_info.cores,
    //        (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
    //        (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
    //        (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
    //        (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    // unsigned major_rev = chip_info.revision / 100;
    // unsigned minor_rev = chip_info.revision % 100;
    // printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    // if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
    //     printf("Get flash size failed");
    //     return;
    // }

    // printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
    //        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    // printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    // for (int i = 10; i >= 0; i--) {
    //     printf("Restarting in %d seconds...\n", i);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
    // printf("Restarting now.\n");
    // fflush(stdout);
    // esp_restart();
}
