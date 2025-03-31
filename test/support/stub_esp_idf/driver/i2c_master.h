// test/support/stub_esp_idf/driver/i2c_master.h
#ifndef I2C_MASTER_H
#define I2C_MASTER_H

// Add minimal typedefs and function declarations here
typedef struct
{
    int dummy;
} i2c_config_t;

typedef enum
{
    I2C_NUM_0 = 0,
    I2C_NUM_1 = 1
} i2c_port_t;

#define I2C_CLK_SRC_DEFAULT 0

typedef enum
{
    I2C_MODE_SLAVE,
    I2C_MODE_MASTER,
    I2C_MODE_MAX,
} i2c_mode_t;

typedef struct
{
    int clk_source;
    int glitch_ignore_cnt;
    int i2c_port;
    int sda_io_num;
    int scl_io_num;
    struct
    {
        bool enable_internal_pullup;
    } flags;
} i2c_master_bus_config_t;

typedef void *i2c_master_bus_handle_t;

esp_err_t i2c_new_master_bus(const i2c_master_bus_config_t *bus_config, i2c_master_bus_handle_t *ret_handle);

// Add other typedefs and enums as needed

#endif // I2C_MASTER_H