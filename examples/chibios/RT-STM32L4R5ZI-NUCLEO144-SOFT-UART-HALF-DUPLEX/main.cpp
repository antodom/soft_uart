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
 * This is an example of use and testing of soft_uart objects in half dublex
 * mode in Chibios. It uses two soft_uart objects. A first one which uses timer
 * GPT1 corresponding to timer TIM1, and pins GPIOB_PIN9. The second soft_uart
 * utilizes timer GPT3 corresponding to timer TIM3, and pin GPIOB_PIN8.
 *
 * For correct operation both soft_uart's pins must be connected.
 */

#define CHIBIOS_PORT
#include "soft_uart.h"

using namespace soft_uart;
using namespace soft_uart::chibios;

#define DELAY_MS 1000

#define PORT_1 GPIOB  // first software serial port's port (pin D14)
#define PIN_1 GPIOB_PIN9 // first software serial port's pin

#define PORT_2 GPIOB  // second software serial port's port
#define PIN_2 GPIOB_PIN8 // second software serial port's pin (pin D15)

#define SOFT_UART_BIT_RATE 9600 // 57600 38400 1200 19200 9600 115200 300 2400
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
  my_soft_uart_1.half_duplex_config(
    PORT_1,PIN_1,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT,
    false // on transmission mode (the default is on reception mode)
  );
  my_soft_uart_2.half_duplex_config(
    PORT_2,PIN_2,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
    // initially on reception mode, last argument is true by default
  );

  uint32_t counter=0;
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
    my_soft_uart_1.set_tx_data(static_cast<uint8_t>(counter&0xff));
    counter=(++counter)&0xff;

    while(my_soft_uart_2.available()<=0) { /* nothing */ }
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

    my_soft_uart_2.set_tx_mode();
    my_soft_uart_1.set_rx_mode();

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

    my_soft_uart_1.set_tx_mode();
    my_soft_uart_2.set_rx_mode();

    if(DELAY_MS) chThdSleepMilliseconds(DELAY_MS);
  }
}
