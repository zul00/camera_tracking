/**
 * @file  main.c
 * @brief Control two joint JIWY module
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include "libuart.h"

int main(int argc, char *argv[])
{
  char *rx_str = NULL;
  uint8_t rx_size = 0;
  int16_t stat = 0;

  printf("Hello Camera Tracking!!!\n");

  /* Initialize */
  // UART communication
  stat = uart_config();
  if (stat < 0)
  {printf("UART configuration failed. Exiting...\n");exit(-1);}
  else
  {printf("UART configured\n");}

  rx_str = malloc(100 * sizeof(rx_str));

  uart_transmit("Hello from Serial\n");

  for(;;)
  {
    rx_size = uart_receive(&rx_str);

    printf("Received %u char \n=> %s\n", rx_size, rx_str);
  }

  return 0;
}
