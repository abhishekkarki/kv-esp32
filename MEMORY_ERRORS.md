# Note: Memory Safety & Error Handling

#### Memory

Memory is a sparse resource in embedded systems, such as the ESP32.
Especially when allocating memory on the heap, it is therefore important to check if the allocation actually succeeded.

So do the usual, e.g.:

```c
uint32_t *my_ints = malloc(10 * sizeof(uint32_t));
if (my_ints == NULL) {
    // handle error
}
```

Some additional guidelines:

- Try to avoid heap fragmentation.
- If you can, use stack memory or global variables instead of heap memory.
- If you have to use heap memory, try to allocate it once (e.g., in an init function) and then reuse it.
- Free *all* memory you allocate when you're done with it. We don't want any memory leaks.
- When freeing memory (and any other resource of that matter), always set the pointer to `NULL` afterward. This is to
  avoid double-freeing.

#### Error Handling

Many functions of the ESP-IDF APIs return error codes of type `esp_err_t` from the `esp_err.h` header file. It is
paramount that you *always* handle
these errors appropriately.

Success is indicated by `ESP_OK`, while any other value is an error, e.g., `ESP_ERR_NO_MEM` or `ESP_ERR_INVALID_ARG`.

You mainly have two options concerning error handling:

1. Abort on error. You can do this in your main code or when the error is fatal/unrecoverable.
2. Forward the error by returning an `esp_err_t` yourself. You should do this in library code or when the error is
   recoverable.

_**For this assignment:** If a function that you need to implement returns an `esp_err_t`, forward any errors, otherwise
abort on error._

For the first case, use the ESP_ERROR_CHECK macro from `esp_err.h` like this:

```c
ESP_ERROR_CHECK(i_return_error());
```

If the enclosed function returns anything but `ESP_OK`, the program prints the type of error, file and line number where
the error occured, and a helpful trace. The ESP32 will then terminate.

For the second case, see the available macros from `esp_check.h` and further explanation in
the [documentation](https://docs.espressif.com/projects/esp-idf/en/v5.2.1/esp32/api-guides/error-handling.html).

As an example:

```c
esp_err_t my_init() {
    esp_err_t ret = ESP_OK;
    
    void *ptr = malloc(10);
    ESP_GOTO_ON_FALSE(ptr, ESP_ERR_NO_MEM, err, TAG, "Malloc failed");
    
    ESP_GOTO_ON_ERROR(init_internal_state(), err, TAG, "Internal state init failed");
    
    // ... more init ...
    
    return ESP_OK;
 err:
    // cleanup
    
    // ... more deinit ...
    
    if (internal_state_ok()) {
        deinit_internal_state();
    }
    
    if (ptr) {
        free(ptr);
        ptr = NULL;
    }
    return ret;
}
```