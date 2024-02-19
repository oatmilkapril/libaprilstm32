#include "april_ssd1315.h"

typedef enum {
    APRIL_SSD1315_ADDR_READ = 0x33,
    APRIL_LSM303_ADDR_WRITE = 0x33,
} april_lsm303_addr;

// Assume SAO bit is 0
const uint16_t READ_ADDR = 0x79;
const uint16_t WRITE_ADDR = 0x78;

const uint16_t COMMAND_CONTROL = 0x00;
const uint16_t COMMAND_DATA = 0x40;

void april_ssd1315_init(
    april_ssd1315 *dev,
    april_logger *logger,
    I2C_HandleTypeDef *i2c
) {
    dev->logger = logger;
    dev->i2c = i2c;

    // Set display off
    uint8_t write_data = 0xae;
    HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);

    // Set column start address to 0
    write_data = 0x00;
    HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);
    write_data = 0x10;
    HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);

    // Enable charge pump
    write_data = 0x8d;
    HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);
    write_data = 0x14;
    HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);

    // Enable display
    write_data = 0xa4;
    HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);
    write_data = 0xa6;
    HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);
    write_data = 0xaf;
    HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);
}

void april_ssd1315_refresh(april_ssd1315 *dev) {
    uint8_t write_data;

    for (uint8_t page_index = 0; page_index < APRIL_SSD1315_NUM_PAGES; page_index++) {
        // Set page number
        write_data = 0xb0 + page_index;
        HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);

        // Set column start index 
        write_data = 0x0;
        HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);
        write_data = 0x10;
        HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_CONTROL, 1, &write_data, 1, 1000);

        for (uint8_t segment_index = 0; segment_index < APRIL_SSD1315_NUM_SEGS; segment_index++) {
            uint8_t write_data = dev->buffer[page_index][segment_index]; // TODO: This is bad stride pattern
            HAL_I2C_Mem_Write(dev->i2c, WRITE_ADDR, COMMAND_DATA, 1, &write_data, 1, 1000);
        }
    }
}

void april_ssd1315_clear(april_ssd1315 *dev) {
    for (uint8_t page_index = 0; page_index < APRIL_SSD1315_NUM_PAGES; page_index++) {
        for (uint8_t segment_index = 0; segment_index < APRIL_SSD1315_NUM_SEGS; segment_index++) {
            if (segment_index % 2 == 0) {
                dev->buffer[page_index][segment_index] = 0xFF;
            } else {
                dev->buffer[page_index][segment_index] = 0x00;
            }
        }
    }
}

void april_ssd1315_set_horiz_data(april_ssd1315 *dev, uint8_t horiz_data[APRIL_SSD1315_NUM_PAGES * APRIL_SSD1315_NUM_SEGS]) {
    for (int page_index = 0; page_index < APRIL_SSD1315_NUM_PAGES; page_index++) {
        for (int segment_index = 0; segment_index < APRIL_SSD1315_NUM_SEGS; segment_index++) {
            dev->buffer[page_index][segment_index] = horiz_data[page_index * APRIL_SSD1315_NUM_SEGS + segment_index];
        }
   }
}

void april_ssd1315_set_pixel(april_ssd1315 *dev, uint8_t col_index, uint8_t row_index, bool pixel) {
    // Compute buffer indices
    uint8_t page_index = row_index / 8;
    uint8_t segment_index = col_index;
    uint8_t pixel_index = row_index % 8;

    if (page_index >= APRIL_SSD1315_NUM_PAGES || segment_index >= APRIL_SSD1315_NUM_SEGS) {
        return;
    }
 
    // Get existing data
    uint8_t existing_data = dev->buffer[page_index][segment_index];

    if (pixel) {
        // TODO: This might be backwards
        existing_data = existing_data | (1 << pixel_index);
    } else {
        existing_data = existing_data & ~(1 << pixel_index);
    }

    dev->buffer[page_index][segment_index] = existing_data;
}
