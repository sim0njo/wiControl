
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
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0010, "CGpiod::_systemOnConfig,emul   =%u", g_appCfg.gpiodEmul);
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0020, "CGpiod::_systemOnConfig,mode   =%u", g_appCfg.gpiodMode);
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0030, "CGpiod::_systemOnConfig,lock   =%u", g_appCfg.gpiodLock);
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0040, "CGpiod::_systemOnConfig,disable=%u", g_appCfg.gpiodDisable);

    // configure heartbeat timer
    memset(&m_hbeat, 0, sizeof(m_hbeat));
    m_hbeat.msPeriod = CGPIOD_HB_PERIOD;
    return m_dwError;
    } // _systemOnConfig

  //--------------------------------------------------------------------------
  // initialise
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemOnInit() 
  {
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0000, "CGpiod::_systemOnInit");
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

    return m_dwError;
    } // CGpiod::_systemOnRun

  //--------------------------------------------------------------------------
  // leave outputs in decent state
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemOnExit() 
  {
    // set outputs to off and switch to input
    Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0000, "CGpiod::_systemOnExit");
    return m_dwError;
    } // CGpiod::_systemOnExit

  //--------------------------------------------------------------------------
  // local emulation handler, called by DoEvt()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemDoEvt(tGpiodEvt* pEvt) 
  { 
    return m_dwError;
    } // CGpiod::_systemDoEvt

  //--------------------------------------------------------------------------
  // execute command for outputs, called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_systemDoCmd(tGpiodCmd* pCmd) 
  { 
    tChar     str[40];
    tUint8    byMac[8];
    tGpiodEvt evt = { pCmd->msNow, pCmd->dwOrig, pCmd->dwObj, 0, 0, 0 };
    tGpiodCmd cmd = { 0 };

    do {
      switch (pCmd->dwCmd & CGPIOD_CMD_NUM_MASK) {
        case CGPIOD_SYS_CMD_PING: // blink led @ D8 for x seconds
          pCmd->dwRsp = 1;

          evt.szObj = "pong";
          wifi_get_macaddr(0x0, byMac);
          gsprintf(str, "%s,%02X:%02X:%02X:%02X:%02X:%02X",
                   Network.getClientAddr().toString().c_str(), byMac[0], byMac[1], byMac[2], byMac[3], byMac[4], byMac[5]);
//        gsprintf(str, "%s;%06X", Network.getClientAddr().toString().c_str(), system_get_chip_id());
          evt.szEvt = str;
          DoEvt(&evt);
          break;

        case CGPIOD_SYS_CMD_VERSION: 
          // report current value
          pCmd->dwRsp = APP_VERSION;

          gsprintf(str, "%s;%s", szAPP_VERSION, szAPP_TOPOLOGY);
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
          if ((pCmd->dwParms == CGPIOD_SYS_PRM_LOGLEVEL) && !g_appCfg.gpiodLock)
            Debug.logClsLevels(DEBUG_CLS_0, pCmd->parmsSystem.dwParm);

          // report current value
          pCmd->dwRsp = Debug.logClsLevels(DEBUG_CLS_0);
          evt.dwEvt   = Debug.logClsLevels(DEBUG_CLS_0);
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_EMUL:
          // handle set command
          if ((pCmd->dwParms == CGPIOD_SYS_PRM_EMUL) && !g_appCfg.gpiodLock) {
            Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0100, "CGpiod::_systemDoCmd,set emul to %u", pCmd->parmsSystem.dwParm);
          
            g_appCfg.gpiodEmul = pCmd->parmsSystem.dwParm;
            OnInit();
            } // if

          // report current value
          pCmd->dwRsp = g_appCfg.gpiodEmul;
          evt.dwEvt   = g_appCfg.gpiodEmul;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_MODE: 
          // handle set command
          if ((pCmd->dwParms == CGPIOD_SYS_PRM_MODE) && !g_appCfg.gpiodLock) {
            Debug.logTxt(CLSLVL_GPIOD_SYSTEM | 0x0100, "CGpiod::_systemDoCmd,set mode to %u", pCmd->parmsSystem.dwParm);
            g_appCfg.gpiodMode = pCmd->parmsSystem.dwParm;
            } // if

          // report current value
          pCmd->dwRsp = g_appCfg.gpiodMode;
          evt.dwEvt   = g_appCfg.gpiodMode;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_LOCK:
          // handle set command
          if (pCmd->dwParms == CGPIOD_SYS_PRM_OFFON) {
            g_appCfg.gpiodLock = pCmd->parmsSystem.dwParm ? 1 : 0;
            } // if

          // report current value
          pCmd->dwRsp = g_appCfg.gpiodLock;
          evt.dwEvt   = g_appCfg.gpiodLock;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_DISABLE: 
          // handle command
          if ((pCmd->dwParms == CGPIOD_SYS_PRM_OFFON) && !g_appCfg.gpiodLock) {
            g_appCfg.gpiodDisable = pCmd->parmsSystem.dwParm ? 1 : 0;
            } // if

          // report current value
          pCmd->dwRsp = g_appCfg.gpiodDisable;
          evt.dwEvt   = g_appCfg.gpiodDisable;
          DoSta(&evt);
          break;

        case CGPIOD_SYS_CMD_RESTART:
          // handle command
          if ((pCmd->dwParms == CGPIOD_SYS_PRM_ACK) && (g_appCfg.gpiodLock == CGPIOD_LOCK_FALSE)) {
            evt.dwEvt = 1;
            DoEvt(&evt);

            g_app.Restart(0);
            } // if

          break;

        case CGPIOD_SYS_CMD_SAVE:
          if (pCmd->dwParms == CGPIOD_SYS_PRM_ACK) {
            g_appCfg.save();

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


