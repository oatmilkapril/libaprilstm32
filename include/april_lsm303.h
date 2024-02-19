#ifndef APRIL_LSM303_H
#define APRIL_LSM303_H

#include "stm32f4xx_hal.h"
#include "april_uart_logger.h"

typedef enum {
    APRIL_LSM303_MODE_NORMAL = 0,  // 10-bit resolution
    APRIL_LSM303_MODE_HIGH = 1, // 12-bit resolution
    APRIL_LSM303_MODE_LOW = 2 // 8-bit resolution (unsure about this)
} april_lsm303_mode;

typedef enum {
    APRIL_LSM303_RANGE_2 = 0x0, // +- 2g
    APRIL_LSM303_RANGE_4 = 0x1, // +- 4g
    APRIL_LSM303_RANGE_8 = 0x2, // +- 8g
    APRIL_LSM303_RANGE_16 = 0x3 // +- 16g
} april_lsm303_range;

typedef struct {
    I2C_HandleTypeDef *i2c;
    april_logger *logger;
    april_lsm303_mode mode;
    april_lsm303_range range;
} april_lsm303;

typedef struct {
    float x;
    float y;
    float z;
} april_lsm303_accel_data;

void april_lsm303_init(
    april_lsm303 *dev, 
    I2C_HandleTypeDef *i2c,
    april_logger *logger,
    april_lsm303_mode mode,
    april_lsm303_range range
);

void april_lsm303_get_event(april_lsm303 *dev, april_lsm303_accel_data *event);

#endif