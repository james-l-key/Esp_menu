// test/support/stub_esp_idf/sdkconfig.h
#ifndef SDKCONFIG_H
#define SDKCONFIG_H

// Add any configuration defines needed by your code
#define CONFIG_FREERTOS_HZ 100
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ 240
#define CONFIG_IDF_TARGET_ESP32 1

#endif // SDKCONFIG_H