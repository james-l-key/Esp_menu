
#include "Esp_menu.h"
#include "ssd1306.h"
#include "driver/gpio.h"
#include "sdkconfig.h" // Generated configuration header
#include "i2c_communication.h" // Include the I2C communication layer

void menu_init() {
    // SSD1306 local I2C configuration.
        ssd1306_init(CONFIG_SSD1306_ADDRESS, CONFIG_SSD1306_SDA_PIN, CONFIG_SSD1306_SCL_PIN);

            gpio_config_t io_conf;
                io_conf.intr_type = GPIO_INTR_ANYEDGE;
                    io_conf.mode = GPIO_MODE_INPUT;
                        io_conf.pin_bit_mask = (1ULL << CONFIG_ENCODER_A_PIN) | (1ULL << CONFIG_ENCODER_B_PIN) | (1ULL << CONFIG_ENCODER_BUTTON_PIN);
                            io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
                                io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
                                    gpio_config(&io_conf);

                                        // Encoder init code...
                                            // Set up interrupt for encoder
                                                i2c_communication_init(); //Init the inter module I2C.
                                                }

                                                void menu_draw() {
                                                    // Draw current menu and selected item on SSD1306
                                                        // Use data from menu_data variable, and current menu state.
                                                        }

                                                        void encoder_isr_handler(void* arg) {
                                                            // Encoder rotation handling
                                                                // Update selected item or parameter value
                                                                    // If a parameter needs to change a module, use i2c_communication function.
                                                                        // i2c_write_bytes(Module_Address, data, length);
                                                                        }

