
#include <AppSettings.h>
#include <HTTP.h>
#include <gpiod.h>

CGpiod               g_gpiod;


//----------------------------------------------------------------------------
// HTTP config callback
//----------------------------------------------------------------------------
void                 gpiodOnHttpConfig(HttpRequest &request, HttpResponse &response)
{
  if (!g_http.isHttpClientAllowed(request, response))
    return;

  // handle new settings
  if (request.getRequestMethod() == RequestMethod::POST) {
    AppSettings.gpiodEmul = (request.getPostParameter("gpiodEmul") == "1") ? CGPIOD_EMUL_OUTPUT     : CGPIOD_EMUL_SHUTTER;
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

  vars["gpiodEmul1"] = (AppSettings.gpiodEmul == CGPIOD_EMUL_OUTPUT)     ? "checked='checked'" : "";
  vars["gpiodEmul2"] = (AppSettings.gpiodEmul == CGPIOD_EMUL_SHUTTER)    ? "checked='checked'" : "";

  vars["gpiodMode1"] = (AppSettings.gpiodMode == CGPIOD_MODE_STANDALONE) ? "checked='checked'" : "";
  vars["gpiodMode2"] = (AppSettings.gpiodMode == CGPIOD_MODE_MQTT)       ? "checked='checked'" : "";
  vars["gpiodMode3"] = (AppSettings.gpiodMode == CGPIOD_MODE_BOTH)       ? "checked='checked'" : "";

  vars["gpiodEfmt0"] = (AppSettings.gpiodEfmt == CGPIOD_EFMT_NUMERICAL)  ? "checked='checked'" : "";
  vars["gpiodEfmt1"] = (AppSettings.gpiodEfmt == CGPIOD_EFMT_TEXTUAL)    ? "checked='checked'" : "";

  response.sendTemplate(tmpl); // will be automatically deleted
  } // gpiodOnHttpConfig

//----------------------------------------------------------------------------
// MQTT client callback
// <cmdpfx>/<obj>, i.e. astr76b32/L3R1W2/cmd/shutter0=up.0.5
//
// <cmdpfx>/in%=<evt>
// <cmdpfx>/out%=<cmd> *[.<parm>]
//
// <cmdpfx>/emul=get | set.<emul>.ack
// <cmdpfx>/mode=get | set.<mode>.ack
// <cmdpfx>/memory=?
//
// <cmdpfx>/system=emul
// <cmdpfx>/system=emul.0|1.ack
// <cmdpfx>/system=emul.output|shutter.ack
// <cmdpfx>/system=memory
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR gpiodOnMqttPublish(tChar* szTopic, tChar* szMsg)
{
  tChar     *pTopic = szTopic;
  tUint32   msNow = millis();
  tGpiodCmd cmd = { 0 };

  do {
    Debug.logTxt(CLSLVL_GPIOD | 0x0000, "gpiodOnMqttPublish,topic=%s,msg=%s", szTopic, szMsg);

    if (gstrnicmp(szTopic, CGPIOD_CMD_PFX, gstrlen(CGPIOD_CMD_PFX)) == 0) {
      // parse object, cmd and optional parms
      pTopic += (gstrlen(CGPIOD_CMD_PFX) + 1);
      if (g_gpiod.ParseCmd(&cmd, pTopic, szMsg, CGPIOD_ORIG_MQTT, g_gpiod.GetEmul())) {
        Debug.logTxt(CLSLVL_GPIOD | 0x0200, "gpiodOnMqttPublish,ParseCmd() failed,dropping");
        break;
        } // if

      cmd.msNow = msNow;
      g_gpiod.DoCmd(&cmd);
      } // if CGPIOD_CMD_PFX

    if (gstrnicmp(szTopic, CGPIOD_CFG_PFX, gstrlen(CGPIOD_CFG_PFX)) == 0) {
      pTopic += (gstrlen(CGPIOD_CFG_PFX) + 1);
      } // if CGPIOD_CFG_PFX

    } while (FALSE);

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
void CGpiod::OnRun() 
{
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
tUint32 CGpiod::OnExit() 
{
  _systemOnExit();
  _inputOnExit();

  if (m_dwEmul == CGPIOD_EMUL_OUTPUT)
    _outputOnExit();
  else
    _shutterOnExit();

  } // OnExit

//--------------------------------------------------------------------------
// check if timer is expired
//--------------------------------------------------------------------------
tUint32 CGpiod::TimerExpired(tUint32 msNow, tUint32 msTimer) 
{
  // handle clock wrap
  if ((msTimer > ESP8266_MILLIS_MID) && (msNow < ESP8266_MILLIS_MID))
    msNow += ESP8266_MILLIS_MAX;
    
  return (msNow > msTimer) ? 1 : 0;
  } // TimerExpired

//--------------------------------------------------------------------------
// report status
//--------------------------------------------------------------------------
tUint32 CGpiod::DoSta(tGpiodEvt* pEvt) 
{
  tChar str1[16], str2[16];

  switch (pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) {
    case CGPIOD_OBJ_CLS_INPUT:
    case CGPIOD_OBJ_CLS_OUTPUT:
    case CGPIOD_OBJ_CLS_SHUTTER:
      if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
        mqttPublish(CGPIOD_STA_PFX, _printObj2String(str1, pEvt->dwObj), _printVal2String(str2, pEvt->dwEvt));
      else
        mqttPublish(CGPIOD_STA_PFX, _printObj2String(str1, pEvt->dwObj), _printObjSta2String(str2, pEvt->dwObj, pEvt->dwEvt));
      break;

    case CGPIOD_OBJ_CLS_SYSTEM:
      if (pEvt->szEvt)
        mqttPublish(CGPIOD_STA_PFX, pEvt->szTopic ? pEvt->szTopic : "system", pEvt->szEvt);

      break;
    } // switch

  } // DoSta

//--------------------------------------------------------------------------
// report event
//--------------------------------------------------------------------------
tUint32 CGpiod::DoEvt(tGpiodEvt* pEvt) 
{
  tUint32 dwObj = pEvt->dwObj & CGPIOD_OBJ_NUM_MASK;
  tChar   str1[16], str2[16];

  switch (pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) {
    case CGPIOD_OBJ_CLS_INPUT:
      // report event
      if ((m_dwMode & CGPIOD_MODE_MQTT) && (m_input[dwObj].dwFlags & (0x1 << pEvt->dwEvt))) {
        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          mqttPublish(CGPIOD_EVT_PFX, _printObj2String(str1, pEvt->dwObj), _printVal2String(str2, pEvt->dwEvt));
        else
          mqttPublish(CGPIOD_EVT_PFX, _printObj2String(str1, pEvt->dwObj), _printObjEvt2String(str2, pEvt->dwObj, pEvt->dwEvt));
        } // if

      // handle standalone emulation
      if (m_dwMode & CGPIOD_MODE_STANDALONE) { 
        if (m_dwEmul == CGPIOD_EMUL_OUTPUT) 
          _outputDoEvt(pEvt);
        else 
          _shutterDoEvt(pEvt);
        } // if

      break;

    case CGPIOD_OBJ_CLS_OUTPUT:
      // report event
      if ((m_dwMode & CGPIOD_MODE_MQTT) && (m_output[dwObj].dwFlags & (0x1 << pEvt->dwEvt))) {
        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          mqttPublish(CGPIOD_EVT_PFX, _printObj2String(str1, pEvt->dwObj), _printVal2String(str2, pEvt->dwEvt));
        else
          mqttPublish(CGPIOD_EVT_PFX, _printObj2String(str1, pEvt->dwObj), _printObjEvt2String(str2, pEvt->dwObj, pEvt->dwEvt));
        } // if

      break;

    case CGPIOD_OBJ_CLS_SHUTTER:
      // report event
      if ((m_dwMode & CGPIOD_MODE_MQTT) && (m_shutter[dwObj].dwFlags & (0x1 << pEvt->dwEvt))) {
        if (m_dwEfmt == CGPIOD_EFMT_NUMERICAL)
          mqttPublish(CGPIOD_EVT_PFX, _printObj2String(str1, pEvt->dwObj), _printVal2String(str2, pEvt->dwEvt));
        else
          mqttPublish(CGPIOD_EVT_PFX, _printObj2String(str1, pEvt->dwObj), _printObjEvt2String(str2, pEvt->dwObj, pEvt->dwEvt));
        } // if

      break;

    case CGPIOD_OBJ_CLS_HBEAT:
      _systemDoEvt(pEvt);
      _outputDoEvt(pEvt);
      break;

    case CGPIOD_OBJ_CLS_SYSTEM:
      if (pEvt->szEvt)
        mqttPublish(CGPIOD_EVT_PFX, pEvt->szTopic ? pEvt->szTopic : "system", pEvt->szEvt);

      break;
    } // switch

  return XERROR_SUCCESS;
  } // CGpiod::DoEvt

//----------------------------------------------------------------------------
// handle command
//----------------------------------------------------------------------------
tUint32 CGpiod::DoCmd(tGpiodCmd* pCmd) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  switch (pCmd->dwObj & CGPIOD_OBJ_CLS_MASK) {
    case CGPIOD_OBJ_CLS_INPUT:   dwErr = _inputDoCmd(pCmd);
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
// begin GPIOD
//----------------------------------------------------------------------------
void CGpiod::begin()
{
  Debug.logTxt(CLSLVL_GPIOD | 0x0000, "CGpiod::begin");
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
  tChar str[32];

  if (WifiStation.isConnected()) {
    
    if (mqttCheckClient()) {
      // subscribe to cmd and cfg topics
      Debug.logTxt(CLSLVL_GPIOD | 0x0010, "CGpiod::checkConnection,subscribe <node-id>/cmd/#");
      mqttSubscribe("cmd/#"); // + AppSettings.mqttCmdPfx + String("/#"));

//    Debug.logTxt(CLSLVL_GPIOD | 0x0010, "CGpiod::checkConnection,subscribe <node-id>/cfg/#");
//    mqttSubscribe("cfg/#"); // + AppSettings.mqttCmdPfx + String("/#"));
      
      // publish boot messages
      Debug.logTxt(CLSLVL_GPIOD | 0x0030, "CGpiod::checkConnection,publish <node-id>/boo/hw");
      mqttPublish(CGPIOD_BOO_PFX, "hw", APP_TOPOLOGY);

      Debug.logTxt(CLSLVL_GPIOD | 0x0040, "CGpiod::checkConnection,publish <node-id>/boo/sw");
      sprintf(str, "%s/%s", APP_ALIAS, APP_VERSION);
      mqttPublish(CGPIOD_BOO_PFX, "sw", str);
      }

    } 
  } // checkConnection

