#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_GPIO CONFIG_LED_GPIO  // Adjust this if your board has the LED on a different pin

void app_main(void) {
    // Set the LED GPIO as an output
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    
    while (true) {
        // Turn the LED on
        gpio_set_level(LED_GPIO, 1);
        ESP_LOGI("BLINK", "LED ON");
        vTaskDelay(500 / portTICK_PERIOD_MS);  // Delay for 500 ms

        // Turn the LED off
        gpio_set_level(LED_GPIO, 0);
        ESP_LOGI("BLINK", "LED OFF");
        vTaskDelay(500 / portTICK_PERIOD_MS);  // Delay for 500 ms
    }
}


/*
cmakelist.txt for this program
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver)

# Add Kconfig.projbuild to the build system
project(Kconfig.projbuild)
*/



