
//----------------------------------------------------------------------------
// network.cpp : network implementation
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>
#include <globals.h>
#include "network.h"
#include <http.h>
#include <app.h>

CNetwork             g_network;
Timer                g_apSetPasswordTimer;

extern void  otaEnable();


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void networkOnEvent(System_Event_t *e)
{
  g_network.OnEvent(e);
  } // networkOnEvent

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CNetwork::Init(NetworkEventDelegate dlg)
{
  WifiStation.enable(false);

  confDelete();
  m_staSSID = "ASTR76-R1";
  m_staPswd = "alcatel-lucent";

  confLoad();
  m_OnEvent = dlg;

  if (m_apMode == CNETWORK_AP_MODE_ALWAYS_ON ||
      m_apMode == CNETWORK_AP_MODE_WHEN_DISCONNECTED)
    apEnable();
  else
    apDisable();

  WifiStation.config(m_staSSID, m_staPswd);

  if (!m_staDHCP && !m_staAddr.isNull())
    WifiStation.setIP(m_staAddr, m_staMask, m_staGtwy);

  // This will work both for wired and wireless
  wifi_set_event_handler_cb(networkOnEvent);

//if (!m_bWired) {
    if (m_staSSID.equals("")) {
      WifiStation.enable(false);
      }
    else {
      staReconnect(1);
      }
//  }

  otaEnable();    
  } // Init

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tCChar* CNetwork::GetStrAttr(tCChar* szAttr) 
{
  if      (!strcmp(szAttr, "staSSID"))
    return m_staSSID.c_str();

  else if (!strcmp(szAttr, "staPswd"))
    return m_staPswd.c_str();

  else if (!strcmp(szAttr, "staAddr"))
    return WifiStation.getIP().toString().c_str();

  else if (!strcmp(szAttr, "staMask"))
    return WifiStation.getNetworkMask().toString().c_str();

  else if (!strcmp(szAttr, "staGtwy"))
    return WifiStation.getNetworkGateway().toString().c_str();

  return "";
  } //

tUint32 CNetwork::GetNumAttr(tCChar* szAttr) 
{
  if      (!strcmp(szAttr, "apMode"))
    return m_apMode;

  else if (!strcmp(szAttr, "staDHCP"))
    return m_staDHCP;

  return 0;
  } //

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
void CNetwork::OnHttpConfig(HttpRequest &request, HttpResponse &response)
{
  if (!g_http.isClientAllowed(request, response))
    return;

  if (request.getRequestMethod() == RequestMethod::POST) {
//  bool connectionTypeChanges =
//   	m_bWired != (request.getPostParameter("wired") == "1");

//  m_bWired  = request.getPostParameter("wired") == "1";

    String oldApPswd = m_apPswd;
    m_apPswd = request.getPostParameter("apPswd");

    if (!m_apPswd.equals(oldApPswd))
      g_apSetPasswordTimer.initializeMs(500, TimerDelegate(&CNetwork::apEnable, this)).startOnce();

    m_staSSID    = request.getPostParameter("staSSID");
    m_staPswd    = request.getPostParameter("staPswd");

    m_staDHCP   = request.getPostParameter("staDHCP") == "1";
    m_staAddr   = request.getPostParameter("staAddr");
    m_staMask   = request.getPostParameter("staMask");
    m_staGtwy   = request.getPostParameter("staGtwy");
//  Debug.printf("Updating IP settings: %d", m_staAddr.isNull());
    confSave();
    
    staReconnect(500);

//  if (connectionTypeChanges && m_OnEvent)
//    m_OnEvent(CNETWORK_EVT_NEEDSRESTART);
    } // if POST

  TemplateFileStream *tmpl = new TemplateFileStream("network.html");
  auto &vars = tmpl->variables();

  vars["appAlias"]  = szAPP_ALIAS;
  vars["appAuthor"] = szAPP_AUTHOR;
  vars["appDesc"]   = szAPP_DESC;
  vars["appNodeId"] = g_app.GetStrAttr("nodeId");

  vars["staSSID"]   = m_staSSID;
  vars["staPswd"]   = m_staPswd;
  vars["apPswd"]    = m_apPswd;

  vars["dhcpon"]    = m_staDHCP  ? "checked='checked'" : "";
  vars["dhcpoff"]   = !m_staDHCP ? "checked='checked'" : "";

  vars["staAddr"]   = staClientAddr().toString();
  vars["staMask"]   = staClientMask().toString();
  vars["staGtwy"]   = staClientGtwy().toString();

  response.sendTemplate(tmpl); // will be automatically deleted
  } // CNetwork::OnHttpConfig

//----------------------------------------------------------------------------
// callback command handler
//----------------------------------------------------------------------------
void CNetwork::OnCmdApMode(String commandLine, CommandOutput* pOut)
{
  Vector<String> commandToken;
  int numToken = splitString(commandLine, ' ' , commandToken);

    if (numToken != 2 ||
        (commandToken[1] != "always" && commandToken[1] != "never" &&
         commandToken[1] != "whenDisconnected"))
    {
      pOut->printf("Usage : \r\n\r\n");
      pOut->printf("  apMode always           : Always have the AP enabled\r\n");
      pOut->printf("  apMode never            : Never have the AP enabled\r\n");
      pOut->printf("  apMode whenDisconnected : Only enable the AP when disconnected\r\n");
      pOut->printf("                            from the network\r\n");
      return;
      }

    if      (commandToken[1] == "always")
      m_apMode = CNETWORK_AP_MODE_ALWAYS_ON;
    else if (commandToken[1] == "never")
      m_apMode = CNETWORK_AP_MODE_ALWAYS_OFF;
    else
      m_apMode = CNETWORK_AP_MODE_WHEN_DISCONNECTED;

  confSave();

  if (m_OnEvent)
    m_OnEvent(CNETWORK_EVT_NEEDSRESTART);

  } // CNetwork::OnCmdApMode

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CNetwork::apEnable()
{
  char SSID[32];

  if (m_apMode == CNETWORK_AP_MODE_ALWAYS_OFF ||
      m_apMode == CNETWORK_AP_MODE_WHEN_DISCONNECTED && m_staConnected) {
    Debug.println("Not enabling AP due to config setting");
    return;
    }

  WifiAccessPoint.enable(false);

  // Start AP for configuration
  sprintf(SSID, "%s-%x", szAPP_ALIAS, system_get_chip_id());

  if (m_apPswd.equals("")) 
    WifiAccessPoint.config(SSID, "", AUTH_OPEN);
//    WifiAccessPoint.config((String)"wiControl-" + id, "", AUTH_OPEN);
  else 
//    WifiAccessPoint.config((String)"wiControl-" + id, m_apPswd, AUTH_WPA_WPA2_PSK);
    WifiAccessPoint.config(SSID, m_apPswd, AUTH_WPA_WPA2_PSK);

  WifiAccessPoint.enable(true);
  } // CNetwork::apEnable

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CNetwork::apDisable()
{
  if (m_apMode == CNETWORK_AP_MODE_ALWAYS_ON ||
      m_apMode == CNETWORK_AP_MODE_WHEN_DISCONNECTED && !m_staConnected) {
    Debug.println("Not disabling AP due to config setting");
    return;
    }

  WifiAccessPoint.enable(false);
  } // CNetwork::apDisable

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CNetwork::OnEvent(System_Event_t *e)
{
  int event = e->event;

  if (event == EVENT_STAMODE_GOT_IP) {
//  Debug.logTxt(CLSLVL_NETWORK | 0x0100, "CNetwork::OnEvent,got ip-address");
    Debug.printf("CNetwork::handleEvent,got ip-address\n");
    if (!m_bHasIp && m_OnEvent) {
//  if (!m_bHasIp) {
      m_OnEvent(CNETWORK_EVT_CONNECTED);

      // Disable SoftAP.
      // This function will check whether the AP can be disabled.
      apDisable();
      }

    m_bHasIp = true;
/*
    // TODO make this work for wired!
    if (!g_appCfg.portalUrl.equals("")) {
      String mac;
      uint8 hwaddr[6] = {0};
      wifi_get_macaddr(STATION_IF, hwaddr);

      for (int i = 0; i < 6; i++) {
        if (hwaddr[i] < 0x10) mac += "0";
        mac += String(hwaddr[i], HEX);
        if (i < 5) mac += ":";
        }

      String body = g_appCfg.portalData;
      body.replace("{ip}", WifiStation.getIP().toString());
      body.replace("{mac}", mac);
      portalLogin.setPostBody(body.c_str());
      String url = g_appCfg.portalUrl;
      url.replace("{ip}", WifiStation.getIP().toString());
      url.replace("{mac}", mac);

      portalLogin.downloadString(url, HttpClientCompletedDelegate(&CNetwork::portalLoginHandler, this));
      } // if
*/
    m_ntpClient.requestTime();
    }

  else if (event == EVENT_STAMODE_CONNECTED) {
    if (!m_staConnected) {
//      m_staConnected = true;
      Debug.printf("Wifi client got connected\n");
      }

    m_staConnected = true;
    }

  else if (event == EVENT_STAMODE_DISCONNECTED) {
    static int  numApNotFound = 0;
    static int  numOther = 0;

    if (e->event_info.disconnected.reason == REASON_NO_AP_FOUND) {
      numOther = 0;
      numApNotFound++;
      if (numApNotFound == 10) {
        wifi_station_disconnect();
        staReconnect(60000);
        numApNotFound = 0;
        }
      }
    else {
      numApNotFound = 0;
      }

    if (m_staConnected) {
      m_staConnected = false;
//    Debug.logTxt(CLSLVL_NETWORK | 0x0000, "CNetwork::OnEvent,client disconnected (%d)", e->event_info.disconnected.reason);
      Debug.printf("Wifi client got disconnected (%d)\n", e->event_info.disconnected.reason);

      // This function will check whether the AP can be enabled.
      apEnable();
      }

    m_staConnected = false;

    if (m_OnEvent)
      m_OnEvent(CNETWORK_EVT_DISCONNECTED);
    }
  } // CNetwork::OnEvent

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CNetwork::staConnect()
{
  Debug.println("Connecting...");

  if (!WifiStation.getSSID().equals(m_staSSID) ||
      !WifiStation.getPassword().equals(m_staPswd))
    WifiStation.config(m_staSSID, m_staPswd, FALSE);

  if (!m_staDHCP && !m_staAddr.isNull())
    WifiStation.setIP(m_staAddr, m_staMask, m_staGtwy);

  WifiStation.enable(true);
  wifi_station_connect();
  if (m_staDHCP)
    wifi_station_dhcpc_start();

  wifi_station_set_reconnect_policy(true);
  } // staConnect

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CNetwork::staReconnect(int delayMs)
{
  m_timerReconnect.initializeMs(delayMs, TimerDelegate(&CNetwork::staConnect, this)).startOnce();
  } // staReconnect

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CNetwork::OnNtpTimeResult(NtpClient& client, time_t ntpTime)
{
  SystemClock.setTime(ntpTime, eTZ_UTC);
  Debug.print("Time after NTP sync: ");
  Debug.println(SystemClock.getSystemTimeString());
//    Clock.setTime(ntpTime);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
IPAddress CNetwork::staClientAddr()
{
  return WifiStation.getIP();
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
IPAddress CNetwork::staClientMask()
{
  return WifiStation.getNetworkMask();
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
IPAddress CNetwork::staClientGtwy()
{
  return WifiStation.getNetworkGateway();
  } //

