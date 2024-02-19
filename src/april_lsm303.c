#include "april_lsm303.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int _get_shift(april_lsm303 *dev);
float _get_mg_per_lsb(april_lsm303 *dev);

typedef enum {
    APRIL_LSM303_RES_8 = 8, // 8-bit resolution, ignore 8 LSB
    APRIL_LSM303_RES_10 = 10, // 10-bit resolution, ignore 6 LSB
    APRIL_LSM303_RES_12 = 12, // 12-bit resolution, ignore 4 LSB
} april_lsm303_res;

const uint16_t APRIL_LSM303_MULTIREAD_OR_MASK = 0x80; // 1000 000

typedef enum {
    APRIL_LSM303_ADDR_READ = 0x33,
    APRIL_LSM303_ADDR_WRITE = 0x33,
} april_lsm303_addr;

typedef enum {
    APRIL_LSM303_CTRL_REG1_A = 0x20,
    APRIL_LSM303_CTRL_REG4_A = 0x23,
    APRIL_LSM303_OUT_X_L_A = 0x28
} april_lsm303_reg;

typedef enum {
    APRIL_LSM303_10_HZ = 0x1
} april_lsm303_data_rate;

typedef enum {
    APRIL_LSM303_X_EN = 1,
    APRIL_LSM303_Y_EN = 2,
    APRIL_LSM303_Z_EN = 4
} april_lsm303_axis;

const float MS2_PER_EARTH_G = 9.80665f;


void april_lsm303_init(
    april_lsm303 *dev, 
    I2C_HandleTypeDef *i2c, 
    april_logger *logger,
    april_lsm303_mode mode,
    april_lsm303_range range
) {
    dev->i2c = i2c;
    dev->mode = mode;
    dev->range = range;
    dev->logger = logger;

    uint8_t write_data = (APRIL_LSM303_10_HZ << 4) 
        | ((mode == APRIL_LSM303_MODE_LOW) << 3) 
        | APRIL_LSM303_X_EN | APRIL_LSM303_Y_EN | APRIL_LSM303_Z_EN;
    HAL_I2C_Mem_Write(dev->i2c, APRIL_LSM303_ADDR_WRITE, APRIL_LSM303_CTRL_REG1_A, 1, &write_data, 1, 1000);

    uint8_t read_data;
    HAL_I2C_Mem_Read(dev->i2c, APRIL_LSM303_ADDR_READ, APRIL_LSM303_CTRL_REG1_A, 1 , &read_data, 1, 1000);
    if (read_data != write_data) {
        char error_message[128]; 
        snprintf(error_message, sizeof(error_message), "Unexpected CTRL_REG1_A value. expected %04x but was %04x\r\n", write_data, read_data);
        april_logger_write(dev->logger, error_message);
    }

    write_data = (range << 4) | ((range == APRIL_LSM303_MODE_HIGH) << 3);
    HAL_I2C_Mem_Write(dev->i2c, APRIL_LSM303_ADDR_WRITE, APRIL_LSM303_CTRL_REG4_A, 1, &write_data, 1, 1000);
}

void april_lsm303_get_event(april_lsm303 *dev, april_lsm303_accel_data *event) {
    uint8_t read_data[6];

    // Set register address MSB to read multiple subsequent registers
    uint16_t reg_addr = APRIL_LSM303_OUT_X_L_A | APRIL_LSM303_MULTIREAD_OR_MASK;

    HAL_I2C_Mem_Read(dev->i2c, APRIL_LSM303_ADDR_WRITE, reg_addr, 1, read_data, sizeof(read_data), 1000);

    uint16_t shift = _get_shift(dev);
    float sensitivity = _get_mg_per_lsb(dev);
    int16_t x_accel = (int16_t) ((read_data[1] << 8) | read_data[0]) >> shift;
    int16_t y_accel = (int16_t) ((read_data[3] << 8) | read_data[2]) >> shift;
    int16_t z_accel = (int16_t) ((read_data[5] << 8) | read_data[4]) >> shift;
    float x_accel_ms2 = (float) x_accel * sensitivity * MS2_PER_EARTH_G;
    float y_accel_ms2 = (float) y_accel * sensitivity * MS2_PER_EARTH_G;
    float z_accel_ms2 = (float) z_accel * sensitivity * MS2_PER_EARTH_G;

    event->x = x_accel_ms2;
    event->y = y_accel_ms2;
    event->z = z_accel_ms2;
}

int _get_shift(april_lsm303 *dev) {
    switch (dev->mode) {
        case APRIL_LSM303_MODE_NORMAL:
            return 16 - APRIL_LSM303_RES_10;
        case APRIL_LSM303_MODE_HIGH:
            return 16 - APRIL_LSM303_RES_12;
        case APRIL_LSM303_MODE_LOW:
            return 16 - APRIL_LSM303_RES_8;
        default:
            exit(1);
    }
}

float _get_mg_per_lsb(april_lsm303 *dev) {
    switch (dev->range) {
        case APRIL_LSM303_RANGE_2:
            return 0.001;
        case APRIL_LSM303_RANGE_4:
            return 0.002;
        case APRIL_LSM303_RANGE_8:
            return 0.004;
        case APRIL_LSM303_RANGE_16:
            return 0.012;
        default: 
            exit(1);
    }
}
