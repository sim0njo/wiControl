/*globals.h*/
#ifndef GLOBALS_H_
#define GLOBALS_H_


extern const char* build_time;
extern const char* build_git_sha;
extern int         g_isNetworkConnected;
/* why is type bool not known ?*/


#define szAPP_ALIAS                    "wiControl" //
#define szAPP_AUTHOR     "Jo Simons (c) 2015-2016" //
#define szAPP_DESC       "Wireless Control Module" //
#define APP_VERSION                     0x04000000 //
#define szAPP_VERSION                    "4.0.0.0" //
#define szAPP_TOPOLOGY                      "4I4O" // 4 input,4 output,2 shutter,1 led
#define szAPP_TOPOVER                          "1" // 

#define PLATFORM_TYPE_GENERIC                    1 //
#define PLATFORM_TYPE_SDSHIELD                   2 //
#define PLATFORM_TYPE_WEMOS_WITH_SHIELDS         2 //

#define CONTROLLER_TYPE_OPENHAB                  1 //
#define CONTROLLER_TYPE_CLOUD                    2 //

#define RTC_TYPE_3213                            1 //
#define RTC_TYPE_1307                            2 //

#define WIRED_ETHERNET_NONE                      0 //
#define WIRED_ETHERNET_W5100                     1 //
#define WIRED_ETHERNET_W5500                     2 //

#endif /* GLOBALS_H_ */
