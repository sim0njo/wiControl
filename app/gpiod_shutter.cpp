
//----------------------------------------------------------------------------
// cgpiod_shutter.hpp : implementation of 1 shutter
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterOnConfig() 
  {
    tUint32       dwObj;
    tGpiodShutter *pObj = m_shutter; 

    // initialise all channels
    Debug.logTxt(CLSLVL_GPIOD_SHUTTER | 0x0000, "CGpiod::_shutterOnConfig");

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

    Debug.logTxt(CLSLVL_GPIOD_SHUTTER | 0x0000, "CGpiod::_shutterOnInit");
    for (dwObj = 0; dwObj < CGPIOD_UDM_COUNT; dwObj++, pObj++) {
      _ioSetPinVal(pObj->dwPinUp,   ((pObj->dwState >> 0) & CGPIOD_OUT_STATE_ON) ^ pObj->dwPol);
      _ioSetPinVal(pObj->dwPinDown, ((pObj->dwState >> 1) & CGPIOD_OUT_STATE_ON) ^ pObj->dwPol);
      _ioSetPinDir(pObj->dwPinUp,   CGPIOD_PIN_DIR_OUTPUT);
      _ioSetPinDir(pObj->dwPinDown, CGPIOD_PIN_DIR_OUTPUT);
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
    tGpiodEvt     evt = { msNow, 0, 0, 0, 0 };

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
    Debug.logTxt(CLSLVL_GPIOD_SHUTTER | 0x0000, "CGpiod::_shutterOnExit");
    for (dwObj = 0; dwObj < CGPIOD_UDM_COUNT; dwObj++, pObj++) {
      _ioSetPinVal(pObj->dwPinUp,    CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
      _ioSetPinVal(pObj->dwPinDown,  CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
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
    tGpiodCmd cmd = { 0 };

    do {
      // initialise vars
      cmd.msNow = pEvt->msNow;

      // only first 2 input channels
      if ((pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) > CGPIOD_UDM_COUNT) break;

//    PrintEvt(pEvt, CLSLVL_GPIOD_SHUTTER | 0x0000, "CGpiod::_shutterDoEvt");
      switch (pEvt->dwEvt) {
        case CGPIOD_IN_EVT_INGT0:
          // stop shutter
          cmd.dwObj = CGPIOD_OBJ_CLS_SHUTTER | ((pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) / 2);
          cmd.dwCmd = CGPIOD_UDM_CMD_STOP;
          _shutterDoCmd(&cmd);
          break;

        case CGPIOD_IN_EVT_INGT1:
          // start shutter up/down
          cmd.dwObj              = CGPIOD_OBJ_CLS_SHUTTER | ((pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) / 2);
          cmd.parmsShutter.dwRun = m_shutter[(pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) / 2].dwRunDef; 

          if (pEvt->dwObj & 0x1)
            // in1, in3
            cmd.dwCmd = CGPIOD_UDM_CMD_DOWN;
          else
            // in0, in2
            cmd.dwCmd = CGPIOD_UDM_CMD_UP;
 
          _shutterDoCmd(&cmd);
          break;
        } // switch

      } while (0);

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
    if (pObj->dwPrioLvl < pCmd->parmsShutter.dwPrioLvl)
      return XERROR_ACCESS;

    // nok if cmd prio is locked
    if (pObj->dwPrioMask & (0x1 << pCmd->parmsShutter.dwPrioLvl))
      return XERROR_ACCESS;

    return XERROR_SUCCESS;
    } // _shutterCheckPrio

  //--------------------------------------------------------------------------
  // called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterDoCmd(tGpiodCmd* pCmd) 
  { 
    tChar         str1[16], str2[16];
    tGpiodShutter *pObj = &m_shutter[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK]; 
    tGpiodEvt     evt   = { pCmd->msNow, pCmd->dwObj, 0, 0, 0 };

    PrintCmd(pCmd, CLSLVL_GPIOD_SHUTTER | 0x0000, "CGpiod::_shutterDoCmd");
    switch (pCmd->dwCmd & CGPIOD_OBJ_CMD_MASK) {
      case CGPIOD_UDM_CMD_STATUS: 
        // only send status for non-session origins
        if (pCmd->dwOrig == CGPIOD_ORIG_MQTT) {
          evt.dwEvt = pObj->dwState;
          DoSta(&evt);
          } // if

        break;

      case CGPIOD_UDM_CMD_STOP: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
        pObj->dwCmd = CGPIOD_UDM_CMD_STOP;
        break;

      case CGPIOD_UDM_CMD_TOGGLEUP: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
        _shutterSetState(pObj, (pObj->dwState == CGPIOD_UDM_STATE_STOP) ? CGPIOD_UDM_STATE_UP : 
                                                                          CGPIOD_UDM_STATE_STOP, &evt);
        pObj->dwCmd = (pObj->dwState == CGPIOD_UDM_STATE_STOP) ? CGPIOD_UDM_CMD_UP : CGPIOD_UDM_CMD_STOP;
        pObj->dwRun = pCmd->msNow + pCmd->parmsShutter.dwRun;
        break;

      case CGPIOD_UDM_CMD_TOGGLEDOWN: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
        _shutterSetState(pObj, (pObj->dwState == CGPIOD_UDM_STATE_STOP) ? CGPIOD_UDM_STATE_DOWN : 
                                                                          CGPIOD_UDM_STATE_STOP, &evt);
        pObj->dwCmd = (pObj->dwState == CGPIOD_UDM_STATE_STOP) ? CGPIOD_UDM_CMD_DOWN : CGPIOD_UDM_CMD_STOP;
        pObj->dwRun = pCmd->msNow + pCmd->parmsShutter.dwRun;
        break;

      case CGPIOD_UDM_CMD_UP: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
          
        // make sure to stop first, then move up
        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
        _shutterSetState(pObj, CGPIOD_UDM_STATE_UP, &evt);
        pObj->dwCmd = CGPIOD_UDM_CMD_UP;
        pObj->dwRun = pCmd->msNow + pCmd->parmsShutter.dwRun;
        break;

      case CGPIOD_UDM_CMD_DOWN: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
          
        // make sure to stop first, then move down
        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
        _shutterSetState(pObj, CGPIOD_UDM_STATE_DOWN, &evt);
        pObj->dwCmd = CGPIOD_UDM_CMD_DOWN;
        pObj->dwRun = pCmd->msNow + pCmd->parmsShutter.dwRun;
        break;

      case CGPIOD_UDM_CMD_TIPUP: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
          
        // make sure to stop first, then move up
        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
        _shutterSetState(pObj, CGPIOD_UDM_STATE_UP, &evt);
        pObj->dwCmd = CGPIOD_UDM_CMD_TIPUP;
        pObj->dwRun = pCmd->msNow + pCmd->parmsShutter.dwRun;
        break;

      case CGPIOD_UDM_CMD_TIPDOWN: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
          
        // make sure to stop first, then move down
        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
        _shutterSetState(pObj, CGPIOD_UDM_STATE_DOWN, &evt);
        pObj->dwCmd = CGPIOD_UDM_CMD_TIPDOWN;
        pObj->dwRun = pCmd->msNow + pCmd->parmsShutter.dwRun;
        break;

      case CGPIOD_UDM_CMD_PRIOLOCK: 
        pObj->dwPrioLvl  = CGPIOD_UDM_PRIO_LVL_5;
        pObj->dwPrioMask = pCmd->parmsShutter.dwPrioMask;
        _SetFlags(&pObj->dwFlags, CGPIOD_UDM_FLG_LOCKED);
        break;

      case CGPIOD_UDM_CMD_PRIOUNLOCK: 
        _RstFlags(&pObj->dwFlags, CGPIOD_UDM_FLG_LOCKED);
        break;

      case CGPIOD_UDM_CMD_LEARNON: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
        break;

      case CGPIOD_UDM_CMD_LEARNOFF: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
        break;

      case CGPIOD_UDM_CMD_PRIOSET: 
        pObj->dwPrioLvl  = pCmd->parmsShutter.dwPrioLvl;
        pObj->dwPrioMask = CGPIOD_UDM_PRIO_MASK_NONE;
        _SetFlags(&pObj->dwFlags, CGPIOD_UDM_FLG_LOCKED);
        break;

      case CGPIOD_UDM_CMD_PRIORESET: 
        _RstFlags(&pObj->dwFlags, CGPIOD_UDM_FLG_LOCKED);
        break;

      case CGPIOD_UDM_CMD_SENSROLUP:
        if (_shutterCheckPrio(pObj, pCmd)) break;
          
        // make sure to stop first, then move up
        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
        _shutterSetState(pObj, CGPIOD_UDM_STATE_UP, &evt);
        pObj->dwCmd   = CGPIOD_UDM_CMD_SENSROLUP;
        pObj->dwDelay = pCmd->msNow + pCmd->parmsShutter.dwDelay;
        pObj->dwRun   = pCmd->msNow + pCmd->parmsShutter.dwDelay + pCmd->parmsShutter.dwRun;
        pObj->dwTip   = 0;
        break;

      case CGPIOD_UDM_CMD_SENSJALUP: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
          
        // make sure to stop first, then move up
        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
        _shutterSetState(pObj, CGPIOD_UDM_STATE_UP, &evt);
        pObj->dwCmd   = CGPIOD_UDM_CMD_SENSJALUP;
        pObj->dwDelay = pCmd->msNow + pCmd->parmsShutter.dwDelay;
        pObj->dwRun   = pCmd->msNow + pCmd->parmsShutter.dwDelay + pCmd->parmsShutter.dwRun;
        pObj->dwTip   = pCmd->msNow + pCmd->parmsShutter.dwDelay + pCmd->parmsShutter.dwRun + pCmd->parmsShutter.dwTip;
        break;

      case CGPIOD_UDM_CMD_SENSROLDOWN: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
          
        // make sure to stop first, then move down
        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
        _shutterSetState(pObj, CGPIOD_UDM_STATE_DOWN, &evt);
        pObj->dwCmd   = CGPIOD_UDM_CMD_SENSROLDOWN;
        pObj->dwDelay = pCmd->msNow + pCmd->parmsShutter.dwDelay;
        pObj->dwRun   = pCmd->msNow + pCmd->parmsShutter.dwDelay + pCmd->parmsShutter.dwRun;
        pObj->dwTip   = 0;
        break;

      case CGPIOD_UDM_CMD_SENSJALDOWN: 
        if (_shutterCheckPrio(pObj, pCmd)) break;
          
        // make sure to stop first, then move up
        _shutterSetState(pObj, CGPIOD_UDM_STATE_STOP, &evt);
        _shutterSetState(pObj, CGPIOD_UDM_STATE_DOWN, &evt);
        pObj->dwCmd   = CGPIOD_UDM_CMD_SENSJALDOWN;
        pObj->dwDelay = pCmd->msNow + pCmd->parmsShutter.dwDelay;
        pObj->dwRun   = pCmd->msNow + pCmd->parmsShutter.dwDelay + pCmd->parmsShutter.dwRun;
        pObj->dwTip   = pCmd->msNow + pCmd->parmsShutter.dwDelay + pCmd->parmsShutter.dwRun + pCmd->parmsShutter.dwTip;
        break;

      default:
        Debug.logTxt(CLSLVL_GPIOD_OUTPUT | 0x9999, "CGpiod::_shutterDoCmd,unknown cmd %u", pCmd->dwCmd);
        break;
      } // switch

    pCmd->dwState = pObj->dwState;
    return XERROR_SUCCESS; 
    } // _shutterDoCmd

  //--------------------------------------------------------------------------
  // set new state to hw and send event as needed
  //--------------------------------------------------------------------------
  void CGpiod::_shutterSetState(tGpiodShutter* pObj, tUint32 dwState, tGpiodEvt* pEvt) 
  {
    // exit if no state change
    if (pObj->dwState == dwState) 
      return;

    // pObj->dwState dwState evt 
    // stop          up      UPON
    // stop          down    DOWNON
    // up            stop    UPOFF
    // up            down    not allowed
    // down          stop    DOWNOFF
    // down          up      not allowed
    switch (dwState) {
      case CGPIOD_UDM_STATE_UP:
        _ioSetPinVal(pObj->dwPinDown,  CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
        _ioSetPinVal(pObj->dwPinUp,    CGPIOD_OUT_STATE_ON  ^ pObj->dwPol);
        if (pEvt) pEvt->dwEvt = CGPIOD_UDM_EVT_UPON;
        break;
      case CGPIOD_UDM_STATE_DOWN:
        _ioSetPinVal(pObj->dwPinUp,    CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
        _ioSetPinVal(pObj->dwPinDown,  CGPIOD_OUT_STATE_ON  ^ pObj->dwPol);
        if (pEvt) pEvt->dwEvt = CGPIOD_UDM_EVT_DOWNON;
        break;
      default:
        _ioSetPinVal(pObj->dwPinUp,    CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
        _ioSetPinVal(pObj->dwPinDown,  CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
        if (pEvt) pEvt->dwEvt = (pObj->dwState == CGPIOD_UDM_STATE_UP  ) ? CGPIOD_UDM_EVT_UPOFF : CGPIOD_UDM_EVT_DOWNOFF;
        break;
      } // switch

    pObj->dwState = dwState;
    if (pEvt) DoEvt(pEvt);
    } // _shutterSetState

