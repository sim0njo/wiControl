
//----------------------------------------------------------------------------
// mqtt_attr.cpp : MQTT attributes
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include "mqtt.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tCChar* CMqtt::GetStrAttr(tCChar* szAttr) 
{
  if      (!strcmp(szAttr, "mqttHost"))
    return m_strHost.c_str();

  else if (!strcmp(szAttr, "mqttUser"))
    return m_strUser.c_str();

  else if (!strcmp(szAttr, "mqttPswd"))
    return m_strPswd.c_str();

  else if (!strcmp(szAttr, "mqttClientId"))
    return m_strClientId.c_str();

  return "";
  } //

tUint32 CMqtt::GetNumAttr(tCChar* szAttr) 
{
  if      (!strcmp(szAttr, "mqttPort"))
    return m_dwPort;

  else if (!strcmp(szAttr, "mqttPktRx"))
    return m_dwPktRx;

  else if (!strcmp(szAttr, "mqttPktRxDropped"))
    return m_dwPktRxDropped;

  else if (!strcmp(szAttr, "mqttPktTx"))
    return m_dwPktTx;

  return 0;
  } //

