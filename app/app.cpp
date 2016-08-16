
//----------------------------------------------------------------------------
// app.cpp : main application implementation
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <user_config.h>
#include <SmingCore.h>
#include <SmingCore/Network/TelnetServer.h>
#include <app.h>
#include <globals.h>
#include <network.h>
#include <http.h>
#include <gpiod.h>
#include <ota.h>

CApplication         g_app;
TelnetServer         g_telnet;
//FTPServer            g_ftp;

//----------------------------------------------------------------------------
// Sming Framework INIT method - called during boot
//----------------------------------------------------------------------------
void init()
{
  Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
  Serial.systemDebugOutput(true); // Enable debug output to serial

  g_app.Init();

  // start servers on system ready
	System.onReady(SystemReadyDelegate(&CApplication::StartServices, &g_app));
  } // init

//----------------------------------------------------------------------------
// callback function for network notifications
//----------------------------------------------------------------------------
void OnNetworkEvent(int event)
{
  if      (event == CNETWORK_EVT_DISCONNECTED) {
    Debug.println("--> Wifi DISCONNECTED");
    }

  else if (event == CNETWORK_EVT_CONNECTED) {
    Debug.println("--> Wifi CONNECTED");
    }

  else if (event == CNETWORK_EVT_NEEDSRESTART) {
    }

  else {
    }

  } // OnNetworkEvent

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::Init() {
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

  confLoad();
  cmdInit();

  // Start either wired or wireless networking
  g_network.Init(OnNetworkEvent);

  // CPU boost
  if (m_cpuBoost)
    System.setCpuFrequency(eCF_160MHz);
  else
    System.setCpuFrequency(eCF_80MHz);

  } // Init

//----------------------------------------------------------------------------
// called when system initialization was completed
//----------------------------------------------------------------------------
void CApplication::StartServices() {
  //HTTP must be first so handlers can be registered
  g_http.Init(); 

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
  m_dwTicks++;

  // periodic check network connection every 1s
  if ((m_dwTicks % (CAPP_PERIOD_NETWCHECK / CAPP_PERIOD)) == 0) 
    CheckConnection();

  // periodic reporting of memory usage every 60s
  if (m_dwTicks > (CAPP_PERIOD_HEAPUSAGE / CAPP_PERIOD)) {
    ReportHeapUsage();
    m_dwTicks = 0;
    } //

  // keep gpiod going
  g_gpiod.OnRun();
  } // OnRun

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::CheckConnection()
{
  tChar str[32];

  if (WifiStation.isConnected()) {
    
    if (g_mqtt.CheckClient()) {
      // subscribe to cmd and cfg topics
      Debug.logTxt(CLSLVL_GPIOD | 0x0010, "CApp::CheckConnection,subscribe <node-id>/cmd/#");
      g_mqtt.Subscribe("cmd/#");

      // publish boot messages
      Debug.logTxt(CLSLVL_GPIOD | 0x0030, "CApp::CheckConnection,publish <node-id>/evt/boot");
      gsprintf(str, "%s;%s", szAPP_VERSION, szAPP_TOPOLOGY);
      g_mqtt.Publish(CGPIOD_EVT_PFX, "boot", str);
      }

    } 
  } // CheckConnection

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

