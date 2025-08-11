Welcome to ESP Menu Documentation
================================

ESP Menu is an ESP-IDF component that renders a JSON-defined menu with LVGL.

.. toctree::
   :maxdepth: 2

   python
   json
   
Quick Start
===========

Build from ``examples/basic_menu``. Edit ``assets/menu.json`` in the repo root, run the generator, then build/flash the example. See README for exact commands.

Doxygen Documentation
====================

.. raw:: html

   <a href="doxygen/html/index.html">C API Documentation</a>

Code Generation
===============

The menu system’s C code is generated from ``menu.json`` using Jinja templates, incorporating callback functions defined in separate C files. See the Doxygen section for API details.

Troubleshooting
===============

- Build errors like ``Unknown CMake command idf_component_register`` indicate you are building from the wrong directory. Use ``examples/basic_menu``.
- Component resolution errors (``lvgl``) require ESP-IDF 5.4+ with Component Manager; re-run ``idf.py reconfigure``.
