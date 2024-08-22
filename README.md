[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/S84GhvRC)
# IoT Lab SS24 - Assignment 1: Light it up

Due date: 2024-MM-DD HH:MM

In this very first assignment we want to get you familiar with the basics of ESP32 microcontroller programming.
Let's do the classic "Hello World" of microcontrollers: Working with LEDs.

---

## Exercise 1: Blinking LED

We start off very easy: Blinking the built-in LED of the ESP32.
For this exercise, only focus on `main.c` inside the `main` directory. You can ignore the rest for now.

_**Note**: The TODOs within the source code files will guide you. Only the TODOs of the format `TODO 1.x` are relevant
for now._

**⚠️Before programming⚠️**
Please read the supplemental information regarding [memory safety and error handling](./MEMORY_ERRORS.md).
It deals with best practices in ESP-IDF and embedded programming in general. We expect you to follow these guidelines.

Implement the following expected functionality:

- [ ] Set up the GPIO pin for the built-in LED to be an output pin.

  Use the `gpio_set_direction` function from the `driver/gpio.h` header file (be sure to include it!).
  The `LED_GPIO` macro already holds the correct pin number for you, please use it.

- [ ] The code then calls the static `blink` function. Finish its implementation.

  The function should turn the LED on, wait a bit, turn it off, wait a bit, and repeat forever.
  *Hint: Use `gpio_set_level` and `vTaskDelay`. For the latter, include both `freertos/FreeRTOS.h`
  and `freertos/task.h`*

- [ ] Experiment a bit with different delays to see how it affects the blinking. You may use the `pdMS_TO_TICKS` macro
  to convert milliseconds to FreeRTOS ticks. You can also change the LED color by changing the pin number
  with `idf.py menuconfig` -> "Assignment Configuration" -> "LED GPIO pin number" and rebuilding.

## Exercise 2: Fancy LEDs

A single blinking LED? Kind of boring, if you ask me. Let's instead use a whole RGB LED strip!
We'll be using a WS2812B LED strip (also known as Neopixel) for this exercise, because it's simple to use and
also widespread.

While many libraries exist for controlling these LEDs, they are either too complex or rely on the "Arduino Core". We're
targeting ESP-IDF here, so let's develop our own little driver library!

Before you get started implementing anything, please read
the [WS2812B datasheet](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf). Here, you not only find the electrical
characteristics and pin layout, but also - important for driver programming - timings and data encoding.

**⚠️Before wiring⚠️**
Please read the supplemental information regarding [wiring](./WIRING.md) to avoid damaging the hardware.
You'll also find wiring diagrams there.

### About RMT

As you may have noticed, the WS2812B LEDs are controlled by a very precise timing of high and low signals.
A single FreeRTOS tick is 10ms long by default! Other software APIs can delay in microsecond precision.
However, that is still not enough.

You have two options now: Count clock cycles and painstakingly create handwritten assembly code (eww), or use dedicated
hardware.

Luckily, the ESP32 has a hardware peripheral for exactly this purpose:
the [Remote Control Transceiver (RMT)](https://docs.espressif.com/projects/esp-idf/en/v5.2.1/esp32/api-reference/peripherals/rmt.html).
You can read through its documentation to get a better understanding of how it works, but TL;DR:

- The RMT consists of channels which can be configured to be either TX (transmit) or RX (receive). We'll only use one TX
  channel.
- Each channel is configured with a few properties, the most important ones are:
    - **GPIO pin**. The pin number on which - in the case of TX - the RMT will output a signal by rapidly modulating the
      pin's voltage.
    - **Clock source**. The ESP32 has many different clocks, for our purposes we'll use the APB clock which has a
      maximum resolution of 80MHz.
    - **Resolution**. Basically the frequency of modulation, that is, the maximum number of high/low switches per
      second. A high resolution is needed to match the WS2812B's timing requirements. Ideally, we'd use 100MHz (once per
      10ns), but since the ABP clock only goes up to 80MHz, we'll use the next lower order of magnitude for simplicity
      (10MHz).
- The RMT modulates the pin based on *RMT symbols*. Such a symbol consists of two pairs of two values: the signal's
  duration in RMT ticks (e.g., a duration of 42 ticks with a resolution of 10MHz would equal 4.2µs) and the signal's
  logic
  level, i.e., high or low. **We'll use these RMT symbols to encode the bits of the LED color data.**

### Your Task

Complete the ws2812b driver library component in `components/ws2812b` using the RMT peripheral. Get started by reading
the `include/ws2812b.h` header file and the `ws2812b.c` source file to get an overview of the API and the driver's
structure.

Implement everything below. Again, follow the TODOs in the source files and keep memory safety and error
handling in mind.

- [ ] Prepare for the WS2812B driver:
    - In `main/CMakeLists.txt`, add the `ws2812b` component to the requirements
    - In `main/Kconfig.projbuild` add new config options for the LED strip GPIO pin and LED count, so that you can
      easily configure these values using `idf.py menuconfig`
    - In `main/main.c`, include the necessary headers, define macros for the new config options, initialize the WS2812B
      driver and call the `led_strip` function instead of `blink` (**don't** call `gpio_set_level` for the LED strip
      pin; the RMT peripheral will handle this)
- [ ] Adapt the timing macros at the very top of `ws2812b.c` with the information you gathered from the datasheet. Keep
  in mind the chosen resolution.
- [ ] Finish the implementation of the `ws2812b_init` function. In this function, return all errors to the caller
  instead of aborting. At the end of the function, we call `ws2812b_clear()` and `ws2812b_show()`. Add the following
  before that:
    - Save important config values for later use
    - Allocate heap memory for the LED pixel buffer (stores the color data for each LED as 3 bytes in GBR order), as
      well as the RMT
      symbol buffer (the RMT peripheral accesses this buffer). Use standard malloc and check for allocation errors!
      *Hint: There are some useful macros at the top that may help you...*
    - Create an RMT TX channel (`rmt_new_tx_channel`) using the provided RMT config
    - Create a "copy encoder" (`rmt_new_copy_encoder`). The encoder copies our RMT symbols as-is to the RMT peripheral
    - Enable the created RMT TX channel (`rmt_enable`). This sets up some stuff internally
- [ ] In the `ws2812b_show` function, at the very end, call `rmt_transmit` twice. First with the RMT symbol buffer, then
  with a pointer to the provided `reset_symbol`. Pass the previously created channel, copy encoder and
  provided `transmit_cfg`. Follow this with a call to `rmt_tx_wait_all_done` (use `-1` as timeout to wait as long as
  necessary)
- [ ] Take a deep breath, compile and flash. Assuming you wired everything correctly, the first LED of the strip should
  now light up in a **bright green**!
- [ ] Finish implementation of the `ws2812b_show` function by replacing the dummy RMT symbol code with functionality
  that encodes the LED pixel buffer to RMT symbols and writes them to the RMT symbol buffer. The dummy code gives you an
  idea of what the struct values mean. Note that you need to encode each bit in the pixel buffer as its own RMT symbol.
- [ ] Implement the `ws2812b_set_pixel` and `ws2812b_get_pixel_rgb` functions to manipulate and query the LED pixel
  buffer
- [ ] Implement `ws2812b_deinit`, check your intellisense or the documentation for the necessary function calls

Phew, that was a **LOT**. Consider it also as a C refresher ;)

But you can now experiment with different colors and patterns by calling the API functions from within the `led_strip`
function
in `main.c`. Finally!

## Exercise 3: Even fancier LEDs

Well, we don't want all that work to go to waste, do we? Let's make a little light show!

In this exercise, we want to see your ✨creativity✨. Implement a few different LED effects. Anything from rainbow
to [Apple's patented breathing effect](https://avital.ca/notes/a-closer-look-at-apples-breathing-light).
Whatever you like!

The only constraint is that we want to cycle through the effects with the press of a button.

Implement the following:

- [ ] In `main.c`, setup a button GPIO pin as an input pin. You can use `gpio_set_pull_mode` if you want to use an
  internal pull-up/pull-down resistor. Be sure to also add a new entry to `Kconfig.projbuild` for the button pin number
  and macro definition accordingly.
- [ ] Add your effects. Best to have a function for each effect, but that's up to you. It's just important that you
  call `gpio_get_level` repeatedly to check the button state and switch to the next effect when the button is pressed.

Note that you're not required to implement debouncing or any fancy logic like that (you can, if you want to though!).
But the button should be at least *kind of* responsive. I.e., we shouldn't have to press it down for 5 seconds to see a
change - which could happen if you have a `vTaskDelay` of 5 seconds of course! Maybe you can come up with something
clever, or just keep your delays short ;)

Final hint: Per default, a FreeRTOS tick is 10ms long. The macro `pdMS_TO_TICKS` will therefore integer-divide a given
value by 10. This means that any call `vTaskDelay(pdMS_TO_TICKS(x))` with x < 10 will result in a delay of 0 ticks. So
don't be confused that a delay of >=10ms will feel way slower because any delay <10ms will be like almost no delay at
all! 