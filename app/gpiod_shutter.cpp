
//----------------------------------------------------------------------------
// cgpiod_shutter.hpp : implementation of 2 shutters
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <AppSettings.h>
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

    for (dwObj = 0; dwObj < CGPIOD_SHU_COUNT; dwObj++, pObj++) {
      // initialise defaults
      pObj->dwFlags    = CGPIOD_SHU_FLG_NONE;
      pObj->dwPinUp    = (dwObj == CGPIOD_SHU0) ? CGPIOD_SHU0_PIN_UP   : 
                         (dwObj == CGPIOD_SHU1) ? CGPIOD_SHU1_PIN_UP   : -1;
      pObj->dwPinDown  = (dwObj == CGPIOD_SHU0) ? CGPIOD_SHU0_PIN_DOWN :
                         (dwObj == CGPIOD_SHU1) ? CGPIOD_SHU1_PIN_DOWN : -1;
      pObj->dwPol      = CGPIOD_IO_POL_NORMAL; 
      pObj->dwRunDef   = AppSettings.gpiodUdmDefRun[dwObj];
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
    for (dwObj = 0; dwObj < CGPIOD_SHU_COUNT; dwObj++, pObj++) {
      pObj->dwState    = CGPIOD_SHU_STATE_STOP;
      pObj->dwPrioLvl  = CGPIOD_SHU_PRIO_LVL_0; 
      pObj->dwPrioMask = CGPIOD_SHU_PRIO_MASK_NONE; 
      pObj->dwCmd      = CGPIOD_SHU_CMD_NONE; 
      _ioSetPinVal(pObj->dwPinUp,   ((pObj->dwState >> 0) & CGPIOD_OUT_STATE_ON) ^ pObj->dwPol);
      _ioSetPinVal(pObj->dwPinDown, ((pObj->dwState >> 1) & CGPIOD_OUT_STATE_ON) ^ pObj->dwPol);
      _ioSetPinDir(pObj->dwPinUp,   CGPIOD_IO_DIR_OUTPUT);
      _ioSetPinDir(pObj->dwPinDown, CGPIOD_IO_DIR_OUTPUT);
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
    tGpiodEvt     evt = { msNow, CGPIOD_ORIG_SHUTTER, 0, 0, 0, 0 };

    for (dwObj = 0; dwObj < CGPIOD_SHU_COUNT; dwObj++, pObj++) {
      evt.dwObj = CGPIOD_OBJ_CLS_SHUTTER + dwObj;

      switch (pObj->dwCmd) {
        case CGPIOD_SHU_CMD_UP:
          // handle delay1 time
          if (pObj->dwDelay1) {
            if (!ChkTimer(msNow, pObj->dwDelay1))
              break;

            // recalculate run timer and initiate up
            pObj->dwDelay1 = 0;
            pObj->dwRun    = SetTimerSec(msNow, pObj->dwRun);
            _shutterSetState(pObj, CGPIOD_SHU_STATE_UP, &evt);
            } // if delay1
          
          // handle run time
          if (pObj->dwRun) {
            if (!ChkTimer(msNow, pObj->dwRun))
              break;

            // initiate stop
            _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt);
            pObj->dwRun = 0;
            
            // handle delay2 (in ms) between run and tip
            if (pObj->dwDelay2) 
              pObj->dwDelay2 = SetTimerMilliSec(msNow, pObj->dwDelay2);

            } // if run

          // handle delay2 (in ms) between run and tip
          if (pObj->dwDelay2) {
            if (!ChkTimer(msNow, pObj->dwDelay2))
              break;

            // recalculate tip timer and initiate down
            pObj->dwDelay2 = 0;
            pObj->dwTip    = SetTimerDeciSec(msNow, pObj->dwTip);
            _shutterSetState(pObj, CGPIOD_SHU_STATE_DOWN, &evt);
            } // if delay2
          
          // handle tip time
          if (pObj->dwTip) {
            if (!ChkTimer(msNow, pObj->dwTip))
              break;

            // initiate stop
            _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt);
            pObj->dwTip = 0;
            pObj->dwCmd = CGPIOD_SHU_CMD_NONE;
            } // if tip

          break;

        case CGPIOD_SHU_CMD_DOWN:
          // handle delay1 time
          if (pObj->dwDelay1) {
            if (!ChkTimer(msNow, pObj->dwDelay1))
              break;

            // recalculate run timer and initiate down
            pObj->dwDelay1 = 0;
            pObj->dwRun    = SetTimerSec(msNow, pObj->dwRun);
            _shutterSetState(pObj, CGPIOD_SHU_STATE_DOWN, &evt);
            } // if delay1

          // handle run time
          if (pObj->dwRun) {
            if (!ChkTimer(msNow, pObj->dwRun))
              break;

            // initiate stop
            _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt);
            pObj->dwRun = 0;
            
            // handle delay2 (in ms) between run and tip
            if (pObj->dwDelay2) 
              pObj->dwDelay2 = SetTimerMilliSec(msNow, pObj->dwDelay2);

            } // if run
         
          // handle delay2 (in ms) between run and tip
          if (pObj->dwDelay2) {
            if (!ChkTimer(msNow, pObj->dwDelay2))
              break;

            // initiate down and recalculate tip timer
            pObj->dwDelay2 = 0;
            pObj->dwTip    = SetTimerDeciSec(msNow, pObj->dwTip);
            _shutterSetState(pObj, CGPIOD_SHU_STATE_UP, &evt);
            } // if delay2
          
          // handle tip time
          if (pObj->dwTip) {
            if (!ChkTimer(msNow, pObj->dwTip))
              break;

            // initiate stop
            _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt);
            pObj->dwTip = 0;
            pObj->dwCmd = CGPIOD_SHU_CMD_NONE;
            } // if tip

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
    for (dwObj = 0; dwObj < CGPIOD_SHU_COUNT; dwObj++, pObj++) {
      _ioSetPinVal(pObj->dwPinUp,   CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
      _ioSetPinVal(pObj->dwPinDown, CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
      _ioSetPinDir(pObj->dwPinUp,   CGPIOD_IO_DIR_INPUT);
      _ioSetPinDir(pObj->dwPinDown, CGPIOD_IO_DIR_INPUT);
      } // for

    return m_dwError;
    } // CGpiod::_shutterOnExit

  //--------------------------------------------------------------------------
  // local emulation handler, called by DoEvt()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterDoEvt(tGpiodEvt* pEvt) 
  { 
    tGpiodCmd cmd;

    do {
      // initialise vars
      memset(&cmd, 0, sizeof(cmd));
      cmd.msNow  = pEvt->msNow;
      cmd.dwOrig = pEvt->dwOrig;

      // handle input event
      if ((pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) == CGPIOD_OBJ_CLS_INPUT) {

        switch (pEvt->dwEvt) {
          case CGPIOD_IN_EVT_INGT0:
            // stop shutter
            cmd.dwObj = CGPIOD_OBJ_CLS_SHUTTER | ((pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) / 2);
            cmd.dwCmd = CGPIOD_SHU_CMD_STOP;
            _shutterDoCmd(&cmd);
            break;

          case CGPIOD_IN_EVT_INGT1:
            // start shutter up/down
            cmd.dwObj              = CGPIOD_OBJ_CLS_SHUTTER | ((pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) / 2);
            cmd.parmsShutter.dwRun = m_shutter[(pEvt->dwObj & CGPIOD_OBJ_NUM_MASK) / 2].dwRunDef; 

            if (pEvt->dwObj & 0x1)
              // in1, in3
              cmd.dwCmd = CGPIOD_SHU_CMD_DOWN;
            else
              // in0, in2
              cmd.dwCmd = CGPIOD_SHU_CMD_UP;
 
            _shutterDoCmd(&cmd);
            break;
          } // switch

        } // if 

      } while (0);

    return m_dwError;
    } // CGpiod::_shutterDoEvt

  //--------------------------------------------------------------------------
  // check cmd prio vs obj prio
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterCheckPrio(tGpiodShutter* pObj, tGpiodCmd* pCmd)
  { 
    // ok if not locked
    if (_GetFlags(&pObj->dwFlags, CGPIOD_SHU_FLG_LOCKED) == 0)
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
  // 
  // toggleup    D1=100        R   D2=0   T=0 + stop
  // toggledown  D1=100        R   D2=0   T=0 + stop
  // 
  // up          D1=100        R   D2=0   T=0 + stop
  // down        D1=100        R   D2=0   T=0 + stop
  // 
  // tipup       D1=0          R=0 D2=100 T   + stop
  // tipdown     D1=0          R=0 D2=100 T   + stop
  // 
  // sensrolup   D1=max(D,100) R   D2=0   T=0 + stop
  // sensjalup   D1=max(D,100) R   D2=100 T   + stop
  // 
  // sensroldown D1=max(D,100) R   D2=0   T=0 + stop
  // sensjaldown D1=max(D,100) R   D2=100 T   + stop
  // 
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterDoCmd(tGpiodCmd* pCmd) 
  { 
    tCChar        *pFunc = "CGpiod::_shutterDoCmd";
    tChar         str1[16], str2[16];
    tGpiodShutter *pObj = &m_shutter[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK]; 
    tGpiodEvt     evt   = { pCmd->msNow, pCmd->dwOrig, pCmd->dwObj, 0, 0, 0 };

    do {
      // exit if cmds disabled
      if (AppSettings.gpiodDisable && (pCmd->dwError = XERROR_ACCESS)) {
        Debug.logTxt(CLSLVL_GPIOD_SHUTTER | 0x0010, "%s,disabled", pFunc);
        break;
        } // if

      switch (pCmd->dwCmd & CGPIOD_CMD_NUM_MASK) {
        case CGPIOD_SHU_CMD_STATUS: 
          // report current value
          evt.dwEvt = pObj->dwState;
          DoSta(&evt);
          break;

        case CGPIOD_SHU_CMD_STOP: 
          if (_shutterCheckPrio(pObj, pCmd)) break;
          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt);
          pObj->dwCmd = CGPIOD_SHU_CMD_NONE;
          break;

        case CGPIOD_SHU_CMD_TOGGLEUP: 
          if (_shutterCheckPrio(pObj, pCmd)) break;

          if (pObj->dwState == CGPIOD_SHU_STATE_STOP) {
            // prepare for up
            pObj->dwDelay1 = SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY);
            pObj->dwRun    = pCmd->parmsShutter.dwRun;
            pObj->dwDelay2 = pObj->dwTip = 0;
            pObj->dwCmd    = CGPIOD_SHU_CMD_UP;
            }
          else
            pObj->dwCmd    = CGPIOD_SHU_CMD_NONE;

          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt); 
          break;

        case CGPIOD_SHU_CMD_TOGGLEDOWN: 
          if (_shutterCheckPrio(pObj, pCmd)) break;

          if (pObj->dwState == CGPIOD_SHU_STATE_STOP) {
            // prepare for down
            pObj->dwDelay1 = SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY);
            pObj->dwRun    = pCmd->parmsShutter.dwRun;
            pObj->dwDelay2 = pObj->dwTip = 0;
            pObj->dwCmd    = CGPIOD_SHU_CMD_DOWN;
            }
          else
            pObj->dwCmd    = CGPIOD_SHU_CMD_NONE;

          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt); 
          break;

        case CGPIOD_SHU_CMD_UP: 
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first
          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt); 

          // prepare for up
          pObj->dwDelay1 = SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY);
          pObj->dwRun    = pCmd->parmsShutter.dwRun;
          pObj->dwDelay2 = pObj->dwTip = 0;
          pObj->dwCmd    = CGPIOD_SHU_CMD_UP;
          break;

        case CGPIOD_SHU_CMD_DOWN: 
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first
          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt); 

          // prepare for down
          pObj->dwDelay1 = SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY);
          pObj->dwRun    = pCmd->parmsShutter.dwRun;
          pObj->dwDelay2 = pObj->dwTip = 0;
          pObj->dwCmd    = CGPIOD_SHU_CMD_DOWN;
          break;

        case CGPIOD_SHU_CMD_TIPUP: 
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first
          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt); 

          // prepare for up
          pObj->dwDelay1 = pObj->dwRun = 0;
          pObj->dwDelay2 = SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY);
          pObj->dwTip    = pCmd->parmsShutter.dwTip;
          pObj->dwCmd    = CGPIOD_SHU_CMD_UP;
          break;
    
        case CGPIOD_SHU_CMD_TIPDOWN: 
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first
          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt); 

          // prepare for down
          pObj->dwDelay1 = pObj->dwRun = 0;
          pObj->dwDelay2 = SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY);
          pObj->dwTip    = pCmd->parmsShutter.dwTip;
          pObj->dwCmd    = CGPIOD_SHU_CMD_DOWN;
          break;

        case CGPIOD_SHU_CMD_PRIOLOCK: 
          pObj->dwPrioLvl  = CGPIOD_SHU_PRIO_LVL_5;
          pObj->dwPrioMask = pCmd->parmsShutter.dwPrioMask;
          _SetFlags(&pObj->dwFlags, CGPIOD_SHU_FLG_LOCKED);
          break;

        case CGPIOD_SHU_CMD_PRIOUNLOCK: 
          _RstFlags(&pObj->dwFlags, CGPIOD_SHU_FLG_LOCKED);
          break;

        case CGPIOD_SHU_CMD_LEARNON: 
          if (_shutterCheckPrio(pObj, pCmd)) break;
          break;

        case CGPIOD_SHU_CMD_LEARNOFF: 
          if (_shutterCheckPrio(pObj, pCmd)) break;
          break;

        case CGPIOD_SHU_CMD_PRIOSET: 
          pObj->dwPrioLvl  = pCmd->parmsShutter.dwPrioLvl;
          pObj->dwPrioMask = CGPIOD_SHU_PRIO_MASK_NONE;
          _SetFlags(&pObj->dwFlags, CGPIOD_SHU_FLG_LOCKED);
          break;

        case CGPIOD_SHU_CMD_PRIORESET: 
          _RstFlags(&pObj->dwFlags, CGPIOD_SHU_FLG_LOCKED);
          break;

        case CGPIOD_SHU_CMD_SENSROLUP:
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first
          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt); 

          // prepare for up
          pObj->dwDelay1 = pCmd->parmsShutter.dwDelay ? SetTimerSec(pCmd->msNow, pCmd->parmsShutter.dwDelay) : 
                                                        SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY);
          pObj->dwRun    = pCmd->parmsShutter.dwRun;
          pObj->dwDelay2 = pObj->dwTip = 0;
          pObj->dwCmd    = CGPIOD_SHU_CMD_UP;
          break;

        case CGPIOD_SHU_CMD_SENSJALUP: 
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first
          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt); 

          // prepare for up
          pObj->dwDelay1 = pCmd->parmsShutter.dwDelay ? SetTimerSec(pCmd->msNow, pCmd->parmsShutter.dwDelay) : 
                                                        SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY);
          pObj->dwRun    = pCmd->parmsShutter.dwRun;
          pObj->dwDelay2 = pCmd->parmsShutter.dwTip   ? SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY) : 0;
          pObj->dwTip    = pCmd->parmsShutter.dwTip;
          pObj->dwCmd    = CGPIOD_SHU_CMD_UP;
          break;

        case CGPIOD_SHU_CMD_SENSROLDOWN: 
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first
          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt); 

          // prepare for down
          pObj->dwDelay1 = pCmd->parmsShutter.dwDelay ? SetTimerSec(pCmd->msNow, pCmd->parmsShutter.dwDelay) : 
                                                        SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY);
          pObj->dwRun    = pCmd->parmsShutter.dwRun;
          pObj->dwDelay2 = pObj->dwTip = 0;
          pObj->dwCmd    = CGPIOD_SHU_CMD_DOWN;
          break;

        case CGPIOD_SHU_CMD_SENSJALDOWN: 
          if (_shutterCheckPrio(pObj, pCmd)) break;
          
          // make sure to stop first
          _shutterSetState(pObj, CGPIOD_SHU_STATE_STOP, &evt); 

          // prepare for up
          pObj->dwDelay1 = pCmd->parmsShutter.dwDelay ? SetTimerSec(pCmd->msNow, pCmd->parmsShutter.dwDelay) : 
                                                        SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY);
          pObj->dwRun    = pCmd->parmsShutter.dwRun;
          pObj->dwDelay2 = pCmd->parmsShutter.dwTip   ? SetTimerMilliSec(pCmd->msNow, CGPIOD_SHU_DEF_DELAY) : 0;
          pObj->dwTip    = pCmd->parmsShutter.dwTip;
          pObj->dwCmd    = CGPIOD_SHU_CMD_DOWN;
          break;

        case CGPIOD_SHU_CMD_DEFRUN: 
          // handle if config commands not locked
          if ((pCmd->dwParms & CGPIOD_SHU_PRM_DEFRUN) && !AppSettings.gpiodLock) {
            pObj->dwRunDef                                                = pCmd->parmsShutter.dwRun;
            AppSettings.gpiodUdmDefRun[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK] = pCmd->parmsShutter.dwRun;
            } // if
        
          pCmd->dwRsp = pObj->dwRunDef;   

          gsprintf(str1, "%s/defrun", PrintObj2String(str2, pCmd->dwObj));
          evt.szObj = str1;
          evt.dwEvt = pObj->dwRunDef;
          DoSta(&evt);
          break;

        default:
          pCmd->dwError = XERROR_NOT_FOUND;
          Debug.logTxt(CLSLVL_GPIOD_SHUTTER | 0x0900, "%s,unknown cmd %u", pFunc, pCmd->dwCmd & CGPIOD_CMD_NUM_MASK);
          break;
        } // switch

      // automatically copy state -> rsp
      if (pCmd->dwCmd & CGPIOD_CMD_RSP_AUTO)
        pCmd->dwRsp = pObj->dwState;
   
      } while (FALSE);

    Debug.logTxt(CLSLVL_GPIOD_SHUTTER | 0x9999, "%s,err=%u", pFunc, pCmd->dwError);
    return pCmd->dwError; 
    } // _shutterDoCmd

  //--------------------------------------------------------------------------
  // return shutter state
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_shutterGetState(tUint32 dwObj)
  {
    dwObj &= CGPIOD_OBJ_NUM_MASK;

    return (dwObj < CGPIOD_SHU_COUNT) ? m_shutter[dwObj].dwState : CGPIOD_SHU_STATE_STOP;
    } // _shutterGetState

  //--------------------------------------------------------------------------
  // set new state to hw and send event as needed
  //--------------------------------------------------------------------------
  void CGpiod::_shutterSetState(tGpiodShutter* pObj, tUint32 dwState, tGpiodEvt* pEvt) 
  {
    // pObj->dwState dwState evt 
    // stop          up      UPON
    // stop          down    DOWNON
    // up            stop    UPOFF
    // up            down    not allowed
    // down          stop    DOWNOFF
    // down          up      not allowed
    switch (pObj->dwState = dwState) {
      case CGPIOD_SHU_STATE_UP:
        _ioSetPinVal(pObj->dwPinDown,  CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
        _ioSetPinVal(pObj->dwPinUp,    CGPIOD_OUT_STATE_ON  ^ pObj->dwPol);
        if (pEvt) pEvt->dwEvt = CGPIOD_SHU_STATE_UP;
        break;
      case CGPIOD_SHU_STATE_DOWN:
        _ioSetPinVal(pObj->dwPinUp,    CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
        _ioSetPinVal(pObj->dwPinDown,  CGPIOD_OUT_STATE_ON  ^ pObj->dwPol);
        if (pEvt) pEvt->dwEvt = CGPIOD_SHU_STATE_DOWN;
        break;
      default:
        _ioSetPinVal(pObj->dwPinUp,    CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
        _ioSetPinVal(pObj->dwPinDown,  CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
        if (pEvt) pEvt->dwEvt = CGPIOD_SHU_STATE_STOP;
        break;
      } // switch

    if (pEvt) DoSta(pEvt);             
    } // _shutterSetState

