#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

//8N1
#define BAUDRATE 19200
#define UART_PIN 0

#define clear_bit(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define set_bit(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

void softUart_init()
{
    set_bit(DDRB, UART_PIN); //Set pin as output
    set_bit(PORTB, UART_PIN); //Set high (Idle state)
}

void softUart_send_byte(char byte)
{
  int delay_bit_us = 1000000 / BAUDRATE;

  cli();  // Disable Interrupts
  
    //Start bit
  set_bit(PORTB, UART_PIN);
  _delay_us(delay_bit_us);
  clear_bit(PORTB, UART_PIN);
  _delay_us(delay_bit_us);

    //Data
  char data = byte;
  for (int i=0; i<8; i++)
  {
    if (data & 0x01)
      set_bit(PORTB, UART_PIN);
    else
      clear_bit(PORTB, UART_PIN);
    data >>= 1;
    _delay_us(delay_bit_us);
  }

    //1 Stop bit
  set_bit(PORTB, UART_PIN);

  sei(); // Enable Interrupts

  _delay_us(delay_bit_us);
}

void softUart_send_string(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  char buffer[16];
  vsprintf(buffer, fmt, args);

  for (int i=0; i<strlen(buffer); i++)
    softUart_send_byte(buffer[i]);

  va_end(args);
}

void softUart_send_buffer(const uint8_t *buf, uint16_t size)
{
  for (int i=0; i<size; i++)
    softUart_send_string("%02X", buf[i]);
}
