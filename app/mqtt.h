
//----------------------------------------------------------------------------
// mqtt.h
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef __mqtt_h__
#define __mqtt_h__
#include <SmingCore/SmingCore.h>
#include <xstdafx.h>

#define CMQTT_CONF_FILE               ".mqtt.conf" //

class CMqtt {
 private:
  String             m_strHost;
  tUint32            m_dwPort = 1883;
  String             m_strUser;
  String             m_strPswd;
  String             m_strNodeId;

  MqttClient*        m_pClient = NULL;
  tUint32            m_bIsConnected = FALSE;

  tUint32            m_dwPktRx = 0;
  tUint32            m_dwPktRxDropped = 0;
  tUint32            m_dwPktTx = 0;

 public:
  // mqtt.cpp
  tUint32            StartClient();
  tUint32            CheckClient();
  void               Subscribe(tCChar* szTopicFilter);
  void               Publish(tCChar* szPfx, tCChar* szTopic, tCChar* szMsg);
  tUint32            IsConnected();

  void               OnHttpConfig(HttpRequest &request, HttpResponse &response);
  void               OnPublish(String strTopic, String strMsg);

  // mqtt_attr.cpp
  tCChar*            GetStrAttr(tCChar* szAttr);
  tUint32            GetNumAttr(tCChar* szAttr);

  // mqtt_conf.cpp
  bool               confExists();
  void               confDelete();
  void               confLoad();
  void               confSave();

  };

extern CMqtt g_mqtt;
/*
// mqtt.cpp
uint32_t             mqttStartClient();
uint32_t             mqttCheckClient();
void                 mqttSubscribe(tCChar* szTopicFilter);
void                 mqttPublish(tCChar* szPfx, tCChar* szTopic, tCChar* szMsg);
void                 mqttOnHttpConfig(HttpRequest &request, HttpResponse &response);
uint32_t             mqttIsConnected();

extern uint32_t      g_mqttPktRx;
extern uint32_t      g_mqttPktRxDropped;
extern uint32_t      g_mqttPktTx;
*/
#endif //__mqtt_h__
