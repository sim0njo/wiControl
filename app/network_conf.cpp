
//----------------------------------------------------------------------------
// network_conf.cpp : network configuration
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include "network.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
bool CNetwork::confExists() {
  return fileExist(CNETWORK_CONF_FILE);
  } // confExists

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CNetwork::confDelete() {
  if (confExists()) 
    fileDelete(CNETWORK_CONF_FILE);
  } // confDelete

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CNetwork::confLoad()
{
  DynamicJsonBuffer jsonBuffer;

  if (!confExists()) 
    confSave();

  if (confExists()) {
    int size = fileGetSize(CNETWORK_CONF_FILE);
    char* strJson = new char[size + 1];

    fileGetContent(CNETWORK_CONF_FILE, strJson, size + 1);
    JsonObject& root = jsonBuffer.parseObject(strJson);

    if (!root.containsKey("apMode"))
      m_apMode = CNETWORK_AP_MODE_ALWAYS_ON;
    else {
      String apMode = (const char *)root["apMode"];
      if (apMode.equals("always"))
        m_apMode = CNETWORK_AP_MODE_ALWAYS_ON;
      else if (apMode.equals("never"))
        m_apMode = CNETWORK_AP_MODE_ALWAYS_OFF;
      else
        m_apMode = CNETWORK_AP_MODE_WHEN_DISCONNECTED;
      } // else
    m_apPswd  = (const char *)root["apPswd"];

    m_staSSID = (const char *)root["staSSID"];
    m_staPswd = (const char *)root["staPswd"];
    m_staDHCP = root["staDHCP"];
    m_staAddr = (const char *)root["staAddr"];
    m_staMask = (const char *)root["staMask"];
    m_staGtwy = (const char *)root["staGtwy"];

    delete[] strJson;
    }
  } // confLoad

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CNetwork::confSave()
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["apMode"]  = (m_apMode == CNETWORK_AP_MODE_ALWAYS_ON)  ? "always" :
                    (m_apMode == CNETWORK_AP_MODE_ALWAYS_OFF) ? "never"  : "whenDisconnected";
  root["apPswd"]  =  m_apPswd.c_str();
  
  root["staSSID"] = m_staSSID.c_str();
  root["staPswd"] = m_staPswd.c_str();
  root["staDHCP"] = m_staDHCP;
  root["staAddr"] = m_staAddr.toString();
  root["staMask"] = m_staMask.toString();
  root["staGtwy"] = m_staGtwy.toString();

  // write to file
  String strRoot;
  root.printTo(strRoot);
  fileSetContent(CNETWORK_CONF_FILE, strRoot);
  } // confSave

