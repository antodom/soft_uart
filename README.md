## soft_uart v2.0

This is soft_uart library for the Arduino DUE electronic prototyping platform. 

Copyright (C) 2015 Antonio C. Domínguez Brito (<adominguez@iusiani.ulpgc.es>). División de Robótica y Oceanografía Computacional (<http://www.roc.siani.es>) and Departamento de Informática y Sistemas (<http://www.dis.ulpgc.es>). Universidad de Las Palmas de Gran  Canaria (ULPGC) (<http://www.ulpgc.es>).
  
### 0. License 

The soft_uart library is an open source project which is openly available under the GNU General Public License (GPL) license.

### 1. Introduction

This library is an implementation of a software UART (Universal Asynchronous Receiver Transmitter) for the Arduino Due's Atmel ATSAM3X8E micro-controller.

Using this library you can have up to nine software serial ports on the Arduino DUE, in addition to the hardware serial ports already available on the Atmel ATSAM3X8E micro-controller, namely objects Serial, Serial1, Serial2 and Serial3. One of the main design principles of the library was to implement software serial objects which were interchangeable with the hardware counterparts actually available on the standard Arduino library. 

The objects provided by this library for the software serial ports are: serial_tc0, serial_tc1, serial_tc2, serial_tc3, serial_tc4, serial_tc5, serial_tc6, serial_tc7 and serial_tc8. As mentioned in the previous paragraph, these software serial port objects can be used in the same way, and with the same interface that its hardware correspondents, Serial, Serial1, Serial2 and Serial3. Those objects are not defined by default, you must declare them before its use. Only the ones declared will be generated by the compiler. 

For declaring any of them you must use a specific macro. For example, for object serial_tc0, the macro would be serial_tc0_declaration, for serial_tc1, declare_serial_tc1, etc. In addition each object has a transmission and a reception buffer. The transmission buffer is a fifo, and the reception buffer is a circular fifo. Thus, when declaring each object you have also to specify the length in bytes of both buffers in the corresponding macro, like in the following code:

```
  // declaration of software serial port object serial_tc4
  // which uses timer/counter channel TC4
  serial_tc4_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
```

Each software serial object implies the use of a Timer Counter (TC) module channel of the ATSAM3X8E. Remember that there are three timer counter modules on the micro-controller, each one with three channels. In total nine timer/counter channels denoted by TC0, TC1, TC2, TC3, TC4, TC5, TC6, TC7 and TC8, respectively. This library uses for implementing a software serial port a specific timer/counter channel. That is, TC0 for serial_tc0, TC1 for serial_tc1, etc. In addition, each software serial port object can be configured for using any pair of I/O pins available on the micro-controller as RX (reception) and TX (transmission) pins for the serial port.

For setting a software serial port object you must use member function begin(), where you specify the RX and TX pins, the serial bit (baud) rate, the data bits (5, 6, 7, 8 or 9), the parity (either no parity, or even or odd parity), and the use of one or two stop bits. The next snippet of code shows and example:

```
  // serial_tc4 initialization
  serial_tc4.begin(
    RX_PIN,
    TX_PIN,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::EVEN_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
```
When using one of those software serial port objects, the TC channel and the RX and TX pins involved can only be used for this purpose, if not its functionality is compromised. Take into account that interrupts for the TC channel and the RX pin involved are used intensively when associated to any of the software serial port objects.

By default, the serial objects work in full-duplex using two pins, namely, a RX pin for data reception, and a TX pin for data transmission. In addition, it is possible to use them in half-duplex mode using only one pin, both for reception and transmission. In this case, it is necessary to set the sotware serial objects in reception or transmission modes, depending on the situation.

### 2. Download & installation

The  library  is  available  through  an  open	git  repository  available   at:

* <https://github.com/antodom/soft_uart>

and also at our own mirror at:

* <http://bizet.dis.ulpgc.es/antodom/soft_uart>

For using it you just have to copy the library on the libraries folder used by your Arduino IDE, the folder should be named "soft_uart".

In addition you must add the flag -std=gnu++11 for compiling. For doing that add -std=gnu++11 to the platform.txt file, concretely to compiler.cpp.flags. In Arduino IDE 1.6.6 and greater versions this flag is already set.

### 3. Examples

On the examples directory you have available several examples illustrating the use of soft_uart.

Example *basic_test* uses software serial port object serial_tc4 with pins 10 and 11, as RX and TX pins respectively, and Serial2. For having this example working, you should connect the RX pin of Serial2 to pin 11, and Serial2's TX pin to pin 10. Mind that soft_uart software serial objects can be used in the same way that Serial, Serial1, Serial2 and Serial3 hardware serial objects.

Example *soft_uart_serial_test.ino* is a version of the previous one but with error handling.

Example *soft_uart_serial_test_auto.ino* illustrates how to use a serial port object with itself, in this case serial_tc4, like in the previous example. In addition, the example uses also a serial mode of 9 bit length which is not possible with the current Arduino standard library. 

Example *soft_uart_serial_test_half_duplex* uses two software serial objects (serial_tc0 and serial_tc1) both in half duplex mode to communicate each other. Example *soft_uart_serial_test_half_duplex_char* is another example using half duplex mode but just sending one byte (char) each time. And example *soft_uart_serial_test_half_duplex_9O2* shows how to use the same objects using a data length of nine bits, odd parity and two stop bits, and similarly to the former one sending one data each time.

### 4. Reception errors

The behavior of the software serial objects provided by soft_uart will depend on the load of interrupts per unit of time of the application where you integrate them. Also on the peaks of interrupts in a given moment. Using the examples provided I have reached to 57600 (and two stop bits) without errors. Evidently those are examples where there is no a great use of interrupts, except the ones used by the software serial objects, and the hardware serial objects involved specifically on each example. I advise you to check first for the best bit rate your application can manage without errors.

Independently of the application where you use this library, what is important is to know when any data have been received incorrectly. Thus, this library allows you to know the reception status of the last received data. In general, for any received data using member function read(), you should check if read() returns -1 or not. When it returns -1, it may be due to that there is no available data, or on the contrary, the data received has an error. As an example, the next code illustrates how to proceed using serial_tc4 software serial object to find out the status of the last data received in those situations.

```
  if(serial_tc4.available()) 
  {
    int data=serial_tc4.read();
    if(data>=0) 
    {
      // data received correctly
      Serial.print(static_cast<char>(data));
      if(serial_tc4.data_lost()) Serial.print("[DATA_LOST]");
    }
    else if(serial_tc4.bad_status())
    {
      // when serial_tc4.read() is negative, it means that there is no data
      // available or that the last data received was erroneous (this is
      // what we check with serial_tc4.bad_status(). If the last data was
      // erroneus we can check the reason with member functions bad_start_bit(),
      // bad_parity() and bad_stop_bit()
      if(serial_tc4.bad_start_bit()) Serial.print("[BAD_START_BIT]");
      if(serial_tc4.bad_parity()) Serial.print("[BAD_PARITY]");
      if(serial_tc4.bad_stop_bit()) Serial.print("[BAD_STOP_BIT]");
    }
  } 
```

In addition, it is also possible to know that some data have been lost on reception, due to that the reception buffer was full (using member function data_lost()). Take into account that the reception buffer is a circular fifo buffer that overwrites the older element when a new element is received and the buffer is full.

### 5. Incompatibilities

Any library using the interrupts associated with any of the timer/counter module channels TC0, TC1, TC2, TC3, TC4, TC5, TC6, TC7 and TC8, has a potential compatibility problem with soft_uart, if it happens to use the same TC module and the same channel. An example of this potential incompatibility is library Servo which uses by default TC0, TC2, TC3, TC4 y TC5 interrupt handlers. In this case, you will be limited to use serial_tc1, serial_tc6, serial_tc7 and/or serial_tc8 software serial objects to preserve the compatibility. Another option could also be to change the Servo library to use less TC modules.

### 6. Compiling with CMake

For compiling on command line using CMake, just proceed in the following manner:

1. Set the following environment variables (a good place to put them is on .bashrc):
  * Set `ARDUINO_DUE_ROOT_PATH` to `~/.arduino15/packages/arduino/`.
  * Set `ARDUINO_DUE_VERSION` to `1.6.17`.
  * Set `ARDUINO_UNO_ROOT_PATH` to the path where you have installed the Arduino IDE, for example, `~/installations/arduino-1.6.5`.
  * Set `ARDUINO_IDE_LIBRARY_PATH` to the path for the Arduino IDE projects you have in preferences. For example, `~/arduino_projects`.

2. Go to `soft_uart` directory (the one you have cloned with the progject).
3. Create directory `build` (if not already created).
4. Execute `cmake ..`.
5. Set the following flags and variables (if not done before), you should execute `ccmake ..`:
  * Set `PORT` to the serial port you will use for uploading.
  * Set `IS_NATIVE_PORT` to `true` (if using DUE's native port) or `false` (if using DUE's programming port).
6. Be sure the changes were applied, usually running `cmake ..`.
7. Compile executing `make`.
8. The previous step has generated the examples available with the library. You can upload the code executing:
  * `make upload_basic_test`, 
  * `make upload_soft_uart_serial_test`, 
  * `make upload_soft_uart_serial_test_auto`, 
  * `make upload_soft_uart_serial_test_half_duplex`,
  * `make upload_soft_uart_serial_test_half_duplex_char`
  * and `make upload_soft_uart_serial_test_half_duplex_9O1`.

### 7. Library users

In this section we would like to enumerate users using the library in their own projects and developments. So please, if your are using the library, drop us an email indicating in what project or development you are using it.

The list of users/projects goes now:

1. **Project:** Autonomous sailboat A-Tirma (<http://velerorobot.blogspot.com.es>). **User**: División de Robótica y Oceanografía Computacional (<http://www.roc.siani.es>). **Description**: The library was a specific development for this project. The sailboat onboard system is based on an Arduino DUE, and we ran out of hardware serial ports for all the hardware we use on the boat.

### 8. Feedback & Suggestions

Please be free to send me any comment, doubt of use, or suggestion in relation to soft_uart.
