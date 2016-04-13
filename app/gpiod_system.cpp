
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
    tChar   str[32];
    tUint32 dwObj;

    m_dwEmul = AppSettings.gpiodEmul;
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0010, "CGpiod::_systemOnConfig,emul=%s", _printVal2String(str, m_dwEmul));

    m_dwMode = AppSettings.gpiodMode;
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0020, "CGpiod::_systemOnConfig,mode=%s", _printVal2String(str, m_dwMode));

    m_dwEfmt = AppSettings.gpiodEfmt;
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0030, "CGpiod::_systemOnConfig,efmt=%s", _printVal2String(str, m_dwEfmt));

    // configure heartbeat timers
    memset(m_hbeat, 0, sizeof(m_hbeat));
    for (dwObj = 0; dwObj < CGPIOD_HB_COUNT; dwObj++) {
      m_hbeat[dwObj].msPeriod = (dwObj == CGPIOD_HB0) ? CGPIOD_HB0_PERIOD : CGPIOD_HB0_PERIOD;
      } // for

    return m_dwError;
    } // _systemOnConfig

  //--------------------------------------------------------------------------
  // initialise
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemOnInit() 
  {
    return m_dwError;
    } // _systemOnInit

  //--------------------------------------------------------------------------
  // run outputs
  //--------------------------------------------------------------------------
  tUint32 CGpiod::timerExpired(tUint32 msNow, tUint32 msTimer) 
  {
    // handle clock wrap
    if ((msTimer > ESP8266_MILLIS_MID) && (msNow < ESP8266_MILLIS_MID))
      msNow += ESP8266_MILLIS_MAX;
    
    return (msNow > msTimer) ? 1 : 0;
    } //

  //--------------------------------------------------------------------------
  // run outputs
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemOnRun(tUint32 msNow) 
  {
    tUint32     dwObj;
    tGpiodHbeat *pObj = m_hbeat; 
    tGpiodEvt   evt = { msNow, 0, 0, 0, 0 };

    // handle heartbeat timers
    for (dwObj = 0; dwObj < CGPIOD_HB_COUNT; dwObj++, pObj++) {
      if (timerExpired(msNow, pObj->msStart + pObj->msPeriod)) {
        pObj->dwCntr++;
        pObj->msStart = msNow;
        evt.dwObj = CGPIOD_OBJ_CLS_HBEAT + dwObj;
        evt.dwEvt = (pObj->dwCntr & 1) ? CGPIOD_HB_EVT_ODD : CGPIOD_HB_EVT_EVEN;
        DoEvt(&evt);
        } // if
      } // for

    return m_dwError;
    } // CGpiod::_systemOnRun

  //--------------------------------------------------------------------------
  // leave outputs in decent state
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemOnExit() 
  {
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0000, "CGpiod::_systemOnExit");
    return m_dwError;
    } // CGpiod::_systemOnExit

  //--------------------------------------------------------------------------
  // execute command for outputs, called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemDoCmd(tGpiodCmd* pCmd) 
  { 
    tChar     str[32];
    tGpiodEvt evt = { pCmd->msNow, pCmd->dwObj, 0, 0, 0 };

    PrintCmd(pCmd, CLSLVL_GPIOD_SYSTEM | 0x0000, "CGpiod::_systemDoCmd");
    switch (pCmd->dwCmd & CGPIOD_OBJ_CMD_MASK) {
      case CGPIOD_SYS_CMD_VERSION: 
        evt.szTopic = "version";
        evt.szEvt   = CGPIOD_VERSION;
        DoEvt(&evt);
        break;

      case CGPIOD_SYS_CMD_MEMORY: 
        gsprintf(str, "%u", system_get_free_heap_size());
        evt.szTopic = "memory";
        evt.szEvt   = str;
        DoEvt(&evt);
        break;

      case CGPIOD_SYS_CMD_UPTIME: 
        break;

      case CGPIOD_SYS_CMD_EMUL:
        gsprintf(str, "%s", (m_dwEmul == CGPIOD_EMUL_OUTPUT) ? "output" : "shutter");
        evt.szTopic = "emul";
        evt.szEvt   = str;
        DoEvt(&evt);
        break;

      case CGPIOD_SYS_CMD_MODE: 
        gsprintf(str, "%s", (m_dwMode == CGPIOD_MODE_STANDALONE) ? "standalone" :
                            (m_dwMode == CGPIOD_MODE_MQTT)       ? "MQTT"       : "both");
        evt.szTopic = "mode";
        evt.szEvt   = str;
        DoEvt(&evt);
        break;

      case CGPIOD_SYS_CMD_EFMT: 
        gsprintf(str, "%s", (m_dwEfmt == CGPIOD_EFMT_NUMERICAL) ? "numerical" : "textual");
        evt.szTopic = "efmt";
        evt.szEvt   = str;
        DoEvt(&evt);
        break;

      case CGPIOD_SYS_CMD_DISABLE: 
        break;

      case CGPIOD_SYS_CMD_ENABLE: 
        break;

      case CGPIOD_SYS_CMD_REBOOT: 
        break;

      default:
        Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x9999, "CGpiod::_systemDoCmd,unknown cmd %u", pCmd->dwCmd);
        break;
      } // switch

    return XERROR_SUCCESS; 
    } // _systemDoCmd


