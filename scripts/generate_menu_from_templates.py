#!/usr/bin/env python3
"""
Script to generate menu.c and menu_data.h from Jinja2 templates and menu.json.
"""

import os
import sys
import json
import jinja2
import logging

# Configure logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def load_json_file(json_path):
    """
    Load and parse the JSON configuration file.
    
    Args:
        json_path (str): Path to the JSON configuration file
        
    Returns:
        dict: The parsed JSON data
    """
    logger.debug(f"Loading JSON from: {json_path}")
    abs_path = os.path.abspath(json_path)
    try:
        with open(abs_path, 'r') as f:
            data = json.load(f)
        logger.info(f"Successfully loaded config from {abs_path}")
        return data
    except FileNotFoundError:
        logger.error(f"JSON file not found: {abs_path}")
        sys.exit(1)
    except json.JSONDecodeError as e:
        logger.error(f"Invalid JSON in {abs_path}: {e}")
        sys.exit(1)
    except Exception as e:
        logger.error(f"Unexpected error loading JSON from {abs_path}: {e}")
        sys.exit(1)

def adapt_json_structure(config):
    """
    Adapts the JSON structure to match what the template expects.
    """
    result = config.copy()
    
    if 'display' not in result:
        logger.info("Adding default display configuration")
        result['display'] = {
            'width': 128,
            'height': 64,
            'sda_pin': 21,
            'scl_pin': 22,
            'i2c_address': "0x3C"
        }
    
    if 'encoders' not in result:
        logger.info("Adding default encoder configuration")
        result['encoders'] = [
            {
                'name': 'encoder1',
                'pins': {
                    'A': 5,
                    'B': 6,
                    'switch': 7
                }
            }
        ]
    
    if 'menu' in result:
        logger.info("JSON has top-level 'menu' key")
    elif 'screens' in result:
        logger.info("Moving 'screens' under 'menu' key")
        result['menu'] = {'screens': result.pop('screens')}
    else:
        logger.warning("No menu structure found. Creating empty menu.")
        result['menu'] = {'screens': []}
    
    if 'menu' not in result or not isinstance(result['menu'], dict):
        logger.warning("Invalid menu structure. Creating empty menu.")
        result['menu'] = {'screens': []}
    
    if 'screens' not in result['menu'] or not isinstance(result['menu']['screens'], list):
        logger.warning("Invalid or missing screens. Creating empty screens list.")
        result['menu']['screens'] = []
    
    return result

def render_template(template_path, output_path, context):
    """
    Render a Jinja2 template and write to a file.
    """
    try:
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        template_dir = os.path.dirname(template_path)
        template_file = os.path.basename(template_path)
        env = jinja2.Environment(loader=jinja2.FileSystemLoader(template_dir))
        template = env.get_template(template_file)
        output = template.render(**context)
        with open(output_path, 'w') as f:
            f.write(output)
        logger.info(f"Rendered {template_path} to {output_path}")
        return True
    except jinja2.exceptions.TemplateError as e:
        logger.error(f"Template error: {e}")
        return False
    except Exception as e:
        logger.error(f"Error rendering template: {e}")
        return False

def process_graphics_code(config):
    """
    Process graphics from the configuration.
    """
    graphics_code = []
    if 'graphics' in config:
        for graphic in config['graphics']:
            if graphic['type'] == 'waveform':
                code = f"""
// Waveform graphic: {graphic['id']}
LV_IMG_DECLARE(waveform_icon_dsc);
"""
                graphics_code.append(code)
    return graphics_code

def extract_action_prototypes(config):
    """
    Extract action function prototypes.
    """
    actions = set()
    if 'menu' in config and 'screens' in config['menu']:
        for screen in config['menu']['screens']:
            if 'widgets' in screen:
                for widget in screen['widgets']:
                    if 'items' in widget:
                        for item in widget['items']:
                            if 'action' in item and item['action']:
                                actions.add(item['action'])
    return [f"void {action}(void);" for action in sorted(actions)]

def main():
    """
    Main function to generate menu files.
    
    Args:
        sys.argv[1]: JSON configuration file path (e.g., assets/menu.json)
        sys.argv[2]: Output directory (e.g., generated/)
        sys.argv[3]: Templates directory (e.g., assets/templates/)
    """
    if len(sys.argv) != 4:
        logger.error("Usage: generate_menu_from_templates.py <json_config_path> <output_dir> <templates_dir>")
        sys.exit(1)
    
    json_path = sys.argv[1]
    output_dir = sys.argv[2]
    templates_dir = sys.argv[3]
    
    logger.debug(f"JSON path: {json_path}")
    logger.debug(f"Output directory: {output_dir}")
    logger.debug(f"Templates directory: {templates_dir}")
    
    config = load_json_file(json_path)
    config = adapt_json_structure(config)
    graphics_code = process_graphics_code(config)
    action_prototypes = extract_action_prototypes(config)
    
    context = {
        'config': config,
        'graphics_code': graphics_code,
        'action_prototypes': action_prototypes
    }
    
    menu_c_template = os.path.join(templates_dir, "menu.c.j2")
    menu_c_output = os.path.join(output_dir, "menu.c")
    if not render_template(menu_c_template, menu_c_output, context):
        logger.error("Failed to generate menu.c")
        sys.exit(1)
    
    menu_h_template = os.path.join(templates_dir, "menu.h.j2")
    menu_h_output = os.path.join(output_dir, "menu_data.h")
    if not render_template(menu_h_template, menu_h_output, context):
        logger.error("Failed to generate menu_data.h")
        sys.exit(1)
    
    logger.info("Menu generation completed successfully")

if __name__ == "__main__":
    main()