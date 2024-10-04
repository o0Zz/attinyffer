#ifndef _SOFTUART_H_
#define _SOFTUART_H_

#include <stdint.h>
#include <stdarg.h>

void softUart_init();
void softUart_send_byte(char byte);
void softUart_send_string(const char *fmt, ...);
void softUart_send_buffer(const uint8_t *buf, uint16_t size);

#endif