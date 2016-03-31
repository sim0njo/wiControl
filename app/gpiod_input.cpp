
//----------------------------------------------------------------------------
// cgpiod_input.cpp : implementation of 2 inputs
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

  //--------------------------------------------------------------------------
  // convert output event into string
  //--------------------------------------------------------------------------
  tCChar* CGpiod::_inputEvt2String(tUint32 dwEvt) 
  {
    return (dwEvt == CGPIOD_IN_EVT_INGT0)  ? "ingt0"  :
           (dwEvt == CGPIOD_IN_EVT_OUTLT1) ? "outlt1" :
           (dwEvt == CGPIOD_IN_EVT_INGT1)  ? "ingt1"  :
           (dwEvt == CGPIOD_IN_EVT_OUTGT1) ? "outgt1" :
           (dwEvt == CGPIOD_IN_EVT_INGT2)  ? "ingt2"  : "out";
    } // _inputEvt2String

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputOnConfig() 
  {
    tUint32     dwObj;
    tGpiodInput *pObj = m_input; 

    // initialise all channels
    Debug.println("CGpiod::_inputOnConfig");
    memset(m_input, 0, sizeof(m_input)); 
    for (dwObj = 0; dwObj < CGPIOD_IN_COUNT; dwObj++, pObj++) {
      // initialise defaults
      pObj->dwFlags     = CGPIOD_IN_FLG_MQTT_ALL; // all events to MQTT
      pObj->dwState     = CGPIOD_IN_STATE_OUT; 
      pObj->dwPin       = (dwObj == 0) ? CGPIOD_IN0_PIN : CGPIOD_IN1_PIN; 
      pObj->dwPol       = CGPIOD_IN_POL_INVERT;  
      pObj->dwVal       = CGPIOD_IN_VAL_OUT; 
      pObj->tmrDebounce = CGPIOD_IN_TMR_DEBOUNCE; 
      pObj->msDebounce  = 0; 
      pObj->msState     = 0; 
      pObj->szName      = (dwObj == 0) ? "input0" : "input1";
      } // for

    return m_dwError;
    } // _inputOnConfig

  //--------------------------------------------------------------------------
  // initialise output hw
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputOnInit() 
  {
    tUint32     dwObj;
    tGpiodInput *pObj = m_input; 

    Debug.println("CGpiod::_inputOnInit");
    for (dwObj = 0; dwObj < CGPIOD_IN_COUNT; dwObj++, pObj++) {
      pinMode(pObj->dwPin, INPUT);

      if (pObj->dwPol = CGPIOD_IN_POL_INVERT)
        pullup(pObj->dwPin);
      else
        noPullup(pObj->dwPin);
      } // for

    return m_dwError;
    } // _inputOnInit

  //--------------------------------------------------------------------------
  // run outputs
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputOnRun(tUint32 msNow) 
  {
    tUint32     dwObj, dwVal;
    tGpiodInput *pObj = m_input; 
    tGpiodEvt   evt = { msNow, 0, 0, 0 };

    // loop all inputs
    for (dwObj = 0; dwObj < CGPIOD_IN_COUNT; dwObj++, pObj++) {
      // debounce input changes
      dwVal     = _inputGetPinVal(pObj, msNow);
      evt.dwObj = CGPIOD_OBJ_CLS_INPUT + dwObj;

      // handle stable changes or running timer
      switch (pObj->dwState) {
        case CGPIOD_IN_STATE_OUT:
          if (pObj->dwVal != dwVal) {
            // set new state
            pObj->dwVal   = CGPIOD_IN_VAL_IN;
            pObj->msState = msNow;
            pObj->dwState = CGPIOD_IN_STATE_INGT0;
            evt.dwEvt     = CGPIOD_IN_EVT_INGT0;
            DoEvt(&evt);
            Debug.println("CGpiod::_inputOnRun,ingt0");
            } // if
          break;

        case CGPIOD_IN_STATE_INGT0:
          if (pObj->dwVal != dwVal) {
            // input opened
            pObj->dwVal   = CGPIOD_IN_VAL_OUT;
            pObj->msState = 0;
            pObj->dwState = CGPIOD_IN_STATE_OUT;
            evt.dwEvt     = CGPIOD_IN_EVT_OUTLT1;
            DoEvt(&evt);
            Debug.println("CGpiod::_inputOnRun,outlt1");
            } 
          else {
            // input still closed
            if ((msNow - pObj->msState) < CGPIOD_IN_TMR_INGT1) break;

            pObj->dwState = CGPIOD_IN_STATE_INGT1;
            evt.dwEvt     = CGPIOD_IN_EVT_INGT1;
            DoEvt(&evt);
            Debug.println("CGpiod::_inputOnRun,ingt1");
            } // else
          break;

        case CGPIOD_IN_STATE_INGT1:
          if (pObj->dwVal != dwVal) {
            // input opened
            pObj->dwVal   = CGPIOD_IN_VAL_OUT;
            pObj->msState = 0;
            pObj->dwState = CGPIOD_IN_STATE_OUT;
            evt.dwEvt     = CGPIOD_IN_EVT_OUTGT1;
            DoEvt(&evt);
            Debug.println("CGpiod::_inputOnRun,outgt1");
            } 
          else {
            // input still closed
            if ((msNow - pObj->msState) < CGPIOD_IN_TMR_INGT2) break;

            pObj->dwState = CGPIOD_IN_STATE_INGT2;
            evt.dwEvt     = CGPIOD_IN_EVT_INGT2;
            DoEvt(&evt);
            Debug.println("CGpiod::_inputOnRun,ingt2");
            } // else
          break;

        case CGPIOD_IN_STATE_INGT2:
          if (pObj->dwVal != dwVal) {
            // input opened
            pObj->dwVal   = CGPIOD_IN_VAL_OUT;
            pObj->msState = 0;
            pObj->dwState = CGPIOD_IN_STATE_OUT;
            evt.dwEvt     = CGPIOD_IN_EVT_OUT;
            DoEvt(&evt);
            Debug.println("CGpiod::_inputOnRun,out");
            } 
          break;
        } // switch
      } // for

    return m_dwError;
    } // CGpiod::_inputOnRun

  //--------------------------------------------------------------------------
  // leave inputs in decent state
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputOnExit() 
  {
    return m_dwError;
    } // CGpiod::_inputOnExit

  //--------------------------------------------------------------------------
  // return valid debounced state for pin
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputGetPinVal(tGpiodInput* pObj, tUint32 msNow) 
  {
    tUint32 dwVal = digitalRead(pObj->dwPin) ^ pObj->dwPol;

    if (pObj->dwVal != dwVal) {
      // set debounce timer on start of change
      if (pObj->msDebounce == 0) 
        pObj->msDebounce = msNow;
        
      // return old state till debounce period passed
      if ((msNow - pObj->msDebounce) < pObj->tmrDebounce)
        return pObj->dwVal;

      } // if

    // clear timer of no change or debounce period passed 
    pObj->msDebounce = 0;
    return dwVal;
    } // _inputGetPinVal

