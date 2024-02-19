
#ifndef APRIL_UART_LOGGER_H
#define APRIL_UART_LOGGER_H
#include "stm32f4xx_hal.h"


typedef struct {
    UART_HandleTypeDef *uart;
} april_logger;

void april_logger_init(april_logger *logger, UART_HandleTypeDef *uart);

void april_logger_write(april_logger *logger, char *message);

#endif
