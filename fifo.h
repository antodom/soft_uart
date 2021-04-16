/**
 ** soft_uart library
 ** Copyright (C) 2015-2020
 **
 **   Antonio C. Domínguez Brito <antonio.dominguez@ulpgc.es>
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
 * File: fifo.h 
 * Description: This is the implementation of a static fifo using templates.
 * The fifo can be circular or not
 * Date: July 8th, 2015
 * Author: Antonio C. Dominguez-Brito <antonio.dominguez@ulpgc.es>
 * ROC-SIANI - Universidad de Las Palmas de Gran Canaria - Spain
 */

#ifndef FIFO_H
#define FIFO_H

#include <cstddef>

template<bool BOOL_VALUE>
struct bool_to_type { static constexpr bool value=BOOL_VALUE; };

template<typename T,size_t LENGTH,bool IS_CIRCULAR = false>
class basic_fifo
{
  public:

    basic_fifo()
    : 
      _first_{0},
      _last_{LENGTH-1},
      _items_{0}
    {}

    ~basic_fifo() {}

    bool push(const T& t)
    { return _push_(t,bool_to_type<IS_CIRCULAR>()); }
   
    bool pop(T& t)
    {
      if(is_empty()) return false;

      t=_buffer_[_first_];

      _items_--;
       _first_=(_first_+1)%LENGTH;

      return true;
    }

    bool is_empty() { return (_items_==0); }

    bool is_full() { return (_items_==LENGTH); }

    int items() { return _items_; }

    int available() { return LENGTH-_items_; }

    void reset() { _items_=0; _first_=0; _last_=LENGTH-1; }

  private:

    T _buffer_[LENGTH];
    int _first_,_last_;

    int _items_;

    // push implementation for a normal fifo
    // when is full, we can not push any further element 
    bool _push_(const T& t,bool_to_type<false>)
    {
      if(is_full()) return false;

      _last_=(_last_+1)%LENGTH;
      _buffer_[_last_]=t;
      _items_++;

      return true;
    }

    // push implementation for a circular fifo
    // when is full, we overwrite the first element 
    bool _push_(const T& t,bool_to_type<true>)
    {
      bool full=is_full();
      
      // NOTE: when the fifo is circular and the fifo is full 
      // the new element overwrites the first one 
      if(full) _first_=(_first_+1)%LENGTH;  
      else _items_++;

      _last_=(_last_+1)%LENGTH;
      _buffer_[_last_]=t;

      return full;
    }
};

template<typename T, size_t LENGTH>
using fifo=basic_fifo<T,LENGTH,false>; 

template<typename T, size_t LENGTH>
using circular_fifo=basic_fifo<T,LENGTH,true>; 

#endif // FIFO_H
