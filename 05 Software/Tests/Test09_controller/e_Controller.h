#pragma once

static QueueHandle_t controllerToMain;
static QueueHandle_t mainToController;

void controller(void *parameters) {

  while(true) {
  
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
};

void startController() {
  controllerToMain = xQueueCreate(10, sizeof(Keys_t));
  mainToController = xQueueCreate(10, sizeof(Keys_t));

  xTaskCreatePinnedToCore(controller, "Controller", 1024, NULL, 1, NULL, app_cpu);
};
