#!/usr/bin/env python3
"""
@file generate_menu.py
@brief Component-specific menu code generator 
@ingroup code_gen

This script handles the component-specific aspects of menu code generation.
It reads a menu.json configuration file, validates it against a schema,
and uses Jinja2 templates to generate C code that implements the specified menu structure.

Usage:
    python3 generate_menu.py [path_to_menu_json] [output_directory]

If no arguments are provided, default paths are used.

@author ESP Menu Team
@date May 10, 2025
"""

import os
import sys
import json
import jsonschema
from jinja2 import Environment, FileSystemLoader

# JSON Schema for validation
schema = {
    "type": "object",
    "required": ["display", "encoders", "menu"],
    "properties": {
        "display": {
            "type": "object",
            "required": ["type", "width", "height", "interface", "i2c_address", "sda_pin", "scl_pin"],
            "properties": {
                "type": {"type": "string", "enum": ["ssd1306"]},
                "width": {"type": "integer"},
                "height": {"type": "integer"},
                "interface": {"type": "string", "enum": ["i2c"]},
                "i2c_address": {"type": "string"},
                "sda_pin": {"type": "integer"},
                "scl_pin": {"type": "integer"}
            }
        },
        "encoders": {
            "type": "array",
            "items": {
                "type": "object",
                "required": ["name", "pins", "role"],
                "properties": {
                    "name": {"type": "string"},
                    "pins": {
                        "type": "object",
                        "required": ["A", "B", "switch"],
                        "properties": {
                            "A": {"type": "integer"},
                            "B": {"type": "integer"},
                            "switch": {"type": "integer"}
                        }
                    },
                    "role": {"type": "string"}
                }
            }
        },
        "graphics": {
            "type": "array",
            "items": {
                "type": "object",
                "required": ["id", "type"],
                "properties": {
                    "id": {"type": "string"},
                    "type": {"type": "string", "enum": ["static", "procedural", "external"]},
                    "data": {"type": "string"},
                    "width": {"type": "integer"},
                    "height": {"type": "integer"},
                    "generator": {"type": "string", "enum": ["checkerboard"]},
                    "params": {
                        "type": "object",
                        "properties": {
                            "width": {"type": "integer"},
                            "height": {"type": "integer"},
                            "tile_size": {"type": "integer"}
                        }
                    },
                    "header": {"type": "string"},
                    "symbol": {"type": "string"}
                },
                "if": {"properties": {"type": {"const": "static"}}},
                "then": {"required": ["data", "width", "height"]},
                "if:1": {"properties": {"type": {"const": "procedural"}}},
                "then:1": {"required": ["generator", "params"]},
                "if:2": {"properties": {"type": {"const": "external"}}},
                "then:2": {"required": ["header", "symbol"]}
            }
        },
        "menu": {
            "type": "object",
            "required": ["screens"],
            "properties": {
                "screens": {
                    "type": "array",
                    "items": {
                        "type": "object",
                        "required": ["name", "widgets"],
                        "properties": {
                            "name": {"type": "string"},
                            "widgets": {
                                "type": "array",
                                "items": {
                                    "type": "object",
                                    "required": ["type"],
                                    "properties": {
                                        "type": {"type": "string", "enum": ["list", "image", "canvas"]},
                                        "items": {
                                            "type": "array",
                                            "items": {
                                                "type": "object",
                                                "required": ["text"],
                                                "properties": {
                                                    "text": {"type": "string"},
                                                    "action": {"type": "string"},
                                                    "screen": {"type": "string"}
                                                }
                                            }
                                        },
                                        "graphic_id": {"type": "string"},
                                        "x": {"type": "integer"},
                                        "y": {"type": "integer"},
                                        "width": {"type": "integer"},
                                        "height": {"type": "integer"},
                                        "controlled_by": {"type": "string"}
                                    },
                                    "if": {"properties": {"type": {"const": "list"}}},
                                    "then": {"required": ["items", "controlled_by"]},
                                    "else": {"not": {"required": ["items"]}}
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


def load_and_validate_config(config_path):
    """Load and validate the JSON configuration file."""
    if not os.path.exists(config_path):
        raise Exception(f"Config file not found: {config_path}")
    try:
        with open(config_path, 'r') as f:
            config = json.load(f)
        jsonschema.validate(config, schema)
        print("DEBUG: Config loaded:", json.dumps(config, indent=2))
        return config
    except jsonschema.ValidationError as e:
        raise Exception(f"JSON validation error: {e.message}")
    except Exception as e:
        raise Exception(f"Error loading config: {str(e)}")


def generate_graphics_code(graphics):
    """Generate C code for graphics (static, procedural, external)."""
    code = []
    for graphic in graphics:
        graphic_id = graphic['id']
        if graphic['type'] == 'static':
            array_data = ', '.join(graphic['data'].split(','))
            code.append(f"""
static const uint8_t {graphic_id}_data[] = {{{array_data}}};
static const lv_image_dsc_t {graphic_id}_dsc = {{
    .header = {{.magic = LV_IMAGE_HEADER_MAGIC, .w = {graphic['width']}, .h = {graphic['height']}, .cf = LV_COLOR_FORMAT_MONO1}},
    .data = {graphic_id}_data,
    .data_size = sizeof({graphic_id}_data)
}};
""")
        elif graphic['type'] == 'procedural' and graphic['generator'] == 'checkerboard':
            params = graphic['params']
            code.append(f"""
static void draw_{graphic_id}(lv_obj_t *canvas, uint16_t w, uint16_t h, uint8_t tile_size) {{
    lv_canvas_set_buffer(canvas, lv_canvas_get_img_data(canvas), w, h, LV_COLOR_FORMAT_MONO1);
    for (uint16_t y = 0; y < h; y++) {{
        for (uint16_t x = 0; x < w; x++) {{
            bool color = ((x / tile_size) + (y / tile_size)) % 2;
            lv_canvas_set_px(canvas, x, y, color ? LV_COLOR_WHITE : LV_COLOR_BLACK);
        }}
    }}
}}
""")
        elif graphic['type'] == 'external':
            code.append(f"#include \"{graphic['header']}\"")
    return code


def generate_action_prototypes(screens):
    """Generate function prototypes for menu actions."""
    actions = set()
    for screen in screens:
        for widget in screen.get('widgets', []):
            if widget['type'] == 'list':
                for item in widget.get('items', []):
                    if 'action' in item:
                        actions.add(item['action'])
    return [f"void {action}(void);" for action in actions]


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <config_json> <output_header>")
        sys.exit(1)

    config_path = sys.argv[1]
    output_header = sys.argv[2]

    script_dir = os.path.dirname(os.path.abspath(__file__))
    template_dir = os.path.join(script_dir, 'templates')
    output_dir = os.path.join(script_dir, 'generated')

    if not os.path.exists(template_dir):
        print(f"Error: Template directory not found: {template_dir}")
        sys.exit(1)

    os.makedirs(output_dir, exist_ok=True)

    config = load_and_validate_config(config_path)
    print("DEBUG: Widgets in main screen:", [
          w['type'] for w in config['menu']['screens'][0]['widgets']])
    for widget in config['menu']['screens'][0]['widgets']:
        print(
            f"DEBUG: Widget details: type={widget['type']}, items={widget.get('items', None)}")

    try:
        env = Environment(loader=FileSystemLoader(template_dir))
        env.get_template('menu.c.j2')
        env.get_template('menu.h.j2')
    except Exception as e:
        print(f"Error loading templates: {str(e)}")
        sys.exit(1)

    graphics_code = generate_graphics_code(config.get('graphics', []))
    action_prototypes = generate_action_prototypes(config['menu']['screens'])

    header_template = env.get_template('menu.h.j2')
    with open(output_header, 'w') as f:
        f.write(header_template.render(
            action_prototypes=action_prototypes
        ))

    source_template = env.get_template('menu.c.j2')
    with open(os.path.join(output_dir, 'menu.c'), 'w') as f:
        f.write(source_template.render(
            config=config,
            graphics_code=graphics_code
        ))

    print(
        f"Generated {output_header} and {os.path.join(output_dir, 'menu.c')}")


if __name__ == "__main__":
    main()
