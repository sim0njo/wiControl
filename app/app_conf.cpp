
//----------------------------------------------------------------------------
// app_conf.cpp : main application configuration
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <app.h>

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
bool CApplication::confExists() {
  return fileExist(CAPP_CONF_FILE);
  } // confExists

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::confDelete() {
  if (confExists()) 
    fileDelete(CAPP_CONF_FILE);
  } // confDelete

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::confLoad()
{
  DynamicJsonBuffer jsonBuffer;

  if (!confExists()) 
    confSave();

  if (confExists()) {
    int   size = fileGetSize(CAPP_CONF_FILE);
    char* strJson = new char[size + 1];

    fileGetContent(CAPP_CONF_FILE, strJson, size + 1);
    JsonObject& root = jsonBuffer.parseObject(strJson);

    m_nodeId             = (const char *)root["nodeId"];
    m_cpuBoost           = root["cpuBoost"];
    m_otaBaseUrl         = (const char *)root["otaBaseUrl"];

    m_gpiodEmul          = root["emul"];
    m_gpiodMode          = root["mode"];
    m_gpiodLock          = root["lock"];
    m_gpiodDisable       = root["disable"];

    m_gpiodInDebounce[0] = root["in0Debounce"];
    m_gpiodInDebounce[1] = root["in1Debounce"];
    m_gpiodInDebounce[2] = root["in2Debounce"];
    m_gpiodInDebounce[3] = root["in3Debounce"];

    m_gpiodOutDefRun[0]  = root["out0DefRun"];
    m_gpiodOutDefRun[1]  = root["out1DefRun"];
    m_gpiodOutDefRun[2]  = root["out2DefRun"];
    m_gpiodOutDefRun[3]  = root["out3DefRun"];

    m_gpiodUdmDefRun[0]  = root["udm0DefRun"];
    m_gpiodUdmDefRun[1]  = root["udm1DefRun"];

    delete[] strJson;
    }
  } // confLoad

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CApplication::confSave()
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  if (confExists()) 
    confDelete();

  root["nodeId"]      = m_nodeId.c_str();
  root["cpuBoost"]    = m_cpuBoost;
  root["otaBaseUrl"]  = m_otaBaseUrl.c_str();

  root["emul"]        = m_gpiodEmul;
  root["mode"]        = m_gpiodMode;
  root["lock"]        = m_gpiodLock;
  root["disable"]     = m_gpiodDisable;

  root["in0Debounce"] = m_gpiodInDebounce[0];
  root["in1Debounce"] = m_gpiodInDebounce[1];
  root["in2Debounce"] = m_gpiodInDebounce[2];
  root["in3Debounce"] = m_gpiodInDebounce[3];

  root["out0DefRun"]  = m_gpiodOutDefRun[0];
  root["out1DefRun"]  = m_gpiodOutDefRun[1];
  root["out2DefRun"]  = m_gpiodOutDefRun[2];
  root["out3DefRun"]  = m_gpiodOutDefRun[3];

  root["udm0DefRun"]  = m_gpiodUdmDefRun[0];
  root["udm1DefRun"]  = m_gpiodUdmDefRun[1];

  // write to file
  String strRoot;
  root.printTo(strRoot);
  fileSetContent(CAPP_CONF_FILE, strRoot);
  } // confSave

