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
 * File: soft_uart_serial_test_auto.ino 
 * Description: This is an example illustrating the use of the soft_uart
 * library. Basically it uses the software serial port object serial_tc4 with
 * itself.
 * Date: September 21st, 2015
 * Author: Antonio C. Dominguez-Brito <adominguez@iusiani.ulpgc.es>
 * ROC-SIANI - Universidad de Las Palmas de Gran Canaria - Spain
 */

#include "soft_uart.h"

using namespace arduino_due;

#define RX_PIN 10 // software serial port's reception pin
#define TX_PIN 11 // software serial port's transmision pin
#define SOFT_UART_BIT_RATE 9600 // 3840057600 38400 1200 19200 9600 115200 115200
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length

uint32_t counter=0;

// declaration of software serial port object serial_tc4
// which uses timer/counter channel TC4
serial_tc4_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  // serial_tc4 initialization
  // we will communicate serial_tc4 with itself, so RX_PIN and TX_PIN
  // should be connected. This example illustrate how to use the serial
  // objects provided by soft_uart with a length of 9 bits, because
  // the serial objects Serial, Serial1, Serial2 and Serial3 provided
  // by the standard Arduino library do not provide 9-bit lenght serial
  // modes
  serial_tc4.begin(
    RX_PIN,
    TX_PIN,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::NINE_BITS,
    soft_uart::parity_codes::ODD_PARITY,
    soft_uart::stop_bit_codes::TWO_STOP_BITS
  );
   
  Serial.println("========================================================");
  Serial.println("========================================================");
  Serial.println("========================================================");  
}

void loop() {
  // put your main code here,to run repeatedly:
  Serial.println("********************************************************");
  Serial.println("********************************************************");  

  Serial.print("--> [serial_tc4] sending: "); Serial.println(counter);
  // IMPORTANT: for sending 9-bit values you should send each value separately 
  // using function write(uint32_t). Using functions print or println, or alike
  // function will truncate each data to be send to 8 bits.
  serial_tc4.write(counter);
  
  Serial.print("<-- [serial_tc4] received: ");
  int data=0; 
  if(serial_tc4.available()) 
  {
    data=serial_tc4.read();
    if(data>=0) 
    {
      Serial.println(data,DEC);
    }
    else if(serial_tc4.bad_status())
    {
      // when serial_tc4.read() is negative, it means that there is no data
      // available or that the last data received was erroneous (this is
      // what we check with serial_tc4.bad_status(). If the last data was
      // erroneus we can check the reason with member functions bad_start_bit(),
      // bad_parity() and bad_stop_bit()
      Serial.print("||");
      if(serial_tc4.bad_start_bit()) Serial.print("[BAD_START_BIT]");
      if(serial_tc4.bad_parity()) Serial.print("[BAD_PARITY]");
      if(serial_tc4.bad_stop_bit()) Serial.print("[BAD_STOP_BIT]");
      Serial.println("||");
    }
  }
  
  counter=(counter+1)%(1<<static_cast<int>(soft_uart::data_bit_codes::NINE_BITS));
}

