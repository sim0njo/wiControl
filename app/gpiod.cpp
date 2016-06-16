
//----------------------------------------------------------------------------
// cgpiod.cpp : main daemon implementation
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
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
    AppSettings.gpiodEmul    = (request.getPostParameter("gpiodEmul")    == "2") ? CGPIOD_EMUL_SHUTTER : CGPIOD_EMUL_OUTPUT;
    AppSettings.gpiodMode    = (request.getPostParameter("gpiodMode")    == "1") ? CGPIOD_MODE_LOCAL   : 
                               (request.getPostParameter("gpiodMode")    == "2") ? CGPIOD_MODE_MQTT    : CGPIOD_MODE_BOTH;
    AppSettings.gpiodLock    = (request.getPostParameter("gpiodLock")    == "1") ? CGPIOD_LOCK_TRUE    : CGPIOD_LOCK_FALSE;
    AppSettings.gpiodDisable = (request.getPostParameter("gpiodDisable") == "1") ? CGPIOD_DISABLE_TRUE : CGPIOD_DISABLE_FALSE;
    AppSettings.save();

    g_gpiod.OnConfig();
    g_gpiod.OnInit();
    } // if

  // send page
  TemplateFileStream *tmpl = new TemplateFileStream("gpiod.html");
  auto &vars = tmpl->variables();
  vars["appAlias"]      = szAPP_ALIAS;
  vars["appAuthor"]     = szAPP_AUTHOR;
  vars["appDesc"]       = szAPP_DESC;
  vars["mqttClientId"]  = AppSettings.mqttClientId;

  vars["gpiodEmul1"]    = (AppSettings.gpiodEmul    == CGPIOD_EMUL_OUTPUT)   ? "checked='checked'" : "";
  vars["gpiodEmul2"]    = (AppSettings.gpiodEmul    == CGPIOD_EMUL_SHUTTER)  ? "checked='checked'" : "";

  vars["gpiodMode1"]    = (AppSettings.gpiodMode    == CGPIOD_MODE_LOCAL)    ? "checked='checked'" : "";
  vars["gpiodMode2"]    = (AppSettings.gpiodMode    == CGPIOD_MODE_MQTT)     ? "checked='checked'" : "";
  vars["gpiodMode3"]    = (AppSettings.gpiodMode    == CGPIOD_MODE_BOTH)     ? "checked='checked'" : "";

  vars["gpiodLock0"]    = (AppSettings.gpiodLock    == CGPIOD_LOCK_FALSE)    ? "checked='checked'" : "";
  vars["gpiodLock1"]    = (AppSettings.gpiodLock    == CGPIOD_LOCK_TRUE)     ? "checked='checked'" : "";

  vars["gpiodDisable0"] = (AppSettings.gpiodDisable == CGPIOD_DISABLE_FALSE) ? "checked='checked'" : "";
  vars["gpiodDisable1"] = (AppSettings.gpiodDisable == CGPIOD_DISABLE_TRUE)  ? "checked='checked'" : "";

  response.sendTemplate(tmpl); // will be automatically deleted
  } // gpiodOnHttpConfig

//----------------------------------------------------------------------------
// MQTT client callback
// <cmdpfx>/<obj>=<cmd> *[.<parm>], i.e. cmd/out0=up.0.5
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

      if (g_gpiod.ParseCmd(&cmd, pTopic, szMsg, CGPIOD_ORIG_MQTT, AppSettings.gpiodEmul)) {
        Debug.logTxt(CLSLVL_GPIOD | 0x0200, "gpiodOnMqttPublish,ParseCmd() failed,dropping");
        break;
        } // if

      cmd.msNow = msNow;
      g_gpiod.DoCmd(&cmd);
      break;
      } // if CGPIOD_CMD_PFX

    Debug.logTxt(CLSLVL_GPIOD | 0x0900, "gpiodOnMqttPublish,not for us,dropping");
    } while (FALSE);

  } // gpiodOnMqttPublish

//--------------------------------------------------------------------------
// configure subsystems
//--------------------------------------------------------------------------
tUint32 CGpiod::OnConfig()
{
  _systemOnConfig();
  _inputOnConfig();
  _timerOnConfig();
  _outputOnConfig();
  _shutterOnConfig();
  } // OnConfig

//--------------------------------------------------------------------------
// initialise subsystems
//--------------------------------------------------------------------------
tUint32 CGpiod::OnInit() 
{
  _systemOnInit();
  _inputOnInit();
  _timerOnInit();

  if (AppSettings.gpiodEmul == CGPIOD_EMUL_OUTPUT)
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
  _timerOnRun(msNow);

  if (AppSettings.gpiodEmul == CGPIOD_EMUL_OUTPUT)
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
  _timerOnExit();

  if (AppSettings.gpiodEmul == CGPIOD_EMUL_OUTPUT)
    _outputOnExit();
  else
    _shutterOnExit();

  } // OnExit

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
tUint32 CGpiod::GetState(tUint32 dwObj)
{
  switch (dwObj & CGPIOD_OBJ_CLS_MASK) {
    case CGPIOD_OBJ_CLS_INPUT:
      return _inputGetState(dwObj);
      break;
    case CGPIOD_OBJ_CLS_OUTPUT:
      return _outputGetState(dwObj);
      break;
    case CGPIOD_OBJ_CLS_SHUTTER:
      return _shutterGetState(dwObj);
      break;
//  case CGPIOD_OBJ_CLS_TIMER:
//    return _timerGetState(dwObj);
//    break;
    } // switch

  return 0;
  } // GetState

//--------------------------------------------------------------------------
// report status
//--------------------------------------------------------------------------
tUint32 CGpiod::DoSta(tGpiodEvt* pEvt) 
{
  tChar str1[16], str2[16];

  if (pEvt->szEvt)
    Debug.logTxt(CLSLVL_GPIOD | 0x0010, "CGpiod::DoSta,%s.%s", 
                 pEvt->szObj ? pEvt->szObj : PrintObj2String(str1, pEvt->dwObj), pEvt->szEvt);
  else
    Debug.logTxt(CLSLVL_GPIOD | 0x0020, "CGpiod::DoSta,%s.%s (%u)", 
                 pEvt->szObj ? pEvt->szObj : PrintObj2String(str1, pEvt->dwObj), 
                 PrintObjSta2String(str2, pEvt->dwObj, pEvt->dwEvt), pEvt->dwEvt);

  if (AppSettings.gpiodMode & CGPIOD_MODE_MQTT)
    mqttPublish(CGPIOD_STA_PFX, pEvt->szObj ? pEvt->szObj : PrintObj2String(str1, pEvt->dwObj),
                                pEvt->szEvt ? pEvt->szEvt : PrintVal2String(str2, pEvt->dwEvt));
  } // DoSta

//--------------------------------------------------------------------------
// report event
//--------------------------------------------------------------------------
tUint32 CGpiod::DoEvt(tGpiodEvt* pEvt) 
{
  tCChar  *pFunc = "CGpiod::DoEvt";
  tChar   str1[16], str2[16];
  tUint32 dwObj = pEvt->dwObj & CGPIOD_OBJ_NUM_MASK;

  switch (pEvt->dwObj & CGPIOD_OBJ_CLS_MASK) {
    case CGPIOD_OBJ_CLS_INPUT:
      Debug.logTxt(CLSLVL_GPIOD | 0x0100, "%s,%s.%s (%u)", pFunc,
                   PrintObj2String(str1, pEvt->dwObj), 
                   PrintObjEvt2String(str2, pEvt->dwObj, pEvt->dwEvt), pEvt->dwEvt);

      // report event if configured
      if (AppSettings.gpiodMode & CGPIOD_MODE_MQTT)
        mqttPublish(CGPIOD_EVT_PFX, PrintObj2String(str1, pEvt->dwObj), PrintVal2String(str2, pEvt->dwEvt));

      // handle standalone emulation
      if (AppSettings.gpiodMode & CGPIOD_MODE_LOCAL) { 
        if (AppSettings.gpiodEmul == CGPIOD_EMUL_OUTPUT) 
          _outputDoEvt(pEvt);
        else 
          _shutterDoEvt(pEvt);
        } // if

      break;

    case CGPIOD_OBJ_CLS_OUTPUT:
    case CGPIOD_OBJ_CLS_SHUTTER:
    case CGPIOD_OBJ_CLS_TIMER:
    case CGPIOD_OBJ_CLS_SYSTEM:
      if (pEvt->szEvt)
        Debug.logTxt(CLSLVL_GPIOD | 0x0200, "%s,%s.%s", pFunc, 
                     pEvt->szObj ? pEvt->szObj : PrintObj2String(str1, pEvt->dwObj), pEvt->szEvt);

      else 
        Debug.logTxt(CLSLVL_GPIOD | 0x0210, "%s,%s.%s (%u)", pFunc,
                     pEvt->szObj ? pEvt->szObj : PrintObj2String(str1, pEvt->dwObj), 
                     PrintObjEvt2String(str2, pEvt->dwObj, pEvt->dwEvt), pEvt->dwEvt);

      // report event if configured
      if (AppSettings.gpiodMode & CGPIOD_MODE_MQTT)
        mqttPublish(CGPIOD_EVT_PFX, pEvt->szObj ? pEvt->szObj : PrintObj2String(str1, pEvt->dwObj), 
                                    pEvt->szEvt ? pEvt->szEvt : PrintVal2String(str2, pEvt->dwEvt));

      break;

    case CGPIOD_OBJ_CLS_HBEAT:
      // handle objects that require heartbeat
      _systemDoEvt(pEvt);

      if (AppSettings.gpiodEmul == CGPIOD_EMUL_OUTPUT)
        _outputDoEvt(pEvt);
      break;

    } // switch

  return XERROR_SUCCESS;
  } // CGpiod::DoEvt

//----------------------------------------------------------------------------
// handle command
//----------------------------------------------------------------------------
tUint32 CGpiod::DoCmd(tGpiodCmd* pCmd) 
{
  tCChar  *pFunc = "CGpiod::DoCmd";
  tChar   str1[16], str2[16], str3[16];
  tUint32 dwErr = XERROR_SUCCESS;

  if ((pCmd->dwObj & CGPIOD_OBJ_CLS_MASK) == CGPIOD_OBJ_CLS_SYSTEM)
    Debug.logTxt(CLSLVL_GPIOD | 0x0000, "%s,%s%s", pFunc,
                 PrintObj2String(str1, pCmd->dwObj), 
                 PrintCmdParamVals(str3, sizeof(str2), pCmd));
  else
    Debug.logTxt(CLSLVL_GPIOD | 0x0010, "%s,%s.%s%s", pFunc, 
                 PrintObj2String(str1, pCmd->dwObj),
                 PrintObjCmd2String(str2, pCmd->dwObj, pCmd->dwCmd), PrintCmdParamVals(str3, sizeof(str2), pCmd));

  // if orig==MQTT and MQTT mode disabled (except for SYSTEM_CMD_MODE) then reject
  if (  (pCmd->dwOrig == CGPIOD_ORIG_MQTT)                &&
        ((AppSettings.gpiodMode & CGPIOD_MODE_MQTT) == 0) &&
      ( ((pCmd->dwObj & CGPIOD_OBJ_CLS_MASK) != CGPIOD_OBJ_CLS_SYSTEM) || 
        ((pCmd->dwCmd & CGPIOD_CMD_NUM_MASK) != CGPIOD_SYS_CMD_MODE) ) 
     )
    return pCmd->dwError = XERROR_ACCESS;
      
  switch (pCmd->dwObj & CGPIOD_OBJ_CLS_MASK) {
    case CGPIOD_OBJ_CLS_INPUT:   dwErr = _inputDoCmd(pCmd);
      break;
    case CGPIOD_OBJ_CLS_OUTPUT:  dwErr = _outputDoCmd(pCmd);
      break;
    case CGPIOD_OBJ_CLS_SHUTTER: dwErr = _shutterDoCmd(pCmd);
      break;
    case CGPIOD_OBJ_CLS_TIMER:   dwErr = _timerDoCmd(pCmd);
      break;
    case CGPIOD_OBJ_CLS_SYSTEM:  dwErr = _systemDoCmd(pCmd);
      break;
    default:                     dwErr = XERROR_SYNTAX;
      break;
    } // switch

  Debug.logTxt(CLSLVL_GPIOD | 0x9999, "%s,err=%u", pFunc, dwErr);
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
  m_timerRoot.initializeMs(50,   TimerDelegate(&CGpiod::OnRun, this)).start(true);
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
      mqttSubscribe("cmd/#");

      // publish boot messages
      Debug.logTxt(CLSLVL_GPIOD | 0x0030, "CGpiod::checkConnection,publish <node-id>/evt/boot");
      gsprintf(str, "%s;%s/%s", szAPP_VERSION, szAPP_TOPOLOGY, szAPP_TOPOVER);
      mqttPublish(CGPIOD_EVT_PFX, "boot", str);
      }

    } 
  } // checkConnection

