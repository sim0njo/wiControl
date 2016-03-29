
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>
#include <AppSettings.h>
#include <mqtt.h>
#include <HTTP.h>
#include <globals.h>

// Forward declarations
void mqttOnPublish(String topic, String message);

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

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
      }
    } // for

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
  } // getValue

//int updateSensorStateInt(int node, int sensor, int type, int value);
//int getTypeFromString(String type);


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR mqttOnPublish(String strTopic, String strMsg)
{
  /*
   * Supported topics:
   *   /? => send version info
   *   <clientid>/<cmdpfx>/<object>, i.e. astr76b32/L3R1W2/cmd/shutter0=up.0.5
   */

  // check to be sure, but we should not receive other messages
  if (strTopic.startsWith(AppSettings.mqttClientId + String("/") + AppSettings.mqttCmdPfx + "/")) {
    g_dwMqttPktRx++;

    String strObj = getValue(strTopic, '/', 3);
    Debug.println();
    Debug.println();
    Debug.println(strObj);
    Debug.println(strMsg);
    //strip leading V_
//    type.remove(0,2);
//      Debug.println(MyGateway::getSensorTypeFromString(type));

//      updateSensorStateInt(node.toInt(), sensor.toInt(),
//                           MyGateway::getSensorTypeFromString(type),
//                           message.toInt());
    }

//  if (strTopic.equals(String("/?")))
//  {
//      mqttPublishVersion();
//      return;
//  }

  Debug.println("mqttOnPublish: " + strTopic + " = " + strMsg);
  } // mqttOnPublish

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
    g_pMqtt = new MqttClient(AppSettings.mqttServer, AppSettings.mqttPort, mqttOnPublish);
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
