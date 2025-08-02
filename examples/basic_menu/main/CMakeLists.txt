idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES "esp_menu" "nvs_flash"
)

# Skip legacy I2C driver conflict check
idf_build_set_property(COMPILE_DEFINITIONS "CONFIG_I2C_SKIP_LEGACY_CONFLICT_CHECK=1" APPEND) 