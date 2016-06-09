
//----------------------------------------------------------------------------
// cgpiod_input.cpp : implementation of 4 inputs
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

  //--------------------------------------------------------------------------
  //
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputGetState(tUint32 dwObj)
  {
    dwObj &= CGPIOD_OBJ_NUM_MASK;

    return (dwObj < CGPIOD_IN_COUNT) ? m_input[dwObj].dwState : CGPIOD_IN_STATE_OUT;
    } // GetState

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputOnConfig() 
  {
    tUint32     dwObj;
    tGpiodInput *pObj = m_input; 

    // initialise all channels
    Debug.logTxt(CLSLVL_GPIOD_INPUT | 0x0000, "CGpiod::_inputOnConfig");
    memset(m_input, 0, sizeof(m_input)); 

    for (dwObj = 0; dwObj < CGPIOD_IN_COUNT; dwObj++, pObj++) {
      // initialise defaults
      pObj->dwFlags     = CGPIOD_IN_FLG_MQTT_ALL; // all events to MQTT
      pObj->dwState     = CGPIOD_IN_STATE_OUT; 
      pObj->dwPin       = (dwObj == CGPIOD_IN0) ? CGPIOD_IN0_PIN : 
                          (dwObj == CGPIOD_IN1) ? CGPIOD_IN1_PIN : 
                          (dwObj == CGPIOD_IN2) ? CGPIOD_IN2_PIN : 
                          (dwObj == CGPIOD_IN3) ? CGPIOD_IN3_PIN : -1; 
      pObj->dwPol       = CGPIOD_IO_POL_INVERT;  
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

    Debug.logTxt(CLSLVL_GPIOD_INPUT | 0x0000, "CGpiod::_inputOnInit");
    for (dwObj = 0; dwObj < CGPIOD_IN_COUNT; dwObj++, pObj++) {
      _ioSetPinDir(pObj->dwPin, CGPIOD_IO_DIR_INPUT);
      _ioSetPinPullup(pObj->dwPin, (pObj->dwPol == CGPIOD_IO_POL_INVERT) ? 1 : 0);
      } // for

    return m_dwError;
    } // _inputOnInit

  //--------------------------------------------------------------------------
  // run inputs
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputOnRun(tUint32 msNow) 
  {
    tUint32     dwObj, dwVal;
    tGpiodInput *pObj = m_input; 
    tGpiodEvt   evt = { msNow, CGPIOD_ORIG_INPUT, 0, 0, 0, 0 };

    // loop all inputs
    for (dwObj = 0; dwObj < CGPIOD_IN_COUNT; dwObj++, pObj++) {
      // debounce input changes
      dwVal     = _inputGetPinVal(pObj, msNow);
      evt.dwObj = CGPIOD_OBJ_CLS_INPUT + dwObj;

      // handle stable changes or running timer
      switch (pObj->dwState) {
        case CGPIOD_IN_STATE_OUT:
          if (pObj->dwVal != dwVal) {
            // input closed
            pObj->dwVal   = CGPIOD_IN_VAL_IN;
            pObj->msState = msNow;
            pObj->dwState = CGPIOD_IN_STATE_INGT0;
            evt.dwEvt     = CGPIOD_IN_EVT_INGT0;
            Debug.logTxt(CLSLVL_GPIOD_INPUT | 0x0200, "CGpiod::_inputOnRun,ingt0");
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
            Debug.logTxt(CLSLVL_GPIOD_INPUT | 0x0300, "CGpiod::_inputOnRun,outlt1");
            DoEvt(&evt);
            } 
          else {
            // input still closed
            if (!ChkTimer(msNow, pObj->msState + CGPIOD_IN_TMR_INGT1)) break;

            pObj->dwState = CGPIOD_IN_STATE_INGT1;
            evt.dwEvt     = CGPIOD_IN_EVT_INGT1;
            Debug.logTxt(CLSLVL_GPIOD_INPUT | 0x0400, "CGpiod::_inputOnRun,ingt1");
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
            Debug.logTxt(CLSLVL_GPIOD_INPUT | 0x0500, "CGpiod::_inputOnRun,outgt1");
            DoEvt(&evt);
            } 
          else {
            // input still closed
            if (!ChkTimer(msNow, pObj->msState + CGPIOD_IN_TMR_INGT2)) break;

            pObj->dwState = CGPIOD_IN_STATE_INGT2;
            evt.dwEvt     = CGPIOD_IN_EVT_INGT2;
            Debug.logTxt(CLSLVL_GPIOD_INPUT | 0x0600, "CGpiod::_inputOnRun,ingt2");
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
            Debug.logTxt(CLSLVL_GPIOD_INPUT | 0x0700, "CGpiod::_inputOnRun,out");
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
    Debug.logTxt(CLSLVL_GPIOD_INPUT | 0x0000, "CGpiod::_inputOnExit");
    return m_dwError;
    } // CGpiod::_inputOnExit

  //--------------------------------------------------------------------------
  // execute command for inputs, called by DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_inputDoCmd(tGpiodCmd* pCmd) 
  { 
    tChar       str1[16], str2[16];
    tGpiodInput *pObj = &m_input[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK]; 
    tGpiodEvt   evt   = { pCmd->msNow, pCmd->dwOrig, pCmd->dwObj, 0, 0, 0 };

    PrintCmd(pCmd, CLSLVL_GPIOD_OUTPUT | 0x0000, "CGpiod::_inputDoCmd");
    switch (pCmd->dwCmd & CGPIOD_CMD_NUM_MASK) {
      case CGPIOD_IN_CMD_STATUS: 
        // report current value
        evt.dwEvt   = pObj->dwState;
        DoSta(&evt);
        break;

      case CGPIOD_IN_CMD_DEBOUNCE:
        // handle command
        if (pCmd->dwParms & CGPIOD_IN_PRM_DEBOUNCE)
          pObj->tmrDebounce = pCmd->parmsInput.dwDebounce; 

        // report current value
        pCmd->dwRsp = pObj->tmrDebounce;

        gsprintf(str1, "%s/debounce", PrintObj2String(str2, pCmd->dwObj));
        evt.szTopic = str1;
        evt.dwEvt   = pObj->tmrDebounce;
        DoSta(&evt);
        break;

      default: // treat as event
        // handle command
        evt.dwEvt = pCmd->dwCmd;
        DoEvt(&evt);
        break;
      } // switch

    // automatically copy state -> rsp
    if (pCmd->dwCmd & CGPIOD_CMD_RSP_AUTO)
      pCmd->dwRsp = pObj->dwState;

    return pCmd->dwError; 
    } // _inputDoCmd

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
      if (!ChkTimer(msNow, pObj->msDebounce + pObj->tmrDebounce))
        return pObj->dwVal;

      } // if

    // clear timer of no change or debounce period passed 
    pObj->msDebounce = 0;
    return dwVal;
    } // _inputGetPinVal

