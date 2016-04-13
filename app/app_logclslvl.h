
//----------------------------------------------------------------------------
// app_logclslvl.h
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
// loglevels                     class level
//
// app                           0     00
// httpc                         0     06
//
//----------------------------------------------------------------------------
#ifndef __app_logclslvl_h__
#define __app_logclslvl_h__

#include "xstdafx.h"

// definitions
#define CLSLVL_APP                    DEBUG_LVL_00 //
#define CLSLVL_MQTT                   DEBUG_LVL_01 //
#define CLSLVL_HTTP                   DEBUG_LVL_02 //
#define CLSLVL_NETWORK                DEBUG_LVL_03 //
                                               
#define CLSLVL_GPIOD                  DEBUG_LVL_08 // 0x0100
#define CLSLVL_GPIOD_INPUT            DEBUG_LVL_09 // 0x0200
#define CLSLVL_GPIOD_OUTPUT           DEBUG_LVL_0A // 0x0400
#define CLSLVL_GPIOD_SHUTTER          DEBUG_LVL_0B // 0x0800
#define CLSLVL_GPIOD_SYSTEM           DEBUG_LVL_0C // 0x1000
                                                          
#endif // __app_logclslvl_h__
