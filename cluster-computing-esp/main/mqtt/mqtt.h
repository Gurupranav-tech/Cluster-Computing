#pragma once
#include "freertos/FreeRTOS.h"
#include "mqtt_client.h"

typedef struct {
  char mac_addr[19];
} MQTT;

void mqtt_init(esp_mqtt_client_config_t* mqtt_cfg, MQTT* mqtt);
