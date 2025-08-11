# ESP Menu Component

Project-agnostic ESP-IDF component that renders a configurable, LVGL-based menu on OLED (SSD1306/SH1107) with a rotary encoder. Menu structure is defined in JSON and compiled to C via Jinja2 templates.

## Highlights

- LVGL UI with focus styles that work on monochrome OLEDs
- JSON-driven menu; code generated from templates (no manual UI wiring)
- Works as a reusable component or via the included example
- Rotary encoder + button input, NVS persistence hooks

## Quick start (example project)

Always build from the example directory, not the repo root.

1. Configure hardware and features

```bash
cd examples/basic_menu
idf.py menuconfig   # ESP Menu Configuration → set display, height, encoder pins/count
```

2. Generate menu code (from repo root) after editing `assets/menu.json`

```bash
cd ../..
python3 scripts/generate_menu_from_templates.py assets/menu.json components/esp_menu/generated assets/templates
```

3. Build, flash, and monitor

```bash
cd examples/basic_menu
idf.py build flash monitor
```

VS Code tasks are provided: Build Example, Flash Example, Monitor Example.

## Using as a component in your app

- Add this repo as a component (via git submodule or by setting `EXTRA_COMPONENT_DIRS`).
- Managed dependencies are declared in `components/esp_menu/idf_component.yml` and resolved by IDF Component Manager (requires ESP-IDF 5.4+):
    - lvgl/lvgl
    - espressif/esp_lvgl_port
    - espressif/button
    - espressif/knob
    - esp_lcd
    - nvs_flash
- Call the one-line init in your app:

```c
#include "esp_menu.h"

void app_main(void) {
        ESP_ERROR_CHECK(esp_menu_init());
}
```

## JSON → C code generation

- Source of truth: `assets/menu.json`
- Templates: `assets/templates/menu.c.j2` and `menu.h.j2`
- Generated files: `components/esp_menu/generated/menu.c` and `menu_data.h`

Regenerate after any JSON or template change:

```bash
python3 scripts/generate_menu_from_templates.py assets/menu.json components/esp_menu/generated assets/templates
```

Do not edit generated C files directly. Make changes in the templates.

## File layout

```text
components/esp_menu/
    include/        # public headers (esp_menu.h, user_actions.h)
    src/            # core implementation (esp_menu.c)
    assets/         # reference JSON/templates (fallbacks)
    generated/      # auto-generated menu.c/menu_data.h
    idf_component.yml
assets/            # project-level JSON/templates & user_* (preferred)
scripts/           # generator script
examples/basic_menu
```

## Configuration via menuconfig

ESP Menu Configuration includes:

- OLED type: SSD1306 or SH1107
- Display height: 64 or 32 px
- I2C host/SDA/SCL/address
- Rotary encoder count (1–4) and pins (A/B/button) per encoder
- Optional NVS integration

See DISPLAY_CONFIG.md for details.

## Actions and persistence

- Implement your app’s actions in `assets/user_actions.c` (project-level) or `components/esp_menu/assets/user_actions.c` (fallback).
- Action prototypes are generated into `menu_data.h` based on callbacks found in `menu.json`.
- If NVS is enabled, implement `user_save_params_to_nvs()` and `user_load_params_from_nvs()` in `user_actions.c`.

## Troubleshooting

- Unknown CMake command idf_component_register / “No project() command”
    - You’re building from the repo root. Build from `examples/basic_menu/`.

- Failed to resolve component 'lvgl' (or 'lvgl__lvgl')
    - Ensure ESP-IDF 5.4+ is active and `examples/basic_menu` is using IDF’s Component Manager. Re-run `idf.py reconfigure`.

- I2C conflict “driver_ng is not allowed to be used with this old driver”
    - Avoid mixing legacy `driver/i2c.h` with `esp_driver_i2c`. This component uses the modern driver.

- No focus highlight when turning encoder
    - The component sets the encoder’s LVGL group as default and applies focus styles; ensure generated code is up to date and the example is flashed.

## License

MIT. See LICENSE.

## Acknowledgements

- ESP-IDF — <https://github.com/espressif/esp-idf>
- LVGL — <https://lvgl.io/>
