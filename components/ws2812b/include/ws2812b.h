#pragma once

#include <driver/gpio.h>
#include <esp_err.h>
#include <stdint.h>

/**
 * Configuration for the WS2812B LED strip driver.
 */
typedef struct {
    size_t pixel_count;       ///< Number of pixels (=LEDs) in the strip
    gpio_num_t data_gpio_num; ///< GPIO number of the data pin
} ws2812b_config_t;

/**
 * Initialize the WS2812B LED strip driver.
 *
 * @param config pointer to configuration for the driver, can be standard stack allocated, doesn't have to be valid
 * after this function call
 * @return ESP_OK on success, any other value indicates an error and should be handled appropriately
 */
esp_err_t ws2812b_init(const ws2812b_config_t *config);

/**
 * Deinitializes the WS2812B LED strip driver.
 *
 * Any error during deinitialization is printed to console and results in program termination.
 */
void ws2812b_deinit(void);

/**
 * Get the underlying pixel buffer.
 *
 * Useful for more advanced effects where direct manipulation of the pixel buffer is desired.
 *
 * @return pointer to the pixel buffer
 */
uint8_t *ws2812b_get_pixels(void);

/**
 * Show the current pixel buffer on the LED strip.
 *
 * @note This function must be called whenever you want to actually "display" the pixel buffer on the LED strip.
 */
void ws2812b_show(void);

/**
 * Set the color of a single pixel.
 * @param index index of the pixel to set
 * @param r red component
 * @param g green component
 * @param b blue component
 */
void ws2812b_set_pixel(size_t index, uint8_t r, uint8_t g, uint8_t b);

/**
 * Set the color of a single pixel.
 * @param index index of the pixel to set
 * @param rgb packed color value (0x00RRGGBB)
 */
void ws2812b_set_pixel_rgb(size_t index, uint32_t rgb);

/**
 * Get the color of a single pixel.
 *
 * @note This doesn't query the physical LED strip, but the internal pixel buffer. May be useful as to avoid tracking
 * which pixels have which color yourself.
 *
 * @param index index of the pixel to get
 * @return packed color value (0x00RRGGBB)
 */
uint32_t ws2812b_get_pixel_rgb(size_t index);

/**
 * Set the color of all pixels.
 * @param r red component
 * @param g green component
 * @param b blue component
 */
void ws2812b_fill(uint8_t r, uint8_t g, uint8_t b);

/**
 * Set the color of all pixels.
 * @param rgb packed color value (0x00RRGGBB)
 */
void ws2812b_fill_rgb(uint32_t rgb);

/**
 * Clear the pixel buffer. Is semantically equivalent to calling `ws2812b_fill(0, 0, 0)`.
 *
 * @note This can be used to turn the LEDs off with a subsequent call to `ws2812b_show()`.
 */
void ws2812b_clear(void);
