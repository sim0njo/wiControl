
//----------------------------------------------------------------------------
// cgpiod_output.hpp : implementation of 2 outputs
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
//#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>
//#include <AppSettings.h>
//#include <HTTP.h>
#include <gpiod.h>

  //--------------------------------------------------------------------------
  // convert output event into string
  //--------------------------------------------------------------------------
  tCChar* CGpiod::_outputEvt2String(tUint32 dwEvt) 
  {
    return (dwEvt == CGPIOD_OUT_EVT_ON)  ? "on"  : 
           (dwEvt == CGPIOD_OUT_EVT_OFF) ? "off" : "timerended";
    } // _outputEvt2String

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputOnConfig() 
  {
    tUint32      dwObj;
    tGpiodOutput *pObj = m_output; 

    // initialise all channels
    Debug.println("CGpiod::_outputOnConfig");
    memset(m_output, 0, sizeof(m_output)); 
    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      // initialise defaults
      pObj->dwFlags  = CGPIOD_OUT_FLG_MQTT_ALL | CGPIOD_OUT_FLG_STANDALONE;
      pObj->dwPin    = (dwObj == 0) ? CGPIOD_OUT0_PIN : CGPIOD_OUT1_PIN; 
      pObj->dwPol    = CGPIOD_OUT_POL_NORMAL; 
      pObj->dwState  = CGPIOD_OUT_STATE_OFF; 
      pObj->dwCmd    = CGPIOD_OUT_CMD_NONE; 
      pObj->szName   = (dwObj == 0) ? "output0" : "output1";
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

    Debug.println("CGpiod::_outputOnInit");
    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      digitalWrite(pObj->dwPin, (pObj->dwState ^ pObj->dwPol) ? HIGH : LOW);
      pinMode(pObj->dwPin, OUTPUT);
//    _hwSetPinVal(pObj->dwPin, pObj->dwState ^ pObj->dwPol);
//    _hwSetPinDir(pObj->dwPin, COGPIO_DIR_OUTPUT);
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
  // handle periodic 1s heartbeat timer tick
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputOnHbTick(tUint32 dwHb, tUint32 dwCntr) 
  {
    tUint32      dwObj;
    tGpiodOutput *pObj = m_output; 
    tGpiodEvt    evt = { 0, 0, 0, 0 };

    do {
      if (dwHb != CGPIOD_HB1) break;

      // handle each output object that requires a tick
      for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
        evt.dwObj = CGPIOD_OBJ_CLS_OUTPUT | dwObj;

        if (pObj->dwCmd == CGPIOD_OUT_CMD_BLINK)
          _outputSetState(pObj, (dwCntr & 0x1) ? CGPIOD_OUT_STATE_ON : CGPIOD_OUT_STATE_OFF, &evt);
        } // for

      } while (FALSE);

    return m_dwError;
    } // CGpiod::_outputOnHbTick

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

//    _hwSetPinVal(pObj->dwPin, (pObj->dwState = CGPIOD_OUT_STATE_OFF) ^ pObj->dwPol);
//    _hwSetPinDir(pObj->dwPin, COGPIO_DIR_INPUT);
      } // for

    return m_dwError;
    } // CGpiod::_outputOnExit

  //--------------------------------------------------------------------------
  // local emulation handler, called by DoEvt()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputDoEvt(tGpiodEvt* pEvt) 
  { 
    tGpiodCmd cmd = { pEvt->msNow, 0, 0, 0, 0, 0, 0 };

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

    return m_dwError;
    } // CGpiod::_outputDoEvt

  //--------------------------------------------------------------------------
  // called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputDoCmd(tGpiodCmd* pCmd) 
  { 
    tGpiodOutput *pObj  = &m_output[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK]; 
    tGpiodEvt    evt    = { pCmd->msNow, pCmd->dwObj, 0, 0 };

    do {
      Debug.println("CGpiod::_outputDoCmd");
      switch (pCmd->dwCmd & CGPIOD_OUT_CMD_MASK) {
        case CGPIOD_OUT_CMD_STATUS: 
//        g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,%s.status", pFunc, pObj->szName);
          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
          DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_ON: 
//        g_log.LogPrt(m_dwClsLvl | 0x0020, "%s,%s.on", pFunc, pObj->szName);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
          _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_OFF: 
//        g_log.LogPrt(m_dwClsLvl | 0x0030, "%s,%s.off", pFunc, pObj->szName);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
          _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
          break;

        case CGPIOD_OUT_CMD_ONLOCKED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0040, "%s,%s.onlocked", pFunc, pObj->szName);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_OFFLOCKED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0050, "%s,%s.offlocked", pFunc, pObj->szName);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
          break;

        case CGPIOD_OUT_CMD_TOGGLE: 
//        g_log.LogPrt(m_dwClsLvl | 0x0060, "%s,%s.toggle", pFunc, pObj->szName);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
          _outputSetState(pObj, pObj->dwState ^ CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_UNLOCK: 
//        g_log.LogPrt(m_dwClsLvl | 0x0070, "%s,%s.unlock", pFunc, pObj->szName);
          pObj->dwFlags &= ~CGPIOD_OUT_FLG_LOCKED;
          break;

        case CGPIOD_OUT_CMD_ONDELAYED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0080, "%s,%s.ondelayed.%u", pFunc, pObj->szName, pCmd->dwDelay);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_ONDELAYED;
          pObj->dwRun = pCmd->msNow + pCmd->dwDelay;
          break;

        case CGPIOD_OUT_CMD_OFFDELAYED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0090, "%s,%s.offdelayed.%u", pFunc, pObj->szName, pCmd->dwDelay);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_OFFDELAYED;
          pObj->dwRun = pCmd->msNow + pCmd->dwDelay;
          break;

        case CGPIOD_OUT_CMD_ONTIMED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0100, "%s,%s.ontimed.%u", pFunc, pObj->szName, pCmd->dwRun);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd   = CGPIOD_OUT_CMD_ONTIMED;
          pObj->dwRun   = pCmd->msNow + pCmd->dwRun;
          _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_OFFTIMED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0110, "%s,%s.offtimed.%u", pFunc, pObj->szName, pCmd->dwRun);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd   = CGPIOD_OUT_CMD_OFFTIMED;
          pObj->dwRun   = pCmd->msNow + pCmd->dwRun;
          _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
          break;

        case CGPIOD_OUT_CMD_TOGGLEDELAYED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0120, "%s,%s.toggledelayed.%u", pFunc, pObj->szName, pCmd->dwDelay);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_TOGGLEDELAYED;
          pObj->dwRun = pCmd->msNow + pCmd->dwDelay;
          break;

        case CGPIOD_OUT_CMD_TOGGLETIMED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0130, "%s,%s.toggletimed.%u", pFunc, pObj->szName, pCmd->dwRun);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_TOGGLETIMED;
          pObj->dwRun    = pCmd->msNow + pCmd->dwRun;
          _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_LOCK: 
//        g_log.LogPrt(m_dwClsLvl | 0x0140, "%s,%s.lock", pFunc, pObj->szName);
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          break;

        case CGPIOD_OUT_CMD_LOCKTIMED:
//        g_log.LogPrt(m_dwClsLvl | 0x0150, "%s,%s.locktimed.%u", pFunc, pObj->szName, pCmd->dwRun);
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          pObj->dwCmd    = CGPIOD_OUT_CMD_LOCKTIMED;
          pObj->dwRun    = pCmd->msNow + pCmd->dwRun;
          break;

        case CGPIOD_OUT_CMD_BLINK: 
//        g_log.LogPrt(m_dwClsLvl | 0x0160, "%s,%s.blink", pFunc, pObj->szName);
          pObj->dwCmd    = CGPIOD_OUT_CMD_BLINK;
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
    if (pObj->dwState == dwState) {
      return;
      } // if

    // pObj->dwState dwState evt 
    // off           on      on
    // on            off     off
    switch (pObj->dwState = dwState) {
      case CGPIOD_OUT_STATE_OFF:
        digitalWrite(pObj->dwPin, (CGPIOD_OUT_STATE_OFF ^ pObj->dwPol) ? HIGH : LOW);
        if (pEvt) pEvt->dwEvt = CGPIOD_OUT_EVT_OFF;
        Debug.println("CGpiod::_outputSetState,off");
        break;

      case CGPIOD_OUT_STATE_ON:
        digitalWrite(pObj->dwPin, (CGPIOD_OUT_STATE_ON  ^ pObj->dwPol) ? HIGH : LOW);
        if (pEvt) pEvt->dwEvt = CGPIOD_OUT_EVT_ON;
        Debug.println("CGpiod::_outputSetState,on");
        break;
      } // switch

    if (pEvt) DoEvt(pEvt);
    } // _outputSetState

