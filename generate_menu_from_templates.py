#!/usr/bin/env python3
"""
Script to generate menu.c and menu.h from Jinja2 templates and menu.json.
This script fixes the missing menu generation functionality by properly
processing the Jinja templates with the menu configuration.
"""

import os
import sys
import json
import jinja2
import logging

# Configure logging
logging.basicConfig(level=logging.DEBUG, 
                    format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def load_json_file(json_path):
    """
    Load and parse the JSON configuration file.
    
    Args:
        json_path (str): Path to the JSON configuration file
        
    Returns:
        dict: The parsed JSON data, or None if an error occurred
    """
    # Log the original path being used
    logger.debug(f"Original json_path argument: {json_path}")
    abs_path = os.path.abspath(json_path)
    logger.debug(f"Resolved menu.json absolute path: {abs_path}")
    
    # Define a list of potential paths to try in order
    potential_paths = [
        json_path,  # Try the provided path first
    ]
    
    # Get the current working directory which should be the project root when run via CMake
    cwd = os.getcwd()
    logger.debug(f"Current working directory: {cwd}")
    
    # If we're in the components directory, the project root is one level up
    if os.path.basename(cwd) == "Esp_menu" or os.path.basename(os.path.dirname(cwd)) == "components":
        project_dir = os.path.abspath(os.path.join(cwd, "..", ".."))
    else:
        project_dir = cwd
    
    logger.debug(f"Detected project directory: {project_dir}")
    
    # Always prioritize project-specific paths
    project_paths = [
        os.path.join(project_dir, "main", "user_menu", "menu.json"),
        # Try other variations if needed
    ]
    
    # Add project paths to the list of paths to try
    potential_paths.extend(project_paths)
    
    # Log all potential paths for debugging
    logger.debug(f"Will try the following paths to find menu.json:")
    for idx, path in enumerate(potential_paths):
        logger.debug(f"  {idx+1}. {path} (absolute: {os.path.abspath(path)})")
    
    # Try each path until we find a valid JSON file
    for try_path in potential_paths:
        try:
            abs_try_path = os.path.abspath(try_path)
            logger.debug(f"Attempting to load JSON from: {abs_try_path}")
            
            if not os.path.exists(abs_try_path):
                logger.debug(f"File does not exist: {abs_try_path}")
                continue
                
            with open(abs_try_path, 'r') as f:
                data = json.load(f)
            logger.info(f"Successfully loaded config from {abs_try_path}")
            return data
        except FileNotFoundError:
            logger.debug(f"JSON file not found: {abs_try_path}")
            continue
        except json.JSONDecodeError as e:
            logger.error(f"Invalid JSON in {abs_try_path}: {e}")
            continue
        except Exception as e:
            logger.error(f"Unexpected error loading JSON from {abs_try_path}: {e}")
            continue
    
    # If we get here, none of the paths worked - create an empty config
    logger.warning("Could not find a valid menu.json file in any location. Using empty config.")
    return {}

def adapt_json_structure(config):
    """
    Adapts the JSON structure to match what the template expects.
    If the JSON already has a top-level 'menu' key, return it as is.
    If it doesn't, create a structure with a top-level 'menu' key.
    
    Args:
        config (dict): The original configuration dictionary
        
    Returns:
        dict: The adapted configuration dictionary
    """
    # Add default values for required fields
    result = config.copy()
    
    # Add default display config if missing
    if 'display' not in result:
        logger.info("Adding default display configuration")
        result['display'] = {
            'width': 128,
            'height': 64,
            'sda_pin': 21,
            'scl_pin': 22,
            'i2c_address': "0x3C"
        }
    
    # Add default encoder config if missing
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
    
    # Ensure menu structure is correct
    if 'menu' in result:
        logger.info("JSON already has a top-level 'menu' key")
    elif 'screens' in result:
        logger.info("Moving top-level 'screens' under a 'menu' key")
        result['menu'] = {'screens': result.pop('screens')}
    else:
        logger.warning("No menu structure found in JSON. Creating empty menu.")
        result['menu'] = {'screens': []}
    
    # Verify the structure is correct for the templates
    if 'menu' not in result or not isinstance(result['menu'], dict):
        logger.warning("Invalid menu structure. Creating empty menu.")
        result['menu'] = {'screens': []}
    
    if 'screens' not in result['menu'] or not isinstance(result['menu']['screens'], list):
        logger.warning("Invalid or missing screens in menu. Creating empty screens list.")
        result['menu']['screens'] = []
    
    return result

def render_template(template_path, output_path, context):
    """
    Render a Jinja2 template and write the result to a file.
    
    Args:
        template_path (str): Path to the Jinja2 template file
        output_path (str): Path where the rendered file should be written
        context (dict): Context data to provide to the template
        
    Returns:
        bool: True if successful, False otherwise
    """
    try:
        # Create output directory if it doesn't exist
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        
        # Set up Jinja environment
        template_dir = os.path.dirname(template_path)
        template_file = os.path.basename(template_path)
        env = jinja2.Environment(loader=jinja2.FileSystemLoader(template_dir))
        
        # Get the template
        template = env.get_template(template_file)
        
        # Render the template
        output = template.render(**context)
        
        # Write the result to the output file
        with open(output_path, 'w') as f:
            f.write(output)
            
        logger.info(f"Successfully rendered {template_path} to {output_path}")
        return True
    except jinja2.exceptions.TemplateError as e:
        logger.error(f"Template error: {e}")
        return False
    except Exception as e:
        logger.error(f"Error rendering template: {e}")
        return False

def process_graphics_code(config):
    """
    Process graphics from the configuration and generate code for them.
    
    Args:
        config (dict): Configuration dictionary
        
    Returns:
        list: List of code snippets for graphics
    """
    graphics_code = []
    
    # Process graphics section if it exists
    if 'graphics' in config:
        for graphic in config['graphics']:
            if graphic['type'] == 'waveform':
                # Generate code for waveform graphics
                code = f"""
// Waveform graphic: {graphic['id']}
LV_IMG_DECLARE(waveform_icon_dsc);
"""
                graphics_code.append(code)
    
    return graphics_code

def extract_action_prototypes(config):
    """
    Extract action function prototypes from the configuration.
    
    Args:
        config (dict): Configuration dictionary
        
    Returns:
        list: List of function prototypes
    """
    actions = set()
    
    # Extract unique action names from menu screens
    if 'menu' in config and 'screens' in config['menu']:
        for screen in config['menu']['screens']:
            if 'widgets' in screen:
                for widget in screen['widgets']:
                    if 'items' in widget:
                        for item in widget['items']:
                            if 'action' in item and item['action']:
                                actions.add(item['action'])
    
    # Generate prototypes
    prototypes = [f"void {action}(void);" for action in sorted(actions)]
    return prototypes

def main():
    """
    Main function to process templates and generate menu files.
    
    Expected arguments:
        sys.argv[1]: JSON configuration file path
                     Note: Even if this path is incorrect (e.g., in ESP-IDF directory),
                     the script will attempt to find the file in the project's main/user_menu/ directory
        sys.argv[2]: Output directory path for generated files
        sys.argv[3]: Templates directory path
    """
    if len(sys.argv) != 4:
        logger.error("Usage: generate_menu_from_templates.py <json_config_path> <output_dir> <templates_dir>")
        sys.exit(1)
    
    json_path = sys.argv[1]
    output_dir = sys.argv[2]
    templates_dir = sys.argv[3]
    
    logger.debug(f"Script called with arguments:")
    logger.debug(f"  JSON path: {json_path}")
    logger.debug(f"  Output directory: {output_dir}")
    logger.debug(f"  Templates directory: {templates_dir}")
    
    # Ensure paths are absolute
    json_path = os.path.abspath(json_path)
    output_dir = os.path.abspath(output_dir)
    templates_dir = os.path.abspath(templates_dir)
    
    # Load configuration from JSON
    config = load_json_file(json_path)
    if config is None:
        logger.error("Failed to load configuration. Exiting.")
        sys.exit(1)
    
    # Adapt JSON structure
    config = adapt_json_structure(config)
    
    # Process graphics and extract action prototypes
    graphics_code = process_graphics_code(config)
    action_prototypes = extract_action_prototypes(config)
    
    # Create context for templates
    context = {
        'config': config,
        'graphics_code': graphics_code,
        'action_prototypes': action_prototypes
    }
    
    # Render menu.c
    menu_c_template = os.path.join(templates_dir, "menu.c.j2")
    menu_c_output = os.path.join(output_dir, "menu.c")
    if not render_template(menu_c_template, menu_c_output, context):
        logger.error("Failed to generate menu.c")
        sys.exit(1)
    
    # Render menu.h
    menu_h_template = os.path.join(templates_dir, "menu.h.j2")
    menu_h_output = os.path.join(output_dir, "menu_data.h")
    if not render_template(menu_h_template, menu_h_output, context):
        logger.error("Failed to generate menu_data.h")
        sys.exit(1)
    
    logger.info("Menu generation completed successfully")

if __name__ == "__main__":
    main()