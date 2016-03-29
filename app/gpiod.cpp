
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <gpiod.h>
#include <AppSettings.h>
#include <HTTP.h>

CGpiod               g_gpiod;

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void gpiodOnConfig(HttpRequest &request, HttpResponse &response)
{
  if (!g_http.isHttpClientAllowed(request, response))
    return;

  // handle new settings
  if (request.getRequestMethod() == RequestMethod::POST) {
    bool bModeChange =
     	AppSettings.gpiodMode != (request.getPostParameter("gpiodMode") == "1");

    AppSettings.gpiodMode = request.getPostParameter("gpiodMode") == "1";
    
//  if (bModeChange)
//
    } // if

  // send page
  TemplateFileStream *tmpl = new TemplateFileStream("gpiod.html");
  auto &vars = tmpl->variables();
  vars["appAlias"] = APP_ALIAS;

  bool gpiodMode     = AppSettings.gpiodMode;
  vars["gpiodMode0"] = gpiodMode ? "" : "checked='checked'";
  vars["gpiodMode1"] = gpiodMode ? "checked='checked'" : "";

  response.sendTemplate(tmpl); // will be automatically deleted
  } // gpiodOnConfig

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
String gpiodGetValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
      }
    } // for

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
  } // gpiodGetValue

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR gpiodOnPublish(String strTopic, String strMsg)
{
  tUint32   dwErr = XERROR_SUCCESS;
  tGpiodCmd cmd = { 0 };

  /*
   * Supported topics:
   *   /? => send version info
   *   <clientid>/<cmdpfx>/<object>, i.e. astr76b32/L3R1W2/cmd/shutter0=up.0.5
   */

  Debug.println("gpiodOnPublish,topic=" + strTopic + ",msg=" + strMsg);
  
  // check to be sure, but we should not receive other messages
  if (strTopic.startsWith(AppSettings.mqttClientId + String("/") + AppSettings.mqttCmdPfx + "/")) {
    g_dwMqttPktRx++;

    String strObj = gpiodGetValue(strTopic, '/', 3);
//  Debug.println();
//  Debug.println();
//  Debug.println(strObj);
//  Debug.println(strMsg);
    //strip leading V_
//    type.remove(0,2);

    cmd.dwObj = g_gpiod.ParseObj(strObj);
    switch (cmd.dwObj & CGPIOD_OBJ_CLS_MASK) {
//    case CGPIOD_OBJ_CLS_INPUT:
//      break;
      case CGPIOD_OBJ_CLS_OUTPUT:  dwErr = g_gpiod.ParseCmdOutput(&cmd, strMsg);
        break;
//    case CGPIOD_OBJ_CLS_SHUTTER:
//      break;
      case CGPIOD_OBJ_CLS_SYSTEM:  dwErr = g_gpiod.ParseCmdSystem(&cmd, strMsg);
        break;
      default:
        Debug.println("gpiodOnPublish, unknown object " + strObj);
        dwErr = XERROR_INPUT;
        break;
      } // switch
    }

    if (dwErr = XERROR_SUCCESS) {
//    dwErr = g_gpiod.DoCmd(&cmd);
      } // if

//  if (strTopic.equals(String("/?")))
//  {
//      mqttPublishVersion();
//      return;
//  }

  } // gpiodOnPublish

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::DoCmd(tGpiodCmd* pCmd) {
  tUint32 dwErr = XERROR_SUCCESS;

  do {
//  Debug.println("CGpiod::ParseCmdSystem,in=" + strIn);
//  String strCmd = gpiodGetValue(strIn, '.', 1);
//  Debug.println("CGpiod::ParseCmdSystem,cmd=" + strCmd);

    switch (pCmd->dwObj & CGPIOD_OBJ_CLS_MASK) {
//    case CGPIOD_OBJ_CLS_INPUT:
//      break;
      case CGPIOD_OBJ_CLS_OUTPUT:  dwErr = _outputDoCmd(pCmd);
        break;
//    case CGPIOD_OBJ_CLS_SHUTTER: dwErr = _shutterDoCmd(pCmd);
//      break;
//    case CGPIOD_OBJ_CLS_COUNTER: dwErr = _counterDoCmd(pCmd);
//      break;
//    case CGPIOD_OBJ_CLS_SYSTEM:  dwErr = _systemDoCmd(pCmd);
//      break;
      default:                     dwErr = XERROR_INPUT;
        break;
      } // switch

    } while (FALSE);

  return dwErr;
  } // DoCmd

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::ParseCmdSystem(tGpiodCmd* pOut, String strIn) {
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    Debug.println("CGpiod::ParseCmdSystem,in=" + strIn);
    String strCmd = gpiodGetValue(strIn, '.', 1);
    Debug.println("CGpiod::ParseCmdSystem,cmd=" + strCmd);

    // parse command
    if      (strCmd.equalsIgnoreCase(String("ping")))
      pOut->dwCmd = CGPIOD_SYS_CMD_PING;
    else if (strCmd.equalsIgnoreCase(String("mode")))
      pOut->dwCmd = CGPIOD_SYS_CMD_MODE;
    else if (strCmd.equalsIgnoreCase(String("version")))
      pOut->dwCmd = CGPIOD_SYS_CMD_VERSION;
    else {
      pOut->dwCmd = CGPIOD_SYS_CMD_PING;
      break;
      } // else

    } while (FALSE);

  return dwErr;
  } // ParseCmdSystem

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::ParseCmdOutput(tGpiodCmd* pOut, String strIn) {
  tUint32 dwErr = XERROR_SUCCESS;
  tChar   str[64];

  do {
    Debug.println("CGpiod::ParseCmdOutput,in=" + strIn);
    String strCmd = gpiodGetValue(strIn, '.', 0);
    Debug.println("CGpiod::ParseCmdOutput,cmd=" + strCmd);

    // parse command
    if      (strCmd.equalsIgnoreCase(String("on")))
      pOut->dwCmd = CGPIOD_OUT_CMD_ON;
    else if (strCmd.equalsIgnoreCase(String("off")))
      pOut->dwCmd = CGPIOD_OUT_CMD_OFF;
    else if (strCmd.equalsIgnoreCase(String("onlocked")))
      pOut->dwCmd = CGPIOD_OUT_CMD_ONLOCKED;
    else if (strCmd.equalsIgnoreCase(String("offlocked")))
      pOut->dwCmd = CGPIOD_OUT_CMD_OFFLOCKED;
    else if (strCmd.equalsIgnoreCase(String("toggle")))
      pOut->dwCmd = CGPIOD_OUT_CMD_TOGGLE;
    else if (strCmd.equalsIgnoreCase(String("unlock")))
      pOut->dwCmd = CGPIOD_OUT_CMD_UNLOCK;
    else if (strCmd.equalsIgnoreCase(String("ondelayed")))
      pOut->dwCmd = CGPIOD_OUT_CMD_ONDELAYED;
    else if (strCmd.equalsIgnoreCase(String("offdelayed")))
      pOut->dwCmd = CGPIOD_OUT_CMD_OFFDELAYED;
    else if (strCmd.equalsIgnoreCase(String("ontimed")))
      pOut->dwCmd = CGPIOD_OUT_CMD_ONTIMED;
    else if (strCmd.equalsIgnoreCase(String("offtimed")))
      pOut->dwCmd = CGPIOD_OUT_CMD_OFFTIMED;
    else if (strCmd.equalsIgnoreCase(String("toggledelayed")))
      pOut->dwCmd = CGPIOD_OUT_CMD_TOGGLEDELAYED;
    else if (strCmd.equalsIgnoreCase(String("toggletimed")))
      pOut->dwCmd = CGPIOD_OUT_CMD_TOGGLETIMED;
    else if (strCmd.equalsIgnoreCase(String("lock")))
      pOut->dwCmd = CGPIOD_OUT_CMD_LOCK;
    else if (strCmd.equalsIgnoreCase(String("locktimed")))
      pOut->dwCmd = CGPIOD_OUT_CMD_LOCKTIMED;
    else if (strCmd.equalsIgnoreCase(String("blink")))
      pOut->dwCmd = CGPIOD_OUT_CMD_BLINK;
    else {
      pOut->dwCmd = CGPIOD_OUT_CMD_NONE;
      break;
      } // else

    // parse params
    dwErr = ParseCmdParams(pOut, strIn);
    } while (FALSE);

  sprintf(str, "%08X", pOut->dwCmd);
  Debug.println("CGpiod::ParseCmdOutput,cmd=" + String(str));
  return dwErr;
  } // ParseCmdOutput

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
unsigned int CGpiod::ParseObj(String strObj) {
  unsigned int dwObj = CGPIOD_OBJ_NONE;

  if      (strObj.equalsIgnoreCase(String("system")))
    dwObj = CGPIOD_OBJ_SYSTEM;
  else if (strObj.equalsIgnoreCase(String("output0")))
    dwObj = CGPIOD_OBJ_OUTPUT0;
  else if (strObj.equalsIgnoreCase(String("output1")))
    dwObj = CGPIOD_OBJ_OUTPUT1;
  else if (strObj.equalsIgnoreCase(String("shutter0")))
    dwObj = CGPIOD_OBJ_SHUTTER0;

  return dwObj;
  } // ParseObj

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::ParseCmdParams(tGpiodCmd* pOut, String strIn) 
{
  tUint32 dwIdx = 1, dwErr = XERROR_SUCCESS;

  do {
    if ((pOut->dwCmd & CGPIOD_OUT_CMD_PARMS) == 0)
      break;
    
    if (pOut->dwCmd & 0x00010000) { // prio-mask 0-63
      if (dwErr = ParseCmdParamsGetNumber(&pOut->dwPrioMask, strIn, '.', dwIdx, 0, 63)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,prio-mask=%u", pFunc, pOut->dwPrioMask);
      } // if

    if (pOut->dwCmd & 0x00020000) { // prio-level 0-5
      if (dwErr = ParseCmdParamsGetNumber(&pOut->dwPrioLvl, strIn, '.', dwIdx, 0, 5)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,prio-lvl=%u", pFunc, pOut->dwPrioLvl);
      } // if

    if (pOut->dwCmd & 0x00040000) { // lock 0-1
      if (dwErr = ParseCmdParamsGetNumber(&pOut->dwLock, strIn, '.', dwIdx, 0, 1)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,lock=%u", pFunc, pOut->dwLock);
      } // if

    if (pOut->dwCmd & 0x00180000) { // delay 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = ParseCmdParamsGetNumber(&pOut->dwDelay, strIn, '.', dwIdx, 1, 65535)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,delay=%u", pFunc, pOut->dwDelay);
      } // if

//  if (pOut->dwCmd & 0x00600000) { // level 0-255
//    if (dwErr = ParseCmdParamsGetNumber(&pOut->dwLevel, strIn, '.', dwIdx, 0, 255)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,level=%u", pFunc, pOut->dwParm[0]);
//    } // if

    if (pOut->dwCmd & 0x01800000) { // runtime 1-160 seconds
      if (dwErr = ParseCmdParamsGetNumber(&pOut->dwRun, strIn, '.', dwIdx, 1, 160)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,run=%u", pFunc, pOut->dwRun);
      } // if

    if (pOut->dwCmd & 0x06000000) { // runtime 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = ParseCmdParamsGetNumber(&pOut->dwRun, strIn, '.', dwIdx, 1, 65535)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,run=%u", pFunc, pOut->dwRun);
      } // if

    if (pOut->dwCmd & 0x08000000) { // tiptime 1-65535 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = ParseCmdParamsGetNumber(&pOut->dwTip, strIn, '.', dwIdx, 1, 65535)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,tip=%u", pFunc, pOut->dwTip);
      } // if

    } while (FALSE);

  return dwErr;
  } // ParseCmdParams

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::ParseCmdParamsGetNumber(tUint32* pOut, String strIn, tChar cSep, tUint32 dwIdx, tUint32 dwMin, tUint32 dwMax)
{
/*
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = strIn.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (strIn.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
      }
    } // for

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
*/
  return 0;
  } // gpiodGetValue

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CGpiod::begin()
{
  checkTimer.initializeMs(1000, TimerDelegate(&CGpiod::checkConnection, this)).start(true);
  checkTimer.initializeMs(5000, TimerDelegate(&CGpiod::onRun, this)).start(true);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CGpiod::notifyChange(String object, String value)
{
  mqttPublishMessage(object, value);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CGpiod::registerHttpHandlers(HttpServer &server)
{
  mqttRegisterHttpHandlers(server);
  server.addPath("/gpiod", gpiodOnConfig);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CGpiod::registerCommandHandlers()
{
  //
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CGpiod::checkConnection()
{
  if (WifiStation.isConnected())
    mqttCheckClient();
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CGpiod::onRun()
{
  tUint32 dwObj, msNow = 0; // g_hrt.MsNow();
  Debug.println("CGpiod::onRun");

  // handle heartbeat timers
  for (dwObj = 0; dwObj < CGPIOD_HB_COUNT; dwObj++) {
    if (msNow > (m_hb[dwObj].msStart + m_hb[dwObj].msPeriod)) {
      m_hb[dwObj].dwCntr++;
      _outputOnHbTick(dwObj, m_hb[dwObj].dwCntr);
      m_hb[dwObj].msStart = msNow;
      } // if
    } // for

  if (m_dwMode == CGPIOD_MODE_OUTPUT)
    _outputOnRun(msNow);

  } // onRun

  //--------------------------------------------------------------------------
  // configure
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputOnConfig() {
    tUint32      dwObj;
    tGpiodOutput *pObj = m_output; 

    // initialise all channels
    memset(m_output, 0, sizeof(m_output)); 
    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      // initialise defaults
      pObj->dwFlags  = CGPIOD_OUT_FLG_MQTT_ALL | CGPIOD_OUT_FLG_STANDALONE;
      pObj->dwPin    = (dwObj == 0) ? CGPIOD_OUT0_PIN : CGPIOD_OUT1_PIN; 
      pObj->dwPol    = CGPIOD_IN_POL_NORMAL; 
      pObj->dwState  = CGPIOD_OUT_STATE_OFF; 
      pObj->dwCmd    = CGPIOD_OUT_CMD_NONE; 
      pObj->szName   = (dwObj == 0) ? "output0" : "output1";
      } // for

    return m_dwError;
    } // _outputOnConfig

  //--------------------------------------------------------------------------
  // run outputs
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputOnRun(tUint32 msNow) {
    tUint32      dwObj;
    tGpiodOutput *pObj = m_output; 
    tGpiodEvt    evt = { msNow, 0, 0, 0 };

    for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
      evt.dwObj = CGPIOD_OBJ_CLS_OUTPUT + dwObj;

      switch (pObj->dwCmd) {
        case CGPIOD_OUT_CMD_ONDELAYED:
        case CGPIOD_OUT_CMD_OFFTIMED:
          if (msNow >= pObj->dwRun) {
            _outputSetState(pObj, CGPIOD_OUT_STATE_ON, &evt);
            pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
            } // if
          break;

        case CGPIOD_OUT_CMD_OFFDELAYED:
        case CGPIOD_OUT_CMD_ONTIMED:
          if (msNow >= pObj->dwRun) {
            _outputSetState(pObj, CGPIOD_OUT_STATE_OFF, &evt);
            pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
            } // if
          break;

        case CGPIOD_OUT_CMD_TOGGLEDELAYED:
        case CGPIOD_OUT_CMD_TOGGLETIMED:
          if (msNow >= pObj->dwRun) {
            _outputSetState(pObj, pObj->dwState ^ CGPIOD_OUT_STATE_ON, &evt);
            pObj->dwCmd = CGPIOD_OUT_CMD_NONE;
            } // if
          break;

        case CGPIOD_OUT_CMD_LOCKTIMED:
          if (msNow >= pObj->dwRun) {
            pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
            pObj->dwFlags &= ~CGPIOD_OUT_FLG_LOCKED;
            } // if
          break;

        case CGPIOD_OUT_CMD_BLINK:
          // handled by _outputOnHbTick() for multi-channel in phase operation
          break;

        default:
          break;
        } // switch
      } // for

    return 0; // m_dwState;
    } // CGpiod::_outputOnRun

  //--------------------------------------------------------------------------
  // handle periodic 1s heartbeat timer tick
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputOnHbTick(tUint32 dwHb, tUint32 dwCntr) {
//  tCChar       *pFunc = "CGpiod::_outputOnHbTick";
    tUint32      dwObj;
    tGpiodOutput *pObj = m_output; 
    tGpiodEvt    evt = { 0, 0, 0, 0 };

    do {
      if (dwHb != CGPIOD_HB1) break;

      // handle each output object that requires a tick
      for (dwObj = 0; dwObj < CGPIOD_OUT_COUNT; dwObj++, pObj++) {
        evt.dwObj = CGPIOD_OBJ_CLS_OUTPUT | dwObj;

        if (pObj->dwCmd == CGPIOD_OUT_CMD_BLINK)
          _outputSetState(pObj, (dwCntr & 0x1) ? CGPIOD_OUT_STATE_ON : CGPIOD_OUT_STATE_OFF, &evt);
        } // for

      } while (FALSE);

    return m_dwError;
    } // CGpiod::_outputOnHbTick

  //--------------------------------------------------------------------------
  // local emulation handler, called by DoEvt()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputDoEvt(tGpiodEvt* pEvt) { 
//  tCChar    *pFunc = "CGpiod::_outputDoEvt";
    tGpiodCmd cmd = { pEvt->msNow, 0, 0, 0, 0, 0, 0 };

    switch (pEvt->dwEvt) {
      case CGPIOD_IN_EVT_OUTLT1:
        cmd.dwObj = CGPIOD_OBJ_CLS_OUTPUT | (pEvt->dwObj & CGPIOD_OBJ_NUM_MASK);
        cmd.dwCmd = CGPIOD_OUT_CMD_TOGGLE;
        _outputDoCmd(&cmd);
        break;

      case CGPIOD_IN_EVT_INGT2:
        cmd.dwObj = CGPIOD_OBJ_CLS_OUTPUT | (pEvt->dwObj & CGPIOD_OBJ_NUM_MASK);
        cmd.dwCmd = CGPIOD_OUT_CMD_BLINK;
        _outputDoCmd(&cmd);
        break;

      default:
        break;
      } // switch

    return m_dwError;
    } // CGpiod::_outputDoEvt

  //--------------------------------------------------------------------------
  // called by _DoCmd()
  //--------------------------------------------------------------------------
  tUint32 CGpiod::_outputDoCmd(tGpiodCmd* pCmd) { 
//  tCChar       *pFunc = "CGpiod::_outputDoCmd";
    tGpiodOutput *pObj  = &m_output[pCmd->dwObj & CGPIOD_OBJ_NUM_MASK]; 
    tGpiodEvt    evt    = { pCmd->msNow, pCmd->dwObj, 0, 0 };

    do {
      switch (pCmd->dwCmd & CGPIOD_OUT_CMD_MASK) {
        case CGPIOD_OUT_CMD_STATUS: 
//        g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,%s.status", pFunc, pObj->szName);
          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
//        _DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_ON: 
//        g_log.LogPrt(m_dwClsLvl | 0x0020, "%s,%s.on", pFunc, pObj->szName);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd   = CGPIOD_OUT_CMD_NONE;
          pObj->dwState = CGPIOD_OUT_STATE_ON;
//        _hwSetPinVal(pObj->dwPin, pObj->dwState ^ pObj->dwPol);

          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
//        _DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_OFF: 
//        g_log.LogPrt(m_dwClsLvl | 0x0030, "%s,%s.off", pFunc, pObj->szName);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd   = CGPIOD_OUT_CMD_NONE;
          pObj->dwState = CGPIOD_OUT_STATE_OFF;
//        _hwSetPinVal(pObj->dwPin, pObj->dwState ^ pObj->dwPol);

          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
//        _DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_ONLOCKED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0040, "%s,%s.onlocked", pFunc, pObj->szName);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          pObj->dwState  = CGPIOD_OUT_STATE_ON;
//        _hwSetPinVal(pObj->dwPin, pObj->dwState ^ pObj->dwPol);

          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
//        _DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_OFFLOCKED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0050, "%s,%s.offlocked", pFunc, pObj->szName);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          pObj->dwState  = CGPIOD_OUT_STATE_OFF;
//        _hwSetPinVal(pObj->dwPin, pObj->dwState ^ pObj->dwPol);

          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
//        _DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_TOGGLE: 
//        g_log.LogPrt(m_dwClsLvl | 0x0060, "%s,%s.toggle", pFunc, pObj->szName);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_NONE;
          pObj->dwState ^= CGPIOD_OUT_STATE_ON;
//        _hwSetPinVal(pObj->dwPin, pObj->dwState ^ pObj->dwPol);

          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
//        _DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_UNLOCK: 
//        g_log.LogPrt(m_dwClsLvl | 0x0070, "%s,%s.unlock", pFunc, pObj->szName);
          pObj->dwFlags &= ~CGPIOD_OUT_FLG_LOCKED;
          break;

        case CGPIOD_OUT_CMD_ONDELAYED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0080, "%s,%s.ondelayed.%u", pFunc, pObj->szName, pCmd->dwDelay);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_ONDELAYED;
          pObj->dwRun = pCmd->msNow + pCmd->dwDelay;
          break;

        case CGPIOD_OUT_CMD_OFFDELAYED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0090, "%s,%s.offdelayed.%u", pFunc, pObj->szName, pCmd->dwDelay);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_OFFDELAYED;
          pObj->dwRun = pCmd->msNow + pCmd->dwDelay;
          break;

        case CGPIOD_OUT_CMD_ONTIMED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0100, "%s,%s.ontimed.%u", pFunc, pObj->szName, pCmd->dwRun);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd   = CGPIOD_OUT_CMD_ONTIMED;
          pObj->dwRun   = pCmd->msNow + pCmd->dwRun;
          pObj->dwState = CGPIOD_OUT_STATE_ON;
//        _hwSetPinVal(pObj->dwPin, pObj->dwState ^ pObj->dwPol);

          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
//        _DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_OFFTIMED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0110, "%s,%s.offtimed.%u", pFunc, pObj->szName, pCmd->dwRun);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd   = CGPIOD_OUT_CMD_OFFTIMED;
          pObj->dwRun   = pCmd->msNow + pCmd->dwRun;
          pObj->dwState = CGPIOD_OUT_STATE_OFF;
//        _hwSetPinVal(pObj->dwPin, pObj->dwState ^ pObj->dwPol);

          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
//        _DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_TOGGLEDELAYED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0120, "%s,%s.toggledelayed.%u", pFunc, pObj->szName, pCmd->dwDelay);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd = CGPIOD_OUT_CMD_TOGGLEDELAYED;
          pObj->dwRun = pCmd->msNow + pCmd->dwDelay;
          break;

        case CGPIOD_OUT_CMD_TOGGLETIMED: 
//        g_log.LogPrt(m_dwClsLvl | 0x0130, "%s,%s.toggletimed.%u", pFunc, pObj->szName, pCmd->dwRun);
          if (pObj->dwFlags & CGPIOD_OUT_FLG_LOCKED) break;
          pObj->dwCmd    = CGPIOD_OUT_CMD_TOGGLETIMED;
          pObj->dwRun    = pCmd->msNow + pCmd->dwRun;
          pObj->dwState ^= CGPIOD_OUT_STATE_ON;
//        _hwSetPinVal(pObj->dwPin, pObj->dwState ^ pObj->dwPol);

          evt.dwEvt = pObj->dwState ? CGPIOD_OUT_EVT_ON : CGPIOD_OUT_EVT_OFF;
//        _DoEvt(&evt);
          break;

        case CGPIOD_OUT_CMD_LOCK: 
//        g_log.LogPrt(m_dwClsLvl | 0x0140, "%s,%s.lock", pFunc, pObj->szName);
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          break;

        case CGPIOD_OUT_CMD_LOCKTIMED:
//        g_log.LogPrt(m_dwClsLvl | 0x0150, "%s,%s.locktimed.%u", pFunc, pObj->szName, pCmd->dwRun);
          pObj->dwFlags |= CGPIOD_OUT_FLG_LOCKED;
          pObj->dwCmd    = CGPIOD_OUT_CMD_LOCKTIMED;
          pObj->dwRun    = pCmd->msNow + pCmd->dwRun;
          break;

        case CGPIOD_OUT_CMD_BLINK: 
//        g_log.LogPrt(m_dwClsLvl | 0x0160, "%s,%s.blink", pFunc, pObj->szName);
          pObj->dwCmd    = CGPIOD_OUT_CMD_BLINK;
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
  void CGpiod::_outputSetState(tGpiodOutput* pObj, tUint32 dwState, tGpiodEvt* pEvt) {
//  tCChar *pFunc = "CGpiod::_outputSetState";

    // exit if no state change
//  g_log.LogPrt(m_dwClsLvl | 0x0000, "%s,old=%u,new=%u", pFunc, pObj->dwState, dwState);
    if (pObj->dwState == dwState)
      return;

    // pObj->dwState dwState evt 
    // off           on      on
    // on            off     off
    switch (pObj->dwState = dwState) {
      case CGPIOD_OUT_STATE_OFF:
//      _hwSetPinVal(pObj->dwPin, CGPIOD_OUT_STATE_OFF ^ pObj->dwPol);
        pEvt->dwEvt = CGPIOD_OUT_EVT_OFF;
        break;

      case CGPIOD_OUT_STATE_ON:
//      _hwSetPinVal(pObj->dwPin, CGPIOD_OUT_STATE_ON ^ pObj->dwPol);
        pEvt->dwEvt = CGPIOD_OUT_EVT_ON;
        break;
      } // switch

//  _DoEvt(pEvt);
    } // _outputSetState

