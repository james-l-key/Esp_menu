.PHONY: generate doxygen html

generate:
    python ../components/Esp_menu/generate_menu_from_templates.py ../main/user_menu/menu.json ../components/Esp_menu/generated ../components/Esp_menu/templates

doxygen: generate
    doxygen doxygen/Doxyfile

html: doxygen
    sphinx-build -b html ./ _build/html
