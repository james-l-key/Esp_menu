Python Script Documentation
==========================

The ``generate_menu_from_templates.py`` script generates C code for the menu system using ``assets/menu.json`` and Jinja templates located in ``assets/templates/``. The generated code (``menu.c`` and ``menu_data.h``) is saved to ``components/esp_menu/generated/``.

Usage:
   python generate_menu_from_templates.py <json_config_path> <output_dir> <templates_dir>

Example (run from the repository root):
   python scripts/generate_menu_from_templates.py assets/menu.json components/esp_menu/generated assets/templates

Notes:

- Always build from ``examples/basic_menu`` (this repository is an ESP-IDF component, not a standalone project).
- Do not edit generated files directly; update the Jinja templates under ``assets/templates`` to persist changes.
