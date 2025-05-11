Python Script Documentation
==========================

The ``generate_menu_from_templates.py`` script generates C code for the menu system using ``menu.json`` and Jinja templates located in ``components/Esp_menu/templates/``. The generated code (``menu.c`` and ``menu_data.h``) is saved to ``components/Esp_menu/generated/``.

Usage:
   python generate_menu_from_templates.py <json_config_path> <output_dir> <templates_dir>

Example:
   python generate_menu_from_templates.py main/user_menu/menu.json components/Esp_menu/generated components/Esp_menu/templates
