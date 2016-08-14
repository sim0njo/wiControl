
//----------------------------------------------------------------------------
// app_cmd.cpp : main application command handlers
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


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::cmdInit() {
  char str[32];

  // set prompt
  sprintf(str, "%s/%s> ", szAPP_ALIAS, szAPP_TOPOLOGY); 
  commandHandler.setCommandPrompt(str);

  // add new commands
  commandHandler.registerCommand(
    CommandDelegate("info", "Show  information", "System",
                    commandFunctionDelegate(&CApplication::cmdOnInfo, this)));

  commandHandler.registerCommand(
    CommandDelegate("debug", "Enable or disable debugging", "System",
                    commandFunctionDelegate(&CApplication::cmdOnDebug, this)));

  commandHandler.registerCommand(
    CommandDelegate("restart", "Restart the system", "System",
                    commandFunctionDelegate(&CApplication::cmdOnRestart, this)));

  commandHandler.registerCommand(
    CommandDelegate("cpu", "Adjust CPU speed", "System",
                    commandFunctionDelegate(&CApplication::cmdOnCpu, this)));

  commandHandler.registerCommand(
    CommandDelegate("show", "Show the current configuration", "System",
                    commandFunctionDelegate(&CApplication::cmdOnShow, this)));

  commandHandler.registerCommand(
    CommandDelegate("apMode", "Adjust the AccessPoint Mode", "System",
                    commandFunctionDelegate(&CNetwork::OnCmdApMode, &g_network)));

  } // cmdInit

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::cmdOnInfo(String commandLine, CommandOutput* pOut)
{
  char str0[16], str1[16], str2[16], str3[16];

  pOut->printf("\r\n");
    
  pOut->printf("wiControl v%s,topology=%s\r\n", szAPP_VERSION, szAPP_TOPOLOGY);
  pOut->printf(" Build time         : %s\r\n", build_time);
  pOut->printf(" Build version      : %s\r\n", build_git_sha);
  pOut->printf(" Sming Version      : %s\r\n", SMING_VERSION);
  pOut->printf(" ESP SDK version    : %s\r\n", system_get_sdk_version());
  pOut->printf(" System Time        : %s\r\n", SystemClock.getSystemTimeString().c_str());
  pOut->printf(" Free Heap          : %d\r\n", system_get_free_heap_size());
  pOut->printf(" CPU Frequency      : %d MHz\r\n", system_get_cpu_freq());
  pOut->printf(" System Chip ID     : %x\r\n", system_get_chip_id());
  pOut->printf(" SPI Flash ID       : %x\r\n", spi_flash_get_id());
  pOut->printf(" SPI Flash Size     : %d\r\n", (1 << ((spi_flash_get_id() >> 16) & 0xff)));
  pOut->printf(" Loglevel           : 0x%08X\r\n", Debug.logClsLevels(DEBUG_CLS_0));        
  pOut->printf("\r\n");

  pOut->printf("App\r\n");
  pOut->printf(" NodeId             : %s\r\n",  m_nodeId.c_str());
  pOut->printf(" Emulation          : %s\r\n", (m_gpiodEmul    == CGPIOD_EMUL_SHUTTER) ? "shutter"  : "output");
  pOut->printf(" Mode               : %s\r\n", (m_gpiodMode    == CGPIOD_MODE_LOCAL)   ? "local"    :
                                               (m_gpiodMode    == CGPIOD_MODE_MQTT)    ? "MQTT"     : "both");
  pOut->printf(" Configuration cmds : %s\r\n", (m_gpiodLock    == CGPIOD_LOCK_TRUE)    ? "disabled" : "enabled");
  pOut->printf(" Operational cmds   : %s\r\n", (m_gpiodDisable == CGPIOD_DISABLE_TRUE) ? "disabled" : "enabled");

  pOut->printf(" Input states       : %s/%s/%s/%s\r\n",
               g_gpiod.PrintObjSta2String(str0, CGPIOD_OBJ_CLS_INPUT | 0, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 0)),
               g_gpiod.PrintObjSta2String(str1, CGPIOD_OBJ_CLS_INPUT | 1, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 1)),
               g_gpiod.PrintObjSta2String(str2, CGPIOD_OBJ_CLS_INPUT | 2, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 2)),
               g_gpiod.PrintObjSta2String(str3, CGPIOD_OBJ_CLS_INPUT | 3, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 3)));

  if (m_gpiodEmul == CGPIOD_EMUL_OUTPUT)
    pOut->printf(" Output states      : %s/%s/%s/%s\r\n",
                 g_gpiod.PrintObjSta2String(str0, CGPIOD_OBJ_CLS_OUTPUT | 0, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 0)),
                 g_gpiod.PrintObjSta2String(str1, CGPIOD_OBJ_CLS_OUTPUT | 1, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 1)),
                 g_gpiod.PrintObjSta2String(str2, CGPIOD_OBJ_CLS_OUTPUT | 2, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 2)),
                 g_gpiod.PrintObjSta2String(str3, CGPIOD_OBJ_CLS_OUTPUT | 3, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 3)));
  else
    pOut->printf(" Shutter states     : %s/%s\r\n",
                 g_gpiod.PrintObjSta2String(str0, CGPIOD_OBJ_CLS_SHUTTER | 0, g_gpiod.GetState(CGPIOD_OBJ_CLS_SHUTTER | 0)),
                 g_gpiod.PrintObjSta2String(str1, CGPIOD_OBJ_CLS_SHUTTER | 1, g_gpiod.GetState(CGPIOD_OBJ_CLS_SHUTTER | 1)));

  pOut->printf("\r\n");

  pOut->printf("MQTT\r\n");
  pOut->printf(" Broker             : %s:%d\r\n", g_mqtt.GetStrAttr("mqttHost"), g_mqtt.GetNumAttr("mqttPort"));
  pOut->printf(" User/pswd          : %s:%s\r\n", g_mqtt.GetStrAttr("mqttUser"), g_mqtt.GetStrAttr("mqttPswd"));
  pOut->printf(" ClientId           : %s\r\n",    g_mqtt.GetStrAttr("mqttClientId"));
  pOut->printf(" Connected          : %s\r\n",    g_mqtt.IsConnected() ? "yes" : "no");
  pOut->printf("\r\n");

  pOut->printf("Network\r\n");
  pOut->printf(" Station connected  : %s\r\n", g_network.staConnected() ? "yes" : "no");
  pOut->printf(" Station SSID       : %s\r\n", g_network.GetStrAttr("staSSID"));
  pOut->printf(" Station DHCP       : %s\r\n", g_network.GetNumAttr("staDHCP") ? "enabled" : "disabled");
  pOut->printf(" Station IP@        : %s\r\n", g_network.GetStrAttr("staAddr"));

  String apModeStr;
  if      (g_network.GetNumAttr("apMode") == CNETWORK_AP_MODE_ALWAYS_ON)
    apModeStr = "always";
  else if (g_network.GetNumAttr("apMode") == CNETWORK_AP_MODE_ALWAYS_OFF)
    apModeStr = "never";
  else
    apModeStr = "whenDisconnected";

  pOut->printf(" AccessPoint mode   : %s\r\n", apModeStr.c_str());
  pOut->printf(" AccessPoint enabled: %s\r\n", WifiAccessPoint.isEnabled() ? "yes" : "no");
  pOut->printf("\r\n");

  } // cmdOnInfo

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::cmdOnShow(String commandLine, CommandOutput* pOut)
{
  pOut->println(fileGetContent(".settings.conf"));
  pOut->println("");
  pOut->println(fileGetContent(CAPP_CONF_FILE));
  pOut->println("");
  pOut->println(fileGetContent(CMQTT_CONF_FILE));
  pOut->println("");
  pOut->println(fileGetContent(CNETWORK_CONF_FILE));
  } // cmdOnShow

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::cmdOnCpu(String commandLine, CommandOutput* pOut)
{
  Vector<String> commandToken;
  int numToken = splitString(commandLine, ' ' , commandToken);

  if (numToken != 2 ||
      (commandToken[1] != "80" && commandToken[1] != "160"))
  {
    pOut->printf("Usage : \r\n\r\n");
    pOut->printf("  cpu 80  : Run at 80MHz\r\n");
    pOut->printf("  cpu 160 : Run at 160MHz\r\n");
    return;
    }

  if (commandToken[1] == "80")
  {
    System.setCpuFrequency(eCF_80MHz);
    m_cpuBoost = false;
    }
  else
  {
    System.setCpuFrequency(eCF_160MHz);
    m_cpuBoost = true;
    }

  confSave();
  } // cmdOnCpu

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::cmdOnDebug(String commandLine, CommandOutput* pOut)
{
  Vector<String> commandToken;
  int            numToken = splitString(commandLine, ' ' , commandToken);
  tUint32        dwLevels;

  if (numToken < 2) {
    pOut->printf("Usage : \r\n\r\n");
    pOut->printf("  debug 0       : Disable debug\r\n");
    pOut->printf("  debug <level> : Enable debug, level is decimal (123)\r\n");
    pOut->printf("                : or hexadecimal (0x12345678) value\r\n\r\n");
    }
  else {
    dwLevels = xstrToUint32(commandToken[1].c_str());
    Debug.logClsLevels(DEBUG_CLS_0, dwLevels);
    Debug.logPrintMsgId(true);
    (dwLevels) ? Debug.start() : Debug.stop();    
    // toggle msgId usage
    } 

  pOut->printf("debug = 0x%08X\r\n\r\n", Debug.logClsLevels(DEBUG_CLS_0));
  } // CApplication::cmdOnDebug

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::cmdOnRestart(String commandLine, CommandOutput* pOut)
{
  Restart(0);
  } // cmdOnRestart

