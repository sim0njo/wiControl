/*globals.h*/
#ifndef GLOBALS_H_
#define GLOBALS_H_


extern const char* build_time;
extern const char* build_git_sha;
extern int         g_isNetworkConnected;
/* why is type bool not known ?*/

#define APP_ALIAS              "wiControl" //
#define APP_TOPOLOGY                 "U02" // unterputz, 2 channels
#define APP_VERSION              "4.0.0.0" //

#define PLATFORM_TYPE_GENERIC            1 //
#define PLATFORM_TYPE_SDSHIELD           2 //
#define PLATFORM_TYPE_WEMOS_WITH_SHIELDS 2 //

#define CONTROLLER_TYPE_OPENHAB          1 //
#define CONTROLLER_TYPE_CLOUD            2 //

#define RTC_TYPE_3213                    1 //
#define RTC_TYPE_1307                    2 //

#define WIRED_ETHERNET_NONE              0 //
#define WIRED_ETHERNET_W5100             1 //
#define WIRED_ETHERNET_W5500             2 //

#endif /* GLOBALS_H_ */
