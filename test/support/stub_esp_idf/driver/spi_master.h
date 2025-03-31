#ifndef SPI_MASTER_H
#define SPI_MASTER_H

#include "../esp_common/esp_err.h"

typedef void *spi_host_device_t;
typedef void *spi_device_handle_t;

#define SPI2_HOST 1
#define SPI3_HOST 2

typedef struct
{
    int mosi_io_num;
    int miso_io_num;
    int sclk_io_num;
    int max_transfer_sz;
    int flags;
} spi_bus_config_t;

esp_err_t spi_bus_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan);

#endif // SPI_MASTER_H