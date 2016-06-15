
//----------------------------------------------------------------------------
// cgpiod_system.hpp : implementation system objects
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <AppSettings.h>
#include <Network.h>
#include <gpiod.h>

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemOnConfig() 
  {
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0010, "CGpiod::_systemOnConfig,emul   =%u", AppSettings.gpiodEmul);
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0020, "CGpiod::_systemOnConfig,mode   =%u", AppSettings.gpiodMode);
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0030, "CGpiod::_systemOnConfig,lock   =%u", AppSettings.gpiodLock);
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0040, "CGpiod::_systemOnConfig,disable=%u", AppSettings.gpiodDisable);

    // configure heartbeat timer
    memset(&m_hbeat, 0, sizeof(m_hbeat));
    m_hbeat.msPeriod = CGPIOD_HB_PERIOD;

    // configure system led
    memset(&m_led, 0, sizeof(m_led)); 
    m_led.dwPin   = CGPIOD_LED_PIN; 
    m_led.dwPol   = CGPIOD_IO_POL_NORMAL; 
    m_led.dwState = CGPIOD_OUT_STATE_OFF; 
    m_led.dwCmd   = CGPIOD_LED_CMD_NONE; 
    return m_dwError;
    } // _systemOnConfig

  //--------------------------------------------------------------------------
  // initialise
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemOnInit() 
  {
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0000, "CGpiod::_systemOnInit");
    _ioSetPinVal(m_led.dwPin, m_led.dwState ^ m_led.dwPol);
    _ioSetPinDir(m_led.dwPin, CGPIOD_IO_DIR_OUTPUT);
    return m_dwError;
    } // _systemOnInit

  //--------------------------------------------------------------------------
  // run outputs
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemOnRun(tUint32 msNow) 
  {
    tGpiodEvt    evt = { msNow, CGPIOD_ORIG_SYSTEM, 0, 0, 0, 0 };

    // handle heartbeat timer
    if (ChkTimer(msNow, m_hbeat.msStart + m_hbeat.msPeriod)) {
      m_hbeat.dwCntr++;
      m_hbeat.msStart = msNow;
      evt.dwObj = CGPIOD_OBJ_CLS_HBEAT;
      evt.dwEvt = (m_hbeat.dwCntr & 1) ? CGPIOD_HB_EVT_ODD : CGPIOD_HB_EVT_EVEN;
      DoEvt(&evt);
      } // if

    // handle system leds
    switch (m_led.dwCmd) {
      case CGPIOD_LED_CMD_ONTIMED:
        if (msNow >= m_led.dwRun) {
          _outputSetState(&m_led, CGPIOD_OUT_STATE_OFF, 0);
          m_led.dwCmd = CGPIOD_LED_CMD_NONE;
          } // if
        break;

      case CGPIOD_LED_CMD_BLINK:
        // handled by _systemDoEvt() for multi-channel in phase operation
        break;

      case CGPIOD_LED_CMD_BLINKTIMED:
        if (msNow >= m_led.dwRun) {
          _outputSetState(&m_led, CGPIOD_OUT_STATE_OFF, 0);
          m_led.dwCmd = CGPIOD_LED_CMD_NONE;
          } // if
        break;

      default:
        break;
      } // switch

    return m_dwError;
    } // CGpiod::_systemOnRun

  //--------------------------------------------------------------------------
  // leave outputs in decent state
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemOnExit() 
  {
    // set outputs to off and switch to input
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0000, "CGpiod::_systemOnExit");
    _ioSetPinVal(m_led.dwPin, (m_led.dwState = CGPIOD_OUT_STATE_OFF) ^ m_led.dwPol);
    _ioSetPinDir(m_led.dwPin, CGPIOD_IO_DIR_INPUT);
    return m_dwError;
    } // CGpiod::_systemOnExit

  //--------------------------------------------------------------------------
  // local emulation handler, called by DoEvt()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemDoEvt(tGpiodEvt* pEvt) 
  { 
    // handle 1 second heartbeat for synchronous blinking leds
    if ((pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) == CGPIOD_OBJ_CLS_HBEAT) {
      
      if ((m_led.dwCmd == CGPIOD_LED_CMD_BLINK) || (m_led.dwCmd == CGPIOD_LED_CMD_BLINKTIMED))
        _outputSetState(&m_led, (pEvt->dwEvt == CGPIOD_HB_EVT_ODD) ? CGPIOD_OUT_STATE_ON : CGPIOD_OUT_STATE_OFF, 0);

      } // if

    return m_dwError;
    } // CGpiod::_systemDoEvt

  //--------------------------------------------------------------------------
  // execute command for outputs, called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemDoCmd(tGpiodCmd* pCmd) 
  { 
    tChar     str[32];
    tGpiodEvt evt = { pCmd->msNow, pCmd->dwOrig, pCmd->dwObj, 0, 0, 0 };
    tGpiodCmd cmd = { 0 };

    do {
      switch (pCmd->dwCmd & CGPIOD_CMD_NUM_MASK) {
        case CGPIOD_SYS_CMD_PING: // blink led @ D8 for x seconds
          m_led.dwCmd = CGPIOD_LED_CMD_BLINKTIMED;
          m_led.dwRun = pCmd->msNow + CGPIOD_LED_BLINKTIME;
          _outputSetState(&m_led, m_led.dwState ^ CGPIOD_OUT_STATE_ON, 0);
          pCmd->dwRsp = 1;

          evt.szObj = "pong";
          gsprintf(str, "%s", Network.getClientIP().toString().c_str());
          evt.szEvt = str;
          DoEvt(&evt);
          break;

        case CGPIOD_SYS_CMD_VERSION: 
          // report current value
          pCmd->dwRsp = APP_VERSION;

          gsprintf(str, "%s;%s/%s", szAPP_VERSION, szAPP_TOPOLOGY, szAPP_TOPOVER);
          evt.szEvt   = str;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_MEMORY: 
          // report current value
          pCmd->dwRsp = system_get_free_heap_size();
//        pCmd->dwRsp = system_get_vdd33(); // 3000..3307
//        pCmd->dwRsp = system_adc_read(); // always 65535
//        pCmd->dwRsp = analogRead(A0);

          evt.dwEvt   = pCmd->dwRsp;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_UPTIME: 
          // report current value
          pCmd->dwRsp = pCmd->msNow;
          evt.dwEvt   = pCmd->msNow;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_LOGLEVEL:
          // handle set command
          if ((pCmd->dwParms == CGPIOD_SYS_PRM_LOGLEVEL) && !AppSettings.gpiodLock)
            Debug.logClsLevels(DEBUG_CLS_0, pCmd->parmsSystem.dwParm);

          // report current value
          pCmd->dwRsp = Debug.logClsLevels(DEBUG_CLS_0);
          evt.dwEvt   = Debug.logClsLevels(DEBUG_CLS_0);
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_EMUL:
          // handle set command
          if ((pCmd->dwParms == CGPIOD_SYS_PRM_EMUL) && !AppSettings.gpiodLock) {
            Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0100, "CGpiod::_systemDoCmd,set emul to %u", pCmd->parmsSystem.dwParm);
          
            AppSettings.gpiodEmul = pCmd->parmsSystem.dwParm;
            OnInit();
            } // if

          // report current value
          pCmd->dwRsp = AppSettings.gpiodEmul;
          evt.dwEvt   = AppSettings.gpiodEmul;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_MODE: 
          // handle set command
          if ((pCmd->dwParms == CGPIOD_SYS_PRM_MODE) && !AppSettings.gpiodLock) {
            Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0100, "CGpiod::_systemDoCmd,set mode to %u", pCmd->parmsSystem.dwParm);
            AppSettings.gpiodMode = pCmd->parmsSystem.dwParm;
            } // if

          // report current value
          pCmd->dwRsp = AppSettings.gpiodMode;
          evt.dwEvt   = AppSettings.gpiodMode;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_LOCK:
          // handle set command
          if (pCmd->dwParms == CGPIOD_SYS_PRM_OFFON) {
            AppSettings.gpiodLock = pCmd->parmsSystem.dwParm ? 1 : 0;
            } // if

          // report current value
          pCmd->dwRsp = AppSettings.gpiodLock;
          evt.dwEvt   = AppSettings.gpiodLock;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_DISABLE: 
          // handle command
          if ((pCmd->dwParms == CGPIOD_SYS_PRM_OFFON) && !AppSettings.gpiodLock) {
            AppSettings.gpiodDisable = pCmd->parmsSystem.dwParm ? 1 : 0;
            } // if

          // report current value
          pCmd->dwRsp = AppSettings.gpiodDisable;
          evt.dwEvt   = AppSettings.gpiodDisable;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_RESTART:
          // handle command
          if ((pCmd->dwParms == CGPIOD_SYS_PRM_ACK) && (AppSettings.gpiodLock == CGPIOD_LOCK_FALSE)) {
            evt.dwEvt = 1;
            DoEvt(&evt);

            System.restart();
            } // if

          break;

        case CGPIOD_SYS_CMD_SAVE:
          if (pCmd->dwParms == CGPIOD_SYS_PRM_ACK) {
            AppSettings.save();

            evt.dwEvt = 1;
            DoEvt(&evt);
            } // if
          break;

        default:
          pCmd->dwError = XERROR_NOT_FOUND;
          Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0900, "CGpiod::_systemDoCmd,unknown cmd %u", pCmd->dwCmd & CGPIOD_CMD_NUM_MASK);
          break;
        } // switch

      } while (FALSE);

    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x9999, "CGpiod::_systemDoCmd,err=%u", pCmd->dwError);
    return pCmd->dwError  ; 
    } // _systemDoCmd


