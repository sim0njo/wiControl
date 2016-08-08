
#include <user_config.h>
#include <SmingCore.h>
#include <SmingCore/Network/TelnetServer.h>
#include <application.h>
#include <AppSettings.h>
#include <globals.h>
#include <Network.h>
#include <HTTP.h>
#include <gpiod.h>
#include <cparse.hpp>

CApplication   g_app;

//FTPServer      g_ftp;
TelnetServer   g_telnet;
static boolean g_firstTime = TRUE;
int            g_isNetworkConnected = FALSE;

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
// Will be called when WiFi station was connected to AP
void wifiCb(bool connected)
{
  g_isNetworkConnected = connected;
  if (connected) {
    Debug.println("--> Wifi CONNECTED");
    if (g_firstTime) 
      g_firstTime = FALSE;
    }
  else
    Debug.println("--> Wifi DISCONNECTED");
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void processRestartCommandWeb(void)
{
  System.restart();
  }

//----------------------------------------------------------------------------
// Sming Framework INIT method - called during boot
//----------------------------------------------------------------------------
void init()
{
  Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
  Serial.systemDebugOutput(true); // Enable debug output to serial

  g_app.OnInit();

  // start servers on system ready
	System.onReady(SystemReadyDelegate(&CApplication::StartServices, &g_app));
  } // init

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::OnInit() {
  char str[32];

  // make sure wifi does not start yet! 
  wifi_station_set_auto_connect(0);

  // make sure all chip enable pins are HIGH 
//g_gpiod.OnInit();

/*
	// load boot information
	tUint8 bootMode;
	if (rboot_get_last_boot_mode(&bootMode)) {
		if (bootMode == MODE_TEMP_ROM)
      Debug.printf("Booting after OTA\n");
		else
      Debug.printf("Normal boot\n");

//  m_bootMode = bootMode;
  	}
*/
  // mount the internal storage 
  int slot = rboot_get_current_rom();
  if (slot == 0) {
    Debug.printf("Mounting spiffs at %x, length %d\n", 0x40300000, SPIFF_SIZE);
    spiffs_mount_manual(0x40300000, SPIFF_SIZE);
    }
  else {
    Debug.printf("Mounting spiffs at %x, length %d\n", 0x40500000, SPIFF_SIZE);
    spiffs_mount_manual(0x40500000, SPIFF_SIZE);
    }

  Serial.commandProcessing(true);
  Debug.start();

  // set prompt
  sprintf(str, "%s/%s> ", szAPP_ALIAS, szAPP_TOPOLOGY); 
  commandHandler.setCommandPrompt(str);

  // add new commands
  commandHandler.registerCommand(
    CommandDelegate("loglevel", "Set logging levels", "System",
                    commandFunctionDelegate(&CApplication::CmdLogLevel, this)));

  commandHandler.registerCommand(
    CommandDelegate("info", "Show  information", "System",
                    commandFunctionDelegate(&CApplication::CmdInfo, this)));

  commandHandler.registerCommand(
    CommandDelegate("debug", "Enable or disable debugging", "System",
                    commandFunctionDelegate(&CApplication::CmdDebug, this)));

  commandHandler.registerCommand(
    CommandDelegate("restart", "Restart the system", "System",
                    commandFunctionDelegate(&CApplication::CmdRestart, this)));

  commandHandler.registerCommand(
    CommandDelegate("cpu", "Adjust CPU speed", "System",
                    commandFunctionDelegate(&CApplication::CmdCpu, this)));

  commandHandler.registerCommand(
    CommandDelegate("apMode", "Adjust the AccessPoint Mode", "System",
                    commandFunctionDelegate(&CApplication::CmdApMode, this)));

  commandHandler.registerCommand(
    CommandDelegate("showConfig", "Show the current configuration", "System",
                    commandFunctionDelegate(&CApplication::CmdShowConfig, this)));

  g_appCfg.load();

  // Start either wired or wireless networking
  Network.begin(wifiCb);

  // CPU boost
  if (g_appCfg.cpuBoost)
    System.setCpuFrequency(eCF_160MHz);
  else
    System.setCpuFrequency(eCF_80MHz);

  } // OnInit

//----------------------------------------------------------------------------
// called when system initialization was completed
//----------------------------------------------------------------------------
void CApplication::StartServices() {
  //HTTP must be first so handlers can be registered
  g_http.begin(); 

  // Start FTP server
  if (!fileExist("index.html"))
    fileSetContent("index.html", "<h3>No content provided</h3>");

//g_ftp.listen(21);
//g_ftp.addUser("admin", "12345678"); // FTP account

  g_telnet.listen(23);

  g_gpiod.OnConfig();
  g_gpiod.OnInit();

  m_timer.initializeMs(CAPP_PERIOD, TimerDelegate(&CApplication::OnRun, this)).start(true);
  } // StartServices

//----------------------------------------------------------------------------
// called every CAPP_PERIOD = 50ms
//----------------------------------------------------------------------------
void CApplication::OnRun() {
  // periodic reporting of memory usage
  if (++m_dwTicks > (CAPP_PERIOD_HEAPUSAGE / CAPP_PERIOD)) {
    ReportHeapUsage();
    m_dwTicks = 0;
    } //

  // keep gpiod going
  g_gpiod.OnRun();
  } // OnRun

//----------------------------------------------------------------------------
// periodic reporting memory usage
//----------------------------------------------------------------------------
void CApplication::ReportHeapUsage()
{
  tGpiodCmd cmd = { 0 };

  cmd.dwOrig = CGPIOD_ORIG_MQTT;
  cmd.dwObj  = CGPIOD_SYS_CMD_MEMORY;
  cmd.dwCmd  = CGPIOD_SYS_CMD_MEMORY;
  g_gpiod.DoCmd(&cmd);
  } // ReportHeapUsage

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::StopServices() {
  g_gpiod.OnExit();
  } // StopServices

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::Restart(tUint32 msDelay) {
  Debug.logTxt(CLSLVL_APP | 0x0000, "CApplication::Restart");

  if (msDelay)
    delay(msDelay);

  StopServices();
	System.restart();
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::Reset() {
  Debug.logTxt(CLSLVL_APP | 0x0000, "CApplication::Reset");
//Serial.println("Resetting controller");
//	cfg.reset();
//	rgbwwctrl.color_reset();
//	network.forget_wifi();
	Restart(500);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::CmdInfo(String commandLine, CommandOutput* pOut)
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

  pOut->printf("Network\r\n");
  pOut->printf(" Wifi SSID          : %s\r\n", g_appCfg.staSSID.c_str());
  pOut->printf(" DHCP enabled       : %s\r\n", g_appCfg.netwDHCP ? "TRUE" : "FALSE");
  pOut->printf(" IP address         : %s\r\n", Network.getClientAddr().toString().c_str());

  String apModeStr;
  if (g_appCfg.apMode == apModeAlwaysOn)
    apModeStr = "always";
  else if (g_appCfg.apMode == apModeAlwaysOff)
    apModeStr = "never";
  else
    apModeStr= "whenDisconnected";

  pOut->printf(" AccessPoint mode   : %s\r\n", apModeStr.c_str());
  pOut->printf(" AccessPoint enabled: %s\r\n", WifiAccessPoint.isEnabled() ? "yes" : "no");
  pOut->printf("\r\n");

  pOut->printf("MQTT\r\n");
  pOut->printf(" Broker             : %s:%d\r\n", g_appCfg.mqttServer.c_str(), g_appCfg.mqttPort);
  pOut->printf(" User/pswd          : %s:%s\r\n", g_appCfg.mqttUser.c_str(), g_appCfg.mqttPswd.c_str());
  pOut->printf(" ClientId           : %s\r\n", g_appCfg.mqttClientId.c_str());
  pOut->printf(" Connected          : %s\r\n", mqttIsConnected() ? "yes" : "no");
  pOut->printf("\r\n");

  pOut->printf("GPIOD\r\n");
  pOut->printf(" Emulation          : %s\r\n", (g_appCfg.gpiodEmul    == CGPIOD_EMUL_SHUTTER) ? "shutter"  : "output");
  pOut->printf(" Mode               : %s\r\n", (g_appCfg.gpiodMode    == CGPIOD_MODE_LOCAL)   ? "local"    :
                                               (g_appCfg.gpiodMode    == CGPIOD_MODE_MQTT)    ? "MQTT"     : "both");
  pOut->printf(" Configuration cmds : %s\r\n", (g_appCfg.gpiodLock    == CGPIOD_LOCK_TRUE)    ? "disabled" : "enabled");
  pOut->printf(" Operational cmds   : %s\r\n", (g_appCfg.gpiodDisable == CGPIOD_DISABLE_TRUE) ? "disabled" : "enabled");

  pOut->printf(" Input states       : %s/%s/%s/%s\r\n",
               g_gpiod.PrintObjSta2String(str0, CGPIOD_OBJ_CLS_INPUT | 0, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 0)),
               g_gpiod.PrintObjSta2String(str1, CGPIOD_OBJ_CLS_INPUT | 1, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 1)),
               g_gpiod.PrintObjSta2String(str2, CGPIOD_OBJ_CLS_INPUT | 2, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 2)),
               g_gpiod.PrintObjSta2String(str3, CGPIOD_OBJ_CLS_INPUT | 3, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 3)));

  if (g_appCfg.gpiodEmul == CGPIOD_EMUL_OUTPUT)
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

  } // CmdInfo

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::CmdLogLevel(String commandLine, CommandOutput* pOut)
{
  Vector<String> commandToken;
  int            numToken = splitString(commandLine, ' ' , commandToken);
  tUint32        dwLevels;

  if (numToken > 1) {
    // commandToken[1] is loglevel
    dwLevels = xstrToUint32(commandToken[1].c_str());
    Debug.logClsLevels(DEBUG_CLS_0, dwLevels);
        
    // toggle msgId usage
    Debug.logPrintMsgId(Debug.logPrintMsgId() ? false : true);
    } // if

  pOut->printf("Loglevel = 0x%08X\r\n\r\n", Debug.logClsLevels(DEBUG_CLS_0));
//g_appCfg.save();
//System.restart();
  } // CmdLogLevel

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::CmdShowConfig(String commandLine, CommandOutput* pOut)
{
  pOut->println(fileGetContent(".settings.conf"));
  } // CmdShowConfig

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::CmdCpu(String commandLine, CommandOutput* pOut)
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
    g_appCfg.cpuBoost = false;
    }
  else
  {
    System.setCpuFrequency(eCF_160MHz);
    g_appCfg.cpuBoost = true;
    }

  g_appCfg.save();
  } // CmdCpu

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::CmdDebug(String commandLine, CommandOutput* pOut)
{
  Vector<String> commandToken;
  int            numToken = splitString(commandLine, ' ' , commandToken);

  if ((numToken != 2) || (commandToken[1] != "on" && commandToken[1] != "off")) {
    pOut->printf("Usage : \r\n\r\n");
    pOut->printf("  debug on  : Enable debug\r\n");
    pOut->printf("  debug off : Disable debug\r\n");
    return;
    }

  if (commandToken[1] == "on")
    Debug.start();
  else
    Debug.stop();
  } // CApplication::CmdDebug

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::CmdApMode(String commandLine, CommandOutput* pOut)
{
  Vector<String> commandToken;
  int numToken = splitString(commandLine, ' ' , commandToken);

    if (numToken != 2 ||
        (commandToken[1] != "always" && commandToken[1] != "never" &&
         commandToken[1] != "whenDisconnected"))
    {
      pOut->printf("Usage : \r\n\r\n");
      pOut->printf("  apMode always           : Always have the AP enabled\r\n");
      pOut->printf("  apMode never            : Never have the AP enabled\r\n");
      pOut->printf("  apMode whenDisconnected : Only enable the AP when disconnected\r\n");
      pOut->printf("                            from the network\r\n");
      return;
    }

    if (commandToken[1] == "always")
    {
      g_appCfg.apMode = apModeAlwaysOn;
    }
    else if (commandToken[1] == "never")
    {
      g_appCfg.apMode = apModeAlwaysOff;
    }        
    else
    {
      g_appCfg.apMode = apModeWhenDisconnected;
    }

  g_appCfg.save();
  Restart(0);
  } // CmdApMode

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::CmdRestart(String commandLine, CommandOutput* pOut)
{
  Restart(0);
  } // CmdRestart

