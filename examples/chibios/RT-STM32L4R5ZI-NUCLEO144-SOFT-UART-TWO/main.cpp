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
 * uses two soft_uart objects. A first one which uses timer GPT1 corresponding
 * to timer TIM1, and pins GPIOB_PIN9 and GPIOB_PIN8 as RX and TX pins. The
 * second soft_uart utilizes timer GPT3 corresponding to timer TIM3, and pins
 * GPIOA_PIN6 and GPIOA_PIN5 as RX and TX pins.
 *
 * For correct operation RX and TX pins must be cross connected for both
 * soft_uart objects.
 */

#define CHIBIOS_PORT
#include "soft_uart.h"

using namespace soft_uart;
using namespace soft_uart::chibios;

#define DELAY_MS 0 // 1000

#define RX_PORT_1 GPIOB  // first software serial port's reception port (pin D14)
#define RX_PIN_1 GPIOB_PIN9 // first software serial port's reception pin

#define TX_PORT_1 GPIOB  // first software serial port's transmission port
#define TX_PIN_1 GPIOB_PIN8 // first software serial port's transmission pin (pin D15)

#define RX_PORT_2 GPIOA  // second software serial port's reception port (pin D14)
#define RX_PIN_2 GPIOA_PIN6 // second software serial port's reception pin

#define TX_PORT_2 GPIOA  // second software serial port's transmission port
#define TX_PIN_2 GPIOA_PIN5 // second software serial port's transmission pin (pin D15)

#define SOFT_UART_BIT_RATE 57600 // 57600 38400 1200 19200 9600 115200 300 2400
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length
#define TX_BUF_LENGTH 256 // software serial port's transmission buffer length

#include "chprintf.h"

uart<
  &GPTD1, // we are using TIM1 for the GPT
  RX_BUF_LENGTH,
  TX_BUF_LENGTH
> my_soft_uart_1;

uart<
  &GPTD3, // we are using TIM1 for the GPT
  RX_BUF_LENGTH,
  TX_BUF_LENGTH
> my_soft_uart_2;

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

  // soft_uart's objects initialization
  my_soft_uart_1.config(
    RX_PORT_1,RX_PIN_1,
    TX_PORT_1,TX_PIN_1,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::NINE_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  my_soft_uart_2.config(
    RX_PORT_2,RX_PIN_2,
    TX_PORT_2,TX_PIN_2,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::NINE_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );

  uint32_t counter=257;
  uint32_t data,status;

  /*
   * Normal main() thread activity, it changes the GPT4 period every
   * five seconds.
   */
  while (true) {

    // sending from soft_uart 1 to soft_uart 2
    chprintf(
      (BaseSequentialStream *) &LPSD1,
      "==============================\r\n"
      "[%D] soft_uart 1 ====> soft_uart 2: %d\r\n",
      chVTGetSystemTime(),
      counter
    );
    // waiting for sending
    while(my_soft_uart_1.is_tx_full()) { /* nothing */ }
    my_soft_uart_1.set_tx_data(counter);
    counter=(counter+1)&0x1ff;

    while(my_soft_uart_2.available()>0)
    {
      status=my_soft_uart_2.get_rx_data(data);
      chprintf(
        (BaseSequentialStream *) &LPSD1,
        "[%D] soft_uart 2 --> received: %d ",
        chVTGetSystemTime(),
        data
      );
      if(my_soft_uart_2.bad_status(status))
      {
        if(my_soft_uart_2.bad_start_bit(status))
          chprintf((BaseSequentialStream *) &LPSD1," [BAD_START_BIT]");
        else if(my_soft_uart_2.bad_parity(status))
          chprintf((BaseSequentialStream *) &LPSD1," [BAD_PARITY]");
        else if(my_soft_uart_2.bad_stop_bit(status))
          chprintf((BaseSequentialStream *) &LPSD1," [BAD_STOP_BIT]");
      }
      chprintf((BaseSequentialStream *) &LPSD1,"\r\n");

      // sending back from from soft_uart 2 to soft_uart 1
      chprintf(
        (BaseSequentialStream *) &LPSD1,
        "[%D] soft_uart 2 ====> soft_uart 1: %d\r\n",
        chVTGetSystemTime(),
        data
      );
      // waiting for sending
      while(my_soft_uart_2.is_tx_full()) { /* nothing */ }
      my_soft_uart_2.set_tx_data(data);

      // waiting for something
      while(my_soft_uart_1.available()<=0) { /* nothing */ }
      status=my_soft_uart_1.get_rx_data(data);
      chprintf(
        (BaseSequentialStream *) &LPSD1,
        "[%D] soft_uart 1 --> received: %d ",
        chVTGetSystemTime(),
        data
      );
      if(my_soft_uart_1.bad_status(status))
      {
        if(my_soft_uart_1.bad_start_bit(status))
          chprintf((BaseSequentialStream *) &LPSD1," [BAD_START_BIT]");
        else if(my_soft_uart_1.bad_parity(status))
          chprintf((BaseSequentialStream *) &LPSD1," [BAD_PARITY]");
        else if(my_soft_uart_1.bad_stop_bit(status))
          chprintf((BaseSequentialStream *) &LPSD1," [BAD_STOP_BIT]");
      }
      chprintf((BaseSequentialStream *) &LPSD1,"\r\n");
    }

    if(DELAY_MS) chThdSleepMilliseconds(DELAY_MS);
  }
}
