
//----------------------------------------------------------------------------
// ota.cpp : OTA wrapper
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Network/TelnetServer.h>
#include <app.h>
#include <mqtt.h>
#include <network.h>

/*******/
/* OTA */
/*******/

#ifdef RBOOT_TWO_ROMS
  #error "RBOOT_TWO_ROMS not supported"
#endif

void otaUpdateHandler();
void otaUpdate_CallBack(bool result);


#define ROM_0_FNAME "/rom0.bin"
#define SPIFFS_FNAME "/spiff_rom.bin"

String           g_otaBaseUrl;
Timer            g_otaStartTimer;
rBootHttpUpdate* g_otaUpdater = 0;
bool             g_otaInProgress = false;
int              g_otaNumTrials = 0;

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void StartOtaUpdate() 
{
  g_otaNumTrials++;
  g_otaStartTimer.initializeMs(50, TimerDelegate(otaUpdateHandler)).start(false);
  } // 

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void StartOtaUpdateWeb(String webOtaBaseUrl) 
{
  Debug.println("====== In StartOtaUpdateWeb(). ======");
  g_otaBaseUrl = webOtaBaseUrl;
  Debug.printf("Starting upgrade with images from %s\r\n", g_otaBaseUrl.c_str());
  StartOtaUpdate();
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void otaUpdate_CallBack(bool result) {
	
    Debug.println("In OTA callback...");
    if (result == true)
    {
        // success
        uint8 slot;
        slot = rboot_get_current_rom();
        if (slot == 0) slot = 1; else slot = 0;
#ifndef DISABLE_SPIFFS
        Debug.printf("Firmware updated, storing g_appCfg to new spiffs...\r\n");
        g_app.confLoad();
        g_mqtt.confLoad();
        g_network.confLoad();

        spiffs_unmount();
        if (slot == 0)
        {
#ifdef RBOOT_SPIFFS_0
          Debug.printf("trying to mount spiffs at %x, length %d", RBOOT_SPIFFS_0 + 0x40200000, SPIFF_SIZE);
          spiffs_mount_manual(RBOOT_SPIFFS_0 + 0x40200000, SPIFF_SIZE);
#else
          Debug.printf("trying to mount spiffs at %x, length %d", 0x40300000, SPIFF_SIZE);
          spiffs_mount_manual(0x40300000, SPIFF_SIZE);
#endif
        }
        else
        {
#ifdef RBOOT_SPIFFS_1
          Debug.printf("trying to mount spiffs at %x, length %d", RBOOT_SPIFFS_1 + 0x40200000, SPIFF_SIZE);
          spiffs_mount_manual(RBOOT_SPIFFS_1 + 0x40200000, SPIFF_SIZE);
#else
          Debug.printf("trying to mount spiffs at %x, length %d", 0x40500000, SPIFF_SIZE);
          spiffs_mount_manual(0x40500000, SPIFF_SIZE);
#endif
        }
      g_app.confSave();
      g_mqtt.confSave();
      g_network.confSave();
#else
      Debug.printf("spiffs disabled");
#endif
      // set to boot new rom and then reboot
      Debug.printf("Firmware updated, rebooting to rom %d...\r\n", slot);
      rboot_set_current_rom(slot);
      System.restart();
    }
    else
    {
      // fail
      Debug.println("Firmware update failed!");
      if (g_otaNumTrials < 6)
        StartOtaUpdate();
      else
        g_otaNumTrials = 0;
    }
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void otaUpdateHandler()
{
	uint8 slot;
	rboot_config bootconf;
	
	Debug.println("Updating...");
	
	// need a clean object, otherwise if run before and failed will not run again
	if (g_otaUpdater) delete g_otaUpdater;
	g_otaUpdater = new rBootHttpUpdate();
	
	// select rom slot to flash
	bootconf = rboot_get_config();
	slot = bootconf.current_rom;
	if (slot == 0) slot = 1; else slot = 0;

	// flash rom to position indicated in the rBoot config rom table
	g_otaUpdater->addItem(bootconf.roms[slot], g_otaBaseUrl + ROM_0_FNAME);
	
#ifndef DISABLE_SPIFFS
	// use user supplied values (defaults for 4mb flash in makefile)
	if (slot == 0) 
		g_otaUpdater->addItem(RBOOT_SPIFFS_0, g_otaBaseUrl + SPIFFS_FNAME);
	else
		g_otaUpdater->addItem(RBOOT_SPIFFS_1, g_otaBaseUrl + SPIFFS_FNAME);
#endif

	// set a callback (called on failure or success without switching requested)
	g_otaUpdater->setCallback(otaUpdateDelegate(otaUpdate_CallBack));

	// start update
	g_otaUpdater->start();
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void processUpgradeCommand(String commandLine, CommandOutput* out)
{
  Vector<String> commandToken;
  int numToken = splitString(commandLine, ' ' , commandToken);

  if (numToken != 2)
  {
    out->printf("Usage: upgrade <HTTP URL>\r\n");
  }
  else
  {
    g_otaBaseUrl = commandToken[1];
    out->printf("Starting upgrade with images from %s\r\n", g_otaBaseUrl.c_str());
    StartOtaUpdate();
    }
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void otaEnable()
{
  commandHandler.registerCommand(CommandDelegate("upgrade",
                                                 "Upgrade the system",
                                                 "System",
                                                 processUpgradeCommand));
  } //

