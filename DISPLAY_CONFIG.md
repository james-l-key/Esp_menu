# Display Configuration Guide

## SSD1306 vs SH1107 Support

The ESP Menu component now supports both display types through device-neutral configuration.

### Switching Display Types

Via menuconfig:
```bash
cd examples/basic_menu
idf.py menuconfig
# ESP Menu Configuration → OLED Display Type → SH1107
```

Via sdkconfig.defaults:
```
CONFIG_ESPMENU_DISPLAY_SH1107=y  # instead of SSD1306
```

### I2C and geometry settings

In menuconfig under ESP Menu Configuration:

- I2C host: CONFIG_ESPMENU_I2C_HOST
- SDA pin: CONFIG_ESPMENU_DISPLAY_I2C_SDA
- SCL pin: CONFIG_ESPMENU_DISPLAY_I2C_SCL
- I2C address: CONFIG_ESPMENU_DISPLAY_I2C_ADDRESS (0x3C common)
- Display height: CONFIG_ESPMENU_DISPLAY_HEIGHT (32 or 64)

These symbols are consumed by `components/esp_menu/src/esp_menu.c` at build time.

### Status

- SSD1306: Native ESP-IDF support
- SH1107: Compatibility mode (works for most applications)
