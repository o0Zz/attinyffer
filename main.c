#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

// oscillator calibration routine
#include "libs-device/osccal.h"

// v-usb lib
#include "usbconfig.h"
#include "usbdrv/usbdrv.h"

#include "gamepad/gamepad.h"

#include "uart/uart.h"
#include <string.h>

/* ------------------------------------------------------------- 
                        LOGGING
   ------------------------------------------------------------- */

uint32_t logTotalLen = 0;
uint8_t logBuffer[420];
uint8_t logBufferLen = 0;

void log_buffer(uint8_t *data, uint8_t len)
{
  if (logBufferLen + len + 2 > sizeof(logBuffer))
    return;

  logBuffer[logBufferLen++] = len;
  memcpy(&logBuffer[logBufferLen], data, len);  
  logBufferLen += len;
}

void log_flush()
{
  if (logBufferLen == 0)
    return;

  softUart_send_string("******\r\n");
  for (int i=0; i<logBufferLen;)
  {
    uint8_t len = logBuffer[i++];

    softUart_send_string("-> ");
    softUart_send_buffer(&logBuffer[i], len);
    softUart_send_string("\r\n");
    
    i += len;
  }
  softUart_send_string("------\r\n");
  logBufferLen = 0;
}

void usb_rx_hook(uint8_t *data, uint8_t len)
{
  log_buffer(data, len);
}

/* ------------------------------------------------------------- */

// USB reset hook, see usbconfig.h
void hadUsbReset(void) {
  cli();  // usbMeasureFrameLength() counts CPU cycles, so disable interrupts.
  calibrateOscillator();
  sei();
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  return 0; // Nothing implemented
}

int main(void) {

  // enable Watchdog Timer
  wdt_enable(WDTO_1S);

  // Disable analog comparator to save power
  ACSR |= (1<<ACD);

  // force USB re-enumeration
  usbDeviceDisconnect();
  _delay_ms(500);
  usbDeviceConnect();

  initController();
  softUart_init();

  // init USB
  usbInit();

  // enable interrupts
  sei();

  // MAIN LOOP START
  for(uint32_t i=0;;i++) {

    // pet the Watchdog
    wdt_reset();

    // poll USB
    usbPoll();

    // send report if host is ready
    if(usbInterruptIsReady()){
        memset(&reportBuffer, 0, sizeof(reportBuffer));
        readController();
        usbSetInterrupt((void *)&reportBuffer, sizeof(reportBuffer));
    }

    if ((i % 100000) == 0) //Regularly flush logs (~4s)
      log_flush();
  }
  
  // MAIN LOOP end
}
