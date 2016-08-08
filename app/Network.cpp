
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>
#include <globals.h>
#include <AppSettings.h>
#include "Network.h"
#include <HTTP.h>

NetworkClass Network;
Timer        softApSetPasswordTimer;

void         processRestartCommandWeb(void);
extern void  otaEnable();


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void apEnable()
{
  Network.apEnable();
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void networkOnHttpConfig(HttpRequest &request, HttpResponse &response)
{
  if (!g_http.isHttpClientAllowed(request, response))
    return;

  if (request.getRequestMethod() == RequestMethod::POST) {
    bool connectionTypeChanges =
     	g_appCfg.netwWired != (request.getPostParameter("wired") == "1");

    g_appCfg.netwWired  = request.getPostParameter("wired") == "1";

    String oldApPass   = g_appCfg.apPswd;
    g_appCfg.apPswd = request.getPostParameter("apPassword");

    if (!g_appCfg.apPswd.equals(oldApPass))
      softApSetPasswordTimer.initializeMs(500, apEnable).startOnce();

    g_appCfg.staSSID    = request.getPostParameter("ssid");
    g_appCfg.staPswd    = request.getPostParameter("password");
    g_appCfg.portalUrl  = request.getPostParameter("portalUrl");
    g_appCfg.portalData = request.getPostParameter("portalData");

    g_appCfg.netwDHCP   = request.getPostParameter("dhcp") == "1";
    g_appCfg.netwAddr   = request.getPostParameter("ip");
    g_appCfg.netwMask   = request.getPostParameter("netmask");
    g_appCfg.netwGtwy   = request.getPostParameter("gateway");
    Debug.printf("Updating IP settings: %d", g_appCfg.netwAddr.isNull());
    g_appCfg.save();
    
    Network.reconnect(500);

    if (connectionTypeChanges)
      processRestartCommandWeb();
    } // if POST

  TemplateFileStream *tmpl = new TemplateFileStream("network.html");
  auto &vars = tmpl->variables();

  vars["appAlias"]     = szAPP_ALIAS;
  vars["appAuthor"]    = szAPP_AUTHOR;
  vars["appDesc"]      = szAPP_DESC;
  vars["mqttClientId"] = g_appCfg.mqttClientId;

  vars["wiredon"]      = g_appCfg.netwWired ? "checked='checked'" : "";
  vars["wiredoff"]     = g_appCfg.netwWired ? "" : "checked='checked'";

  vars["ssid"]         = g_appCfg.staSSID;
  vars["password"]     = g_appCfg.staPswd;
  vars["apPassword"]   = g_appCfg.apPswd;

  vars["portalUrl"]    = g_appCfg.portalUrl;
  vars["portalData"]   = g_appCfg.portalData;

  bool dhcp            = g_appCfg.netwDHCP;
  vars["dhcpon"]       = dhcp  ? "checked='checked'" : "";
  vars["dhcpoff"]      = !dhcp ? "checked='checked'" : "";

  vars["ip"]           = Network.getClientAddr().toString();
  vars["netmask"]      = Network.getClientMask().toString();
  vars["gateway"]      = Network.getClientGtwy().toString();

  response.sendTemplate(tmpl); // will be automatically deleted
  } // networkOnHttpConfig

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void network_cb ( System_Event_t *e )
{
  Network.handleEvent(e);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::begin(NetworkStateChangeDelegate dlg)
{
  WifiStation.enable(false);

  changeDlg = dlg;

  if (g_appCfg.apMode == apModeAlwaysOn ||
      g_appCfg.apMode == apModeWhenDisconnected)
    apEnable();
  else
    apDisable();

  WifiStation.config(g_appCfg.staSSID, g_appCfg.staPswd);
  if (!g_appCfg.netwDHCP && !g_appCfg.netwAddr.isNull())
    WifiStation.setIP(g_appCfg.netwAddr, g_appCfg.netwMask, g_appCfg.netwGtwy);

  // This will work both for wired and wireless
  wifi_set_event_handler_cb(network_cb);

  if (!g_appCfg.netwWired) {
    if (g_appCfg.staSSID.equals("")) {
      WifiStation.enable(false);
      }
    else {
      reconnect(1);
      }
    }

  otaEnable();    
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::apEnable()
{
  char id[16];

  if (g_appCfg.apMode == apModeAlwaysOff ||
      g_appCfg.apMode == apModeWhenDisconnected && connected) {
    Debug.println("Not enabling AP due to config setting");
    return;
    }

  WifiAccessPoint.enable(false);

  // Start AP for configuration
  sprintf(id, "%x", system_get_chip_id());
  if (g_appCfg.apPswd.equals("")) {
    WifiAccessPoint.config((String)"wiControl-" + id, "", AUTH_OPEN);
    }
  else {
    WifiAccessPoint.config((String)"wiControl-" + id, g_appCfg.apPswd, AUTH_WPA_WPA2_PSK);
    }

  WifiAccessPoint.enable(true);
  } // apEnable

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::apDisable()
{
  if (g_appCfg.apMode == apModeAlwaysOn ||
      g_appCfg.apMode == apModeWhenDisconnected && !connected) {
    Debug.println("Not disabling AP due to config setting");
    return;
    }

  WifiAccessPoint.enable(false);
  } // apDisable

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::handleEvent(System_Event_t *e)
{
  int event = e->event;

  if (event == EVENT_STAMODE_GOT_IP) {
//  Debug.logTxt(CLSLVL_NETWORK | 0x0000, "NetworkClass::handleEvent,got ip-address");
    Debug.printf("NetworkClass::handleEvent,got ip-address\n");
    if (!haveIp && changeDlg) {
      changeDlg(true);

      // Disable SoftAP.
      // This function will check whether the AP can be disabled.
      apDisable();
      }

    haveIp = true;

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

      portalLogin.downloadString(url, HttpClientCompletedDelegate(&NetworkClass::portalLoginHandler, this));
      } // if

    ntpClient.requestTime();
    }

  else if (event == EVENT_STAMODE_CONNECTED) {
    if (!connected) {
      connected = true;
      Debug.printf("Wifi client got connected\n");
      }

    connected = true;
    }

  else if (event == EVENT_STAMODE_DISCONNECTED) {
    static int  numApNotFound = 0;
    static int  numOther = 0;

    if (e->event_info.disconnected.reason == REASON_NO_AP_FOUND) {
      numOther = 0;
      numApNotFound++;
      if (numApNotFound == 10) {
        wifi_station_disconnect();
        reconnect(60000);
        numApNotFound = 0;
        }
      }
    else {
      numApNotFound = 0;
      }

    if (connected) {
      connected = false;
//    Debug.logTxt(CLSLVL_NETWORK | 0x0000, "NetworkClass::_inputOnConfig");
      Debug.printf("Wifi client got disconnected (%d)\n", e->event_info.disconnected.reason);

      // Enable SoftAP.
      // This function will check whether the AP can be enabled.
      apEnable();
      }

    connected = false;

    if (changeDlg)
      changeDlg(false);
    }
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::portalLoginHandler(HttpClient& client, bool successful)
{
  String response = client.getResponseString();
  Debug.println("Portal server response: '" + response + "'");
  } // 

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::connect()
{
  Debug.println("Connecting...");

  if (!WifiStation.getSSID().equals(g_appCfg.staSSID) ||
      !WifiStation.getPassword().equals(g_appCfg.staPswd))
    WifiStation.config(g_appCfg.staSSID, g_appCfg.staPswd, FALSE);

  if (!g_appCfg.netwDHCP && !g_appCfg.netwAddr.isNull())
    WifiStation.setIP(g_appCfg.netwAddr, g_appCfg.netwMask, g_appCfg.netwGtwy);

  WifiStation.enable(true);
  wifi_station_connect();
  if (g_appCfg.netwDHCP)
    wifi_station_dhcpc_start();

  wifi_station_set_reconnect_policy(true);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::reconnect(int delayMs)
{
  reconnectTimer.initializeMs(delayMs, TimerDelegate(&NetworkClass::connect, this)).startOnce();
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::ntpTimeResultHandler(NtpClient& client, time_t ntpTime)
{
  SystemClock.setTime(ntpTime, eTZ_UTC);
  Debug.print("Time after NTP sync: ");
  Debug.println(SystemClock.getSystemTimeString());
//    Clock.setTime(ntpTime);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
IPAddress NetworkClass::getClientAddr()
{
  return WifiStation.getIP();
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
IPAddress NetworkClass::getClientMask()
{
  return WifiStation.getNetworkMask();
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
IPAddress NetworkClass::getClientGtwy()
{
  return WifiStation.getNetworkGateway();
  } //

