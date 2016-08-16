
//----------------------------------------------------------------------------
// globals.h
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef __globals_hpp__
#define __globals_hpp__

extern const char* build_time;
extern const char* build_git_sha;


#define szAPP_ALIAS                    "wiControl" //
#define szAPP_AUTHOR     "Jo Simons (c) 2015-2016" //
#define szAPP_DESC       "Wireless Control Module" //
#define APP_VERSION                     0x04000200 //
#define szAPP_VERSION                    "4.0.2.0" //

//#define TOPOLOGY_4I4O_ED01 
//#define TOPOLOGY_4I4O_ED02 
#define TOPOLOGY_4I4O_ED03 

#ifdef TOPOLOGY_4I4O_ED01
#define szAPP_TOPOLOGY                    "4I4O-1" // 4 input,4 output,2 shutter
#define TOPOLOGY_HAS_INPUT
#define TOPOLOGY_HAS_OUTPUT
#endif

#ifdef TOPOLOGY_4I4O_ED02
#define szAPP_TOPOLOGY                    "4I4O-2" // 4 input,4 output,2 shutter
#define TOPOLOGY_HAS_INPUT
#define TOPOLOGY_HAS_OUTPUT
#endif

#ifdef TOPOLOGY_4I4O_ED03
#define szAPP_TOPOLOGY                    "4I4O-3" // 4 input,4 output,2 shutter
#define TOPOLOGY_HAS_INPUT
#define TOPOLOGY_HAS_OUTPUT
#endif

//#define PLATFORM_TYPE_GENERIC                    1 //
//#define PLATFORM_TYPE_SDSHIELD                   2 //
//#define PLATFORM_TYPE_WEMOS_WITH_SHIELDS         2 //

//#define CONTROLLER_TYPE_OPENHAB                  1 //
//#define CONTROLLER_TYPE_CLOUD                    2 //

//#define RTC_TYPE_3213                            1 //
//#define RTC_TYPE_1307                            2 //

#define WIRED_ETHERNET_NONE                      0 //
#define WIRED_ETHERNET_W5100                     1 //
#define WIRED_ETHERNET_W5500                     2 //

#endif // __globals_hpp__
