
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
  char              str[32];
  DynamicJsonBuffer jsonBuffer;
  tUint32           dwVersion;

  if (!confExists()) 
    confSave();

  if (confExists()) {
    int size = fileGetSize(CMQTT_CONF_FILE);
    char* strJson = new char[size + 1];

    fileGetContent(CMQTT_CONF_FILE, strJson, size + 1);
    JsonObject& root = jsonBuffer.parseObject(strJson);

//  dwVersion = root["version"];
//  Debug.printf("CMqtt::confLoad,version = %u\r\n", dwVersion);
//  if (!root.containsKey("version")) {
//    }

    m_strHost   = (const char *)root["host"];
    m_dwPort    =               root["port"];
    m_strUser   = (const char *)root["user"];
    m_strPswd   = (const char *)root["pswd"];
    m_strNodeId = (const char *)root["nodeId"];

    if (m_strNodeId.equals("")) {
      sprintf(str, "%x", system_get_chip_id());
      m_strNodeId = str;
      confSave();
      Debug.printf("CMqtt::confLoad,nodeId = %s\r\n", m_strNodeId.c_str());
      }

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

  root["host"]   = m_strHost.c_str();
  root["port"]   = m_dwPort;
  root["user"]   = m_strUser.c_str();
  root["pswd"]   = m_strPswd.c_str();
  root["nodeId"] = m_strNodeId.c_str();

  // write to file
  String strRoot;
  root.printTo(strRoot);
  fileSetContent(CMQTT_CONF_FILE, strRoot);
  } // confSave

