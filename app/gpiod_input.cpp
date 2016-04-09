
//----------------------------------------------------------------------------
// cgpiod_input.cpp : implementation of 2 inputs
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputOnConfig() 
  {
    tUint32     dwObj;
    tGpiodInput *pObj = m_input; 

    // initialise all channels
    g_log.LogPrt(CGPIOD_CLSLVL_INPUT | 0x0000, "CGpiod::_inputOnConfig");
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

    g_log.LogPrt(CGPIOD_CLSLVL_INPUT | 0x0000, "CGpiod::_inputOnInit");
    for (dwObj = 0; dwObj < CGPIOD_IN_COUNT; dwObj++, pObj++) {
      _ioSetPinDir(pObj->dwPin, CGPIOD_PIN_DIR_INPUT);
      _ioSetPinPullup(pObj->dwPin, (pObj->dwPol == CGPIOD_IN_POL_INVERT) ? 1 : 0);
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
    tGpiodEvt   evt = { msNow, 0, 0, 0, 0 };

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
            g_log.LogPrt(CGPIOD_CLSLVL_INPUT | 0x0200, "CGpiod::_inputOnRun,ingt0");
            DoEvt(&evt);
            } // if
          break;

        case CGPIOD_IN_STATE_INGT0:
          if (pObj->dwVal != dwVal) {
            // input opened
            pObj->dwVal   = CGPIOD_IN_VAL_OUT;
            pObj->msState = 0;
            pObj->dwState = CGPIOD_IN_STATE_OUT;
            evt.dwEvt     = CGPIOD_IN_EVT_OUTLT1;
            g_log.LogPrt(CGPIOD_CLSLVL_INPUT | 0x0300, "CGpiod::_inputOnRun,outlt1");
            DoEvt(&evt);
            } 
          else {
            // input still closed
            if ((msNow - pObj->msState) < CGPIOD_IN_TMR_INGT1) break;

            pObj->dwState = CGPIOD_IN_STATE_INGT1;
            evt.dwEvt     = CGPIOD_IN_EVT_INGT1;
            g_log.LogPrt(CGPIOD_CLSLVL_INPUT | 0x0400, "CGpiod::_inputOnRun,ingt1");
            DoEvt(&evt);
            } // else
          break;

        case CGPIOD_IN_STATE_INGT1:
          if (pObj->dwVal != dwVal) {
            // input opened
            pObj->dwVal   = CGPIOD_IN_VAL_OUT;
            pObj->msState = 0;
            pObj->dwState = CGPIOD_IN_STATE_OUT;
            evt.dwEvt     = CGPIOD_IN_EVT_OUTGT1;
            g_log.LogPrt(CGPIOD_CLSLVL_INPUT | 0x0500, "CGpiod::_inputOnRun,outgt1");
            DoEvt(&evt);
            } 
          else {
            // input still closed
            if ((msNow - pObj->msState) < CGPIOD_IN_TMR_INGT2) break;

            pObj->dwState = CGPIOD_IN_STATE_INGT2;
            evt.dwEvt     = CGPIOD_IN_EVT_INGT2;
            g_log.LogPrt(CGPIOD_CLSLVL_INPUT | 0x0600, "CGpiod::_inputOnRun,ingt2");
            DoEvt(&evt);
            } // else
          break;

        case CGPIOD_IN_STATE_INGT2:
          if (pObj->dwVal != dwVal) {
            // input opened
            pObj->dwVal   = CGPIOD_IN_VAL_OUT;
            pObj->msState = 0;
            pObj->dwState = CGPIOD_IN_STATE_OUT;
            evt.dwEvt     = CGPIOD_IN_EVT_OUT;
            g_log.LogPrt(CGPIOD_CLSLVL_INPUT | 0x0700, "CGpiod::_inputOnRun,out");
            DoEvt(&evt);
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
    g_log.LogPrt(CGPIOD_CLSLVL_INPUT | 0x0000, "CGpiod::_inputOnExit");
    return m_dwError;
    } // CGpiod::_inputOnExit

  //--------------------------------------------------------------------------
  // return valid debounced state for pin
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputGetPinVal(tGpiodInput* pObj, tUint32 msNow) 
  {
    tUint32 dwVal = _ioGetPinVal(pObj->dwPin) ^ pObj->dwPol;

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

