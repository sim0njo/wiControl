
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

//  Debug.println("CGpiod::_systemOnConfig");
    m_dwEmul = AppSettings.gpiodEmul;
    Debug.println("CGpiod::_systemOnConfig,emul=" + String(_printVal2String(str, m_dwEmul)));
    m_dwMode = AppSettings.gpiodMode;
    Debug.println("CGpiod::_systemOnConfig,mode=" + String(_printVal2String(str, m_dwMode)));
    m_dwEfmt = AppSettings.gpiodEfmt;
    Debug.println("CGpiod::_systemOnConfig,efmt=" + String(_printVal2String(str, m_dwEfmt)));

    // configure heartbeat timers
    memset(m_hbeat, 0, sizeof(m_hbeat));
    for (dwObj = 0; dwObj < CGPIOD_HB_COUNT; dwObj++) {
      m_hbeat[dwObj].msPeriod = (dwObj == CGPIOD_HB1) ? CGPIOD_HB1_PERIOD :
                                (dwObj == CGPIOD_HB2) ? CGPIOD_HB2_PERIOD : CGPIOD_HB0_PERIOD;
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
  tUint32 CGpiod::_systemOnRun(tUint32 msNow) 
  {
    tUint32     dwObj;
    tGpiodHbeat *pObj = m_hbeat; 
    tGpiodEvt   evt = { msNow, 0, 0, 0 };

    // handle heartbeat timers
    for (dwObj = 0; dwObj < CGPIOD_HB_COUNT; dwObj++, pObj++) {
      evt.dwObj = CGPIOD_OBJ_CLS_HBEAT + dwObj;

      if (msNow > (pObj->msStart + pObj->msPeriod)) {
        pObj->dwCntr++;
        pObj->msStart = msNow;
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
    return m_dwError;
    } // CGpiod::_systemOnExit

  //--------------------------------------------------------------------------
  // execute command for outputs, called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemDoCmd(tGpiodCmd* pCmd) 
  { 
    tChar     str[32];
    tGpiodEvt evt = { pCmd->msNow, pCmd->dwObj, 0, 0 };

    do {
      Debug.println("CGpiod::_systemDoCmd");
      switch (pCmd->dwCmd & CGPIOD_OBJ_CMD_MASK) {
        case CGPIOD_SYS_CMD_VERSION: 
          gsprintf(str, "version=%s", CGPIOD_VERSION);
          evt.szEvt = str;
          DoEvt(&evt);
          break;

        case CGPIOD_SYS_CMD_MEMORY: 
          gsprintf(str, "memory=%u", system_get_free_heap_size());
          evt.szEvt = str;
          DoEvt(&evt);
          break;

        case CGPIOD_SYS_CMD_UPTIME: 
          break;

        case CGPIOD_SYS_CMD_EMUL:
          gsprintf(str, "emul=%s", (m_dwEmul == CGPIOD_EMUL_OUTPUT) ? "output" : "shutter");
          evt.szEvt = str;
          DoEvt(&evt);
          break;

        case CGPIOD_SYS_CMD_MODE: 
          gsprintf(str, "mode=%s", (m_dwMode == CGPIOD_MODE_STANDALONE) ? "standalone" :
                                   (m_dwMode == CGPIOD_MODE_MQTT)       ? "MQTT"       : "both");
          evt.szEvt = str;
          DoEvt(&evt);
          break;

        case CGPIOD_SYS_CMD_EFMT: 
          gsprintf(str, "efmt=%s", (m_dwEfmt == CGPIOD_EFMT_NUMERICAL) ? "numerical" : "textual");
          evt.szEvt = str;
          DoEvt(&evt);
          break;

        case CGPIOD_SYS_CMD_DISABLE: 
          break;

        case CGPIOD_SYS_CMD_ENABLE: 
          break;

        case CGPIOD_SYS_CMD_REBOOT: 
          break;

        default:
//        g_log.LogPrt(m_dwClsLvl | 0x0099, "%s,dropping unknown cmd %u", pFunc, pCmd->dwCmd);
          break;
        } // switch

      } while (FALSE);

    return XERROR_SUCCESS; 
    } // _systemDoCmd


