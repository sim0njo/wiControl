
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>
#include <AppSettings.h>
#include <mqtt.h>
#include <HTTP.h>
#include <globals.h>
#include <gpiod.h>

// Forward declarations

// MQTT client
MqttClient*          g_pMqtt = NULL;
bool                 g_bMqttIsConfigured = FALSE;
bool                 g_bMqttIsConnected = FALSE;
unsigned long        g_dwMqttPktRx = 0;
unsigned long        g_dwMqttPktTx = 0;


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR mqttPublishMessage(String strTopic, String strMsg)
{
  if (!g_pMqtt)
    return;

  g_pMqtt->publish(AppSettings.mqttClientId + String("/") + AppSettings.mqttEvtPfx + String("/") + strTopic, strMsg);
  g_dwMqttPktTx++;
  } // mqttPublishMessage

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR mqttPublishVersion()
{
  g_pMqtt->publish(AppSettings.mqttClientId + String("/") + AppSettings.mqttEvtPfx + String("/version"), APP_ALIAS);
  g_dwMqttPktTx++;
  } // mqttPublishVersion

// Callback for messages, arrived from MQTT server

//int updateSensorStateInt(int node, int sensor, int type, int value);
//int getTypeFromString(String type);


//----------------------------------------------------------------------------
// start MQTT client
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR mqttStartClient()
{
  // delete existing instance
  if (g_pMqtt)
    delete g_pMqtt;

  AppSettings.load();
  if (!AppSettings.mqttServer.equals(String("")) && AppSettings.mqttPort != 0) {
    g_pMqtt = new MqttClient(AppSettings.mqttServer, AppSettings.mqttPort, gpiodOnPublish);
    g_bMqttIsConnected = g_pMqtt->connect(AppSettings.mqttClientId, AppSettings.mqttUser, AppSettings.mqttPass);

    g_pMqtt->subscribe(AppSettings.mqttClientId + String("/") + AppSettings.mqttCmdPfx + String("/#"));
    mqttPublishVersion();
    g_bMqttIsConfigured = TRUE;
    }
  } // mqttStartClient

//----------------------------------------------------------------------------
// restart MQTT client if needed
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR mqttCheckClient()
{
  if (g_pMqtt && g_pMqtt->isProcessing())
    return;

  mqttStartClient();
  } // mqttCheckClient

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void mqttOnConfig(HttpRequest &request, HttpResponse &response)
{
  AppSettings.load();
  g_bMqttIsConfigured = FALSE;

  if (!g_http.isHttpClientAllowed(request, response))
    return;

  if (request.getRequestMethod() == RequestMethod::POST) {
    AppSettings.mqttUser     = request.getPostParameter("user");
    AppSettings.mqttPass     = request.getPostParameter("password");
    AppSettings.mqttServer   = request.getPostParameter("server");
    AppSettings.mqttPort     = atoi(request.getPostParameter("port").c_str());
    AppSettings.mqttClientId = request.getPostParameter("clientId");
    AppSettings.mqttEvtPfx   = request.getPostParameter("evtPfx");
    AppSettings.mqttCmdPfx   = request.getPostParameter("cmdPfx");
    AppSettings.save();

    if (WifiStation.isConnected())
      mqttStartClient();
    } // if

  TemplateFileStream *tmpl = new TemplateFileStream("mqtt.html");
  auto &vars = tmpl->variables();

  vars["appAlias"] = APP_ALIAS;
  vars["user"]     = AppSettings.mqttUser;
  vars["password"] = AppSettings.mqttPass;
  vars["server"]   = AppSettings.mqttServer;
  vars["port"]     = AppSettings.mqttPort;
  vars["clientId"] = AppSettings.mqttClientId;
  vars["evtPfx"]   = AppSettings.mqttEvtPfx;
  vars["cmdPfx"]   = AppSettings.mqttCmdPfx;
  response.sendTemplate(tmpl); // will be automatically deleted
  } // mqttOnConfig

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR mqttRegisterHttpHandlers(HttpServer &server)
{
  server.addPath("/mqtt", mqttOnConfig);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
bool mqttIsConnected(void) 
{
  if (g_pMqtt == NULL)
    return (FALSE);
  else
    return((g_pMqtt->getConnectionState() == eTCS_Connected));
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
bool   mqttIsConfigured(void) { return g_bMqttIsConfigured; }

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
String mqttServer(void)       { return AppSettings.mqttServer; }
