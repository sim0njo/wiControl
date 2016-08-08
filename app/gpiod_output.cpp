
//----------------------------------------------------------------------------
// cgpiod_output.hpp : implementation of 4 outputs
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <AppSettings.h>
#include <gpiod.h>

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputOnConfig() 
  {
    tUint32      dwObj;
    tGpiodOutput *pObj = m_output; 

    // initialise all channels
    Debug.logTxt(CLSLVL_GPIOD_OUTPUT | 0x0000, "CGpiod::_outputOnConfig");
    memset(m_output, 0, sizeof(m_output)); 

    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      // initialise defaults
      pObj->dwFlags  = CGPIOD_OUT_FLG_NONE;
      pObj->dwPin    = (dwObj == CGPIOD_OUT0) ? CGPIOD_OUT0_PIN : 
                       (dwObj == CGPIOD_OUT1) ? CGPIOD_OUT1_PIN : 
                       (dwObj == CGPIOD_OUT2) ? CGPIOD_OUT2_PIN : 
                       (dwObj == CGPIOD_OUT3) ? CGPIOD_OUT3_PIN : -1; 
      pObj->dwPol    = CGPIOD_IO_POL_NORMAL; 
      pObj->dwRunDef = g_appCfg.gpiodOutDefRun[dwObj]; // CGPIOD_OUT_RUN_DEF;
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

    Debug.logTxt(CLSLVL_GPIOD_OUTPUT | 0x0000, "CGpiod::_outputOnInit");
    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      pObj->dwState  = CGPIOD_OUT_STATE_OFF; 
      pObj->dwCmd    = CGPIOD_OUT_CMD_NONE; 
      _ioSetPinVal(pObj->dwPin, pObj->dwState ^ pObj->dwPol);
      _ioSetPinDir(pObj->dwPin, CGPIOD_IO_DIR_OUTPUT);
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
    tGpiodEvt    evt = { msNow, CGPIOD_ORIG_OUTPUT, 0, 0, 0, 0 };

    // handle regular output objects
    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      evt.dwObj = CGPIOD_OBJ_CLS_OUTPUT + dwObj;

      switch (pObj->dwCmd) {
        case CGPIOD_OUT_CMD_OFF:
          if (pObj->dwDelay) {
            if (!ChkTimer(msNow, pObj->dwDelay))
              break;

            // recalculate run timer and set off
            pObj->dwDelay = 0;
            _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);

            if (pObj->dwRun)
              pObj->dwRun = SetTimerSec(msNow, pObj->dwRun);
            } // if delay
          
          // handle run time
          if (pObj->dwRun) {
            if (!ChkTimer(msNow, pObj->dwRun))
              break;

            // set on
            _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
            pObj->dwRun = 0;
            } // if run

          // handle unlocking if locked by this command
          if (GetFlags(&pObj->dwFlags, CGPIOD_OUT_FLG_LOCKED))
            _outputSetLock(pObj, CGPIOD_OUT_LOCK_OFF, &evt);
            
          pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
          break;

        case CGPIOD_OUT_CMD_ON: 
          if (pObj->dwDelay) {
            if (!ChkTimer(msNow, pObj->dwDelay))
              break;

            // recalculate run timer and set on
            pObj->dwDelay = 0;
            _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);

            if (pObj->dwRun)
              pObj->dwRun = SetTimerSec(msNow, pObj->dwRun);
            } // if delay
          
          // handle run time
          if (pObj->dwRun) {
            if (!ChkTimer(msNow, pObj->dwRun))
              break;

            // set off
            _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
            pObj->dwRun = 0;
            } // if run

          // handle unlocking if locked by this command
          if (GetFlags(&pObj->dwFlags, CGPIOD_OUT_FLG_LOCKED))
            _outputSetLock(pObj, CGPIOD_OUT_LOCK_OFF, &evt);
            
          pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
          break;

        case CGPIOD_OUT_CMD_TOGGLE:
          if (pObj->dwDelay) {
            if (!ChkTimer(msNow, pObj->dwDelay))
              break;

            // recalculate run timer and toggle
            pObj->dwDelay = 0;
            _outputSetState(pObj, pObj->dwState ^ CGPIOD_OUT_STATE_ON, &evt);

            if (pObj->dwRun)
              pObj->dwRun = SetTimerSec(msNow, pObj->dwRun);
            } // if delay
          
          // handle run time
          if (pObj->dwRun) {
            if (!ChkTimer(msNow, pObj->dwRun))
              break;

            // toggle
            _outputSetState(pObj, pObj->dwState ^ CGPIOD_OUT_STATE_ON, &evt);
            pObj->dwRun = 0;
            } // if run

          // handle unlocking if locked by this command
          if (GetFlags(&pObj->dwFlags, CGPIOD_OUT_FLG_LOCKED))
            _outputSetLock(pObj, CGPIOD_OUT_LOCK_OFF, &evt);
            
          pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
          break;

        case CGPIOD_OUT_CMD_BLINK:
          // handled by _outputDoEvt() for multi-channel in phase operation

          // handle run time
          if (pObj->dwRun) {
            if (!ChkTimer(msNow, pObj->dwRun))
              break;

            // runtime expired stop blinking
            _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
            pObj->dwRun = 0;

            // handle unlocking if locked by this command
            if (GetFlags(&pObj->dwFlags, CGPIOD_OUT_FLG_LOCKED))
              _outputSetLock(pObj, CGPIOD_OUT_LOCK_OFF, &evt);
            
            pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
            } // if run

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
    Debug.logTxt(CLSLVL_GPIOD_OUTPUT | 0x0000, "CGpiod::_outputOnExit");
    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      _ioSetPinVal(pObj->dwPin, (pObj->dwState = CGPIOD_OUT_STATE_OFF) ^ pObj->dwPol);
      _ioSetPinDir(pObj->dwPin, CGPIOD_IO_DIR_INPUT);
      } // for

    return m_dwError;
    } // CGpiod::_outputOnExit

  //--------------------------------------------------------------------------
  // local emulation handler, called by DoEvt()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputDoEvt(tGpiodEvt* pEvt) 
  { 
    tUint32      dwObj;
    tGpiodCmd    cmd;
    tGpiodEvt    evt   = { pEvt->msNow, pEvt->dwOrig, 0, 0, 0, 0 };
    tGpiodOutput *pObj = m_output; 

    do {
      // initialise vars
      memset(&cmd, 0, sizeof(cmd));
      cmd.msNow  = pEvt->msNow;
      cmd.dwOrig = pEvt->dwOrig;
      
      // handle input event
      if ((pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) == CGPIOD_OBJ_CLS_INPUT) {
        cmd.dwObj = CGPIOD_OBJ_CLS_OUTPUT | (pEvt->dwObj & CGPIOD_OBJ_NUM_MASK);

        switch (pEvt->dwEvt) {
          case CGPIOD_IN_EVT_OUTLT1:
            cmd.dwCmd             = CGPIOD_OUT_CMD_TOGGLE;
            _outputDoCmd(&cmd);
            break;

          case CGPIOD_IN_EVT_INGT2:
            cmd.parmsOutput.dwRun = m_output[pEvt->dwObj & CGPIOD_OBJ_NUM_MASK].dwRunDef;
            cmd.dwCmd             = CGPIOD_OUT_CMD_BLINK;
            _outputDoCmd(&cmd);
            break;
          } // switch

        break;
        } // if 

      // handle 1 second heartbeat for synchronous blinking outputs
      if ((pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) == CGPIOD_OBJ_CLS_HBEAT) {
        for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
          evt.dwObj = CGPIOD_OBJ_CLS_OUTPUT | dwObj;

          if (pObj->dwCmd == CGPIOD_OUT_CMD_BLINK)
            _outputSetState(pObj, (pEvt->dwEvt == CGPIOD_HB_EVT_ODD) ? CGPIOD_OUT_STATE_ON : CGPIOD_OUT_STATE_OFF, &evt);
          } // for

        } // if

      } while (FALSE);

    return m_dwError;
    } // CGpiod::_outputDoEvt

  //--------------------------------------------------------------------------
  // execute command for outputs, called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputDoCmd(tGpiodCmd* pCmd) 
  { 
    tCChar       *pFunc = "CGpiod::_outputDoCmd";
    tChar        str1[16], str2[16];
    tGpiodOutput *pObj = &m_output[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK]; 
    tGpiodEvt    evt   = { pCmd->msNow, pCmd->dwOrig, pCmd->dwObj, 0, 0, 0 };

    do {
      // exit if cmds disabled
      if (g_appCfg.gpiodDisable && (pCmd->dwError = XERROR_ACCESS)) {
        Debug.logTxt(CLSLVL_GPIOD_OUTPUT | 0x0010, "%s,disabled", pFunc);
        break;
        } // if

      switch (pCmd->dwCmd & CGPIOD_CMD_NUM_MASK) {
        case CGPIOD_OUT_CMD_STATUS: 
          // report current value
          evt.dwEvt = pObj->dwState;
          DoSta(&evt);
          break;

        case CGPIOD_OUT_CMD_OFF: // <lock>.<delay>.<run>
          // exit if locked
          if (pObj->dwLock == CGPIOD_OUT_LOCK_ON) break;

          // handle locking + notification
          if (_outputSetLock(pObj, pCmd->parmsOutput.dwLock, &evt))
            // remember that we locked output for this command
            SetFlags(&pObj->dwFlags, CGPIOD_OUT_FLG_LOCKED);

          pObj->dwCmd = CGPIOD_OUT_CMD_OFF;
          if      (pCmd->parmsOutput.dwDelay) {
            pObj->dwDelay = SetTimerSec(pCmd->msNow, pCmd->parmsOutput.dwDelay);
            pObj->dwRun   = pCmd->parmsOutput.dwRun;
            } // if
          else if (pCmd->parmsOutput.dwRun) {
            pObj->dwDelay = 0;
            pObj->dwRun   = SetTimerSec(pCmd->msNow, pCmd->parmsOutput.dwRun);
            _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
            } 
          else {
            _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
            pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
            } // else

          break;

        case CGPIOD_OUT_CMD_ON: // <lock>.<delay>.<run>
          // exit if locked
          if (pObj->dwLock == CGPIOD_OUT_LOCK_ON) break;

          // handle locking + notification
          if (_outputSetLock(pObj, pCmd->parmsOutput.dwLock, &evt))
            // remember that we locked output for this command
            SetFlags(&pObj->dwFlags, CGPIOD_OUT_FLG_LOCKED);

          pObj->dwCmd = CGPIOD_OUT_CMD_ON;
          if      (pCmd->parmsOutput.dwDelay) {
            pObj->dwDelay = SetTimerSec(pCmd->msNow, pCmd->parmsOutput.dwDelay);
            pObj->dwRun   = pCmd->parmsOutput.dwRun;
            } // if
          else if (pCmd->parmsOutput.dwRun) {
            pObj->dwDelay = 0;
            pObj->dwRun   = SetTimerSec(pCmd->msNow, pCmd->parmsOutput.dwRun);
            _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
            } 
          else {
            _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
            pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
            } // else

          break;

        case CGPIOD_OUT_CMD_TOGGLE: // <lock>.<delay>.<run>
          // exit if locked
          if (pObj->dwLock == CGPIOD_OUT_LOCK_ON) break;

          // handle locking + notification
          if (_outputSetLock(pObj, pCmd->parmsOutput.dwLock, &evt))
            // remember that we locked output for this command
            SetFlags(&pObj->dwFlags, CGPIOD_OUT_FLG_LOCKED);

          pObj->dwCmd = CGPIOD_OUT_CMD_TOGGLE;
          if      (pCmd->parmsOutput.dwDelay) {
            pObj->dwDelay = SetTimerSec(pCmd->msNow, pCmd->parmsOutput.dwDelay);
            pObj->dwRun   = pCmd->parmsOutput.dwRun;
            } // if
          else if (pCmd->parmsOutput.dwRun) {
            pObj->dwDelay = 0;
            pObj->dwRun   = SetTimerSec(pCmd->msNow, pCmd->parmsOutput.dwRun);
            _outputSetState(pObj, pObj->dwState ^ CGPIOD_OUT_STATE_ON, &evt);
            } 
          else {
            _outputSetState(pObj, pObj->dwState ^ CGPIOD_OUT_STATE_ON, &evt);
            pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
            } // else

          break;

        case CGPIOD_OUT_CMD_BLINK: // <lock>.<run>
          // exit if locked
          if (pObj->dwLock == CGPIOD_OUT_LOCK_ON) break;

          // handle locking + notification
          if (_outputSetLock(pObj, pCmd->parmsOutput.dwLock, &evt))
            // remember that we locked output for this command
            SetFlags(&pObj->dwFlags, CGPIOD_OUT_FLG_LOCKED);

          pObj->dwCmd = CGPIOD_OUT_CMD_BLINK;
          pObj->dwRun = (pCmd->parmsOutput.dwRun) ? SetTimerSec(pCmd->msNow, pCmd->parmsOutput.dwRun) : 0;

          // toggle output to provide visual feedback
          _outputSetState(pObj, pObj->dwState ^ CGPIOD_OUT_STATE_ON, &evt);
          break;

        case CGPIOD_OUT_CMD_LOCK: // lock [.<lock>]
          // try to lock or unlock
          if (pCmd->dwParms & CGPIOD_OUT_PRM_OLOCK) 
            _outputSetLock(pObj, pCmd->parmsOutput.dwLock, &evt);
          else {
            gsprintf(str1, "%s/lock", PrintObj2String(str2, pCmd->dwObj));
            evt.szObj = str1;
            evt.dwEvt = pObj->dwLock;
            DoSta(&evt);
            } // else

          // report current state
          pCmd->dwRsp = pObj->dwLock;
          break;

        case CGPIOD_OUT_CMD_DEFRUN: 
          // handle if config commands not locked
          if ((pCmd->dwParms & CGPIOD_OUT_PRM_DEFRUN) && !g_appCfg.gpiodLock) {
            pObj->dwRunDef                                                = pCmd->parmsOutput.dwRun;
            g_appCfg.gpiodOutDefRun[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK] = pCmd->parmsOutput.dwRun;
            } // if
   
          gsprintf(str1, "%s/defrun", PrintObj2String(str2, pCmd->dwObj));
          evt.szObj = str1;
          evt.dwEvt = pObj->dwRunDef;
          DoSta(&evt);
        
          pCmd->dwRsp = pObj->dwRunDef;   
          break;

        default:
          pCmd->dwError = XERROR_NOT_FOUND;
          Debug.logTxt(CLSLVL_GPIOD_OUTPUT | 0x0900, "%s,unknown cmd %u", pFunc, pCmd->dwCmd & CGPIOD_CMD_NUM_MASK);
          break;
        } // switch

      // automatically copy state -> rsp
      if (pCmd->dwCmd & CGPIOD_CMD_RSP_AUTO)
        pCmd->dwRsp = pObj->dwState;

      } while (FALSE);

    Debug.logTxt(CLSLVL_GPIOD_OUTPUT | 0x9999, "%s,err=%u", pFunc, pCmd->dwError);
    return pCmd->dwError; 
    } // _outputDoCmd

  //--------------------------------------------------------------------------
  // return lock state
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputGetLock(tUint32 dwObj)
  {
    dwObj &= CGPIOD_OBJ_NUM_MASK;

    if (dwObj < CGPIOD_OUT_COUNT)
      return GetFlags(&m_output[dwObj].dwFlags, CGPIOD_OUT_FLG_LOCKED) ? CGPIOD_OUT_LOCK_ON : CGPIOD_OUT_LOCK_OFF; 
    else
      return CGPIOD_OUT_LOCK_OFF;
    } // _outputGetLock

  //--------------------------------------------------------------------------
  // set lock state and send notification as needed
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputSetLock(tGpiodOutput* pObj, tUint32 dwLock, tGpiodEvt* pEvt) 
  {
    tChar str1[16];

    do {
      // exit if no difference
      if (pObj->dwLock == dwLock)
        break;

      // pObj->dwLock dwLock evt 
      // off          on     on
      // on           off    off
      switch (pObj->dwLock = dwLock) {
        case CGPIOD_OUT_LOCK_OFF:
          if (pEvt) pEvt->dwEvt = CGPIOD_OUT_LOCK_OFF;
          break;

        case CGPIOD_OUT_LOCK_ON:
          if (pEvt) pEvt->dwEvt = CGPIOD_OUT_LOCK_ON;
          break;
        } // switch

      if (pEvt) {
        gsprintf(str1, "%s/lock", PrintObj2String(str1, pEvt->dwObj));
        pEvt->szObj = str1;
        DoSta(pEvt);
        } // if

      } while (FALSE);

    return pObj->dwLock;
    } // _outputSetLock

  //--------------------------------------------------------------------------
  // return output state
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputGetState(tUint32 dwObj)
  {
    dwObj &= CGPIOD_OBJ_NUM_MASK;

    return (dwObj < CGPIOD_OUT_COUNT) ? m_output[dwObj].dwState : CGPIOD_OUT_STATE_OFF;
    } // _outputGetState

  //--------------------------------------------------------------------------
  // set output state to hw and send notification as needed
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputSetState(tGpiodOutput* pObj, tUint32 dwState, tGpiodEvt* pEvt) 
  {
    do {
      // exit if no difference
      if (pObj->dwState == dwState)
        break;

      // pObj->dwState dwState evt 
      // off           on      on
      // on            off     off
      switch (pObj->dwState = dwState) {
        case CGPIOD_OUT_STATE_OFF:
          _ioSetPinVal(pObj->dwPin, CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
          if (pEvt) pEvt->dwEvt = CGPIOD_OUT_STATE_OFF;
          break;

        case CGPIOD_OUT_STATE_ON:
          _ioSetPinVal(pObj->dwPin, CGPIOD_OUT_STATE_ON ^ pObj->dwPol);
          if (pEvt) pEvt->dwEvt = CGPIOD_OUT_STATE_ON;
          break;
        } // switch

      if (pEvt) {
        pEvt->szObj = 0;
        DoSta(pEvt);
        } // if

      } while (FALSE);

    return pObj->dwState;
    } // _outputSetState

