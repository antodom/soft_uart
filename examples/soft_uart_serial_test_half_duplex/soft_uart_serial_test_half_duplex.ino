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
#define SOFT_UART_BIT_RATE 57600 // 38400 57600 38400 1200 19200 9600 115200 115200
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length

uint32_t counter=0;

// declaration of software serial port object serial_tc0
// which uses timer/counter channel TC0
serial_tc0_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);

// declaration of software serial port object serial_tc1
// which uses timer/counter channel TC1
serial_tc1_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  // serial_tc0 and serital_tc1 initializations.
  // We will initialize both in half-duplex mode, serial_tc1 in reception mode
  // (RX_MODE) and the other one in transmission mode (TX_MODE). Remember that
  // the pins used by both software serial objects should be connected. In this
  // case we are using a 8E1 serial transmission mode
  serial_tc0.begin(
    SERIAL_TC0_PIN, // in half-duplex mode rx and tx pins on the call to func-
    SERIAL_TC0_PIN, // tion begin() should be the same
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT,
    soft_uart::mode_codes::TX_MODE // half-duplex, initially on transmission mode
  );
  serial_tc1.begin(
    SERIAL_TC1_PIN, // in half-duplex mode rx and tx pins on the call to func-
    SERIAL_TC1_PIN, // tion begin() should be the same
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT,
    soft_uart::mode_codes::RX_MODE // half-duplex, initially on reception mode
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
  
  Serial.print("<-- [serial_tc1] received: ");
  int data=0; 
  do
  {  
    if(serial_tc1.available()) 
    {
      data=serial_tc1.read();
      if(data>0) 
      {
        Serial.print(static_cast<char>(data));
      }
      else if(serial_tc1.bad_status())
      {
  // when serial_tc1.read() is negative, it means that there is no data
  // available or that the last data received was erroneous (this is
  // what we check with serial_tc4.bad_status(). If the last data was
  // erroneus we can check the reason with member functions bad_start_bit(),
  // bad_parity() and bad_stop_bit()
        Serial.print("||");
        if(serial_tc1.bad_start_bit()) Serial.print("[BAD_START_BIT]");
        if(serial_tc1.bad_parity()) Serial.print("[BAD_PARITY]");
        if(serial_tc1.bad_stop_bit()) Serial.print("[BAD_STOP_BIT]");
        Serial.print("||");
      }
    }
  } while(!serial_tc1.bad_status() && (data!=0xA));

  Serial.println("--------------------------------------------------------");
  Serial.println("--------------------------------------------------------");  

  // sending data on the opposite direction
  // changing half-duplex modes for both software serial objects
  // set_rx_mode() implicitly flushes the tx buffer before getting into rx mode
  serial_tc0.set_rx_mode();
  serial_tc1.set_tx_mode();

  Serial.print("--> [serial_tc1] sending: "); Serial.println(counter);
  serial_tc1.println(counter);
  
  Serial.print("<-- [serial_tc0] received: ");
  do
  {  
    if(serial_tc0.available()) 
    {
      data=serial_tc0.read();
      if(data>0) 
      {
        Serial.print(static_cast<char>(data));
      }
      else if(serial_tc0.bad_status())
      {
  // when serial_tc0.read() is negative, it means that there is no data
  // available or that the last data received was erroneous (this is
  // what we check with serial_tc4.bad_status(). If the last data was
  // erroneus we can check the reason with member functions bad_start_bit(),
  // bad_parity() and bad_stop_bit()
        Serial.print("||");
        if(serial_tc0.bad_start_bit()) Serial.print("[BAD_START_BIT]");
        if(serial_tc0.bad_parity()) Serial.print("[BAD_PARITY]");
        if(serial_tc0.bad_stop_bit()) Serial.print("[BAD_STOP_BIT]");
        Serial.print("||");
      }
    }
  } while(!serial_tc0.bad_status() && (data!=0xA));
  
  // changing half-duplex modes for both software serial objects
  // set_rx_mode() implicitly flushes the tx buffer before getting into rx mode
  serial_tc1.set_rx_mode();
  serial_tc0.set_tx_mode();

  counter++;
}

