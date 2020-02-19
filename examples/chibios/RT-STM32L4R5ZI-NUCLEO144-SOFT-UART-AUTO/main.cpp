/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
 * This is an example of use and testing of soft_uart objects in Chibios. It
 * uses timer GPT1 corresponding to timer TIM1 for the soft_uart object, and
 * pins GPIOB_PIN9 and GPIOB_PIN8 as RX and TX pins for the soft_uart object.
 *
 * This example connects soft_uart's TX and RX pins, so they must be connected.
 */

#define CHIBIOS_PORT
#include "soft_uart.h"

using namespace soft_uart;
using namespace soft_uart::chibios;

#define DELAY_MS 0//1000

#define RX_PORT GPIOB  // software serial port's reception port (pin D14)
#define RX_PIN GPIOB_PIN9 // software serial port's reception pin

#define TX_PORT GPIOB  // software serial port's transmission port
#define TX_PIN GPIOB_PIN8 // software serial port's transmission pin (pin D15)

#define SOFT_UART_BIT_RATE 75 // 57600 38400 1200 19200 9600 115200 300 2400
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length
#define TX_BUF_LENGTH 256 // software serial port's transmission buffer length

#include "chprintf.h"

uart<
  &GPTD1, // we are using TIM1 for the GPT
  RX_BUF_LENGTH,
  TX_BUF_LENGTH
> my_soft_uart;


/*
 * Application entry point.
 */
int main(void)
{

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  // starting hardware serial ports
  sdStart(&LPSD1, NULL);

  // serial_obj initialization
  my_soft_uart.config(
    RX_PORT,RX_PIN,
    TX_PORT,TX_PIN,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );

  uint32_t counter=0;
  uint32_t data,status;

  /*
   * Normal main() thread activity, it changes the GPT4 period every
   * five seconds.
   */
  while (true) {

    // waiting for sending
    while(my_soft_uart.is_tx_full()) { /* nothing */ }

    chprintf(
      (BaseSequentialStream *) &LPSD1,
      "==============================\r\n"
      "[%D] soft_uart TX ====> soft_uart RX: %d\r\n",
      chVTGetSystemTime(),
      counter
    );
    my_soft_uart.set_tx_data(static_cast<uint8_t>(counter&0xff));
    counter=(++counter)&0xff;

    while (my_soft_uart.available()>0)
    {
      status=my_soft_uart.get_rx_data(data);
      chprintf(
        (BaseSequentialStream *) &LPSD1,
        "[%D] soft_uart --> received: %d ",
        chVTGetSystemTime(),
        data
      );
      if(my_soft_uart.bad_status(status))
      {
        if(my_soft_uart.bad_start_bit(status))
          chprintf((BaseSequentialStream *) &LPSD1," [BAD_START_BIT]");
        else if(my_soft_uart.bad_parity(status))
          chprintf((BaseSequentialStream *) &LPSD1," [BAD_PARITY]");
        else if(my_soft_uart.bad_stop_bit(status))
          chprintf((BaseSequentialStream *) &LPSD1," [BAD_STOP_BIT]");
      }
      chprintf((BaseSequentialStream *) &LPSD1,"\r\n");
    }

    if(DELAY_MS) chThdSleepMilliseconds(DELAY_MS);
  }
}
