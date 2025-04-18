#include "freertos/idf_additions.h"
#include "portmacro.h"
#include "cJSON.h"
#include "wifi/wifi.h"

#define SSID "ivdc"
#define PASSWORD "hellhell"

void app_main(void) {
  if (!init_wifi()) {
    printf("Cannot intialize wifi\n");
    esp_restart();
  }
  wifi_config_t config = {.sta = {
                              .ssid = SSID,
                              .password = PASSWORD,
                          }};
  WiFi wifi = create_wifi(config);
  if (wifi.ip == NULL) {
    printf("Cannot connect to WIFI\n");
    esp_restart();
  }
  printf("ip-%s\n", wifi.ip);

  cJSON* object = cJSON_CreateObject();
  cJSON* stats = cJSON_CreateObject();
  cJSON_AddItemToObject(object, "name", cJSON_CreateString("GP"));
  cJSON_AddItemToObject(object, "stats", stats);
  cJSON_AddItemToObject(stats, "age", cJSON_CreateNumber(18));
  cJSON_AddItemToObject(stats, "best-anime", cJSON_CreateString("Attack On Titan"));
  printf("%s\n", cJSON_Print(object));
  cJSON_Delete(object);

  while (1) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
