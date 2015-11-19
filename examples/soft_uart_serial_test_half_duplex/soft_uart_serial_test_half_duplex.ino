/**
 ** soft_uart library
 ** Copyright (C) 2015
 **
 **   Antonio C. Domínguez Brito <adominguez@iusiani.ulpgc.es>
 **     División de Robótica y Oceanografía Computacional <www.roc.siani.es>
 **     and Departamento de Informática y Sistemas <www.dis.ulpgc.es>
 **     Universidad de Las Palmas de Gran  Canaria (ULPGC) <www.ulpgc.es>
 **  
 ** This file is part of the soft_uart library.
 ** The soft_uart library is free software: you can redistribute it and/or modify
 ** it under  the  terms of  the GNU  General  Public  License  as  published  by
 ** the  Free Software Foundation, either  version  3  of  the  License,  or  any
 ** later version.
 ** 
 ** The  soft_uart library is distributed in the hope that  it  will  be  useful,
 ** but   WITHOUT   ANY WARRANTY;   without   even   the  implied   warranty   of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE.  See  the  GNU  General
 ** Public License for more details.
 ** 
 ** You should have received a copy  (COPYING file) of  the  GNU  General  Public
 ** License along with the soft_uart library.
 ** If not, see: <http://www.gnu.org/licenses/>.
 **/
/*
 * File: soft_uart_serial_test_half_duplex.ino 
 * Description: This is an example illustrating the use of  the soft_uart
 * library. In this case, the example two software serial objects to show
 * how to use them in half duplex mode
 * Date: October 12th, 2015
 * Author: Antonio C. Dominguez-Brito <adominguez@iusiani.ulpgc.es>
 * ROC-SIANI - Universidad de Las Palmas de Gran Canaria - Spain
 */

#include "soft_uart.h"

using namespace arduino_due;

#define SERIAL_TC0_PIN 10 // TC0 software serial port's half duplex pin
#define SERIAL_TC1_PIN 11 // TC1 software serial port's half duplex pin
#define SOFT_UART_BIT_RATE 38400 // 38400 57600 38400 1200 19200 9600 115200 
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length
#define RECEPTION_TIMEOUT 100 // milliseconds

uint32_t counter=0;

// declaration of software serial port object serial_tc0
// which uses timer/counter channel TC0
serial_tc0_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);

// declaration of software serial port object serial_tc1
// which uses timer/counter channel TC1
serial_tc1_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);

template<typename serial_tc_t>
void receive_tc(serial_tc_t& serial_tc, unsigned long timeout)
{
  Serial.print("<-- [serial_tc"); 
  Serial.print(static_cast<int>(serial_tc.get_timer())); 
  Serial.print("] received: ");
  
  int data=0; 
  unsigned long last_time=millis();
  do
  {
    if(serial_tc.available()) 
    {
      if((data=serial_tc.read())>=0)
      { last_time=millis(); Serial.print(static_cast<char>(data)); }
      else
      {
	if(serial_tc.bad_status())
	{
	  last_time=millis();
	  Serial.print("||");
	  if(serial_tc.bad_start_bit()) Serial.print("[BAD_START_BIT]");
	  if(serial_tc.bad_parity()) Serial.print("[BAD_PARITY]");
	  if(serial_tc.bad_stop_bit()) Serial.print("[BAD_STOP_BIT]");
	  Serial.print((serial_tc.get_last_data_status()>>16),BIN);
	  Serial.print("||");
	}
      }
    }
  }
  while(millis()-last_time<timeout); 
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  // serial_tc0 and serital_tc1 initializations.
  // We will initialize both in half-duplex mode, serial_tc1 in reception mode
  // (RX_MODE) and the other one in transmission mode (TX_MODE). In this
  // case we are using a 8E1 serial transmission mode
  serial_tc0.half_duplex_begin(
    SERIAL_TC0_PIN, 
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT,
    false // on transmission mode (the default is on reception mode) 
  );
  serial_tc1.half_duplex_begin(
    SERIAL_TC1_PIN, 
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
    // initially on reception mode, last argument is true by default
  );   
  Serial.println("========================================================");
  Serial.println("========================================================");
  Serial.println("========================================================");  
}

void loop() {
  // put your main code here,to run repeatedly:
  Serial.println("********************************************************");
  Serial.println("********************************************************");  

  Serial.print("--> [serial_tc0] sending: "); Serial.println(counter);
  serial_tc0.println(counter);
  
  unsigned long timeout=
    static_cast<unsigned long>(2*1000*serial_tc1.get_frame_time());
  if(timeout<RECEPTION_TIMEOUT) timeout=RECEPTION_TIMEOUT;
  receive_tc(serial_tc1,timeout);

  Serial.println("--------------------------------------------------------");
  Serial.println("--------------------------------------------------------");  

  // sending data on the opposite direction
  // changing half-duplex modes for both software serial objects
  // set_rx_mode() implicitly flushes the tx buffer before getting into rx mode
  serial_tc1.set_tx_mode();
  serial_tc0.set_rx_mode();

  Serial.print("--> [serial_tc1] sending: "); Serial.println(counter);
  serial_tc1.println(counter);

  timeout=
    static_cast<unsigned long>(2*1000*serial_tc0.get_frame_time());
  if(timeout<RECEPTION_TIMEOUT) timeout=RECEPTION_TIMEOUT;
  receive_tc(serial_tc0,timeout);
  
 
  // changing half-duplex modes for both software serial objects
  // set_rx_mode() implicitly flushes the tx buffer before getting into rx mode
  serial_tc0.set_tx_mode();
  serial_tc1.set_rx_mode();

  counter++;
}

