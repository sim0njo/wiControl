
//----------------------------------------------------------------------------
// cgpiod_io.cpp : low level IO interface
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

  //--------------------------------------------------------------------------
  // set pin direction, pinDir=0 -> INPUT, pinDir=1 -> OUTPUT
  //--------------------------------------------------------------------------
  void    CGpiod::_ioSetPinDir(tInt32 pinNum, tUint32 pinDir) 
  {
    pinMode(pinNum, pinDir ? OUTPUT : INPUT);
    } // _ioSetPinDir

  //--------------------------------------------------------------------------
  // set pin pullup for inputs, bPullup=0 -> no pullup, bPullu1 -> pullup
  //--------------------------------------------------------------------------
  void    CGpiod::_ioSetPinPullup(tInt32 pinNum, tUint32 bPullup) 
  {
    if (bPullup)
      pullup(pinNum);
    else
      noPullup(pinNum);
    } // _ioSetPinPullup

  //--------------------------------------------------------------------------
  // set pin value, pinVal=0 -> LOW, pinVal=1 -> HIGH
  //--------------------------------------------------------------------------
  void    CGpiod::_ioSetPinVal(tUint32 pinNum, tUint32 pinVal) 
  {
    digitalWrite(pinNum, (pinVal) ? HIGH : LOW);
    } // _ioSetPinVal

  //--------------------------------------------------------------------------
  // get pin value depending on HW implementation
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_ioGetPinVal(tUint32 pinNum) 
  {
    return digitalRead(pinNum);
    } // _ioGetPinVal

