/*
 * AppSettings.h
 *
 *  Created on: 13 мая 2015 г.
 *      Author: Anakod
 */

#ifndef INCLUDE_APPSETTINGS_H_
#define INCLUDE_APPSETTINGS_H_

#include <SmingCore/SmingCore.h>
#include "globals.h"

#define APP_SETTINGS_FILE ".settings.conf" // leading point for security reasons :)

typedef enum
{
    apModeAlwaysOn = 1,
    apModeAlwaysOff,
    apModeWhenDisconnected
} eAppMode;

class ApplicationSettingsStorage
{
 public:
  bool               wired = false;
  String             ssid;
  String             password;
  eAppMode           apMode = apModeAlwaysOn;
  String             apPassword;

  String             portalUrl;
  String             portalData;

  bool               dhcp = true;

  IPAddress          ip;
  IPAddress          netmask;
  IPAddress          gateway;

  String             mqttUser;
  String             mqttPass;
  String             mqttServer;
  int                mqttPort = 1883;
  String             mqttClientId;

  bool               cpuBoost = true;
  bool               useOwnBaseAddress = true;

  String             webOtaBaseUrl;

  unsigned int       gpiodEmul = 0;
  unsigned int       gpiodMode = 3;
  unsigned int       gpiodLock = 0;
  unsigned int       gpiodDisable = 0;

  unsigned int       gpiodInDebounce[4] = { 100, 100, 100, 100 };
  unsigned int       gpiodOutDefRun[4]  = {   0,   0,   0,   0 };
  unsigned int       gpiodUdmDefRun[4]  = {  30,  30,  30,  30 };

  unsigned int       dwLogLevel = 0x3FFF;

  void               load();
  void               save();
  bool               exist() { return fileExist(APP_SETTINGS_FILE); }
  };

extern ApplicationSettingsStorage AppSettings;

#endif /* INCLUDE_APPSETTINGS_H_ */
