# ESP Menu Component - AI Coding Instructions

## Architecture Overview

ESP Menu is a **project-agnostic ESP-IDF v5.4.2 component** that provides a configurable menu system for ESP32 devices with OLED displays (SSD1306/SH1107) and rotary encoders. The architecture separates concerns into:

- **Core Component** (`components/esp_menu/`): Hardware abstraction, LVGL integration, display/encoder management
- **User Layer** (`user_actions.h/c`): Project-specific parameter definitions and action callbacks
- **Generated Code** (`generated/`): Menu structure and widgets auto-generated from JSON configuration
- **Configuration** (`Kconfig.projbuild`): Hardware pin assignments, display settings, feature toggles

### Key Design Principles
- **JSON-driven**: Menu structure defined in `assets/menu.json`, processed by Python scripts to generate C code
- **Component isolation**: Core ESP Menu code never contains project-specific logic
- **Template-based generation**: Uses Jinja2 templates in `assets/templates/` to generate menu code
- **LVGL integration**: All UI rendering through LVGL with esp_lvgl_port

## Critical Workflows

### Building & Development
```bash
# Primary build workflow (always from examples/basic_menu/)
cd examples/basic_menu && idf.py build

# Use VS Code tasks for common operations:
# Ctrl+Shift+P -> "Tasks: Run Task" -> "Build Example"
# Available tasks: Build, Clean, Flash, Monitor, Menuconfig
```

### ⚠️ Common Build Error
**NEVER build from the root directory!** This is a component, not a standalone project.
```bash
# ❌ WRONG - will fail with "Unknown CMake command idf_component_register"
cd /path/to/Esp_menu && idf.py build

# ✅ CORRECT - always build from example directory
cd examples/basic_menu && idf.py build
```

### Menu Code Generation
```bash
# When modifying menu.json, regenerate code:`   
python3 scripts/generate_menu_from_templates.py 2
# This updates generated/menu.c and generated/menu_data.h
```

### Configuration Changes
```bash
# Hardware pins, display settings, encoder count:
cd examples/basic_menu && idf.py menuconfig
# Navigate to "ESP Menu Configuration"
```

## Project-Specific Conventions

### Type Naming
- **User types**: `menu_params_t`, `current_slot` (lowercase with underscores)
- **Generated types**: Follow LVGL conventions (lv_obj_t*, etc.)
- **Avoid domain-specific names**: Use generic terms like "setting1", "toggle1" rather than "frequency", "amplitude"

### File Organization Pattern
```
components/esp_menu/
├── include/           # Public API headers
├── src/              # Core implementation (esp_menu.c)
├── assets/           # User-customizable files (user_actions.c)
├── generated/        # Auto-generated from templates
└── Kconfig.projbuild # Hardware configuration
```

### NVS (Non-Volatile Storage) Pattern
- **Component level**: Handles current_slot and basic persistence
- **User level**: `user_save_params_to_nvs()` / `user_load_params_from_nvs()` for project parameters
- **Namespace**: Always use "esp_menu" for consistency

### Error Handling Pattern
```c
// Use this macro pattern in esp_menu.c:
#define BSP_ERROR_CHECK_RETURN_ERR(x) \
    do { \
        esp_err_t err_rc = (x); \
        if (err_rc != ESP_OK) { \
            ESP_LOGE(TAG, "Error %s at line %d", esp_err_to_name(err_rc), __LINE__); \
            return err_rc; \
        } \
    } while (0)
```

## Integration Points

### With ESP-IDF
- **Component dependencies**: LVGL, esp_lvgl_port, button, knob, driver components
- **Build system**: Uses `EXTRA_COMPONENT_DIRS` in example projects
- **Configuration**: Integrates with ESP-IDF's Kconfig system

### Hardware Integration
- **I2C Master Bus**: Configured in `esp_menu_init()` for OLED display
- **GPIO**: Rotary encoders configured through Kconfig (supports 1-4 encoders)
- **Interrupts**: Handled by button/knob components, not directly in ESP Menu

### LVGL Integration
- **Display driver**: OLED panel (SSD1306/SH1107) through esp_lcd framework
- **Input devices**: Rotary encoders registered as LVGL input devices
- **Threading**: LVGL port manages task creation and timing

## Development Environment Setup

### VS Code Configuration
- **ESP-IDF Path**: Set in `.vscode/settings.json` (currently v5.4.2)
- **Terminal**: Configured to auto-source ESP-IDF environment via bash profiles
- **Build tasks**: Use `examples/basic_menu` as working directory for all operations

### Python Dependencies
Code generation requires:
- `jinja2` for template processing
- Standard library `json` for config parsing

## Common Patterns to Follow

### Adding New User Parameters
1. Update `menu_params_t` structure in `user_actions.h`
2. Modify save/load functions in `user_actions.c`
3. Add setter functions following the `set_settingN()` pattern
4. Update `init_menu_params()` with default values

### Hardware Configuration Changes
1. Modify `Kconfig.projbuild` for new config options
2. Update `esp_menu.c` to read new CONFIG_ values
3. Test with `idf.py menuconfig` to verify options appear correctly

### When Adding Dependencies
Update both:
- `components/esp_menu/CMakeLists.txt` (REQUIRES list)
- `components/esp_menu/idf_component.yml` (dependencies section)

## Troubleshooting

### Build Errors
- **"Unknown CMake command idf_component_register"**: You're building from root directory. Always build from `examples/basic_menu/`
- **"No project() command"**: Same issue - this is a component, not a standalone project
- **Component not found**: Ensure `EXTRA_COMPONENT_DIRS` is set correctly in example's CMakeLists.txt

### Runtime Errors
- **"CONFLICT! driver_ng is not allowed to be used with this old driver"**: I2C driver conflict between old `driver/i2c.h` and new `esp_driver_i2c`. Component uses new driver, ensure dependencies don't pull in old driver
