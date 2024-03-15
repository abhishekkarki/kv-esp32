#include "include/ws2812b.h"
#include <driver/rmt_tx.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string.h>

static const char *TAG = "WS2812B";

/**** CONSTANTS ****/
#define PIXEL_BYTES (pixel_count * 3)
#define SYMBOL_BYTES (PIXEL_BYTES * 8 * sizeof(rmt_symbol_word_t)) // 8 bits per byte -> 8 symbols per byte

/**** TIMINGS ****/
// See https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
// Timings are given in 0.1us (we use 10Mhz resolution, round if necessary)
// TODO 2.2: Find out the correct timings and replace the '0's
#define T0H_TIME (0) // 0 bit high time
#define T0L_TIME (0) // 0 bit low time
#define T1H_TIME (0) // 1 bit high time
#define T1L_TIME (0) // 1 bit low time
#define RST_TIME (0) // reset time

/**** GLOBAL STATE ****/
static size_t pixel_count;
static uint8_t *pixel_buffer;
static rmt_symbol_word_t *symbol_buffer;
// TODO 2.3: What else do you need here?

esp_err_t ws2812b_init(const ws2812b_config_t *config) {
    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Initializing WS2812B driver...");
    ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG, "WS2812B driver not implemented yet. REMOVE ME!");

    // TODO 2.3: Save necessary config values in global state if you need them later and initialize everything
    // RMT-related use this RMT channel config
    rmt_tx_channel_config_t rmt_config = {
        .gpio_num = config->data_gpio_num,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10000000, // 10MHz resolution, 1 tick = 0.1us (timings need to be rounded but this is ok)
        .mem_block_symbols = 64,   // increasing this in steps of 64 can reduce flickering
        .trans_queue_depth = 4,    // maximum pending background transfers, 4 is a good default value
        // we leave everything else at default values
    };

    // turn LEDs off in case they were on previously
    ws2812b_clear();
    ws2812b_show();

    ESP_LOGI(TAG, "WS2812B driver initialized");

    return ESP_OK;

err:
    // TODO 2.3: Clean up everything that you've already initialized.
    // Hint: Do you already have this functionality somewhere?
    return ret;
}

void ws2812b_deinit(void) {
    ESP_LOGI(TAG, "Deinitializing WS2812B driver...");

    /*
     * Cleanup routine
     * You should always provide a deinit function to free memory and resources that were previously allocated.
     * In general, everything is deinitialized in the reverse order of initialization.
     */

    /*
     * deinit should always work (if it doesn't accept faulty arguments) and therefore not return an error
     * so in case there is a serious error we directly abort with ESP_ERROR_CHECK instead of propagating it
     */

    // NOTE: always set pointers to NULL after freeing them to prevent all kinds of undefined behavior

    // TODO 2.7: Implement me. Deinitialize everything that was initialized in ws2812b_init.I.e. don't blindly
    // deinitialize anything that *wasn't* initialized. You can check this by comparing pointers to NULL.

    ESP_LOGI(TAG, "WS2812B driver deinitialized");
}

uint8_t *ws2812b_get_pixels(void) { return pixel_buffer; }

void ws2812b_show(void) {
    /*
     * Static variables prevent re-initialization for every function call.
     * We place them here as they're not used outside of this function.
     */
    static rmt_transmit_config_t transmit_cfg = {
        .loop_count = 0, // no transfer loop, we only want to send this once
        .flags = {0},    // no special flags
    };
    static rmt_symbol_word_t reset_symbol = {
        // the API always works with 2 intervals, so we simply set both to 0 for half the reset duration
        .level0 = 0,
        .duration0 = RST_TIME / 2,
        .level1 = 0,
        .duration1 = RST_TIME / 2,
    };

    /*
     * This function converts the pixel buffer to RMT symbols and sends them to the RMT TX channel.
     * Creating a custom encoder is more memory efficient but out of scope for this assignment.
     */
    // TODO 2.5: This turns the first LED green. Replace this by encoding the pixel buffer into the symbol buffer
    // Hint: Each LED has 3 color values (= 3 bytes), each of which consists of 1 bit. Each bit needs its own symbol.
    // Note: Per datasheet, the most significant bit of each color value is sent first
    for (size_t i = 0; i < 8; i++) {
        symbol_buffer[i].level0 = 1;
        symbol_buffer[i].duration0 = T1H_TIME;
        symbol_buffer[i].level1 = 0;
        symbol_buffer[i].duration1 = T1L_TIME;
    }

    // TODO 2.4: Transmit the symbol buffer, followed by a reset symbol and wait for all transmits to finish
}

void ws2812b_set_pixel(size_t index, uint8_t r, uint8_t g, uint8_t b) {
    assert(index < pixel_count && "Index out of bounds");

    // GRB order
    // TODO 2.6: Set the three color values in the pixel buffer
}

void ws2812b_set_pixel_rgb(size_t index, uint32_t rgb) { ws2812b_set_pixel(index, (rgb >> 16), (rgb >> 8), rgb); }

uint32_t ws2812b_get_pixel_rgb(size_t index) {
    assert(index < pixel_count && "Index out of bounds");

    // GRB order
    // TODO 2.6: Read back and pack to uint32_t
    return 0;
}

void ws2812b_fill(uint8_t r, uint8_t g, uint8_t b) {
    for (size_t i = 0; i < pixel_count; i++) {
        ws2812b_set_pixel(i, r, g, b);
    }
}

void ws2812b_fill_rgb(uint32_t rgb) { ws2812b_fill((rgb >> 16), (rgb >> 8), rgb); }

void ws2812b_clear(void) {
    // memset is more efficient than looping through the buffer ourselves
    memset(pixel_buffer, 0, PIXEL_BYTES);
}
