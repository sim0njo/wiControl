
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
  Network.softApEnable();
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
     	AppSettings.wired != (request.getPostParameter("wired") == "1");

    AppSettings.wired      = request.getPostParameter("wired") == "1";

    String oldApPass       = AppSettings.apPassword;
    AppSettings.apPassword = request.getPostParameter("apPassword");

    if (!AppSettings.apPassword.equals(oldApPass))
      softApSetPasswordTimer.initializeMs(500, apEnable).startOnce();

    AppSettings.ssid       = request.getPostParameter("ssid");
    AppSettings.password   = request.getPostParameter("password");
    AppSettings.portalUrl  = request.getPostParameter("portalUrl");
    AppSettings.portalData = request.getPostParameter("portalData");

    AppSettings.dhcp       = request.getPostParameter("dhcp") == "1";
    AppSettings.ip         = request.getPostParameter("ip");
    AppSettings.netmask    = request.getPostParameter("netmask");
    AppSettings.gateway    = request.getPostParameter("gateway");
    Debug.printf("Updating IP settings: %d", AppSettings.ip.isNull());
    AppSettings.save();
    
    Network.reconnect(500);

    if (connectionTypeChanges)
      processRestartCommandWeb();
    } // if POST

  TemplateFileStream *tmpl = new TemplateFileStream("network.html");
  auto &vars = tmpl->variables();

  vars["appAlias"]   = APP_ALIAS;

  vars["wiredon"]    = AppSettings.wired ? "checked='checked'" : "";
  vars["wiredoff"]   = AppSettings.wired ? "" : "checked='checked'";

  vars["ssid"]       = AppSettings.ssid;
  vars["password"]   = AppSettings.password;
  vars["apPassword"] = AppSettings.apPassword;

  vars["portalUrl"]  = AppSettings.portalUrl;
  vars["portalData"] = AppSettings.portalData;

  bool dhcp          = AppSettings.dhcp;
  vars["dhcpon"]     = dhcp  ? "checked='checked'" : "";
  vars["dhcpoff"]    = !dhcp ? "checked='checked'" : "";

  vars["ip"]         = Network.getClientIP().toString();
  vars["netmask"]    = Network.getClientMask().toString();
  vars["gateway"]    = Network.getClientGW().toString();

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

  if (AppSettings.apMode == apModeAlwaysOn ||
      AppSettings.apMode == apModeWhenDisconnected)
    softApEnable();
  else
    softApDisable();

  WifiStation.config(AppSettings.ssid, AppSettings.password);
  if (!AppSettings.dhcp && !AppSettings.ip.isNull())
    WifiStation.setIP(AppSettings.ip, AppSettings.netmask, AppSettings.gateway);

  // This will work both for wired and wireless
  wifi_set_event_handler_cb(network_cb);

  if (!AppSettings.wired)
  {
    if (AppSettings.ssid.equals(""))
    {
      WifiStation.enable(false);
      }
    else
    {
      reconnect(1);
      }
    }

  otaEnable();    
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::softApEnable()
{
    char id[16];

    if (AppSettings.apMode == apModeAlwaysOff ||
        AppSettings.apMode == apModeWhenDisconnected && connected)
    {
        Debug.println("Not enabling AP due to config setting");
        return;
    }

    WifiAccessPoint.enable(false);

    // Start AP for configuration
    sprintf(id, "%x", system_get_chip_id());
    if (AppSettings.apPassword.equals(""))
    {
        WifiAccessPoint.config((String)"WifiIoNode-" + id, "", AUTH_OPEN);
    }
    else
    {
        WifiAccessPoint.config((String)"WifiIoNode-" + id,
                               AppSettings.apPassword, AUTH_WPA_WPA2_PSK);
    }

    WifiAccessPoint.enable(true);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::softApDisable()
{
    if (AppSettings.apMode == apModeAlwaysOn ||
        AppSettings.apMode == apModeWhenDisconnected && !connected)
    {
        Debug.println("Not disabling AP due to config setting");
        return;
    }

    WifiAccessPoint.enable(false);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NetworkClass::handleEvent(System_Event_t *e)
{
  int event = e->event;

  if (event == EVENT_STAMODE_GOT_IP)
  {
    Debug.printf("NetworkClass::handleEvent,got ip-address\n");
        if (!haveIp && changeDlg)
        {
            changeDlg(true);

            // Disable SoftAP.
            // This function will check whether the AP can be disabled.
            softApDisable();
        }
        haveIp = true;

        // TODO make this work for wired!
        if (!AppSettings.portalUrl.equals(""))
        {
            String mac;
            uint8 hwaddr[6] = {0};
            wifi_get_macaddr(STATION_IF, hwaddr);
            for (int i = 0; i < 6; i++)
            {
                if (hwaddr[i] < 0x10) mac += "0";
                    mac += String(hwaddr[i], HEX);
                if (i < 5) mac += ":";
            }

            String body = AppSettings.portalData;
            body.replace("{ip}", WifiStation.getIP().toString());
            body.replace("{mac}", mac);
            portalLogin.setPostBody(body.c_str());
            String url = AppSettings.portalUrl;
            url.replace("{ip}", WifiStation.getIP().toString());
            url.replace("{mac}", mac);

            portalLogin.downloadString(
                url, HttpClientCompletedDelegate(&NetworkClass::portalLoginHandler, this));
        }

        ntpClient.requestTime();
    }
    else if (event == EVENT_STAMODE_CONNECTED)
    {
      if (!connected)
        {
            connected = true;
            Debug.printf("Wifi client got connected\n");
        }
        connected = true;
    }
    else if (event == EVENT_STAMODE_DISCONNECTED)
    {
        static int  numApNotFound = 0;
        static int  numOther = 0;

        if (e->event_info.disconnected.reason == REASON_NO_AP_FOUND)
        {
            numOther = 0;
            numApNotFound++;
            if (numApNotFound == 10)
            {
	        wifi_station_disconnect();
                reconnect(60000);
                numApNotFound = 0;
            }
        }
        else
        {
            numApNotFound = 0;
        }

        if (connected)
        {
            connected = false;
            Debug.printf("Wifi client got disconnected (%d)\n",
                         e->event_info.disconnected.reason);
            // Enable SoftAP.
            // This function will check whether the AP can be enabled.
            softApEnable();
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

  if (!WifiStation.getSSID().equals(AppSettings.ssid) ||
      !WifiStation.getPassword().equals(AppSettings.password))
    WifiStation.config(AppSettings.ssid, AppSettings.password, FALSE);

  if (!AppSettings.dhcp && !AppSettings.ip.isNull())
    WifiStation.setIP(AppSettings.ip, AppSettings.netmask, AppSettings.gateway);

  WifiStation.enable(true);
  wifi_station_connect();
  if (AppSettings.dhcp)
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
IPAddress NetworkClass::getClientIP()
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
IPAddress NetworkClass::getClientGW()
{
  return WifiStation.getNetworkGateway();
  } //

