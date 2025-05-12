# ESP Menu

A customizable menu system for ESP32 microcontrollers with SSD1306 OLED displays and rotary encoder input.

![ESP Menu System](https://via.placeholder.com/640x320.png?text=ESP+Menu+System)

## Overview

ESP Menu is a flexible and extensible menu creation framework for ESP32-based projects that use SSD1306 OLED displays and rotary encoders for user interaction. It provides a JSON-based configuration system to easily define menu structures and navigation.

### Features

- **SSD1306 OLED Display Support**: Built-in support for I2C-connected SSD1306 OLED displays
- **Rotary Encoder Input**: Support for up to 4 rotary encoders with push buttons
- **LVGL Integration**: High-quality graphics using the LVGL library
- **JSON-based Menu Definition**: Define menus using simple JSON structures
- **Template-based Code Generation**: Automatically generates menu code from templates
- **NVS Support**: Optional save/load of menu parameters to/from non-volatile storage
- **Component-based Architecture**: Designed as an ESP-IDF component for easy integration

## Hardware Requirements

- ESP32 microcontroller
- SSD1306 OLED display (128x64 or 128x32)
- Rotary encoder(s) with push button
- I2C connections for the display (default: SDA=21, SCL=22)
- GPIO pins for rotary encoders (configurable)

## Installation

### As a Component in Your ESP-IDF Project

1. Create a `components` directory in your ESP-IDF project if it doesn't exist:

   ```bash
   mkdir -p components
   ```

2. Clone this repository into your components directory:

   ```bash
   cd components
   git clone https://github.com/yourusername/esp_menu.git
   ```

3. Include the component in your project's `CMakeLists.txt`:

   ```cmake
   set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/components/esp_menu)
   ```

4. Configure the menu system via `menuconfig`:

   ```bash
   idf.py menuconfig
   ```

   Navigate to "ESP Menu Configuration" to set your display and encoder settings.

## Usage

### Basic Setup

1. Initialize the menu system in your main application:

   ```c
   #include "esp_menu.h"

   void app_main(void)
   {
       // Initialize the menu system
       esp_err_t result = esp_menu_init();
       if (result != ESP_OK) {
           ESP_LOGE(TAG, "Failed to initialize menu system");
           return;
       }
       
       // Your application code here
   }
   ```

## Configuration

- Edit `assets/menu.json` or set a custom path via `idf.py menuconfig`.
- Example JSON:

  ```json
  {
      "display": {
          "width": 128,
          "height": 64,
          "sda_pin": 21,
          "scl_pin": 22,
          "i2c_address": "0x3C"
      },
      "encoders": [
          {
              "name": "encoder1",
              "pins": {
                  "A": 5,
                  "B": 6,
                  "switch": 7
              }
          }
      ],
      "menu": {
          "screens": [
              {
                  "name": "main",
                  "widgets": [
                      {
                          "type": "list",
                          "controlled_by": "encoder1",
                          "items": [
                              {"id": 1, "text": "Option 1", "action": "action1"},
                              {"id": 2, "text": "Option 2", "screen": "screen2"}
                          ]
                      }
                  ]
              },
              {
                  "name": "screen2",
                  "widgets": [
                      {
                          "type": "image",
                          "graphic_id": "waveform",
                          "x": 10,
                          "y": 10
                      }
                  ]
              }
          ]
      },
      "graphics": [
          {
              "id": "waveform",
              "type": "waveform"
          }
      ]
  }'''

### Implementing Menu Actions

1. Define action callbacks in your code:

```c
// In your_app.c
void option1_action(void) {
    // Handle option 1 selection
    printf("Option 1 selected!\n");
}
```

2. Automatically generated header includes:

```c
#include "menu_data.h"  // Contains prototypes for your menu actions
```

## Code Generation

The menu system uses Python scripts to generate the C code needed for your menu based on your `menu.json` configuration:

```bash
# Run from your project root to generate menu files
python3 components/esp_menu/generate_menu.py
```

This will:

1. Read your `menu.json` definition
2. Validate it against the schema
3. Generate the necessary C code for your menu
4. Place the files in the appropriate build folders

## Configuration Options

The ESP Menu system is configurable through ESP-IDF's `menuconfig` system. Key options include:

- Display type and resolution (SSD1306 in 64 or 32 pixel heights)
- I2C addressing and pin configuration
- Number of rotary encoders (1-4)
- GPIO pin assignments for each encoder
- Optional NVS (Non-Volatile Storage) support

## Contributing

Contributions to the ESP Menu project are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgements

- [ESP-IDF](https://github.com/espressif/esp-idf) - Espressif IoT Development Framework
- [LVGL](https://lvgl.io/) - Light and Versatile Graphics Library
