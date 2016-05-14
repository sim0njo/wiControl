
//----------------------------------------------------------------------------
// cgpiod_timer.cpp : implementation of 2 timer outputs
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_timerOnConfig() 
  {
    tUint32      dwObj;
    tGpiodTimer  *pObj = m_timer; 

    // initialise all channels
    Debug.logTxt(CLSLVL_GPIOD_TIMER | 0x0000, "CGpiod::_timerOnConfig");
    memset(m_timer, 0, sizeof(m_timer)); 

    for (dwObj = 0; dwObj < CGPIOD_TMR_COUNT; dwObj++, pObj++) {
      pObj->dwFlags = CGPIOD_TMR_FLG_MQTT_ALL;
      pObj->dwState = CGPIOD_TMR_STATE_OFF;
      pObj->dwCmd   = CGPIOD_TMR_CMD_NONE;
      pObj->dwRun   = 0;
      } // for

    return m_dwError;
    } // _timerOnConfig

  //--------------------------------------------------------------------------
  // initialise 
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_timerOnInit() 
  {
    Debug.logTxt(CLSLVL_GPIOD_TIMER | 0x0000, "CGpiod::_timerOnInit");
    return m_dwError;
    } // _timerOnInit

  //--------------------------------------------------------------------------
  // run 
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_timerOnRun(tUint32 msNow) 
  {
    tUint32     dwObj;
    tGpiodTimer *pObj = m_timer; 
    tGpiodEvt   evt = { msNow, 0, 0, 0, 0 };

    // handle regular output objects
    for (dwObj = 0; dwObj < CGPIOD_TMR_COUNT; dwObj++, pObj++) {
      evt.dwObj = CGPIOD_OBJ_CLS_TIMER + dwObj;

      switch (pObj->dwCmd) {
        case CGPIOD_TMR_CMD_TIMERONDELAYED:
//        case CGPIOD_OUT_CMD_OFFTIMED:
          if (ChkTimer(msNow, pObj->dwRun)) {
            _timerSetState(pObj, CGPIOD_TMR_STATE_ON, &evt);
            pObj->dwCmd = CGPIOD_TMR_CMD_NONE;
            } // if
          break;

        case CGPIOD_TMR_CMD_TIMEROFFDELAYED:
        case CGPIOD_TMR_CMD_TIMERONTIMED:
          if (ChkTimer(msNow, pObj->dwRun)) {
            _timerSetState(pObj, CGPIOD_TMR_STATE_OFF, &evt);
            pObj->dwCmd = CGPIOD_TMR_CMD_NONE;
            } // if
          break;

        default:
          break;
        } // switch
      } // for

    return 0; // m_dwState;
    } // CGpiod::_timerOnRun

  //--------------------------------------------------------------------------
  // exit, nothing to do
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_timerOnExit() 
  {
    Debug.logTxt(CLSLVL_GPIOD_TIMER | 0x0000, "CGpiod::_timerOnExit");
    return m_dwError;
    } // CGpiod::_timerOnExit

  //--------------------------------------------------------------------------
  // local emulation handler, called by DoEvt() or DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_timerDoEvt(tGpiodEvt* pEvt) 
  { 
    return m_dwError;
    } // CGpiod::_timerDoEvt

  //--------------------------------------------------------------------------
  // execute command for outputs, called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_timerDoCmd(tGpiodCmd* pCmd) 
  { 
    tChar       str1[16], str2[16];
    tGpiodTimer *pObj = &m_timer[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK]; 
    tGpiodEvt   evt   = { pCmd->msNow, pCmd->dwObj, 0, 0, 0 };

    Debug.logTxt(CLSLVL_GPIOD_TIMER | 0x0000, "CGpiod::_timerDoCmd,obj=%08X", pCmd->dwObj);

    PrintCmd(pCmd, CLSLVL_GPIOD_TIMER | 0x0000, "CGpiod::_timerDoCmd");
    switch (pCmd->dwCmd & CGPIOD_CMD_NUM_MASK) {
      case CGPIOD_TMR_CMD_STATUS: 
        // only send status for non-session origins
        if (pCmd->dwOrig == CGPIOD_ORIG_MQTT) {
          evt.dwEvt = pObj->dwState;
          DoSta(&evt);
          } // if

        break;

      case CGPIOD_TMR_CMD_TIMERONDELAYED:
        pObj->dwCmd = CGPIOD_TMR_CMD_TIMERONDELAYED;
        pObj->dwRun = pCmd->msNow + pCmd->parmsTimer.dwDelay;
        break;

      case CGPIOD_TMR_CMD_TIMEROFFDELAYED:
        pObj->dwCmd = CGPIOD_TMR_CMD_TIMEROFFDELAYED;
        pObj->dwRun = pCmd->msNow + pCmd->parmsTimer.dwDelay;
        break;

      case CGPIOD_TMR_CMD_TIMERONTIMED:
        pObj->dwCmd   = CGPIOD_TMR_CMD_TIMERONTIMED;
        pObj->dwRun   = pCmd->msNow + pCmd->parmsTimer.dwRun;
        _timerSetState(pObj, CGPIOD_TMR_STATE_ON, &evt);
        break;

      case CGPIOD_TMR_CMD_TIMERABORT:
        pObj->dwCmd = CGPIOD_TMR_CMD_NONE;
        pObj->dwRun = 0;
        evt.dwEvt   = CGPIOD_TMR_EVT_TIMERABORT;
        DoEvt(&evt);
        break;

      default:
        pCmd->dwError = XERROR_INPUT;
        Debug.logTxt(CLSLVL_GPIOD_TIMER | 0x9999, "CGpiod::_outputDoCmd,unknown cmd %u", pCmd->dwCmd);
        break;
      } // switch

    pCmd->dwState = pObj->dwState;
    return pCmd->dwError; 
    } // _timerDoCmd

  //--------------------------------------------------------------------------
  // set new state and send event as needed
  //--------------------------------------------------------------------------
  void CGpiod::_timerSetState(tGpiodTimer* pObj, tUint32 dwState, tGpiodEvt* pEvt) 
  {
    // exit if no state change
    Debug.logTxt(CLSLVL_GPIOD_TIMER | 0x0000, "CGpiod::_timerSetState,state=%u", dwState);
    if (pObj->dwState == dwState) 
      return;

    // pObj->dwState dwState evt 
    // off           on      on
    // on            off     off
    switch (pObj->dwState = dwState) {
      case CGPIOD_TMR_STATE_OFF:
        if (pEvt) pEvt->dwEvt = CGPIOD_TMR_EVT_TIMEROFF;
        break;

      case CGPIOD_TMR_STATE_ON:
        if (pEvt) pEvt->dwEvt = CGPIOD_TMR_EVT_TIMERON;
        break;
      } // switch

    if (pEvt) {
      Debug.logTxt(CLSLVL_GPIOD_TIMER | 0x9999, "CGpiod::_timerSetState,obj=%08X,evt=%u", pEvt->dwObj, pEvt->dwEvt);
      DoEvt(pEvt);
      }

    } // _timerSetState

