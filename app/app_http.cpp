
//----------------------------------------------------------------------------
// app_http.cpp : main application http handlers
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <SmingCore.h>
#include <globals.h>
#include <app.h>
#include <gpiod.h>
#include <mqtt.h>
#include <ota.h>
#include <network.h>
#include <http.h>

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::httpOnStatus(HttpRequest &request, HttpResponse &response)
{
  char               buf[200], str0[16], str1[16], str2[16], str3[16];
  TemplateFileStream *tmpl = new TemplateFileStream("status.html");
  auto               &vars = tmpl->variables();

  vars["appAlias"]  = szAPP_ALIAS;
  vars["appAuthor"] = szAPP_AUTHOR;
  vars["appDesc"]   = szAPP_DESC;

  // -- STA ---------------------------------------------------
  if (g_network.staConnected()) {
    sprintf(buf, "Connected to %s, %s address %s", 
            g_network.GetStrAttr("staSSID"), g_network.GetNumAttr("staDHCP") ? "DHCP" : "static", g_network.staClientAddr().toString().c_str());
    vars["staStatus"] = buf;
    } 
  else
    vars["staStatus"] = "Disconnected";
  
  // -- AP ----------------------------------------------------
  if (g_network.apEnabled()) {
    sprintf(buf, "Enabled, address %s", g_network.apServerAddr().toString().c_str());
    vars["apStatus"] = buf;
    } 
  else 
    vars["apStatus"] = "Disabled";

  // -- MQTT --------------------------------------------------
  vars["mqttRx"]        = g_mqtt.GetNumAttr("mqttPktRx");
  vars["mqttRxDropped"] = g_mqtt.GetNumAttr("mqttPktRxDropped");
  vars["mqttTx"]        = g_mqtt.GetNumAttr("mqttPktTx");
    
  if (g_mqtt.IsConnected()) {
    sprintf(buf, "Connected to %s:%u as '%s'", 
            g_mqtt.GetStrAttr("mqttHost"), g_mqtt.GetNumAttr("mqttPort"), g_mqtt.GetStrAttr("mqttNodeId"));
    vars["mqttStatus"] = buf;
    } 
  else
    vars["mqttStatus"] = "Disconnected";

  vars["gpiodVersion"]  = szAPP_VERSION;
  vars["gpiodTopology"] = szAPP_TOPOLOGY;

  vars["gpiodEmul"]    = (m_gpiodEmul    == CGPIOD_EMUL_OUTPUT)  ? "output"   :
                         (m_gpiodEmul    == CGPIOD_EMUL_SHUTTER) ? "shutter"  : "<unknown>";
  vars["gpiodMode"]    = (m_gpiodMode    == CGPIOD_MODE_LOCAL)   ? "local"    :
                         (m_gpiodMode    == CGPIOD_MODE_MQTT)    ? "MQTT"     :
                         (m_gpiodMode    == CGPIOD_MODE_BOTH)    ? "both"     : "<unknown>";
  vars["gpiodLock"]    = (m_gpiodLock    == CGPIOD_LOCK_TRUE)    ? "disabled" : "enabled";
  vars["gpiodDisable"] = (m_gpiodDisable == CGPIOD_DISABLE_TRUE) ? "disabled" : "enabled";

  sprintf(buf, "%s/%s/%s/%s",
          g_gpiod.PrintObjSta2String(str0, CGPIOD_OBJ_CLS_INPUT | 0, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 0)),
          g_gpiod.PrintObjSta2String(str1, CGPIOD_OBJ_CLS_INPUT | 1, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 1)),
          g_gpiod.PrintObjSta2String(str2, CGPIOD_OBJ_CLS_INPUT | 2, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 2)),
          g_gpiod.PrintObjSta2String(str3, CGPIOD_OBJ_CLS_INPUT | 3, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 3)));
  vars["gpiodInputs"] = buf;

  if (m_gpiodEmul == CGPIOD_EMUL_OUTPUT)
    sprintf(buf, "%s/%s/%s/%s",
            g_gpiod.PrintObjSta2String(str0, CGPIOD_OBJ_CLS_OUTPUT | 0, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 0)),
            g_gpiod.PrintObjSta2String(str1, CGPIOD_OBJ_CLS_OUTPUT | 1, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 1)),
            g_gpiod.PrintObjSta2String(str2, CGPIOD_OBJ_CLS_OUTPUT | 2, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 2)),
            g_gpiod.PrintObjSta2String(str3, CGPIOD_OBJ_CLS_OUTPUT | 3, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 3)));
  else
    sprintf(buf, "%s/%s",
            g_gpiod.PrintObjSta2String(str0, CGPIOD_OBJ_CLS_SHUTTER | 0, g_gpiod.GetState(CGPIOD_OBJ_CLS_SHUTTER | 0)),
            g_gpiod.PrintObjSta2String(str1, CGPIOD_OBJ_CLS_SHUTTER | 1, g_gpiod.GetState(CGPIOD_OBJ_CLS_SHUTTER | 1)));
  vars["gpiodOutputs"] = buf;

   
  // --- System info -------------------------------------------------
  sprintf (buf, "%x", system_get_chip_id());
  vars["systemVersion"]  = build_git_sha;
  vars["systemBuild"]    = build_time;
  vars["systemFreeHeap"] = system_get_free_heap_size();
  vars["systemStartup"]  = "todo";
  vars["systemChipId"]   = buf;
  uint32_t curMillis     = millis();
  sprintf(buf, "%d s, %03d ms : ", curMillis / 1000, curMillis % 1000);
  vars["systemUptime"]   = buf;


  response.sendTemplate(tmpl); // will be automatically deleted
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::httpOnTools(HttpRequest &request, HttpResponse &response)
{
  if (!g_http.isClientAllowed(request, response))
    return;

  confLoad();

  if (request.getRequestMethod() == RequestMethod::POST) {
    String command = request.getPostParameter("Command");
        
    if (command.equals("Upgrade")) {
      m_otaBaseUrl = request.getPostParameter("otaBaseUrl");
      confSave();

      Debug.printf("Calling otaStartUpdateHttp(%s)\r\n", m_otaBaseUrl.c_str());
      otaStartUpdateHttp(m_otaBaseUrl);
      Debug.println("Called: otaStartUpdateHttp()");
      }

    else if (command.equals("Restart")) {
      Debug.println("Restarting node...");
      Restart(0);
      }

    else {
      Debug.printf("Unknown command: %s\r\n", command.c_str());
      }

    }

  TemplateFileStream *tmpl = new TemplateFileStream("tools.html");
  auto &vars = tmpl->variables();

  vars["appAlias"]   = szAPP_ALIAS;
  vars["appAuthor"]  = szAPP_AUTHOR;
  vars["appDesc"]    = szAPP_DESC;

  vars["otaBaseUrl"] = m_otaBaseUrl;

  response.sendTemplate(tmpl); // will be automatically deleted
  } //

