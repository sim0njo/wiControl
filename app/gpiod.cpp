
#include <SmingCore/SmingCore.h>
#include <AppSettings.h>
#include <HTTP.h>
#include <gpiod.h>

CGpiod               g_gpiod;

//----------------------------------------------------------------------------
// HTTP config callback
//----------------------------------------------------------------------------
void gpiodOnHttpConfig(HttpRequest &request, HttpResponse &response)
{
  if (!g_http.isHttpClientAllowed(request, response))
    return;

  // handle new settings
  if (request.getRequestMethod() == RequestMethod::POST) {
    bool bEmulChange =
     	AppSettings.gpiodEmul != (request.getPostParameter("gpiodEmul") == "1");

    AppSettings.gpiodEmul = request.getPostParameter("gpiodEmul") == "1";
    AppSettings.gpiodMode = request.getPostParameter("gpiodMode") == "1";
    
    AppSettings.save();

    if (bEmulChange) {
      g_gpiod.OnConfig();
      g_gpiod.OnInit();
      } // if
    } // if

  // send page
  TemplateFileStream *tmpl = new TemplateFileStream("gpiod.html");
  auto &vars = tmpl->variables();
  vars["appAlias"] = APP_ALIAS;

  bool gpiodEmul     = AppSettings.gpiodEmul;
  vars["gpiodEmul0"] = gpiodEmul ? "" : "checked='checked'";
  vars["gpiodEmul1"] = gpiodEmul ? "checked='checked'" : "";

  bool gpiodMode     = AppSettings.gpiodMode;
  vars["gpiodMode0"] = gpiodMode ? "" : "checked='checked'";
  vars["gpiodMode1"] = gpiodMode ? "checked='checked'" : "";

  response.sendTemplate(tmpl); // will be automatically deleted
  } // gpiodOnHttpConfig

//----------------------------------------------------------------------------
// MQTT client callback
// <clientid>/<cmdpfx>/<object>, i.e. astr76b32/L3R1W2/cmd/shutter0=up.0.5
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR gpiodOnMqttPublish(String strTopic, String strMsg)
{
  tUint32   dwErr = XERROR_SUCCESS;
  tUint32   msNow = millis();
  tChar     str[64], *pTopic = xstrdup(strTopic.c_str()), *pMsg = xstrdup(strMsg.c_str());
  tGpiodCmd cmd = { 0 };

  do {
    Debug.println("gpiodOnPublish,topic=" + strTopic + ",msg=" + strMsg);
  
    // check to be sure, but we should not receive other messages
    gsprintf(str, "%s/%s/", AppSettings.mqttClientId.c_str(), AppSettings.mqttCmdPfx.c_str());
    if (xstrnicmp(pTopic, str, gstrlen(str)) && (dwErr = XERROR_DATA)) {
      Debug.println("gpiodOnPublish,not for us,dropping");
      break;
      } // if

    // point to <obj>
    pTopic += gstrlen(str);
    g_dwMqttPktRx++;

    // parse object, cmd and optional parms
    if (g_gpiod.ParseCmd(&cmd, pTopic, pMsg, (g_gpiod.GetEmul() == CGPIOD_EMUL_OUTPUT) ? CGPIOD_OBJ_CLS_OUTPUT : CGPIOD_OBJ_CLS_SHUTTER)) {
      Debug.println("gpiodOnPublish,ParseCmd() failed,dropping");
      dwErr = XERROR_DATA;
      break;
      } // if

    cmd.msNow = msNow;
    dwErr = g_gpiod.DoCmd(&cmd);
    } while (FALSE);

  if (pTopic) free(pTopic);
  if (pMsg) free(pMsg);
  } // gpiodOnMqttPublish

//--------------------------------------------------------------------------
// configure daemon
//--------------------------------------------------------------------------
tUint32 CGpiod::OnConfig()
{
  tUint32 dwObj;

  do {
    m_dwEmul = AppSettings.gpiodEmul;
    m_dwMode = AppSettings.gpiodMode;

    // configure heartbeat timers
    Debug.println("CGpiod::OnConfig");
    memset(m_hb, 0, sizeof(m_hb));
    for (dwObj = 0; dwObj < CGPIOD_HB_COUNT; dwObj++) {
      m_hb[dwObj].dwFlags  = CGPIOD_CNT_FLG_CNTR;
      m_hb[dwObj].msPeriod = (dwObj == CGPIOD_HB1) ? CGPIOD_HB1_PERIOD :
                             (dwObj == CGPIOD_HB2) ? CGPIOD_HB2_PERIOD : CGPIOD_HB0_PERIOD;
      } // for

    // configure inputs
    _inputOnConfig();

    // configure outputs
    if (m_dwEmul == CGPIOD_EMUL_OUTPUT)
      _outputOnConfig();
    else
      _shutterOnConfig();
    } while (FALSE);

  } // OnConfig

//--------------------------------------------------------------------------
// initialise daemon
//--------------------------------------------------------------------------
tUint32 CGpiod::OnInit() 
{

  Debug.println("CGpiod::OnInit");

  // initialise inputs
  _inputOnInit();

  // initialise outputs
  if (m_dwEmul == CGPIOD_EMUL_OUTPUT)
    _outputOnInit();
  else
    _shutterOnInit();

  return m_dwError;
  } // OnInit

//--------------------------------------------------------------------------
// run daemon
//--------------------------------------------------------------------------
void CGpiod::OnRun() {
  tUint32 dwObj, msNow = millis();

  do {
    // handle heartbeat timers
    for (dwObj = 0; dwObj < CGPIOD_HB_COUNT; dwObj++) {
      if (msNow > (m_hb[dwObj].msStart + m_hb[dwObj].msPeriod)) {
        m_hb[dwObj].dwCntr++;
        _outputOnHbTick(dwObj, m_hb[dwObj].dwCntr);
        m_hb[dwObj].msStart = msNow;
        } // if
      } // for

    _inputOnRun(msNow);

    if (m_dwEmul == CGPIOD_EMUL_OUTPUT)
      _outputOnRun(msNow);
    else
      _shutterOnRun(msNow);
    } while (FALSE);

  } // OnRun

//--------------------------------------------------------------------------
// exit daemon
//--------------------------------------------------------------------------
tUint32 CGpiod::OnExit() {
//  tChar szTopic[64];

//  gsprintf(szTopic, "%s/%s/#", g_mqttcd.GetStrAttr("mqttcd.clientid"), m_szCmdPfx);

  if (m_dwEmul == CGPIOD_EMUL_OUTPUT)
    _outputOnExit();
  else
    _shutterOnExit();

  _inputOnExit();
//_systemOnExit();
  } // OnExit

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::DumpCmd(tGpiodCmd* pCmd) {
  tChar     str[64];

  sprintf(str, "%08X", pCmd->dwObj);
  Debug.println("CGpiod::DumpCmd,obj=" + String(str));
  sprintf(str, "%08X", pCmd->dwCmd);
  Debug.println("CGpiod::DumpCmd,cmd=" + String(str));
  } // DumpCmd

//--------------------------------------------------------------------------
// handle event, dwObj = 0x----TTOO, dwEvt = 0|1-n, szEvt = 0 | "str"
//--------------------------------------------------------------------------
tUint32 CGpiod::DoEvt(tGpiodEvt* pEvt) 
{
  tUint32 dwObj = pEvt->dwObj & CGPIOD_OBJ_NUM_MASK;

//g_log.LogPrt(m_dwClsLvl | 0x0000, "%s,now=%u,obj=%04X,evt=%u/'%s'", 
//             pFunc, pEvt->msNow, pEvt->dwObj, pEvt->dwEvt, pEvt->szEvt ? pEvt->szEvt : "");

  Debug.println("CGpiod::DoEvt");
  switch (pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) {
    case CGPIOD_OBJ_CLS_INPUT:
      if      (m_dwMode == CGPIOD_MODE_STANDALONE) { 
        if (m_dwEmul == CGPIOD_EMUL_OUTPUT) 
          _outputDoEvt(pEvt);
        else 
          _shutterDoEvt(pEvt);
        } // if

      else if (m_input[dwObj].dwFlags & (0x1 << pEvt->dwEvt))
        _DoPublish(0, 0, 0, m_input[dwObj].szName, _inputEvt2String(pEvt->dwEvt));

      break;
    case CGPIOD_OBJ_CLS_OUTPUT:
      if (m_output[dwObj].dwFlags & (0x1 << pEvt->dwEvt))
        _DoPublish(0, 0, 0, m_output[dwObj].szName, _outputEvt2String(pEvt->dwEvt));

      break;
    case CGPIOD_OBJ_CLS_SHUTTER:
      if (m_shutter[dwObj].dwFlags & (0x1 << pEvt->dwEvt))
        _DoPublish(0, 0, 0, m_shutter[dwObj].szName, _shutterEvt2String(pEvt->dwEvt));

      break;
//  case CGPIOD_OBJ_CLS_COUNTER:
//    if (m_counter[dwObj].dwFlags & (0x1 << pEvt->dwEvt))
//      _DoPublish(0, 0, 0, m_counter[dwObj].szName, _counterEvt2String(pEvt->dwEvt));

//    break;
//  case CGPIOD_OBJ_CLS_SYSTEM:
//    if (pEvt->szEvt)
//      _DoPublish(0, 0, 0, "system", pEvt->szEvt);

//    if      ((dwObj & 0xFF) == CGPIOD_OBJ_SYS_VERSION)
//      _DoPublish(0, 0, 0, "version", szEvt);
//    else if ((dwObj & 0xFF) == CGPIOD_OBJ_SYS_CAPS)
//      _DoPublish(0, 0, 0, "caps", szEvt);
//    break;
    } // switch

  return XERROR_SUCCESS;
  } // CGpiod::DoEvt

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::DoCmd(tGpiodCmd* pCmd) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    Debug.println("CGpiod::DoCmd");
    DumpCmd(pCmd);

    switch (pCmd->dwObj & CGPIOD_OBJ_CLS_MASK) {
//    case CGPIOD_OBJ_CLS_INPUT:
//      break;
      case CGPIOD_OBJ_CLS_OUTPUT:  dwErr = _outputDoCmd(pCmd);
        break;
      case CGPIOD_OBJ_CLS_SHUTTER: dwErr = _shutterDoCmd(pCmd);
        break;
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
void CGpiod::_DoPublish(tUint32 fDup, tUint32 fQoS, tUint32 fRetain, tCChar* szObj, tCChar* szMsg) 
{
  mqttPublishMessage(String(szObj), String(szMsg));
  } // _DoPublish

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CGpiod::begin()
{
  Debug.println("CGpiod::begin");
  OnConfig();
  OnInit();

  m_timerMqtt.initializeMs(1000, TimerDelegate(&CGpiod::checkConnection, this)).start(true);
  m_timer.initializeMs(50, TimerDelegate(&CGpiod::OnRun, this)).start(true);
  } // begin

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CGpiod::checkConnection()
{
  if (WifiStation.isConnected()) {
//  Debug.println("CGpiod::checkConnection,make sure MQTT is running");
    mqttCheckClient();
    } 
  } // checkConnection

