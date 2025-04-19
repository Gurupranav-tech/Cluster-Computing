#include "mqtt.h"
#include "cJSON.h"
#include "driver/gpio.h"
#include "esp_wifi.h"

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data);

void mqtt_init(esp_mqtt_client_config_t *mqtt_cfg, MQTT* mqtt) {

  uint8_t mac[6];
  char mac_str[18];

  esp_wifi_get_mac(WIFI_IF_STA, mac);
  snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],
           mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (int i = 0; i < 18; i++) {
    mqtt->mac_addr[i] = mac_str[i];
  }
  mqtt->mac_addr[18] = '\0';

  esp_mqtt_client_handle_t client = esp_mqtt_client_init(mqtt_cfg);
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler,
                                 mqtt);
  esp_mqtt_client_start(client);
}

static void handle_json_payload(const char* topic, int topic_len, const char *data, int data_len) {
  char* topic_string = strndup(topic, topic_len);
  char *json_string = strndup(data, data_len);

  printf("Topic: %s \t Data: %s\n", topic_string, json_string);

  if (!json_string) {
    printf("Memory allocation failed\n");
    return;
  }

  cJSON *root = cJSON_Parse(json_string);
  if (!root) {
    printf("Failed to parse JSON\n");
    free(json_string);
    return;
  }

  cJSON_Delete(root);
  free(json_string);
  free(topic_string);
}

static void send_macaddr(esp_mqtt_client_handle_t client, MQTT *mqtt) {
  esp_mqtt_client_publish(client, "/topic/identity", mqtt->mac_addr, 0, 0, 0);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data) {
  MQTT mqtt = *(MQTT*)handler_args;
  esp_mqtt_event_handle_t event = event_data;
  esp_mqtt_client_handle_t client = event->client;
  switch ((esp_mqtt_event_id_t)event_id) {
  case MQTT_EVENT_CONNECTED:
    printf("Connected to MQTT\n");
    send_macaddr(client, handler_args);
    char buf[256];
    sprintf(buf, "/topic/%s", mqtt.mac_addr);
    esp_mqtt_client_subscribe(client, buf, 1);
    break;
  case MQTT_EVENT_DISCONNECTED:
    printf("Disconnected from mqtt\n");
    break;
  case MQTT_EVENT_DATA:
    handle_json_payload(event->topic, event->topic_len, event->data, event->data_len);
    break;
  default:
    break;
  }
}
