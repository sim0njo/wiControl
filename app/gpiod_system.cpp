
//----------------------------------------------------------------------------
// cgpiod_system.hpp : implementation system objects
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <AppSettings.h>
#include <gpiod.h>

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemOnConfig() 
  {
    m_dwFlags = CGPIOD_FLG_NONE;

    m_dwEmul  = AppSettings.gpiodEmul;
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0010, "CGpiod::_systemOnConfig,emul=%u", m_dwEmul);

    m_dwMode  = AppSettings.gpiodMode;
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0020, "CGpiod::_systemOnConfig,mode=%u", m_dwMode);

    m_dwEfmt  = AppSettings.gpiodEfmt;
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0030, "CGpiod::_systemOnConfig,efmt=%u", m_dwEfmt);

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
    if ((pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) == CGPIOD_OBJ_CLS_HBEAT) {
      
      // handle 1 second heartbeat for synchronous blinking leds
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

    PrintCmd(pCmd, CLSLVL_GPIOD_SYSTEM | 0x0000, "CGpiod::_systemDoCmd");
    switch (pCmd->dwCmd & CGPIOD_CMD_NUM_MASK) {
      case CGPIOD_SYS_CMD_PING: // blink led @ D8 for x seconds
        m_led.dwCmd = CGPIOD_LED_CMD_BLINKTIMED;
        m_led.dwRun = pCmd->msNow + CGPIOD_LED_BLINKTIME;
        _outputSetState(&m_led, m_led.dwState ^ CGPIOD_OUT_STATE_ON, 0);
        pCmd->dwRsp = 1;
        break;

      case CGPIOD_SYS_CMD_VERSION: 
        // report current value
        pCmd->dwRsp = APP_VERSION;

        evt.szTopic = "version";
        evt.szEvt   = szAPP_VERSION;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_MEMORY: 
        // report current value
        pCmd->dwRsp = system_get_free_heap_size();

        gsprintf(str, "%u", pCmd->dwRsp);
        evt.szTopic = "memory";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_UPTIME: 
        // report current value
        pCmd->dwRsp = pCmd->msNow;

        evt.szTopic = "uptime";
        gsprintf(str, "%u", pCmd->dwRsp);
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_LOGLEVEL:
        // handle set command
        if ((pCmd->dwParms & CGPIOD_SYS_PRM_LOGLEVEL) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK))
          Debug.logClsLevels(DEBUG_CLS_0, pCmd->parmsSystem.dwParm);

        // report current value
        pCmd->dwRsp = Debug.logClsLevels(DEBUG_CLS_0);

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u",     pCmd->dwRsp);
        else
          gsprintf(str, "0x%08X", pCmd->dwRsp);

        evt.szTopic = "loglevel";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_EMUL:
        // handle set command
        if ((pCmd->dwParms & CGPIOD_SYS_PRM_EMUL) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK)) {
          Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0100, "CGpiod::_systemDoCmd,set emul to %u", pCmd->parmsSystem.dwParm);
          
          AppSettings.gpiodEmul = m_dwEmul = pCmd->parmsSystem.dwParm;
          AppSettings.save();
          OnInit();
          } // if

        // report current value
        pCmd->dwRsp = m_dwEmul;

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u",  pCmd->dwRsp);
        else
          gsprintf(str, "%s", (pCmd->dwRsp == CGPIOD_EMUL_OUTPUT) ? "output" : "shutter");

        evt.szTopic = "emul";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_MODE: 
        // handle set command
        if ((pCmd->dwParms & CGPIOD_SYS_PRM_MODE) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK)) {
          Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0100, "CGpiod::_systemDoCmd,set mode to %u", pCmd->parmsSystem.dwParm);
          AppSettings.gpiodMode = m_dwMode = pCmd->parmsSystem.dwParm;
          AppSettings.save();
          m_dwMode = pCmd->parmsSystem.dwParm;
          } // if

        // report current value
        pCmd->dwRsp = m_dwMode;

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u",  pCmd->dwRsp);
        else
          gsprintf(str, "%s", (pCmd->dwRsp == CGPIOD_MODE_STANDALONE) ? "standalone" :
                              (pCmd->dwRsp == CGPIOD_MODE_MQTT)       ? "MQTT"       : "both");

        evt.szTopic = "mode";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_EFMT: 
        // handle set command
        if ((pCmd->dwParms & CGPIOD_SYS_PRM_EFMT) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK)) {
          Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0100, "CGpiod::_systemDoCmd,set efmt to %u", pCmd->parmsSystem.dwParm);
          AppSettings.gpiodEfmt = m_dwEfmt = pCmd->parmsSystem.dwParm;
          AppSettings.save();
          m_dwEfmt = pCmd->parmsSystem.dwParm;
          } // if

        // report current value
        pCmd->dwRsp = m_dwEfmt;

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u",  pCmd->dwRsp);
        else
          gsprintf(str, "%s", (pCmd->dwRsp == CGPIOD_EFMT_NUMERICAL) ? "numerical" : "textual");

        evt.szTopic = "efmt";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_LOCK:
        // handle set command
        if (pCmd->dwParms & CGPIOD_SYS_PRM_OFFON)
          pCmd->parmsSystem.dwParm ? _SetFlags(&m_dwFlags, CGPIOD_FLG_LOCK) : _RstFlags(&m_dwFlags, CGPIOD_FLG_LOCK); 

        // report current value
        pCmd->dwRsp = _GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK) ? 1 : 0;

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u", pCmd->dwRsp);
        else
          gsprintf(str, "%s", pCmd->dwRsp ? "on" : "off");

        evt.szTopic = "lock";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_DISABLE: 
        // handle command
        if ((pCmd->dwParms & CGPIOD_SYS_PRM_OFFON) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK)) 
          pCmd->parmsSystem.dwParm ? _SetFlags(&m_dwFlags, CGPIOD_FLG_DISABLE) : _RstFlags(&m_dwFlags, CGPIOD_FLG_DISABLE); 

        // report current value
        pCmd->dwRsp = _GetFlags(&m_dwFlags, CGPIOD_FLG_DISABLE) ? 1 : 0;

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u", pCmd->dwRsp);
        else
          gsprintf(str, "%s", pCmd->dwRsp ? "on" : "off");

        evt.szTopic = "disable";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_RESTART:
        // handle command
        if ((pCmd->dwParms & CGPIOD_SYS_PRM_ACK) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK)) {
          evt.szTopic = "restart";
          evt.szEvt   = "1";
          DoEvt(&evt);

          System.restart();
          } // if

        break;

      default:
        Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x9999, "CGpiod::_systemDoCmd,unknown cmd %u", pCmd->dwCmd);
        break;
      } // switch

    return XERROR_SUCCESS; 
    } // _systemDoCmd


