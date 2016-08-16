
//----------------------------------------------------------------------------
// mqtt.cpp : MQTT client wrapper
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <user_config.h>
#include <SmingCore.h>
#include <http.h>
#include <mqtt.h>
#include <network.h>
#include <Services/WebHelpers/base64.h>
#include <Wiring/SplitString.h>
#include <ats.h>
#include <gpiod.h>

CHttp                g_http;


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void httpOnFile(HttpRequest &request, HttpResponse &response)
{
  if (!g_http.isClientAllowed(request, response))
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
bool CHttp::isClientAllowed(HttpRequest &request, HttpResponse &response)
{
  if (!strcmp(g_network.GetStrAttr("apPswd"), ""))
      return true;

  String authHeader = request.getHeader("Authorization");
  char userpass[32+1+32+1];
  if (!authHeader.equals("") && authHeader.startsWith("Basic")) {
    Debug.printf("Authorization header %s\n", authHeader.c_str());
    int r = base64_decode(authHeader.length() - 6, authHeader.substring(6).c_str(),
                          sizeof(userpass), (unsigned char *)userpass);

    if (r > 0) {
      userpass[r]=0; //zero-terminate user:pass string
      Debug.printf("Authorization header decoded %s\n", userpass);
      String validUserPass = "admin:"; // + g_network.GetStrAttr("apPswd");
      validUserPass += g_network.GetStrAttr("apPswd");
      if (validUserPass.equals(userpass)) {
        return true;
        }
      }
    }

  response.authorizationRequired();
  response.setHeader("Content-Type", "text/plain");
  response.setHeader("WWW-Authenticate",
                     String("Basic realm=\"MySensors Gateway ") + system_get_chip_id() + "\"");
  return false;
  } // isClientAllowed

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CHttp::wsConnected(WebSocket& socket)
{
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CHttp::wsMessageReceived(WebSocket& socket, const String& message)
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
void CHttp::wsBinaryReceived(WebSocket& socket, uint8_t* data, size_t size)
{
	Serial.printf("Websocket binary data recieved, size: %d\r\n", size);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CHttp::wsDisconnected(WebSocket& socket)
{
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CHttp::addWsCommand(String command, WebSocketMessageDelegate callback)
{
  debugf("'%s' registered", command.c_str());
  wsCommandHandlers[command] = callback;
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CHttp::notifyWsClients(String message)
{
  WebSocketsList &clients = m_server.getActiveWebSockets();
  for (int i = 0; i < clients.count(); i++)
    clients[i].sendString(message);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CHttp::Init()
{
  m_server.listen(80);
  m_server.enableHeaderProcessing("Authorization");
  m_server.addPath("/",        HttpPathDelegate(&CApplication::httpOnStatus, &g_app));
  m_server.addPath("/status",  HttpPathDelegate(&CApplication::httpOnStatus, &g_app));
  m_server.addPath("/network", HttpPathDelegate(&CNetwork::OnHttpConfig, &g_network));
  m_server.addPath("/tools",   HttpPathDelegate(&CApplication::httpOnTools, &g_app));
  m_server.addPath("/mqtt",    HttpPathDelegate(&CMqtt::OnHttpConfig, &g_mqtt));
  m_server.addPath("/gpiod",   gpiodOnHttpConfig);
  m_server.addPath("/ats",     atsOnHttpQuery);

  m_server.setDefaultHandler(httpOnFile);

  // Web Sockets configuration
  m_server.enableWebSockets(true);
  m_server.setWebSocketConnectionHandler(WebSocketDelegate(&CHttp::wsConnected, this));
  m_server.setWebSocketMessageHandler(WebSocketMessageDelegate(&CHttp::wsMessageReceived, this));
  m_server.setWebSocketBinaryHandler(WebSocketBinaryDelegate(&CHttp::wsBinaryReceived, this));
  m_server.setWebSocketDisconnectionHandler(WebSocketDelegate(&CHttp::wsDisconnected, this));
  } // Init

