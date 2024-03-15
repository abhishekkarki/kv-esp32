#include <esp_log.h>
#include <stdbool.h>
// TODO 1.0: Add missing includes here
// TODO 2.1: More missing includes

// Adapt these macro definitions using `idf.py menuconfig`
#define LED_GPIO CONFIG_LED_GPIO
// TODO add your other macro definitions here

static const char *TAG = "LED";

/**
 * Blinks the LED on and off forever.
 */
static void blink() {
    // TODO 1.2: Implement the loop
    while (true) {
        // Blink on (output high)

        // Blink off (output low)
    }
}

/**
 * Runs various LED effects on the WS2812B LED strip.
 * Effect can be changed by pressing the button.
 */
static void led_strip() {
    /*
     * TODO 3.2: implement your creative LED strip effects here. You should be able to cycle through them with the press
     * of a button. You may make use of `color.h`
     */
}

/**
 * Entrypoint. Called from bootloader and is where all application logic is executed from.
 */
void app_main(void) {
    ESP_LOGI(TAG, "Setting up...");
    // TODO 1.1: Set up the builtin LED GPIO pin as an output
    // TODO 2.1: Comment out the builtin LED GPIO setup and initialize the LED strip (create a config struct on the
    // stack and pass it by pointer to ws2812b_init...)
    // TODO 3.1: Set up the button GPIO pin as an input
    ESP_LOGI(TAG, "Setup complete!");

    blink();
    // TODO 2.1: Comment out the line above and comment in the line below. We'll now be working with the LED strip :D
    //    led_strip();
}
