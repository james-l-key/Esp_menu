idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES Esp_menu esp_lcd nvs_flash)