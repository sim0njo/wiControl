
//#include <SmingCore/SmingCore.h>
#include <AppSettings.h>
#include <HTTP.h>
#include <gpiod.h>

CGpiod               g_gpiod;

//----------------------------------------------------------------------------
// HTTP config callback
//----------------------------------------------------------------------------
void                 atsOnHttpQuery(HttpRequest &request, HttpResponse &response)
{
  String str;

  if (!g_http.isHttpClientAllowed(request, response))
    return;

  // handle new settings
  if (request.getRequestMethod() == RequestMethod::POST) {
    if (request.getPostParameter("terse")) 
      g_log.LogPrt(CLOG_CLS_0 | 0x0100, "gpiodOnHttpQuery,terse posted");
    } // if
  else {
    if (request.getQueryParameter("terse")) 
      g_log.LogPrt(CLOG_CLS_0 | 0x0200, "gpiodOnHttpQuery,terse in query");
    
    str = request.getQueryParameter(String("ccmd"));
    g_log.LogPrt(CLOG_CLS_0 | 0x0210, "gpiodOnHttpQuery,ccmd=%s", str.c_str());

    str = request.getPostParameter(String("ccmd"));
    g_log.LogPrt(CLOG_CLS_0 | 0x0220, "gpiodOnHttpQuery,ccmd=%s", str.c_str());
    } // else

  // send page
  response.sendString("0");
  } // atsOnHttpQuery

//----------------------------------------------------------------------------
// HTTP config callback
//----------------------------------------------------------------------------
void                 gpiodOnHttpConfig(HttpRequest &request, HttpResponse &response)
{
  if (!g_http.isHttpClientAllowed(request, response))
    return;

  // handle new settings
  if (request.getRequestMethod() == RequestMethod::POST) {
    AppSettings.gpiodEmul = (request.getPostParameter("gpiodEmul") == "0") ? CGPIOD_EMUL_OUTPUT     : CGPIOD_EMUL_SHUTTER;
    AppSettings.gpiodMode = (request.getPostParameter("gpiodMode") == "1") ? CGPIOD_MODE_STANDALONE : 
                            (request.getPostParameter("gpiodMode") == "2") ? CGPIOD_MODE_MQTT       : CGPIOD_MODE_BOTH;
    AppSettings.gpiodEfmt = (request.getPostParameter("gpiodEfmt") == "0") ? CGPIOD_EFMT_NUMERICAL  : CGPIOD_EFMT_TEXTUAL;
    
    AppSettings.save();

    g_gpiod.OnConfig();
    g_gpiod.OnInit();
    } // if

  // send page
  TemplateFileStream *tmpl = new TemplateFileStream("gpiod.html");
  auto &vars = tmpl->variables();
  vars["appAlias"] = APP_ALIAS;

  vars["gpiodEmul0"] = (AppSettings.gpiodEmul == CGPIOD_EMUL_OUTPUT)     ? "checked='checked'" : "";
  vars["gpiodEmul1"] = (AppSettings.gpiodEmul == CGPIOD_EMUL_SHUTTER)    ? "checked='checked'" : "";

  vars["gpiodMode1"] = (AppSettings.gpiodMode == CGPIOD_MODE_STANDALONE) ? "checked='checked'" : "";
  vars["gpiodMode2"] = (AppSettings.gpiodMode == CGPIOD_MODE_MQTT)       ? "checked='checked'" : "";
  vars["gpiodMode3"] = (AppSettings.gpiodMode == CGPIOD_MODE_BOTH)       ? "checked='checked'" : "";

  vars["gpiodEfmt0"] = (AppSettings.gpiodEfmt == CGPIOD_EFMT_NUMERICAL)  ? "checked='checked'" : "";
  vars["gpiodEfmt1"] = (AppSettings.gpiodEfmt == CGPIOD_EFMT_TEXTUAL)    ? "checked='checked'" : "";

  response.sendTemplate(tmpl); // will be automatically deleted
  } // gpiodOnHttpConfig

//----------------------------------------------------------------------------
// MQTT client callback
// <clientid>/<cmdpfx>/<object>, i.e. astr76b32/L3R1W2/cmd/shutter0=up.0.5
//
// <clientid>/<cmdpfx>/input%=<evt>
// <clientid>/<cmdpfx>/output%=<cmd> *[.<parm>]
// <clientid>/<cmdpfx>/shutter%=<cmd> *[.<parm>]
//
// <clientid>/<cmdpfx>/emul=get | set.<emul>.ack
// <clientid>/<cmdpfx>/mode=get | set.<mode>.ack
// <clientid>/<cmdpfx>/memory=?
//
// <clientid>/<cmdpfx>/system=emul
// <clientid>/<cmdpfx>/system=emul.0|1.ack
// <clientid>/<cmdpfx>/system=emul.output|shutter.ack
// <clientid>/<cmdpfx>/system=memory
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR gpiodOnMqttPublish(String strTopic, String strMsg)
{
  tUint32   dwErr = XERROR_SUCCESS;
  tUint32   msNow = millis();
  tChar     str[64], *pTopic = xstrdup(strTopic.c_str()), *pMsg = xstrdup(strMsg.c_str());
  tGpiodCmd cmd = { 0 };

  do {
    g_log.LogPrt(CGPIOD_CLSLVL | 0x0000, "gpiodOnPublish,topic=%s,msg=%s", strTopic.c_str(), strMsg.c_str());
  
    // check to be sure, but we should not receive other messages
    gsprintf(str, "%s/%s/", AppSettings.mqttClientId.c_str(), AppSettings.mqttCmdPfx.c_str());
    if (xstrnicmp(pTopic, str, gstrlen(str)) && (dwErr = XERROR_DATA)) {
      g_log.LogPrt(CGPIOD_CLSLVL | 0x0100, "gpiodOnMqttPublish,not for us,dropping");
      break;
      } // if

    // point to <obj>
    pTopic += gstrlen(str);
    g_dwMqttPktRx++;

    // parse object, cmd and optional parms
    if (g_gpiod.ParseCmd(&cmd, pTopic, pMsg, (g_gpiod.GetEmul() == CGPIOD_EMUL_OUTPUT) ? CGPIOD_OBJ_CLS_WBS : CGPIOD_OBJ_CLS_WBR)) {
      g_log.LogPrt(CGPIOD_CLSLVL | 0x0200, "gpiodOnMqttPublish,ParseCmd() failed,dropping");
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
// configure subsystems
//--------------------------------------------------------------------------
tUint32 CGpiod::OnConfig()
{
  _systemOnConfig();
  _inputOnConfig();

  if (m_dwEmul == CGPIOD_EMUL_OUTPUT)
    _outputOnConfig();
  else
    _shutterOnConfig();

  } // OnConfig

//--------------------------------------------------------------------------
// initialise subsystems
//--------------------------------------------------------------------------
tUint32 CGpiod::OnInit() 
{
  _systemOnInit();
  _inputOnInit();

  if (m_dwEmul == CGPIOD_EMUL_OUTPUT)
    _outputOnInit();
  else
    _shutterOnInit();

  return m_dwError;
  } // OnInit

//--------------------------------------------------------------------------
// run subsystems
//--------------------------------------------------------------------------
void CGpiod::OnRun() {
  tUint32 msNow = millis();

  _systemOnRun(msNow);
  _inputOnRun(msNow);

  if (m_dwEmul == CGPIOD_EMUL_OUTPUT)
    _outputOnRun(msNow);
  else
    _shutterOnRun(msNow);

  } // OnRun

//--------------------------------------------------------------------------
// exit subsystems
//--------------------------------------------------------------------------
tUint32 CGpiod::OnExit() {

  if (m_dwEmul == CGPIOD_EMUL_OUTPUT)
    _outputOnExit();
  else
    _shutterOnExit();

  _inputOnExit();
  _systemOnExit();
  } // OnExit

//--------------------------------------------------------------------------
// handle event, dwObj = 0x----TTOO, dwEvt = 0|1-n, szEvt = 0 | "str"
//--------------------------------------------------------------------------
tUint32 CGpiod::DoEvt(tGpiodEvt* pEvt) 
{
  tUint32 dwObj = pEvt->dwObj & CGPIOD_OBJ_NUM_MASK;
  tChar   str1[32], str2[32];
  tCChar  *szTopic;

  switch (pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) {
    case CGPIOD_OBJ_CLS_INPUT:
      PrintEvt(pEvt, CGPIOD_CLSLVL | 0x0100, "CGpiod::DoEvt");

      if (m_dwMode & CGPIOD_MODE_STANDALONE) { 
        if (m_dwEmul == CGPIOD_EMUL_OUTPUT) 
          _outputDoEvt(pEvt);
        else 
          _shutterDoEvt(pEvt);
        } // if

      if ((m_dwMode & CGPIOD_MODE_MQTT) && (m_input[dwObj].dwFlags & (0x1 << pEvt->dwEvt))) {
        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          _DoPublish(0, 0, 0, _printObj2String(str1, pEvt->dwObj), _printVal2String(str2, pEvt->dwEvt));
        else
          _DoPublish(0, 0, 0, _printObj2String(str1, pEvt->dwObj), _printObjEvt2String(str2, pEvt->dwObj, pEvt->dwEvt));
        } // if

      break;

    case CGPIOD_OBJ_CLS_OUTPUT:
      PrintEvt(pEvt, CGPIOD_CLSLVL | 0x0200, "CGpiod::DoEvt");

      if ((m_dwMode & CGPIOD_MODE_MQTT) && (m_output[dwObj].dwFlags & (0x1 << pEvt->dwEvt))) {
        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          _DoPublish(0, 0, 0, _printObj2String(str1, pEvt->dwObj), _printVal2String(str2, pEvt->dwEvt));
        else
          _DoPublish(0, 0, 0, _printObj2String(str1, pEvt->dwObj), _printObjEvt2String(str2, pEvt->dwObj, pEvt->dwEvt));
        } // if

      break;

    case CGPIOD_OBJ_CLS_SHUTTER:
      PrintEvt(pEvt, CGPIOD_CLSLVL | 0x0400, "CGpiod::DoEvt");

      if ((m_dwMode & CGPIOD_MODE_MQTT) && (m_shutter[dwObj].dwFlags & (0x1 << pEvt->dwEvt))) {
        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          _DoPublish(0, 0, 0, _printObj2String(str1, pEvt->dwObj), _printVal2String(str2, pEvt->dwEvt));
        else
          _DoPublish(0, 0, 0, _printObj2String(str1, pEvt->dwObj), _printObjEvt2String(str2, pEvt->dwObj, pEvt->dwEvt));
        } // if

      break;

    case CGPIOD_OBJ_CLS_HBEAT:
      _outputDoEvt(pEvt);
      break;

    case CGPIOD_OBJ_CLS_SYSTEM:
      PrintEvt(pEvt, CGPIOD_CLSLVL | 0x1000, "CGpiod::DoEvt");
      szTopic = pEvt->szTopic ? pEvt->szTopic : "system";

      if (pEvt->szEvt)
        _DoPublish(0, 0, 0, szTopic, pEvt->szEvt);

      break;
    } // switch

  return XERROR_SUCCESS;
  } // CGpiod::DoEvt

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::DoCmd(tGpiodCmd* pCmd) 
{
  tUint32   dwErr = XERROR_SUCCESS;
  tGpiodEvt evt = { 0, 0, 0, 0, 0 };

  switch (pCmd->dwObj & CGPIOD_OBJ_CLS_MASK) {
    case CGPIOD_OBJ_CLS_INPUT:   evt.dwObj = pCmd->dwObj;
                                 evt.dwEvt = pCmd->dwCmd;
                                 evt.msNow = pCmd->msNow;
                                 dwErr = DoEvt(&evt);
      break;
    case CGPIOD_OBJ_CLS_OUTPUT:  dwErr = _outputDoCmd(pCmd);
      break;
    case CGPIOD_OBJ_CLS_SHUTTER: dwErr = _shutterDoCmd(pCmd);
      break;
    case CGPIOD_OBJ_CLS_SYSTEM:  dwErr = _systemDoCmd(pCmd);
      break;
    default:                     dwErr = XERROR_INPUT;
      break;
    } // switch

  return dwErr;
  } // DoCmd

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CGpiod::_DoPublish(tUint32 fDup, tUint32 fQoS, tUint32 fRetain, tCChar* szTopic, tCChar* szMsg) 
{
//  g_log.LogPrt(CGPIOD_CLSLVL | 0x0000, "CGpiod::_DoPublish,topic=%s,msg=%s", szTopic, szMsg);
  mqttPublishMessage(String(szTopic), String(szMsg));
  } // _DoPublish

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CGpiod::begin()
{
  g_log.LogPrt(CGPIOD_CLSLVL | 0x0000, "CGpiod::begin");
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

