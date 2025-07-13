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

### Status
- SSD1306: Native ESP-IDF support
- SH1107: Compatibility mode (works for most applications)
