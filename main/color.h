#pragma once

#include <esp_err.h>
#include <stdint.h>

/*
 * This file contains some useful color-related helper functions - if you need them.
 * Feel free to add your own helpers here.
 */

/**
 * Packs the given RGB values into a single 32-bit integer.
 *
 * @param r red component
 * @param g green component
 * @param b blue component
 * @return the packed color value
 */
static inline uint32_t color_rgb_pack(uint8_t r, uint8_t g, uint8_t b) { return (r << 16) | (g << 8) | b; }

/**
 * Converts the given HSV color to RGB color.
 * You may use this if you want to create fancy gradient effects and such.
 *
 * @note see https://en.wikipedia.org/wiki/HSL_and_HSV for the algorithm
 *
 * @param h hue in degrees (0-359, but higher values are allowed and wrap around)
 * @param s saturation (0-100)
 * @param v value (0-100)
 * @return the packed RGB color value
 */
static inline uint32_t color_hsv2rgb(uint32_t h, uint8_t s, uint8_t v) {
    assert(s <= 100 && v <= 100 && "Saturation and value must be <=100");

    h %= 360; // 0 <= h < 360

    uint8_t r, g, b;

    uint32_t rgb_max = v * 255 / 100;
    uint32_t rgb_min = rgb_max * (100 - s) / 100;

    uint8_t i = h / 60;
    uint16_t diff = h % 60;

    uint32_t rgb_inc = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
    case 0:
        r = rgb_max;
        g = rgb_min + rgb_inc;
        b = rgb_min;
        break;
    case 1:
        r = rgb_max - rgb_inc;
        g = rgb_max;
        b = rgb_min;
        break;
    case 2:
        r = rgb_min;
        g = rgb_max;
        b = rgb_min + rgb_inc;
        break;
    case 3:
        r = rgb_min;
        g = rgb_max - rgb_inc;
        b = rgb_max;
        break;
    case 4:
        r = rgb_min + rgb_inc;
        g = rgb_min;
        b = rgb_max;
        break;
    default:
        r = rgb_max;
        g = rgb_min;
        b = rgb_max - rgb_inc;
        break;
    }

    return color_rgb_pack(r, g, b);
}
