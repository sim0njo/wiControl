
//----------------------------------------------------------------------------
// cgpiod.hpp
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef __cgpiod_hpp__
#define __cgpiod_hpp__

#ifndef INCLUDE_OPENHAB_MQTT_CONTROLLER_H_
#define INCLUDE_OPENHAB_MQTT_CONTROLLER_H_

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <controller.h>
#include <mqtt.h>

#define CGPIOD_VERSION                   "4.0.1.0" //                                   

#define CGPIOD_CMD_PFX                       "cmd" //
#define CGPIOD_EVT_PFX                       "evt" //

#define CGPIOD_MODE_OUTPUT                       0 // configured as out0-3
#define CGPIOD_MODE_SHUTTER                      1 // configured as udm0-1

#define CGPIOD_OUT0_PIN                          0 //
#define CGPIOD_OUT1_PIN                          2 //
#define CGPIOD_IN0_PIN                           5 //
#define CGPIOD_IN1_PIN                           4 //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class CGpiod {
 private:
  unsigned int       m_dwMode;                   //
  const char*        m_szCmdPfx;                   //
  const char*        m_szEvtPfx;                   //

 public:
  //--------------------------------------------------------------------------
  // constructor/destructor   
  //--------------------------------------------------------------------------
  CGpiod();
  ~CGpiod();

  //--------------------------------------------------------------------------
  // run daemon
  //--------------------------------------------------------------------------
  unsigned int       OnInit() {
    pinMode(OUTPUT0_PIN, OUTPUT);
    digitalWrite(OUTPUT0_PIN, HIGH);
    pinMode(OUTPUT1_PIN, OUTPUT);
    digitalWrite(OUTPUT1_PIN, HIGH);
    pinMode(INPUT0_PIN, INPUT);
    pinMode(INPUT1_PIN, INPUT);
    return 0;
    } //

  //--------------------------------------------------------------------------
  // run daemon
  //--------------------------------------------------------------------------
  unsigned int       OnRun();

  }; // CGpiod

extern CGpiod g_gpiod;


class OpenHabMqttController : public Controller
{
 public:
  void begin();
  void notifyChange(String object, String value);
  void registerHttpHandlers(HttpServer &server);
  void registerCommandHandlers();

  unsigned int       OnInit() {
    pinMode(OUTPUT0_PIN, OUTPUT);
    digitalWrite(OUTPUT0_PIN, HIGH);
    pinMode(OUTPUT1_PIN, OUTPUT);
    digitalWrite(OUTPUT1_PIN, HIGH);
    pinMode(INPUT0_PIN, INPUT);
    pinMode(INPUT1_PIN, INPUT);
    return 0;
    } //

 private:
  void checkConnection();
  void onRun();

 private:
  Timer checkTimer;
  };

#endif //INCLUDE_OPENHAB_MQTT_CONTROLLER_H_

#endif // __cgpiod_hpp__
