
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <openHabMqttController.h>
#include <AppSettings.h>
#include <HTTP.h>

#if CONTROLLER_TYPE == CONTROLLER_TYPE_OPENHAB
OpenHabMqttController controller;
#endif

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void onGpiod(HttpRequest &request, HttpResponse &response)
{
  if (!HTTP.isHttpClientAllowed(request, response))
    return;

  // handle new settings
  if (request.getRequestMethod() == RequestMethod::POST) {
    } // if

  // send page
  TemplateFileStream *tmpl = new TemplateFileStream("gpiod.html");
  auto &vars = tmpl->variables();
  vars["appAlias"] = APP_ALIAS;
  response.sendTemplate(tmpl); // will be automatically deleted
  } // onGpiod

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void OpenHabMqttController::begin()
{
  checkTimer.initializeMs(1000, TimerDelegate(&OpenHabMqttController::checkConnection, this)).start(true);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void OpenHabMqttController::notifyChange(String object, String value)
{
  mqttPublishMessage(object, value);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void OpenHabMqttController::registerHttpHandlers(HttpServer &server)
{
  mqttRegisterHttpHandlers(server);
  server.addPath("/gpiod", onGpiod);
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void OpenHabMqttController::registerCommandHandlers()
{
  //
  } //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void OpenHabMqttController::checkConnection()
{
  if (WifiStation.isConnected())
    mqttCheckClient();
  } //

