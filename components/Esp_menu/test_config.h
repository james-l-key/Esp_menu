#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

// Define the TAG for ESP_LOGI
#define TAG "Esp_menu"

// Define missing constants
#define I2C_HOST 0
#define EXAMPLE_PIN_NUM_SDA 2
#define EXAMPLE_PIN_NUM_SCL 4
#define CONFIG_ENCODER_A_PIN 18
#define CONFIG_ENCODER_B_PIN 19
#define CONFIG_ENCODER_BUTTON_PIN 21

// Define ESP_ERROR_CHECK if not defined
#ifndef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)                                         \
    do                                                             \
    {                                                              \
        esp_err_t rc = (x);                                        \
        if (rc != ESP_OK)                                          \
        {                                                          \
            printf("Error %d at %s:%d\n", rc, __FILE__, __LINE__); \
        }                                                          \
    } while (0)
#endif

#endif // TEST_CONFIG_H