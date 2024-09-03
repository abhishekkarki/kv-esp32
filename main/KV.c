// Example to write and read a value in the flash memory

#include <stdio.h>
#include <inttypes.h>      // For PRI macros like PRId32
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG = "NVS_EXAMPLE";

// Function to write an integer value to NVS
void write_nvs_value(const char* key, int32_t value) {
    nvs_handle_t nvs_handle;
    esp_err_t err;

    // Open NVS handle in read-write mode
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }

    // Write the value to NVS
    err = nvs_set_i32(nvs_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write value to NVS!");
    } else {
        // Commit the written value
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to commit written value!");
        } else {
            ESP_LOGI(TAG, "Successfully written and committed value: %" PRId32, value);
        }
    }

    // Close NVS handle
    nvs_close(nvs_handle);
}

// Function to read an integer value from NVS
int32_t read_nvs_value(const char* key) {
    nvs_handle_t nvs_handle;
    esp_err_t err;
    int32_t value = 0;  // Default value if not found

    // Open NVS handle in read-only mode
    err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return value;
    }

    // Read the value from NVS
    err = nvs_get_i32(nvs_handle, key, &value);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Read value: %" PRId32, value);
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "The value is not initialized yet!");
    } else {
        ESP_LOGE(TAG, "Error (%s) reading value from NVS!", esp_err_to_name(err));
    }

    // Close NVS handle
    nvs_close(nvs_handle);
    return value;
}

// Main application entry point
void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Example of writing and reading to/from NVS
    const char* key = "my_key";
    int32_t value_to_store = 12345;

    write_nvs_value(key, value_to_store);
    int32_t stored_value = read_nvs_value(key);

    ESP_LOGI(TAG, "Final read value: %" PRId32, stored_value);
}


/*
cmake file for this
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

*/