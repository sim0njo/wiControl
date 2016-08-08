/*
 * AppSettings.h
 *
 *  Created on: 13 мая 2015 г.
 *      Author: Anakod
 */

#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>
#include <AppSettings.h>

void ApplicationSettingsStorage::load()
{
  char              str[64];
  DynamicJsonBuffer jsonBuffer;

  if (exist()) {
    int size = fileGetSize(APP_SETTINGS_FILE);
    char* jsonString = new char[size + 1];
    fileGetContent(APP_SETTINGS_FILE, jsonString, size + 1);
    JsonObject& root = jsonBuffer.parseObject(jsonString);

    JsonObject& network = root["network"];

    netwWired = false;

    staSSID    = (const char *)network["staSSID"];
    staPswd    = (const char *)network["staPswd"];
    apPswd     = (const char *)network["apPswd"];
    portalUrl  = (const char *)network["portalUrl"];
    portalData = (const char *)network["portalData"];

    if (!network.containsKey("apMode"))
      apMode = apModeAlwaysOn;
    else {
      String newMode = (const char *)network["apMode"];
      if (newMode.equals("always"))
        apMode = apModeAlwaysOn;
      else if (newMode.equals("never"))
        apMode = apModeAlwaysOff;
      else if (newMode.equals("whenDisconnected"))
        apMode = apModeWhenDisconnected;
      else
        apMode = apModeAlwaysOff;
      } // else

    netwDHCP = network["dhcp"];
    netwAddr = (const char *)network["addr"];
    netwMask = (const char *)network["mask"];
    netwGtwy = (const char *)network["gtwy"];

    JsonObject& mqtt = root["mqtt"];
    mqttUser     = (const char *)mqtt["user"];
    mqttPswd     = (const char *)mqtt["pswd"];
    mqttServer   = (const char *)mqtt["server"];
    mqttPort     = mqtt["port"];
    mqttClientId = (const char *)mqtt["clientId"];

    if (mqttClientId.equals(String(""))) {
      sprintf(str, "ESP_%08X", system_get_chip_id());
      mqttClientId = (const char *) str; 
      }

    JsonObject& gpiod = root["gpiod"];
    gpiodEmul    = gpiod["emul"];
    gpiodMode    = gpiod["mode"];
    gpiodLock    = gpiod["lock"];
    gpiodDisable = gpiod["disable"];

    gpiodInDebounce[0] = gpiod["in0Debounce"];
    gpiodInDebounce[1] = gpiod["in1Debounce"];
    gpiodInDebounce[2] = gpiod["in2Debounce"];
    gpiodInDebounce[3] = gpiod["in3Debounce"];

    gpiodOutDefRun[0] = gpiod["out0DefRun"];
    gpiodOutDefRun[1] = gpiod["out1DefRun"];
    gpiodOutDefRun[2] = gpiod["out2DefRun"];
    gpiodOutDefRun[3] = gpiod["out3DefRun"];

    gpiodUdmDefRun[0] = gpiod["udm0DefRun"];
    gpiodUdmDefRun[1] = gpiod["udm1DefRun"];
    gpiodUdmDefRun[2] = gpiod["udm2DefRun"];
    gpiodUdmDefRun[3] = gpiod["udm3DefRun"];

    cpuBoost = root["cpuBoost"];

    webOtaBaseUrl = (const char *)root["webOtaBaseUrl"];

    delete[] jsonString;
    }
  } //

void ApplicationSettingsStorage::save()
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  JsonObject& network = jsonBuffer.createObject();
  root["network"]       = network;
  network["wired"]      = netwWired;
  network["staSSID"]    = staSSID.c_str();
  network["staPswd"]    = staPswd.c_str();
  network["apPswd"]     = apPswd.c_str();
  network["portalUrl"]  = portalUrl.c_str();
  network["portalData"] = portalData.c_str();

  if (apMode == apModeAlwaysOn)
    network["apMode"] = "always";
  else if (apMode == apModeAlwaysOff)
    network["apMode"] = "never";
  else
    network["apMode"] = "whenDisconnected";

  network["dhcp"] = netwDHCP;

  // Make copy by value for temporary string objects
  network.set("addr", netwAddr.toString());
  network.set("mask", netwMask.toString());
  network.set("gtwy", netwGtwy.toString());

  JsonObject& mqtt = jsonBuffer.createObject();
  root["mqtt"] = mqtt;
  mqtt.set("user", mqttUser);
  mqtt.set("pswd", mqttPswd);
  mqtt.set("server", mqttServer);
  mqtt["port"] = mqttPort;
  mqtt.set("clientId", mqttClientId);

  JsonObject& gpiod = jsonBuffer.createObject();
  root["gpiod"]    = gpiod;
  gpiod["emul"]    = gpiodEmul;
  gpiod["mode"]    = gpiodMode;
  gpiod["lock"]    = gpiodLock;
  gpiod["disable"] = gpiodDisable;

  gpiod["in0Debounce"] = gpiodInDebounce[0];
  gpiod["in1Debounce"] = gpiodInDebounce[1];
  gpiod["in2Debounce"] = gpiodInDebounce[2];
  gpiod["in3Debounce"] = gpiodInDebounce[3];

  gpiod["out0DefRun"]  = gpiodOutDefRun[0];
  gpiod["out1DefRun"]  = gpiodOutDefRun[1];
  gpiod["out2DefRun"]  = gpiodOutDefRun[2];
  gpiod["out3DefRun"]  = gpiodOutDefRun[3];

  gpiod["udm0DefRun"]  = gpiodUdmDefRun[0];
  gpiod["udm1DefRun"]  = gpiodUdmDefRun[1];
  gpiod["udm2DefRun"]  = gpiodUdmDefRun[2];
  gpiod["udm3DefRun"]  = gpiodUdmDefRun[3];
 
  root["cpuBoost"] = cpuBoost;

  root.set("webOtaBaseUrl", webOtaBaseUrl.c_str());

  //TODO: add direct file stream writing
  String out;
  root.printTo(out);
  fileSetContent(APP_SETTINGS_FILE, out);
  } //

ApplicationSettingsStorage g_appCfg;
