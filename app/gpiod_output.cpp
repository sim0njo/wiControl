
//----------------------------------------------------------------------------
// cgpiod_output.hpp : implementation of 2 outputs
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputOnConfig() 
  {
    tUint32      dwObj;
    tGpiodOutput *pObj = m_output; 

    // initialise all channels
//  Debug.println("CGpiod::_outputOnConfig");
    memset(m_output, 0, sizeof(m_output)); 
    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      // initialise defaults
      pObj->dwPin    = (dwObj == 0) ? CGPIOD_OUT0_PIN : CGPIOD_OUT1_PIN; 
      pObj->dwPol    = CGPIOD_OUT_POL_NORMAL; 
      pObj->dwState  = CGPIOD_OUT_STATE_OFF; 
      pObj->dwCmd    = CGPIOD_OUT_CMD_NONE; 
      } // for

    return m_dwError;
    } // _outputOnConfig

  //--------------------------------------------------------------------------
  // initialise output hw
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputOnInit() 
  {
    tUint32      dwObj;
    tGpiodOutput *pObj = m_output; 

//  Debug.println("CGpiod::_outputOnInit");
    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      digitalWrite(pObj->dwPin, (pObj->dwState ^ pObj->dwPol) ? HIGH : LOW);
      pinMode(pObj->dwPin, OUTPUT);
      } // for

    return m_dwError;
    } // _outputOnInit

  //--------------------------------------------------------------------------
  // run outputs
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputOnRun(tUint32 msNow) 
  {
    tUint32      dwObj;
    tGpiodOutput *pObj = m_output; 
    tGpiodEvt    evt = { msNow, 0, 0, 0 };

    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      evt.dwObj = CGPIOD_OBJ_CLS_OUTPUT + dwObj;

      switch (pObj->dwCmd) {
        case CGPIOD_OUT_CMD_ONDELAYED:
        case CGPIOD_OUT_CMD_OFFTIMED:
          if (msNow >= pObj->dwRun) {
            _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
            pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
            } // if
          break;

        case CGPIOD_OUT_CMD_OFFDELAYED:
        case CGPIOD_OUT_CMD_ONTIMED:
          if (msNow >= pObj->dwRun) {
            _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
            pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
            } // if
          break;

        case CGPIOD_OUT_CMD_TOGGLEDELAYED:
        case CGPIOD_OUT_CMD_TOGGLETIMED:
          if (msNow >= pObj->dwRun) {
            _outputSetState(pObj, pObj->dwState ^ CGPIOD_OUT_STATE_ON, &evt);
            pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
            } // if
          break;

        case CGPIOD_OUT_CMD_LOCKTIMED:
          if (msNow >= pObj->dwRun) {
            pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
            pObj->dwFlags &= ~CGPIOD_OUT_FLG_LOCKED;
            } // if
          break;

        case CGPIOD_OUT_CMD_BLINK:
          // handled by _outputOnHbTick() for multi-channel in phase operation
          break;

        default:
          break;
        } // switch
      } // for

    return 0; // m_dwState;
    } // CGpiod::_outputOnRun

  //--------------------------------------------------------------------------
  // leave outputs in decent state
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputOnExit() 
  {
    tUint32      dwObj;
    tGpiodOutput *pObj = m_output; 

    // set outputs to off and switch to input
    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      digitalWrite(pObj->dwPin, ((pObj->dwState = CGPIOD_OUT_STATE_OFF) ^ pObj->dwPol) ? HIGH : LOW);
      pinMode(pObj->dwPin, INPUT);
      } // for

    return m_dwError;
    } // CGpiod::_outputOnExit

  //--------------------------------------------------------------------------
  // local emulation handler, called by DoEvt() or DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputDoEvt(tGpiodEvt* pEvt) 
  { 
    tUint32      dwObj;
    tGpiodCmd    cmd = { pEvt->msNow, 0, 0, 0, 0, 0, 0 };
    tGpiodEvt    evt = { pEvt->msNow, 0, 0, 0 };
    tGpiodOutput *pObj = m_output; 

    if      ((pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) == CGPIOD_OBJ_CLS_INPUT) {
      switch (pEvt->dwEvt) {
        case CGPIOD_IN_EVT_OUTLT1:
          cmd.dwObj = CGPIOD_OBJ_CLS_OUTPUT | (pEvt->dwObj & CGPIOD_OBJ_NUM_MASK);
          cmd.dwCmd = CGPIOD_OUT_CMD_TOGGLE;
          _outputDoCmd(&cmd);
          break;

        case CGPIOD_IN_EVT_INGT2:
          cmd.dwObj = CGPIOD_OBJ_CLS_OUTPUT | (pEvt->dwObj & CGPIOD_OBJ_NUM_MASK);
          cmd.dwCmd = CGPIOD_OUT_CMD_BLINK;
          _outputDoCmd(&cmd);
          break;

        default:
          break;
        } // switch
      } // if 

    else if ((pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) == CGPIOD_OBJ_CLS_HBEAT) {
      
      if ((pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) == CGPIOD_HB1) {

        // handle each output object that requires a tick
        for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
          evt.dwObj = CGPIOD_OBJ_CLS_OUTPUT | dwObj;

          if (pObj->dwCmd == CGPIOD_OUT_CMD_BLINK)
            _outputSetState(pObj, (pEvt->dwEvt == CGPIOD_HB_EVT_ODD) ? CGPIOD_OUT_STATE_ON : CGPIOD_OUT_STATE_OFF, &evt);
          } // for

        } // if

      } // else if

    return m_dwError;
    } // CGpiod::_outputDoEvt

  //--------------------------------------------------------------------------
  // execute command for outputs, called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputDoCmd(tGpiodCmd* pCmd) 
  { 
    tGpiodOutput *pObj = &m_output[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK]; 
    tGpiodEvt    evt   = { pCmd->msNow, pCmd->dwObj, 0, 0 };

    do {
      Debug.println("CGpiod::_outputDoCmd");
      switch (pCmd->dwCmd & CGPIOD_OBJ_CMD_MASK) {
        case CGPIOD_OUT_CMD_STATUS: 
          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
          DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_ON: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
          _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_OFF: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
          _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
          break;

        case CGPIOD_OUT_CMD_ONLOCKED: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_OFFLOCKED: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
          break;

        case CGPIOD_OUT_CMD_TOGGLE: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
          _outputSetState(pObj, pObj->dwState ^ CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_UNLOCK: 
          pObj->dwFlags &= ~CGPIOD_OUT_FLG_LOCKED;
          break;

        case CGPIOD_OUT_CMD_ONDELAYED: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_ONDELAYED;
          pObj->dwRun = pCmd->msNow + pCmd->dwDelay;
          break;

        case CGPIOD_OUT_CMD_OFFDELAYED: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_OFFDELAYED;
          pObj->dwRun = pCmd->msNow + pCmd->dwDelay;
          break;

        case CGPIOD_OUT_CMD_ONTIMED: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd   = CGPIOD_OUT_CMD_ONTIMED;
          pObj->dwRun   = pCmd->msNow + pCmd->dwRun;
          _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_OFFTIMED: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd   = CGPIOD_OUT_CMD_OFFTIMED;
          pObj->dwRun   = pCmd->msNow + pCmd->dwRun;
          _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
          break;

        case CGPIOD_OUT_CMD_TOGGLEDELAYED: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_TOGGLEDELAYED;
          pObj->dwRun    = pCmd->msNow + pCmd->dwDelay;
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          break;

        case CGPIOD_OUT_CMD_TOGGLETIMED: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_TOGGLETIMED;
          pObj->dwRun    = pCmd->msNow + pCmd->dwRun;
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_LOCK: 
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          break;

        case CGPIOD_OUT_CMD_LOCKTIMED:
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          pObj->dwCmd    = CGPIOD_OUT_CMD_LOCKTIMED;
          pObj->dwRun    = pCmd->msNow + pCmd->dwRun;
          break;

        case CGPIOD_OUT_CMD_TIMEADD:
          if (pObj->dwRun) 
            pObj->dwRun += pCmd->dwRun;
          break;

        case CGPIOD_OUT_CMD_TIMESET:
          if (pObj->dwRun) 
            pObj->dwRun  = pCmd->msNow + pCmd->dwRun;
          break;

        case CGPIOD_OUT_CMD_TIMEABORT:
          pObj->dwRun = 0;
          break;

        case CGPIOD_OUT_CMD_BLINK: 
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_BLINK;
          _outputSetState(pObj, pObj->dwState ^ CGPIOD_OUT_STATE_ON, &evt);
          break;

        default:
//        g_log.LogPrt(m_dwClsLvl | 0x0099, "%s,dropping unknown cmd %u", pFunc, pCmd->dwCmd);
          break;
        } // switch

      } while (FALSE);

    return XERROR_SUCCESS; 
    } // _outputDoCmd

  //--------------------------------------------------------------------------
  // set new state to hw and send event as needed
  //--------------------------------------------------------------------------
  void CGpiod::_outputSetState(tGpiodOutput* pObj, tUint32 dwState, tGpiodEvt* pEvt) 
  {
    // exit if no state change
    if (pObj->dwState == dwState) 
      return;

    // pObj->dwState dwState evt 
    // off           on      on
    // on            off     off
    switch (pObj->dwState = dwState) {
      case CGPIOD_OUT_STATE_OFF:
        digitalWrite(pObj->dwPin, (CGPIOD_OUT_STATE_OFF ^ pObj->dwPol) ? HIGH : LOW);
        if (pEvt) pEvt->dwEvt = CGPIOD_OUT_EVT_OFF;
        break;

      case CGPIOD_OUT_STATE_ON:
        digitalWrite(pObj->dwPin, (CGPIOD_OUT_STATE_ON  ^ pObj->dwPol) ? HIGH : LOW);
        if (pEvt) pEvt->dwEvt = CGPIOD_OUT_EVT_ON;
        break;
      } // switch

    if (pEvt) DoEvt(pEvt);
    } // _outputSetState

