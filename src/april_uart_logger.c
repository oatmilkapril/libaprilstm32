#include "april_uart_logger.h"
#include <stdint.h>
#include <string.h>

void april_logger_init(april_logger *logger, UART_HandleTypeDef *uart) {
    logger->uart = uart;
}

void april_logger_write(april_logger *logger, char *message) {
    HAL_UART_Transmit_IT(logger->uart, (uint8_t*) message, strlen(message));

}

