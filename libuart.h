#ifndef __LIBUART_H
#define __LIBUART_H

#include <stdint.h>

int16_t uart_config(void);
void uart_transmit(char *format, ...);
int16_t uart_receive(char **string);

#endif
