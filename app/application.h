
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

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class CApplication {
 private:
  tUint32            m_dwError;                    //
  tUint32            m_dwFlags;                    //

	tInt32             m_bootMode = 0;               //

  Timer              m_timer;                      //
  tUint32            m_dwTicks = 0;                //

 public:
  //--------------------------------------------------------------------------
	void               OnInit();
	void               StartServices();
	void               OnRun();
  void               ReportHeapUsage();
	void               StopServices();

	void               Reset();
	void               Restart(tUint32 msDelay);

  void               CmdInfo(String commandLine, CommandOutput* pOut);
  void               CmdLogLevel(String commandLine, CommandOutput* pOut);
  void               CmdShowConfig(String commandLine, CommandOutput* pOut);
  void               CmdCpu(String commandLine, CommandOutput* pOut);
  void               CmdDebug(String commandLine, CommandOutput* pOut);
  void               CmdApMode(String commandLine, CommandOutput* pOut);
  void               CmdRestart(String commandLine, CommandOutput* pOut);

  void               begin();
  void               checkConnection();

  }; // CApplication

extern CApplication g_app;

#endif // __application_h__
