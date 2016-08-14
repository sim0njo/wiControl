
//----------------------------------------------------------------------------
// network.h
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef __network_h__
#define __network_h__
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>
#include <xstdafx.h>

// definitions
#define CNETWORK_CONF_FILE         ".network.conf" //

#define CNETWORK_AP_MODE_ALWAYS_OFF              0 //
#define CNETWORK_AP_MODE_WHEN_DISCONNECTED       1 //
#define CNETWORK_AP_MODE_ALWAYS_ON               2 //

#define CNETWORK_STA_STATE_DISCONNECTED          0 //
#define CNETWORK_STA_STATE_CONNECTED             1 //
#define CNETWORK_STA_STATE_HAS_ADDRESS           2 //

#define CNETWORK_EVT_NONE                        0 //
#define CNETWORK_EVT_DISCONNECTED                1 //
#define CNETWORK_EVT_CONNECTED                   2 //
#define CNETWORK_EVT_NEEDSRESTART                3 //

typedef Delegate<void(int)> NetworkEventDelegate;

//void                 networkOnHttpConfig(HttpRequest &request, HttpResponse &response);

class CNetwork {
 public:
  CNetwork() : m_ntpClient(NTP_DEFAULT_SERVER,
                           0,
                           NtpTimeResultDelegate(&CNetwork::OnNtpTimeResult, this))
  {
//  m_bWired       = false;
    m_apMode       = CNETWORK_AP_MODE_ALWAYS_ON;
    m_staDHCP      = true;
    m_staConnected = false;
    };

  tCChar*            GetStrAttr(tCChar* szAttr);
  tUint32            GetNumAttr(tCChar* szAttr);

  void               Init(NetworkEventDelegate dlg = NULL);

  void               OnHttpConfig(HttpRequest &request, HttpResponse &response);
  void               OnCmdApMode(String commandLine, CommandOutput* pOut);

  void               apEnable();
  void               apDisable();

  void               staConnect();
  void               staReconnect(int delayMs);
  bool               staConnected() { return m_staConnected; }

  IPAddress          staClientAddr();
  IPAddress          staClientMask();
  IPAddress          staClientGtwy();
    
  // network_conf.cpp
  bool               confExists();
  void               confDelete();
  void               confLoad();
  void               confSave();

  void               OnEvent(System_Event_t *e);

 private:
  void               OnNtpTimeResult(NtpClient& client, time_t ntpTime);

 private:
//int                m_bWired;
  
  tUint32            m_apMode = CNETWORK_AP_MODE_ALWAYS_ON;
  String             m_apPswd;

  String             m_staSSID;
  String             m_staPswd;
  bool               m_staDHCP = true;
  IPAddress          m_staAddr;
  IPAddress          m_staMask;
  IPAddress          m_staGtwy;

  bool               m_staConnected = false;
  Timer              m_timerReconnect;

  NetworkEventDelegate m_OnEvent;
  bool               m_bHasIp = false;
  NtpClient          m_ntpClient;
  };

extern CNetwork g_network;

#endif //__network_h__
