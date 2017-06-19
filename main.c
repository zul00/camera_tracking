/**
 * @file  main.c
 * @brief Control two joint JIWY module
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#include "libuart.h"

int main(int argc, char *argv[])
{
  char *rx_str = NULL;
  int16_t rx_size = 0;
  int16_t stat = 0;

  int16_t encPan, encTilt = 0;
  char *token;

  printf("Hello Camera Tracking!!!\n");

  /* Initialize */
  // UART communication
  stat = uart_config();
  if (stat < 0)
  {printf("UART configuration failed. Exiting...\n");exit(-1);}
  else
  {printf("UART configured\n");}

  rx_str = malloc(20 * sizeof(rx_str));

  uart_transmit("Hello from Serial\n");

  for(;;)
  {
    // Receive UART
    rx_size = uart_receive(&rx_str);
    if (rx_size > 0)
    {
      printf("Out %d => %s\n", rx_size, rx_str);

      /* Parsing */
      // First token
      token = strtok(rx_str, ";");
      encPan = strtoimax(token, NULL, 10);
      printf("token 1 = %s; %d\n", token, encPan);
      // Next token(s)
      token = strtok(NULL, ";");
      encTilt = strtoimax(token, NULL, 10);
      printf("token 2 = %s; %d\n", token, encTilt);
    }
  }

  return 0;
}
