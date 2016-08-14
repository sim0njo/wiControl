
//----------------------------------------------------------------------------
// http.h
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef __http_hpp__
#define __http_hpp__

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>

class CHttp {
 public:
  void               Init();
  bool               isClientAllowed(HttpRequest &request, HttpResponse &response);

  void               addWsCommand(String command, WebSocketMessageDelegate callback);
  void               notifyWsClients(String message);

 private:
  // Websocket handlers
  void               wsConnected(WebSocket& socket);
  void               wsMessageReceived(WebSocket& socket, const String& message);
  void               wsBinaryReceived(WebSocket& socket, uint8_t* data, size_t size);
  void               wsDisconnected(WebSocket& socket);

 private:
  HttpServer         m_server;
  HashMap<String, WebSocketMessageDelegate> wsCommandHandlers;
  }; //

extern CHttp     g_http;

#endif // __http_hpp__
