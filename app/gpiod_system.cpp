
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
    tGpiodEvt    evt = { msNow, 0, 0, 0, 0 };

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
        // handled by _outputDoEvt() for multi-channel in phase operation
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
    tGpiodEvt evt = { pCmd->msNow, pCmd->dwObj, 0, 0, 0 };
    tGpiodCmd cmd = { 0 };

    PrintCmd(pCmd, CLSLVL_GPIOD_SYSTEM | 0x0000, "CGpiod::_systemDoCmd");
    switch (pCmd->dwCmd & CGPIOD_CMD_NUM_MASK) {
      case CGPIOD_SYS_CMD_PING: // blink led @ D8 for x seconds
        m_led.dwCmd = CGPIOD_LED_CMD_BLINKTIMED;
        m_led.dwRun = pCmd->msNow + CGPIOD_LED_BLINKTIME;
        _outputSetState(&m_led, m_led.dwState ^ CGPIOD_OUT_STATE_ON, 0);
        break;

      case CGPIOD_SYS_CMD_VERSION: 
        evt.szTopic = "version";
        evt.szEvt   = CGPIOD_VERSION;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_MEMORY: 
        gsprintf(str, "%u", system_get_free_heap_size());
        evt.szTopic = "memory";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_UPTIME: 
        break;

      case CGPIOD_SYS_CMD_LOGLEVEL:
        if ((pCmd->dwParms & 0x00010000) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK))
          Debug.logClsLevels(DEBUG_CLS_0, pCmd->parmsSystem.dwParm);

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u",     Debug.logClsLevels(DEBUG_CLS_0));
        else
          gsprintf(str, "0x%08X", Debug.logClsLevels(DEBUG_CLS_0));

        evt.szTopic = "loglevel";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_EMUL:
        if ((pCmd->dwParms & 0x00020000) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK)) {
          Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0100, "CGpiod::_systemDoCmd,set emul to %u", pCmd->parmsSystem.dwParm);
          AppSettings.gpiodEmul = pCmd->parmsSystem.dwParm;
          AppSettings.save();
//        m_dwEmul = pCmd->parmsSystem.dwParm;
          OnConfig();
          OnInit();
          } // if

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u",  m_dwEmul);
        else
          gsprintf(str, "%s", (m_dwEmul == CGPIOD_EMUL_OUTPUT) ? "output" : "shutter");

        evt.szTopic = "emul";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_MODE: 
        if ((pCmd->dwParms & 0x00040000) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK)) {
          Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0100, "CGpiod::_systemDoCmd,set mode to %u", pCmd->parmsSystem.dwParm);
          AppSettings.gpiodMode = pCmd->parmsSystem.dwParm;
          AppSettings.save();
          m_dwMode = pCmd->parmsSystem.dwParm;
          } // if

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u",  m_dwMode);
        else
          gsprintf(str, "%s", (m_dwMode == CGPIOD_MODE_STANDALONE) ? "standalone" :
                              (m_dwMode == CGPIOD_MODE_MQTT)       ? "MQTT"       : "both");

        evt.szTopic = "mode";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_EFMT: 
        if ((pCmd->dwParms & 0x00080000) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK)) {
          Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0100, "CGpiod::_systemDoCmd,set efmt to %u", pCmd->parmsSystem.dwParm);
          AppSettings.gpiodEfmt = pCmd->parmsSystem.dwParm;
          AppSettings.save();
          m_dwEfmt = pCmd->parmsSystem.dwParm;
          } // if

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u",  m_dwEfmt);
        else
          gsprintf(str, "%s", (m_dwEfmt == CGPIOD_EFMT_NUMERICAL) ? "numerical" : "textual");

        evt.szTopic = "efmt";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_LOCK:
        if (pCmd->dwParms & 0x00100000)
          pCmd->parmsSystem.dwParm ? _SetFlags(&m_dwFlags, CGPIOD_FLG_LOCK) : _RstFlags(&m_dwFlags, CGPIOD_FLG_LOCK); 

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u", _GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK) ? 1 : 0);
        else
          gsprintf(str, "%s", _GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK) ? "on" : "off");

        evt.szTopic = "lock";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_DISABLE: 
        if ((pCmd->dwParms & 0x00100000) && !_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK)) 
          pCmd->parmsSystem.dwParm ? _SetFlags(&m_dwFlags, CGPIOD_FLG_DISABLE) : _RstFlags(&m_dwFlags, CGPIOD_FLG_DISABLE); 

        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          gsprintf(str, "%u", _GetFlags(&m_dwFlags, CGPIOD_FLG_DISABLE) ? 1 : 0);
        else
          gsprintf(str, "%s", _GetFlags(&m_dwFlags, CGPIOD_FLG_DISABLE) ? "on" : "off");

        evt.szTopic = "disable";
        evt.szEvt   = str;
        DoSta(&evt);
        break;

      case CGPIOD_SYS_CMD_RESTART: 
        if (!_GetFlags(&m_dwFlags, CGPIOD_FLG_LOCK)) 
          System.restart();
        break;

      default:
        Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x9999, "CGpiod::_systemDoCmd,unknown cmd %u", pCmd->dwCmd);
        break;
      } // switch

    return XERROR_SUCCESS; 
    } // _systemDoCmd


