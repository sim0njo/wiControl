
#include <user_config.h>
#include <SmingCore.h>
#include <HTTP.h>
#include <mqtt.h>
#include <Network.h>
#include <AppSettings.h>
#include <Services/WebHelpers/base64.h>
#include <Wiring/SplitString.h>
#include <ats.h>
#include <gpiod.h>

// Forward declarations
void                 StartOtaUpdateWeb(String);
void                 processRestartCommandWeb(void);

HTTPClass            g_http;


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void httpOnStatus(HttpRequest &request, HttpResponse &response)
{
  char               buf[200], str0[16], str1[16], str2[16], str3[16];
  TemplateFileStream *tmpl = new TemplateFileStream("status.html");
  auto               &vars = tmpl->variables();

  vars["appAlias"]     = szAPP_ALIAS;
  vars["appAuthor"]    = szAPP_AUTHOR;
  vars["appDesc"]      = szAPP_DESC;
  vars["mqttClientId"] = g_appCfg.mqttClientId;

  vars["ssid"] = g_appCfg.staSSID;
  vars["wifiStatus"] = g_isNetworkConnected ? "Connected" : "Not connected";
    
  bool dhcp = g_appCfg.netwDHCP;
  if (dhcp)
    vars["ipOrigin"] = "DHCP";
  else
    vars["ipOrigin"] = "Static";

  if (!Network.getClientAddr().isNull())
  {
    vars["ip"] = Network.getClientAddr().toString();
    }
  else
  {
    vars["ip"] = "0.0.0.0";
    vars["ipOrigin"] = "not configured";
    }

  if (g_appCfg.mqttServer != "")
  {
    vars["mqttHost"]   = g_appCfg.mqttServer;
    vars["mqttPort"]   = g_appCfg.mqttPort;
    vars["mqttStatus"] = mqttIsConnected() ? "Connected":"Not connected";
    }
  else
  {
    vars["mqttHost"]   = "0.0.0.0";
    vars["mqttPort"]   = "1883";
    vars["mqttStatus"] = "Not configured";
    }

  vars["gpiodVersion"]  = szAPP_VERSION;
  vars["gpiodTopology"] = szAPP_TOPOLOGY;

  vars["gpiodEmul"]    = (g_appCfg.gpiodEmul    == CGPIOD_EMUL_OUTPUT)  ? "output"   :
                         (g_appCfg.gpiodEmul    == CGPIOD_EMUL_SHUTTER) ? "shutter"  : "<unknown>";
  vars["gpiodMode"]    = (g_appCfg.gpiodMode    == CGPIOD_MODE_LOCAL)   ? "local"    :
                         (g_appCfg.gpiodMode    == CGPIOD_MODE_MQTT)    ? "MQTT"     :
                         (g_appCfg.gpiodMode    == CGPIOD_MODE_BOTH)    ? "both"     : "<unknown>";
  vars["gpiodLock"]    = (g_appCfg.gpiodLock    == CGPIOD_LOCK_TRUE)    ? "disabled" : "enabled";
  vars["gpiodDisable"] = (g_appCfg.gpiodDisable == CGPIOD_DISABLE_TRUE) ? "disabled" : "enabled";

  sprintf(buf, "%s/%s/%s/%s",
          g_gpiod.PrintObjSta2String(str0, CGPIOD_OBJ_CLS_INPUT | 0, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 0)),
          g_gpiod.PrintObjSta2String(str1, CGPIOD_OBJ_CLS_INPUT | 1, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 1)),
          g_gpiod.PrintObjSta2String(str2, CGPIOD_OBJ_CLS_INPUT | 2, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 2)),
          g_gpiod.PrintObjSta2String(str3, CGPIOD_OBJ_CLS_INPUT | 3, g_gpiod.GetState(CGPIOD_OBJ_CLS_INPUT | 3)));
  vars["gpiodInputs"] = buf;

  if (g_appCfg.gpiodEmul == CGPIOD_EMUL_OUTPUT)
    sprintf(buf, "%s/%s/%s/%s",
            g_gpiod.PrintObjSta2String(str0, CGPIOD_OBJ_CLS_OUTPUT | 0, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 0)),
            g_gpiod.PrintObjSta2String(str1, CGPIOD_OBJ_CLS_OUTPUT | 1, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 1)),
            g_gpiod.PrintObjSta2String(str2, CGPIOD_OBJ_CLS_OUTPUT | 2, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 2)),
            g_gpiod.PrintObjSta2String(str3, CGPIOD_OBJ_CLS_OUTPUT | 3, g_gpiod.GetState(CGPIOD_OBJ_CLS_OUTPUT | 3)));
  else
    sprintf(buf, "%s/%s",
            g_gpiod.PrintObjSta2String(str0, CGPIOD_OBJ_CLS_SHUTTER | 0, g_gpiod.GetState(CGPIOD_OBJ_CLS_SHUTTER | 0)),
            g_gpiod.PrintObjSta2String(str1, CGPIOD_OBJ_CLS_SHUTTER | 1, g_gpiod.GetState(CGPIOD_OBJ_CLS_SHUTTER | 1)));
  vars["gpiodOutputs"] = buf;

   
  // --- System info -------------------------------------------------
  sprintf (buf, "%x", system_get_chip_id());
  vars["systemVersion"]  = build_git_sha;
  vars["systemBuild"]    = build_time;
  vars["systemFreeHeap"] = system_get_free_heap_size();
  vars["systemStartup"]  = "todo";
  vars["systemChipId"]   = buf;
  uint32_t curMillis     = millis();
  sprintf(buf, "%d s, %03d ms : ", curMillis / 1000, curMillis % 1000);
  vars["systemUptime"]   = buf;


  // --- Statistics --------------------------------------------------
  vars["mqttRx"]        = g_mqttPktRx;
  vars["mqttRxDropped"] = g_mqttPktRxDropped;
  vars["mqttTx"]        = g_mqttPktTx;
    
  response.sendTemplate(tmpl); // will be automatically deleted
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void httpOnTools(HttpRequest &request, HttpResponse &response)
{
    g_appCfg.load();

    if (!g_http.isHttpClientAllowed(request, response))
        return;

    if (request.getRequestMethod() == RequestMethod::POST)
    {
        String command = request.getPostParameter("Command");
        
        if (command.equals("Upgrade")) {
            g_appCfg.webOtaBaseUrl = request.getPostParameter("webOtaBaseUrl");

            g_appCfg.save();

            Debug.println("Going to call: StartOtaUpdateWeb()");
            StartOtaUpdateWeb(g_appCfg.webOtaBaseUrl);
            Debug.println("Called: StartOtaUpdateWeb()");
        }
        else if (command.equals("Restart")) {
            Debug.println("Going to call: processRestartCommandWeb()");
            processRestartCommandWeb();
            Debug.println("Called: processRestartCommandWeb()");
        }
        else {
            Debug.printf("Unknown command: [%s]\r\n", command.c_str());
        }

    }

  TemplateFileStream *tmpl = new TemplateFileStream("tools.html");
  auto &vars = tmpl->variables();

  vars["appAlias"]     = szAPP_ALIAS;
  vars["appAuthor"]    = szAPP_AUTHOR;
  vars["appDesc"]      = szAPP_DESC;
  vars["mqttClientId"] = g_appCfg.mqttClientId;

  vars["webOtaBaseUrl"] = g_appCfg.webOtaBaseUrl;

  response.sendTemplate(tmpl); // will be automatically deleted
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void httpOnFile(HttpRequest &request, HttpResponse &response)
{
  if (!g_http.isHttpClientAllowed(request, response))
    return;

  String file = request.getPath();
  if (file[0] == '/')
    file = file.substring(1);

  if (file[0] == '.')
    response.forbidden();
  else
    {
    response.setCache(86400, true); // It's important to use cache for better performance.

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    Debug.printf("httpOnFile,get %s\n", file.c_str());
    response.sendFile(file);
    }
  } // httpOnFile

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
bool HTTPClass::isHttpClientAllowed(HttpRequest &request, HttpResponse &response)
{
    if (g_appCfg.apPswd.equals(""))
        return true;

    String authHeader = request.getHeader("Authorization");
    char userpass[32+1+32+1];
    if (!authHeader.equals("") && authHeader.startsWith("Basic"))
    {
        Debug.printf("Authorization header %s\n", authHeader.c_str());
        int r = base64_decode(authHeader.length()-6,
                              authHeader.substring(6).c_str(),
                              sizeof(userpass),
                              (unsigned char *)userpass);
        if (r > 0)
        {
            userpass[r]=0; //zero-terminate user:pass string
            Debug.printf("Authorization header decoded %s\n", userpass);
            String validUserPass = "admin:" + g_appCfg.apPswd;
            if (validUserPass.equals(userpass))
            {
                return true;
            }
        }
    }

    response.authorizationRequired();
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("WWW-Authenticate",
                       String("Basic realm=\"MySensors Gateway ") + system_get_chip_id() + "\"");
    return false;
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void HTTPClass::wsConnected(WebSocket& socket)
{
    //
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void HTTPClass::wsMessageReceived(WebSocket& socket, const String& message)
{
  Vector<String> commandToken;
  int numToken = splitString((String &)message,' ' , commandToken);

  if (numToken > 0 && wsCommandHandlers.contains(commandToken[0])) {
    Serial.printf("WebSocket command received: %s\r\n", commandToken[0].c_str());
    wsCommandHandlers[commandToken[0]](socket, message);
    return;
    }
  else {
    Serial.printf("WebSocket received unknown string: %s\r\n", message.c_str());
    String response = "Unknown command: " + message;
    socket.sendString(response);
    }
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void HTTPClass::wsBinaryReceived(WebSocket& socket, uint8_t* data, size_t size)
{
	Serial.printf("Websocket binary data recieved, size: %d\r\n", size);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void HTTPClass::wsDisconnected(WebSocket& socket)
{
    //
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void HTTPClass::addWsCommand(String command, WebSocketMessageDelegate callback)
{
  debugf("'%s' registered", command.c_str());
  wsCommandHandlers[command] = callback;
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void HTTPClass::notifyWsClients(String message)
{
  WebSocketsList &clients = m_server.getActiveWebSockets();
  for (int i = 0; i < clients.count(); i++)
    clients[i].sendString(message);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void HTTPClass::begin()
{
  m_server.listen(80);
  m_server.enableHeaderProcessing("Authorization");
  m_server.addPath("/",        httpOnStatus);
  m_server.addPath("/status",  httpOnStatus);
  m_server.addPath("/network", networkOnHttpConfig);
  m_server.addPath("/tools",   httpOnTools);
  m_server.addPath("/mqtt",    mqttOnHttpConfig);
  m_server.addPath("/gpiod",   gpiodOnHttpConfig);
  m_server.addPath("/ats",     atsOnHttpQuery);

  m_server.setDefaultHandler(httpOnFile);

  // Web Sockets configuration
  m_server.enableWebSockets(true);
  m_server.setWebSocketConnectionHandler(WebSocketDelegate(&HTTPClass::wsConnected, this));
  m_server.setWebSocketMessageHandler(WebSocketMessageDelegate(&HTTPClass::wsMessageReceived, this));
  m_server.setWebSocketBinaryHandler(WebSocketBinaryDelegate(&HTTPClass::wsBinaryReceived, this));
  m_server.setWebSocketDisconnectionHandler(WebSocketDelegate(&HTTPClass::wsDisconnected, this));
  } //

