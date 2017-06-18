#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "libuart.h"

#define DEV_PATH  "/dev/ttyO0"
#define BAUDRATE  B115200
#define UART_TX_BUF_SIZE  20
#define UART_RX_BUF_SIZE  20

int16_t fd = 0;

char uart_tx_buffer[UART_TX_BUF_SIZE];
char uart_rx_buffer[UART_RX_BUF_SIZE];

/**                                                                             
 * @brief  Configure UART communication                   
 * @retval None                                                                 
 */                                                                             
int16_t uart_config(void)                                                      
{                                                                               
  struct termios newtio;

  fd = open(DEV_PATH, O_RDWR | O_NOCTTY | O_NONBLOCK);                       
  if (fd < 0) {perror("Opening " DEV_PATH); return -1; }                                  

  bzero(&newtio, sizeof(newtio));                                               
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;                   
  newtio.c_iflag = IGNPAR;                                                      
  newtio.c_oflag = 0;                                                           

  /* set input mode (non-canonical, no echo,...) */                             
  newtio.c_lflag = 0;                                                           

  /* Configure for polling read, non-blocking */
  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */               
  newtio.c_cc[VMIN]     = 0;   /* Min number of char for noncanonical read */       

  tcflush(fd, TCIFLUSH);                                                        
  tcsetattr(fd,TCSANOW,&newtio);                                                

  return(0);                                                                    
}                                                                               

/**                                                                             
 * @brief     Transmit string via UART
 * @param[in] format  Pointer to string format                             
 * @retval    None                                                                
 */                                                                             
void uart_transmit(char *format, ...)                                             
{                                                                               
  va_list args;                                                                 
  uint16_t len = 0;                                                             

  /* Get string format */                                                      
  va_start(args, format);                                                       
  vsnprintf(uart_tx_buffer, UART_TX_BUF_SIZE, format, args);                      
  len = strlen(uart_tx_buffer);                                                   

  /* Send string */                            
  write(fd, uart_tx_buffer, len);                                                 
}                                                                               


/**                                                                             
 * @brief     Receive string via UART                                                  
 * @param[in] format Double pointer to received string                            
 * @retval                                                                  
 */                                                                             
int16_t uart_receive(char **str)                                                  
{                                                                               
  char c = 0;
  uint16_t ctr = 0;                                                             
  ssize_t len_read = 0;                                                         

  /* Reset buffer */
  memset(uart_rx_buffer, 0, UART_RX_BUF_SIZE);                                    

  /* Dump stream to buffer while new line not found */
  do 
  {
    // Read one char
    len_read = read(fd, &c, 1);                                         

    if (len_read != 0)                // Char found
    {
      if (ctr > UART_TX_BUF_SIZE - 1)
      { // Buffer overflow
        return -1;
      }
      uart_rx_buffer[ctr] = c;      // Append char
    }
  } while (uart_rx_buffer[ctr++] != '\n');

  // Remove '\n'
  uart_rx_buffer[--ctr] = 0;
  // Pass pointer
  *str = uart_rx_buffer;

  return ctr;
}                
