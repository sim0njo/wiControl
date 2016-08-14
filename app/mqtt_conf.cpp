
//----------------------------------------------------------------------------
// mqtt_conf.cpp : MQTT configuration
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include "mqtt.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
bool CMqtt::confExists() {
  return fileExist(CMQTT_CONF_FILE);
  } // confExists

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CMqtt::confDelete() {
  if (confExists()) 
    fileDelete(CMQTT_CONF_FILE);
  } // confDelete

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CMqtt::confLoad()
{
  DynamicJsonBuffer jsonBuffer;

  if (!confExists()) 
    confSave();

  if (confExists()) {
    int size = fileGetSize(CMQTT_CONF_FILE);
    char* strJson = new char[size + 1];

    fileGetContent(CMQTT_CONF_FILE, strJson, size + 1);
    JsonObject& root = jsonBuffer.parseObject(strJson);

    m_strHost     = (const char *)root["Host"];
    m_dwPort      =               root["Port"];
    m_strUser     = (const char *)root["User"];
    m_strPswd     = (const char *)root["Pswd"];
//  m_strClientId = (const char *)root["ClientId"];

    delete[] strJson;
    }
  } // confLoad

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CMqtt::confSave()
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  if (confExists()) 
    confDelete();

  root["Host"]     = m_strHost.c_str();
  root["Port"]     = m_dwPort;
  root["User"]     = m_strUser.c_str();
  root["Pswd"]     = m_strPswd.c_str();
//root["ClientId"] = m_strClientId.c_str();

  // write to file
  String strRoot;
  root.printTo(strRoot);
  fileSetContent(CMQTT_CONF_FILE, strRoot);
  } // confSave

