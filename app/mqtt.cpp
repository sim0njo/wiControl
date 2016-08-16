
//----------------------------------------------------------------------------
// mqtt.cpp : MQTT client wrapper
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <user_config.h>
#include <mqtt.h>
#include <http.h>
#include <globals.h>
#include <gpiod.h>

CMqtt                g_mqtt;

//----------------------------------------------------------------------------
// callback function
//----------------------------------------------------------------------------
void mqttOnPublish(String strTopic, String strMsg)
{
  g_mqtt.OnPublish(strTopic, strMsg);
  } //

//----------------------------------------------------------------------------
// restart MQTT client if needed, return true if restarted
//----------------------------------------------------------------------------
tUint32 CMqtt::CheckClient()
{
  if (m_pClient && m_pClient->isProcessing())
    return FALSE;

  return StartClient();
  } // CMqtt::CheckClient

//----------------------------------------------------------------------------
// start MQTT client  MqttStringSubscriptionCallback
//----------------------------------------------------------------------------
tUint32 CMqtt::StartClient()
{
  // delete existing instance
  Debug.logTxt(CLSLVL_MQTT | 0x0000, "CMqtt::StartClient");
  if (m_pClient) {
    delete m_pClient;
    m_pClient = 0;
    }

  confLoad();

  if (!m_strHost.equals(String("")) && m_dwPort != 0) {
    m_pClient = new MqttClient(m_strHost, m_dwPort, mqttOnPublish);
//  m_pClient = new MqttClient(m_strHost, m_dwPort, MqttStringSubscriptionCallback(CMqtt::OnPublish, this));
    m_bIsConnected = m_pClient->connect(m_strNodeId, m_strUser, m_strPswd);
    return m_bIsConnected;
    }

  return FALSE;
  } // CMqtt::StartClient

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CMqtt::Subscribe(tCChar* szTopicFilter)
{
  if (!m_pClient) {
    Debug.logTxt(CLSLVL_MQTT | 0x0010, "CMqtt::Subscribe,MQTT client not running");
    return;
    } // if

  Debug.logTxt(CLSLVL_MQTT | 0x0000, "CMqtt::Subscribe,topicFilter=%s/%s",
               m_strNodeId.c_str(), szTopicFilter);
  m_pClient->subscribe(m_strNodeId + String("/") + String(szTopicFilter));
  } // CMqtt::Subscribe

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CMqtt::Publish(tCChar* szPfx, tCChar* szTopic, tCChar* szMsg)
{
  if (!m_pClient) {
    Debug.logTxt(CLSLVL_MQTT | 0x0010, "CMqtt::Publish,MQTT client not running");
    return;
    } // if

  Debug.logTxt(CLSLVL_MQTT | 0x0030, "CMqtt::Publish,topic=%s/%s/%s,msg=%s", 
               m_strNodeId.c_str(), szPfx, szTopic, szMsg);
  m_pClient->publish(m_strNodeId + String("/") + String(szPfx) + String("/") + String(szTopic), String(szMsg));
  m_dwPktTx++;
  } // CMqtt::Publish

//----------------------------------------------------------------------------
// MQTT client callback, topic=<client-id>/... msg=...
//----------------------------------------------------------------------------
void CMqtt::OnPublish(String strTopic, String strMsg)
{
  tUint32 cbTopic = strTopic.length(), cbMsg = strMsg.length();

  do {
    // make sure topic starts with <client-id>
    Debug.logTxt(CLSLVL_MQTT | 0x0000, "CMqtt::OnPublish,topic=%s,msg=%s", strTopic.c_str(), strMsg.c_str());
    if (strTopic.startsWith(m_strNodeId + "/") == 0) {
      Debug.logTxt(CLSLVL_MQTT | 0x0010, "CMqtt::OnPublish,not for us,dropping");
      m_dwPktRxDropped++;
      return;
      } // if

    m_dwPktRx++;

    // copy topic/msg to modifiable buffer on stack
    tChar szTopic[cbTopic + 1], szMsg[cbMsg + 1];
    tChar *pTopic = szTopic;
    gstrcpy(szTopic, strTopic.c_str());
    gstrcpy(szMsg, strMsg.c_str());

    // pass topic=<pfx>/<obj>... on to GPIOD
    pTopic += (m_strNodeId.length() + 1);
    gpiodOnMqttPublish(pTopic, szMsg);
    } while (FALSE);

  } // CMqtt::OnPublish

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CMqtt::OnHttpConfig(HttpRequest &request, HttpResponse &response)
{
  if (!g_http.isClientAllowed(request, response))
    return;

  confLoad();

  if (request.getRequestMethod() == RequestMethod::POST) {
    m_strHost     = request.getPostParameter("mqttHost");
    m_dwPort      = atoi(request.getPostParameter("mqttPort").c_str());
    m_strUser     = request.getPostParameter("mqttUser");
    m_strPswd     = request.getPostParameter("mqttPswd");
    m_strNodeId   = request.getPostParameter("mqttNodeId");
    confSave();
    g_app.Restart(0);

//  if (WifiStation.isConnected())
//    StartClient();
    } // if

  TemplateFileStream *tmpl = new TemplateFileStream("mqtt.html");
  auto &vars = tmpl->variables();

  vars["appAlias"]   = szAPP_ALIAS;
  vars["appAuthor"]  = szAPP_AUTHOR;
  vars["appDesc"]    = szAPP_DESC;

  vars["mqttHost"]   = m_strHost;
  vars["mqttPort"]   = m_dwPort;
  vars["mqttUser"]   = m_strUser;
  vars["mqttPswd"]   = m_strPswd;
  vars["mqttNodeId"] = m_strNodeId;
  response.sendTemplate(tmpl); // will be automatically deleted
  } // CMqtt::OnHttpConfig

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CMqtt::IsConnected(void) 
{
  if (m_pClient == NULL)
    return (FALSE);
  else
    return (m_pClient->getConnectionState() == eTCS_Connected);
  } //

