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
 * File: soft_uart_serial_test.ino 
 * Description: This is an example illustrating the use of the soft_uart
 * library.  Basically it uses the software serial port object serial_tc4 and
 * the hardware serial port object Serial2 to test the transmission and
 * reception of the soft_uart library using one of the objects the library
 * provides.
 * Date: July 8th, 2015
 * Author: Antonio C. Dominguez-Brito <adominguez@iusiani.ulpgc.es>
 * ROC-SIANI - Universidad de Las Palmas de Gran Canaria - Spain
 */

#include "soft_uart.h"

using namespace arduino_due;

#define RX_PIN 10 // software serial port's reception pin
#define TX_PIN 11 // software serial port's transmision pin
#define SOFT_UART_BIT_RATE 57600 // 57600 38400 1200 19200 9600 115200 300
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length
#define RECEPTION_TIMEOUT 100 // milliseconds

uint32_t counter=0;

// declaration of software serial port object serial_tc4
// which uses timer/counter channel TC4
serial_tc4_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& serial_obj=serial_tc4; // serial_tc4_t& serial_obj=serial_tc4;

// FIX: function template receive_tc is defined in
// #define to avoid it to be considered a function
// prototype when integrating all .ino files in one
// whole .cpp file. Without this trick the compiler
// complains about the definition of the template
// function.
#define receive_tc_definition \
template<typename serial_tc_t> \
void receive_tc(serial_tc_t& serial_tc,unsigned long timeout) \
{ \
  Serial.print("<-- [serial_tc"); \
  Serial.print(static_cast<int>(serial_tc.get_timer())); \  
  Serial.print("] received: "); \ 
  \
  int data=0; \ 
  unsigned long last_time=millis(); \
  do \
  { \
    if(serial_tc.available()) \ 
    { \
      if((data=serial_tc.read())>=0) \
      { last_time=millis(); Serial.print(static_cast<char>(data)); } \
      else \
      { \
	if(serial_tc.bad_status()) \
	{ \
	  last_time=millis(); \
          Serial.print("||"); \
          if(serial_tc.bad_start_bit()) Serial.print("[BAD_START_BIT]"); \
          if(serial_tc.bad_parity()) Serial.print("[BAD_PARITY]"); \
          if(serial_tc.bad_stop_bit()) Serial.print("[BAD_STOP_BIT]"); \
          Serial.println("||"); \
	} \
      } \
    } \
  } while( (millis()-last_time<timeout) && (data!=0xa) ); \
} 

// FIX: here we instantiate the template definition
// of receive_tc
receive_tc_definition;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  // serial_obj initialization
  serial_obj.begin(
    RX_PIN,
    TX_PIN,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );

  // Serial2's initialization
  // we will communicate serial_obj to Serial2 to test the library,
  // so we have to config Serial2 in the same way that serial_obj,
  // same bit rate, parity and stop bits
  Serial2.begin(SOFT_UART_BIT_RATE,SERIAL_8E1);
  while(Serial2.available()) { Serial2.read(); }
   
  Serial.println("========================================================");
  Serial.println("========================================================");
  Serial.println("========================================================");  
}

void loop() {
  // put your main code here,to run repeatedly:
  Serial.println("********************************************************");
  Serial.println("********************************************************");  

  Serial.print("--> [Serial2] sending: "); Serial.println(counter);
  Serial2.println(counter);

  unsigned long timeout=
    static_cast<unsigned long>(2*1000*serial_obj.get_frame_time());
  if(timeout<RECEPTION_TIMEOUT) timeout=RECEPTION_TIMEOUT;
  receive_tc(serial_obj,timeout);
 
  Serial.println("--------------------------------------------------------");
  Serial.println("--------------------------------------------------------");
  
  Serial.print("--> [serial_tc"); 
  Serial.print(static_cast<int>(serial_obj.get_timer())); 
  Serial.print("] sending: "); Serial.println(counter);
  serial_obj.println(counter);

  Serial.print("<-- [Serial2] received: ");
  int data=0;
  do
  {  
    if( Serial2.available() && ((data=Serial2.read())>=0) ) 
      Serial.print(static_cast<char>(data));
  } while((data>=0) && (data!=0xa));  
  
  counter++;
}

