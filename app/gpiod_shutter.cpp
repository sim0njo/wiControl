
//----------------------------------------------------------------------------
// cgpiod_shutter.hpp : implementation of 1 shutter
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
  tCChar* CGpiod::_shutterEvt2String(tUint32 dwEvt) 
  {
    return (dwEvt == CGPIOD_UDM_EVT_UPON)    ? "upon"    :
           (dwEvt == CGPIOD_UDM_EVT_UPOFF)   ? "upoff"   :
           (dwEvt == CGPIOD_UDM_EVT_DOWNON)  ? "downon"  :
           (dwEvt == CGPIOD_UDM_EVT_DOWNOFF) ? "downoff" : "stop";
    } // _shutterEvt2String

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterOnConfig() 
  {
    tUint32       dwObj;
    tGpiodShutter *pObj = m_shutter; 

    // initialise all channels
    Debug.println("CGpiod::_shutterOnConfig");
    memset(m_shutter, 0, sizeof(m_shutter)); 
    for (dwObj = 0; dwObj < CGPIOD_UDM_COUNT; dwObj++, pObj++) {
      // initialise defaults
      pObj->dwFlags    = CGPIOD_UDM_FLG_MQTT_ALL;
      pObj->dwPinUp    = CGPIOD_UDM0_PIN_UP;
      pObj->dwPinDown  = CGPIOD_UDM0_PIN_DOWN;
      pObj->dwPol      = CGPIOD_IN_POL_NORMAL; 
      pObj->dwRunDef   = CGPIOD_UDM_RUN_DEF;
      pObj->dwState    = CGPIOD_UDM_STATE_STOP;
      pObj->dwPrioLvl  = CGPIOD_UDM_PRIO_LVL_0; 
      pObj->dwPrioMask = CGPIOD_UDM_PRIO_MASK_NONE; 
      pObj->dwCmd      = CGPIOD_UDM_CMD_NONE; 
      pObj->szName     = "shutter0";
      } // for

    return m_dwError;
    } // _shutterOnConfig

  //--------------------------------------------------------------------------
  // initialise output hw
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterOnInit() 
  {
    tUint32       dwObj;
    tGpiodShutter *pObj = m_shutter; 

    Debug.println("CGpiod::_shutterOnInit");
    for (dwObj = 0; dwObj < CGPIOD_UDM_COUNT; dwObj++, pObj++) {
      digitalWrite(pObj->dwPinUp,   (((pObj->dwState >> 0) & CGPIOD_OUT_STATE_ON) ^ pObj->dwPol) ? HIGH : LOW);
      digitalWrite(pObj->dwPinDown, (((pObj->dwState >> 1) & CGPIOD_OUT_STATE_ON) ^ pObj->dwPol) ? HIGH : LOW);
      pinMode(pObj->dwPinUp,   OUTPUT);
      pinMode(pObj->dwPinDown, OUTPUT);
      } // for

    return m_dwError;
    } // _shutterOnInit

  //--------------------------------------------------------------------------
  // run outputs
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterOnRun(tUint32 msNow) 
  {
    tUint32       dwObj;
    tGpiodShutter *pObj = m_shutter; 
    tGpiodEvt     evt = { msNow, 0, 0, 0 };

    for (dwObj = 0; dwObj < CGPIOD_UDM_COUNT; dwObj++, pObj++) {
      evt.dwObj = CGPIOD_OBJ_CLS_SHUTTER + dwObj;

      switch (pObj->dwCmd) {
        case CGPIOD_UDM_CMD_UP:    // 5
        case CGPIOD_UDM_CMD_TIPUP: // 7
          if (msNow >= pObj->dwRun) {
            _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
            pObj->dwCmd = CGPIOD_UDM_CMD_NONE;
            } // if
          break;

        case CGPIOD_UDM_CMD_DOWN:    // 6
        case CGPIOD_UDM_CMD_TIPDOWN: // 8
          if (msNow >= pObj->dwRun) {
            _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
            pObj->dwCmd = CGPIOD_UDM_CMD_NONE;
            } // if
          break;

        case CGPIOD_UDM_CMD_SENSROLUP:  // 15
        case CGPIOD_UDM_CMD_SENSJALUP:  // 16
          // handle delay time
          if (pObj->dwDelay && (msNow < pObj->dwDelay))
            break;
          
          // initiate up
          if (pObj->dwDelay) {
            _shutterSetState(pObj, CGPIOD_UDM_STATE_UP, &evt);
            pObj->dwDelay = 0;
            } // if
                
          // handle run time
          if (pObj->dwRun && (msNow < pObj->dwRun))
            break;

          // initiate stop
          if (pObj->dwRun) {
            _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
            pObj->dwRun = 0;
            } // if
                
          // handle tip time
//        if (pObj->dwTip && (msNow < pObj->dwTip))
//          break;

//        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          pObj->dwCmd = CGPIOD_UDM_CMD_NONE;
          break;

        case CGPIOD_UDM_CMD_SENSROLDOWN: // 17
        case CGPIOD_UDM_CMD_SENSJALDOWN: // 18
          // handle delay time
          if (pObj->dwDelay && (msNow < pObj->dwDelay))
            break;
          
          // initiate up
          if (pObj->dwDelay) {
            _shutterSetState(pObj, CGPIOD_UDM_STATE_DOWN, &evt);
            pObj->dwDelay = 0;
            } // if
                
          // handle run time
          if (pObj->dwRun && (msNow < pObj->dwRun))
            break;

          // initiate stop
          if (pObj->dwRun) {
            _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
            pObj->dwRun = 0;
            } // if
                
          // handle tip time
//        if (pObj->dwTip && (msNow < pObj->dwTip))
//          break;

//        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          pObj->dwCmd = CGPIOD_UDM_CMD_NONE;
          break;

        default:
          break;
        } // switch
      } // for

    return m_dwError;
    } // CGpiod::_shutterOnRun

  //--------------------------------------------------------------------------
  // leave outputs in decent state
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterOnExit() 
  {
    tUint32       dwObj;
    tGpiodShutter *pObj = m_shutter; 

    // set outputs to off and switch to input
    for (dwObj = 0; dwObj < CGPIOD_UDM_COUNT; dwObj++, pObj++) {
      digitalWrite(pObj->dwPinUp,   (CGPIOD_OUT_STATE_OFF ^ pObj->dwPol) ? HIGH : LOW);
      digitalWrite(pObj->dwPinDown, (CGPIOD_OUT_STATE_OFF ^ pObj->dwPol) ? HIGH : LOW);
      pinMode(pObj->dwPinUp,   INPUT);
      pinMode(pObj->dwPinDown, INPUT);
      } // for

    return m_dwError;
    } // CGpiod::_shutterOnExit

  //--------------------------------------------------------------------------
  // local emulation handler, called by DoEvt()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterDoEvt(tGpiodEvt* pEvt) 
  { 
    tGpiodCmd cmd = { pEvt->msNow, 0, 0, 0, 0, 0, 0 };

    switch (pEvt->dwEvt) {
      case CGPIOD_IN_EVT_INGT0:
        // stop shutter
        cmd.dwObj = CGPIOD_OBJ_CLS_SHUTTER | ((pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) / 2);
        cmd.dwCmd = CGPIOD_UDM_CMD_STOP;
        _shutterDoCmd(&cmd);
        break;

      case CGPIOD_IN_EVT_INGT1:
        // start shutter up/down
        cmd.dwObj = CGPIOD_OBJ_CLS_SHUTTER | ((pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) / 2);
        cmd.dwRun = m_shutter[(pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) / 2].dwRunDef; // 30000;

        if (pEvt->dwObj & 0x1)
          // in1, in3
          cmd.dwCmd = CGPIOD_UDM_CMD_DOWN;
        else
          // in0, in2
          cmd.dwCmd = CGPIOD_UDM_CMD_UP;

        _shutterDoCmd(&cmd);
        break;

      default:
        break;
      } // switch

    return m_dwError;
    } // CGpiod::_shutterDoEvt

  //--------------------------------------------------------------------------
  // check cmd prio vs obj prio
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterCheckPrio(tGpiodShutter* pObj, tGpiodCmd* pCmd)
  { 
    // ok if not locked
    if (_GetFlags(&pObj->dwFlags, CGPIOD_UDM_FLG_LOCKED) == 0)
      return XERROR_SUCCESS;

    // nok if set prio is higher than cmd prio
    if (pObj->dwPrioLvl < pCmd->dwPrioLvl)
      return XERROR_ACCESS;

    // nok if cmd prio is locked
    if (pObj->dwPrioMask & (0x1 << pCmd->dwPrioLvl))
      return XERROR_ACCESS;

    return XERROR_SUCCESS;
    } // _shutterCheckPrio

  //--------------------------------------------------------------------------
  // called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterDoCmd(tGpiodCmd* pCmd) 
  { 
    tGpiodShutter *pObj  = &m_shutter[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK]; 
    tGpiodEvt     evt    = { pCmd->msNow, pCmd->dwObj, 0, 0 };

    do {
      Debug.println("CGpiod::_shutterDoCmd");

      switch (pCmd->dwCmd & CGPIOD_UDM_CMD_MASK) {
        case CGPIOD_UDM_CMD_STATUS: 
//        g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,%s.status", pFunc, pObj->szName);
          evt.dwEvt = (pObj->dwState == CGPIOD_UDM_STATE_UP  ) ? CGPIOD_UDM_EVT_UPON   :
                      (pObj->dwState == CGPIOD_UDM_STATE_DOWN) ? CGPIOD_UDM_EVT_DOWNON : CGPIOD_UDM_EVT_STOP;
          DoEvt(&evt);
          break;

        case CGPIOD_UDM_CMD_STOP: 
//        g_log.LogPrt(m_dwClsLvl | 0x0020, "%s,%s.stop", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          pObj->dwCmd = CGPIOD_UDM_CMD_STOP;
          break;

        case CGPIOD_UDM_CMD_TOGGLEUP: 
//        g_log.LogPrt(m_dwClsLvl | 0x0030, "%s,%s.toggleup", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          _shutterSetState(pObj, (pObj->dwState == CGPIOD_UDM_STATE_STOP) ? CGPIOD_UDM_STATE_UP : 
                                                                            CGPIOD_UDM_STATE_STOP, &evt);
          pObj->dwCmd = (pObj->dwState == CGPIOD_UDM_STATE_STOP) ? CGPIOD_UDM_CMD_UP : CGPIOD_UDM_CMD_STOP;
          pObj->dwRun = pCmd->msNow + pCmd->dwRun;
          break;

        case CGPIOD_UDM_CMD_TOGGLEDOWN: 
//        g_log.LogPrt(m_dwClsLvl | 0x0040, "%s,%s.toggledown", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          _shutterSetState(pObj, (pObj->dwState == CGPIOD_UDM_STATE_STOP) ? CGPIOD_UDM_STATE_DOWN : 
                                                                            CGPIOD_UDM_STATE_STOP, &evt);
          pObj->dwCmd = (pObj->dwState == CGPIOD_UDM_STATE_STOP) ? CGPIOD_UDM_CMD_DOWN : CGPIOD_UDM_CMD_STOP;
          pObj->dwRun = pCmd->msNow + pCmd->dwRun;
          break;

        case CGPIOD_UDM_CMD_UP: 
//        g_log.LogPrt(m_dwClsLvl | 0x0050, "%s,%s.up.%u", pFunc, pObj->szName, pCmd->dwRun);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first, then move up
          _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          _shutterSetState(pObj, CGPIOD_UDM_STATE_UP, &evt);
          pObj->dwCmd = CGPIOD_UDM_CMD_UP;
          pObj->dwRun = pCmd->msNow + pCmd->dwRun;
          break;

        case CGPIOD_UDM_CMD_DOWN: 
//        g_log.LogPrt(m_dwClsLvl | 0x0060, "%s,%s.down.%u", pFunc, pObj->szName, pCmd->dwRun);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first, then move down
          _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          _shutterSetState(pObj, CGPIOD_UDM_STATE_DOWN, &evt);
          pObj->dwCmd = CGPIOD_UDM_CMD_DOWN;
          pObj->dwRun = pCmd->msNow + pCmd->dwRun;
          break;

        case CGPIOD_UDM_CMD_TIPUP: 
//        g_log.LogPrt(m_dwClsLvl | 0x0070, "%s,%s.tipup", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first, then move up
          _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          _shutterSetState(pObj, CGPIOD_UDM_STATE_UP, &evt);
          pObj->dwCmd = CGPIOD_UDM_CMD_TIPUP;
          pObj->dwRun = pCmd->msNow + pCmd->dwRun;
          break;

        case CGPIOD_UDM_CMD_TIPDOWN: 
//        g_log.LogPrt(m_dwClsLvl | 0x0080, "%s,%s.tipdown", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first, then move down
          _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          _shutterSetState(pObj, CGPIOD_UDM_STATE_DOWN, &evt);
          pObj->dwCmd = CGPIOD_UDM_CMD_TIPDOWN;
          pObj->dwRun = pCmd->msNow + pCmd->dwRun;
          break;

        case CGPIOD_UDM_CMD_PRIOLOCK: 
//        g_log.LogPrt(m_dwClsLvl | 0x0090, "%s,%s.priolock", pFunc, pObj->szName);
          pObj->dwPrioLvl  = CGPIOD_UDM_PRIO_LVL_5;
          pObj->dwPrioMask = pCmd->dwPrioMask;
          _SetFlags(&pObj->dwFlags, CGPIOD_UDM_FLG_LOCKED);
          break;

        case CGPIOD_UDM_CMD_PRIOUNLOCK: 
//        g_log.LogPrt(m_dwClsLvl | 0x0100, "%s,%s.priounlock", pFunc, pObj->szName);
          _RstFlags(&pObj->dwFlags, CGPIOD_UDM_FLG_LOCKED);
          break;

        case CGPIOD_UDM_CMD_LEARNON: 
//        g_log.LogPrt(m_dwClsLvl | 0x0110, "%s,%s.learnon", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          break;

        case CGPIOD_UDM_CMD_LEARNOFF: 
//        g_log.LogPrt(m_dwClsLvl | 0x0120, "%s,%s.learnoff", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          break;

        case CGPIOD_UDM_CMD_PRIOSET: 
//        g_log.LogPrt(m_dwClsLvl | 0x0130, "%s,%s.prioset", pFunc, pObj->szName);
          pObj->dwPrioLvl  = pCmd->dwPrioLvl;
          pObj->dwPrioMask = CGPIOD_UDM_PRIO_MASK_NONE;
          _SetFlags(&pObj->dwFlags, CGPIOD_UDM_FLG_LOCKED);
          break;

        case CGPIOD_UDM_CMD_PRIORESET: 
//        g_log.LogPrt(m_dwClsLvl | 0x0140, "%s,%s.prioreset", pFunc, pObj->szName);
          _RstFlags(&pObj->dwFlags, CGPIOD_UDM_FLG_LOCKED);
          break;

        case CGPIOD_UDM_CMD_SENSROLUP:
//        g_log.LogPrt(m_dwClsLvl | 0x0150, "%s,%s.sensrolup", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first, then move up
          _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          _shutterSetState(pObj, CGPIOD_UDM_STATE_UP, &evt);
          pObj->dwCmd   = CGPIOD_UDM_CMD_SENSROLUP;
          pObj->dwDelay = pCmd->msNow + pCmd->dwDelay;
          pObj->dwRun   = pCmd->msNow + pCmd->dwDelay + pCmd->dwRun;
          pObj->dwTip   = 0;
          break;

        case CGPIOD_UDM_CMD_SENSJALUP: 
//        g_log.LogPrt(m_dwClsLvl | 0x0160, "%s,%s.sensjalup", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first, then move up
          _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          _shutterSetState(pObj, CGPIOD_UDM_STATE_UP, &evt);
          pObj->dwCmd   = CGPIOD_UDM_CMD_SENSJALUP;
          pObj->dwDelay = pCmd->msNow + pCmd->dwDelay;
          pObj->dwRun   = pCmd->msNow + pCmd->dwDelay + pCmd->dwRun;
          pObj->dwTip   = pCmd->msNow + pCmd->dwDelay + pCmd->dwRun + pCmd->dwTip;
          break;

        case CGPIOD_UDM_CMD_SENSROLDOWN: 
//        g_log.LogPrt(m_dwClsLvl | 0x0160, "%s,%s.sensroldown", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first, then move down
          _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          _shutterSetState(pObj, CGPIOD_UDM_STATE_DOWN, &evt);
          pObj->dwCmd   = CGPIOD_UDM_CMD_SENSROLDOWN;
          pObj->dwDelay = pCmd->msNow + pCmd->dwDelay;
          pObj->dwRun   = pCmd->msNow + pCmd->dwDelay + pCmd->dwRun;
          pObj->dwTip   = 0;
          break;

        case CGPIOD_UDM_CMD_SENSJALDOWN: 
//        g_log.LogPrt(m_dwClsLvl | 0x0160, "%s,%s.sensjaldown", pFunc, pObj->szName);
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first, then move up
          _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
          _shutterSetState(pObj, CGPIOD_UDM_STATE_DOWN, &evt);
          pObj->dwCmd   = CGPIOD_UDM_CMD_SENSJALDOWN;
          pObj->dwDelay = pCmd->msNow + pCmd->dwDelay;
          pObj->dwRun   = pCmd->msNow + pCmd->dwDelay + pCmd->dwRun;
          pObj->dwTip   = pCmd->msNow + pCmd->dwDelay + pCmd->dwRun + pCmd->dwTip;
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
  void CGpiod::_shutterSetState(tGpiodShutter* pObj, tUint32 dwState, tGpiodEvt* pEvt) 
  {
    // exit if no state change
    if (pObj->dwState == dwState) {
      return;
      } // if

    // pObj->dwState dwState evt 
    // stop          up      UPON
    // stop          down    DOWNON
    // up            stop    UPOFF
    // up            down    not allowed
    // down          stop    DOWNOFF
    // down          up      not allowed
    switch (dwState) {
      case CGPIOD_UDM_STATE_UP:
        digitalWrite(pObj->dwPinDown, (CGPIOD_OUT_STATE_OFF ^ pObj->dwPol) ? HIGH : LOW);
        digitalWrite(pObj->dwPinUp,   (CGPIOD_OUT_STATE_ON  ^ pObj->dwPol) ? HIGH : LOW);
        if (pEvt) pEvt->dwEvt = CGPIOD_UDM_EVT_UPON;
        Debug.println("CGpiod::_shutterSetState,up");
        break;
      case CGPIOD_UDM_STATE_DOWN:
        digitalWrite(pObj->dwPinUp,   (CGPIOD_OUT_STATE_OFF ^ pObj->dwPol) ? HIGH : LOW);
        digitalWrite(pObj->dwPinDown, (CGPIOD_OUT_STATE_ON  ^ pObj->dwPol) ? HIGH : LOW);
        if (pEvt) pEvt->dwEvt = CGPIOD_UDM_EVT_DOWNON;
        Debug.println("CGpiod::_shutterSetState,down");
        break;
      default:
        digitalWrite(pObj->dwPinUp,   (CGPIOD_OUT_STATE_OFF ^ pObj->dwPol) ? HIGH : LOW);
        digitalWrite(pObj->dwPinDown, (CGPIOD_OUT_STATE_OFF ^ pObj->dwPol) ? HIGH : LOW);
        if (pEvt) pEvt->dwEvt = (pObj->dwState == CGPIOD_UDM_STATE_UP  ) ? CGPIOD_UDM_EVT_UPOFF : CGPIOD_UDM_EVT_DOWNOFF;
        Debug.println("CGpiod::_shutterSetState,stop");
        break;
      } // switch

    pObj->dwState = dwState;
    if (pEvt) DoEvt(pEvt);
    } // _shutterSetState

