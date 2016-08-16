
//----------------------------------------------------------------------------
// application.h
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef __application_h__
#define __application_h__

#include <xstdafx.h>

//----------------------------------------------------------------------------
#define CAPP_VERSION                     "4.0.1.0" //                                   
#define CAPP_DATE                         __DATE__ //
#define CAPP_PERIOD                             50 //
#define CAPP_PERIOD_HEAPUSAGE                60000 //
#define CAPP_PERIOD_NETWCHECK                 1000 //
#define CAPP_CONF_FILE                 ".app.conf" //

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class CApplication {
 public: 
  bool               m_cpuBoost;
  String             m_otaBaseUrl;
  
  tUint32            m_gpiodEmul = 0;
  tUint32            m_gpiodMode = 3;
  tUint32            m_gpiodLock = 0;
  tUint32            m_gpiodDisable = 0;

  tUint32            m_gpiodInDebounce[4] = { 100, 100, 100, 100 };
  tUint32            m_gpiodOutDefRun[4]  = {   0,   0,   0,   0 };
  tUint32            m_gpiodUdmDefRun[4]  = {  30,  30,  30,  30 };

 private:
  tUint32            m_dwError;                    //
  tUint32            m_dwFlags;                    //

	tInt32             m_bootMode = 0;               //

  Timer              m_timer;                      //
  tUint32            m_dwTicks = 0;                //

 public:
  //--------------------------------------------------------------------------
  CApplication() {
    m_cpuBoost   = false;
    m_otaBaseUrl = "";
    } //

	void               Init();
	void               StartServices();
	void               StopServices();
	void               Reset();
	void               Restart(tUint32 msDelay);  
  void               CheckConnection();
  void               ReportHeapUsage();

	void               OnRun();

  // app_attr.cpp
  tCChar*            GetStrAttr(tCChar* szAttr);
  tUint32            GetNumAttr(tCChar* szAttr);

  // app_cmd.cpp
  void               cmdInit();
  void               cmdOnInfo(String commandLine, CommandOutput* pOut);
  void               cmdOnShow(String commandLine, CommandOutput* pOut);
  void               cmdOnCpu(String commandLine, CommandOutput* pOut);
  void               cmdOnDebug(String commandLine, CommandOutput* pOut);
  void               cmdOnRestart(String commandLine, CommandOutput* pOut);
  void               cmdOnUpgrade(String commandLine, CommandOutput* pOut);

  // app_conf.cpp
  void               confLoad();
  void               confSave();
  void               confDelete();
  bool               confExists();

  // app_http.cpp
  void               httpOnStatus(HttpRequest &request, HttpResponse &response);
  void               httpOnTools(HttpRequest &request, HttpResponse &response);

  }; // CApplication

extern CApplication g_app;

#endif // __application_h__
