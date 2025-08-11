esp_menu Unity tests
====================

Run with the ESP-IDF Unit Test App on hardware:

1. Configure pins in examples/basic_menu (menuconfig: display + encoder).
2. Build and run Unit Test App including this component:

```bash
idf.py -C "$IDF_PATH"/tools/unit-test-app -D TEST_COMPONENTS=esp_menu build flash monitor
```
3. At the Unity prompt:

```text
unity> *            # run all tests
unity> [esp_menu]   # filter this component’s tests
```

