#ifndef APRIL_SSD1315_H
#define APRIL_SSD1315_H

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "april_uart_logger.h"

#define APRIL_SSD1315_NUM_PAGES 8
#define APRIL_SSD1315_NUM_ROWS 64
#define APRIL_SSD1315_NUM_SEGS 128

typedef struct {
    april_logger *logger;
    I2C_HandleTypeDef *i2c;
    uint8_t buffer[APRIL_SSD1315_NUM_PAGES][APRIL_SSD1315_NUM_SEGS];
} april_ssd1315;

void april_ssd1315_init(
    april_ssd1315 *dev,
    april_logger *logger,
    I2C_HandleTypeDef *i2c
);

void april_ssd1315_refresh(april_ssd1315 *dev);

void april_ssd1315_clear(april_ssd1315 *dev);

void april_ssd1315_set_horiz_data(april_ssd1315 *dev, uint8_t horiz_data[APRIL_SSD1315_NUM_PAGES * APRIL_SSD1315_NUM_SEGS]);

void april_ssd1315_set_pixel(april_ssd1315 *dev, uint8_t col_index, uint8_t row_index, bool pixel);

#endif
