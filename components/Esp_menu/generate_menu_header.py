# Esp_menu/generate_menu_header.py

import json
import os

def generate_menu_header(json_file, header_file):
    # Get the directory where this script is located
    script_dir = os.path.dirname(os.path.realpath(__file__))
    
    # Use absolute paths
    json_path = os.path.join(script_dir, json_file)
    
    with open(json_path, 'r') as f:
        data = json.load(f)

    with open(header_file, 'w') as h:
        h.write('#ifndef MENU_DATA_H\n')
        h.write('#define MENU_DATA_H\n\n')
        h.write('#include <stdint.h>\n')
        h.write('#include "lvgl.h"\n\n')

        # Generate LVGL object creation and data structure code
        h.write('extern lv_obj_t *menu_screen;\n')
        h.write('extern lv_obj_t *menu_items[];\n')
        h.write('extern lv_style_t menu_style;\n\n')

        # Example of menu_items structure.
        h.write('typedef struct {\n')
        h.write('    lv_obj_t *object;\n')
        h.write('    // Add other relevant information\n')
        h.write('} menu_item_t;\n\n')

        # Generate Menu items.
        h.write('menu_item_t menu_items_data[] = {\n')
        for item in data:
            if item["type"] == "menu":
                for sub_item in item["items"]:
                    h.write('    {\n')
                    h.write('    },\n') # will need to be populated with lvgl object creation.
        h.write('};\n\n')
        # Generate the rest of the code as needed.

        h.write('#endif // MENU_DATA_H\n')

if __name__ == '__main__':
    generate_menu_header('menu.json', 'menu_data.h')