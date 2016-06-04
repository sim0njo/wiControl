
#include <user_config.h>
#include <AppSettings.h>
#include <mqtt.h>
#include <HTTP.h>
#include <globals.h>
#include <gpiod.h>


// MQTT client
MqttClient*          g_pMqtt = NULL;
uint32_t             g_bMqttIsConnected = FALSE;
uint32_t             g_mqttPktRx = 0;
uint32_t             g_mqttPktRxDropped = 0;
uint32_t             g_mqttPktTx = 0;


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void     ICACHE_FLASH_ATTR mqttSubscribe(tCChar* szTopicFilter)
{
  Debug.logTxt(CLSLVL_MQTT | 0x0000, "mqttSubscribe,topicFilter=%s/%s",
               AppSettings.mqttClientId.c_str(), szTopicFilter);
  g_pMqtt->subscribe(AppSettings.mqttClientId + String("/") + String(szTopicFilter));
  } // mqttSubscribe

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR mqttPublish(tCChar* szPfx, tCChar* szTopic, tCChar* szMsg)
{
  if (!g_pMqtt)
    return;

  Debug.logTxt(CLSLVL_MQTT | 0x0000, "mqttPublish,topic=%s/%s/%s,msg=%s", 
               AppSettings.mqttClientId.c_str(), szPfx, szTopic, szMsg);
  Debug.logBin(CLSLVL_MQTT | 0x0010, 2, szTopic, gstrlen(szTopic), "mqttPublish,topic=%s/%s/%s,msg=%s", 
               AppSettings.mqttClientId.c_str(), szPfx, szTopic, szMsg);
  g_pMqtt->publish(AppSettings.mqttClientId + String("/") + String(szPfx) + String("/") + String(szTopic), String(szMsg));
  g_mqttPktTx++;
  } // mqttPublishMessage

//----------------------------------------------------------------------------
// MQTT client callback, topic=<client-id>/... msg=...
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR mqttOnPublish(String strTopic, String strMsg)
{
  tUint32 cbTopic = strTopic.length(), cbMsg = strMsg.length();

  do {
    // make sure topic starts with <client-id>
    Debug.logTxt(CLSLVL_MQTT | 0x0000, "mqttOnPublish,topic=%s,msg=%s", strTopic.c_str(), strMsg.c_str());
    if (strTopic.startsWith(AppSettings.mqttClientId + "/") == 0) {
      Debug.logTxt(CLSLVL_MQTT | 0x0010, "mqttOnPublish,not for us,dropping");
      g_mqttPktRxDropped++;
      return;
      } // if

    g_mqttPktRx++;

    // copy topic/msg to modifiable buffer on stack
    tChar szTopic[cbTopic + 1], szMsg[cbMsg + 1];
    tChar *pTopic = szTopic;
    gstrcpy(szTopic, strTopic.c_str());
    gstrcpy(szMsg, strMsg.c_str());

    // pass topic=<pfx>/<obj>... on to GPIOD
    pTopic += (AppSettings.mqttClientId.length() + 1);
    gpiodOnMqttPublish(pTopic, szMsg);
    } while (FALSE);

  } // mqttOnPublish

//----------------------------------------------------------------------------
// start MQTT client
//----------------------------------------------------------------------------
uint32_t ICACHE_FLASH_ATTR mqttStartClient()
{
  // delete existing instance
  Debug.logTxt(CLSLVL_MQTT | 0x0000, "mqttStartClient");
  if (g_pMqtt)
    delete g_pMqtt;

  AppSettings.load();
  if (!AppSettings.mqttServer.equals(String("")) && AppSettings.mqttPort != 0) {
    g_pMqtt = new MqttClient(AppSettings.mqttServer, AppSettings.mqttPort, mqttOnPublish);
    g_bMqttIsConnected = g_pMqtt->connect(AppSettings.mqttClientId, AppSettings.mqttUser, AppSettings.mqttPass);
    return g_bMqttIsConnected;
    }

  return FALSE;
  } // mqttStartClient

//----------------------------------------------------------------------------
// restart MQTT client if needed, return true if restarted
//----------------------------------------------------------------------------
uint32_t ICACHE_FLASH_ATTR mqttCheckClient()
{
  if (g_pMqtt && g_pMqtt->isProcessing())
    return FALSE;

  return mqttStartClient();
  } // mqttCheckClient

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void mqttOnHttpConfig(HttpRequest &request, HttpResponse &response)
{
  AppSettings.load();
  if (!g_http.isHttpClientAllowed(request, response))
    return;

  if (request.getRequestMethod() == RequestMethod::POST) {
    AppSettings.mqttServer   = request.getPostParameter("mqttHost");
    AppSettings.mqttPort     = atoi(request.getPostParameter("mqttPort").c_str());
    AppSettings.mqttUser     = request.getPostParameter("mqttUser");
    AppSettings.mqttPass     = request.getPostParameter("mqttPswd");
    AppSettings.mqttClientId = request.getPostParameter("mqttClientId");
    AppSettings.save();

    if (WifiStation.isConnected())
      mqttStartClient();
    } // if

  TemplateFileStream *tmpl = new TemplateFileStream("mqtt.html");
  auto &vars = tmpl->variables();

  vars["appAlias"]     = szAPP_ALIAS;
  vars["appAuthor"]    = szAPP_AUTHOR;
  vars["appDesc"]      = szAPP_DESC;

  vars["mqttHost"]     = AppSettings.mqttServer;
  vars["mqttPort"]     = AppSettings.mqttPort;
  vars["mqttUser"]     = AppSettings.mqttUser;
  vars["mqttPswd"]     = AppSettings.mqttPass;
  vars["mqttClientId"] = AppSettings.mqttClientId;
  response.sendTemplate(tmpl); // will be automatically deleted
  } // mqttOnHttpConfig

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
uint32_t mqttIsConnected(void) 
{
  if (g_pMqtt == NULL)
    return (FALSE);
  else
    return((g_pMqtt->getConnectionState() == eTCS_Connected));
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
String mqttServer(void)       { return AppSettings.mqttServer; }
