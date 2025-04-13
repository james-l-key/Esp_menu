# Esp_menu/generate_menu_header.py

import json
import os
import sys

def generate_menu_header(json_file, header_file):
    # Get the directory where this script is located
    script_dir = os.path.dirname(os.path.realpath(__file__))
    
    # Use absolute paths for the JSON file
    json_path = os.path.join(script_dir, json_file)

    print(f"Looking for JSON file at: {json_path}")
    if not os.path.exists(json_path):
        print("Error: JSON file not found!")
        return

    # Use the second argument (header_file) as the full output path
    print(f"Writing header file to: {header_file}")
    with open(json_path, 'r') as f:
        try:
            data = json.load(f)
            print("JSON data loaded successfully.")
        except json.JSONDecodeError as e:
            print(f"Error decoding JSON: {e}")
            return

    with open(header_file, 'w') as h:
        h.write('#ifndef MENU_DATA_H\n')
        h.write('#define MENU_DATA_H\n\n')
        h.write('#include <stdint.h>\n')
        h.write('#include "lvgl.h"\n\n')

        # Generate LVGL object creation and data structure code
        h.write('extern lv_obj_t *menu_screen;\n')
        h.write('extern lv_obj_t *menu_items[];\n')
        h.write('extern lv_style_t menu_style;\n\n')

        h.write('typedef struct {\n')
        h.write('    lv_obj_t *object;\n')
        h.write('} menu_item_t;\n\n')

        h.write('menu_item_t menu_items_data[] = {\n')
        for item in data:
            if item["type"] == "menu":
                for sub_item in item["items"]:
                    h.write('    {\n')
                    h.write('    },\n')
        h.write('};\n\n')
        h.write('#endif // MENU_DATA_H\n')

if __name__ == '__main__':
    # Use command-line arguments for flexibility
    if len(sys.argv) != 3:
        print("Usage: python generate_menu_header.py <input_json> <output_header>")
        sys.exit(1)

    generate_menu_header(sys.argv[1], sys.argv[2])