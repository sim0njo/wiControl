
//----------------------------------------------------------------------------
// cgpiod.hpp
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef __cgpiod_hpp__
#define __cgpiod_hpp__

#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>
#include <mqtt.h>
#include <stdafx.h>
#include <xerror.h>
#include <cparse.hpp>

extern tParseRsvd    g_gpiodParseObj[];
extern tParseRsvd    g_gpiodParseEvtInput[];
extern tParseRsvd    g_gpiodParseCmdOutput[];
extern tParseRsvd    g_gpiodParseCmdShutter[];
extern tParseRsvd    g_gpiodParseCmdSystem[];


#define CGPIOD_VERSION                   "4.0.1.0" //                                   
#define CGPIOD_DATE                       __DATE__ //

#define CGPIOD_CMD_PFX                       "cmd" //
#define CGPIOD_EVT_PFX                       "evt" //

#define CGPIOD_EMUL_OUTPUT                       0 // emulate output0-1
#define CGPIOD_EMUL_SHUTTER                      1 // emulate shutter0

#define CGPIOD_MODE_STANDALONE                   1 // handle events locally
#define CGPIOD_MODE_MQTT                         2 // publish events
#define CGPIOD_MODE_BOTH                         3 // do both

#define CGPIOD_EFMT_NUMERICAL                    0 // 
#define CGPIOD_EFMT_TEXTUAL                      1 // 

//                                      0xPPPPCCNN //
#define CGPIOD_OBJ_EVT_MASK             0x0000FFFF //
#define CGPIOD_OBJ_CMD_MASK             0x0000FFFF //
#define CGPIOD_OBJ_PRM_MASK             0xFFFF0000 //

#define CGPIOD_OBJ_NUM_MASK             0x000000FF //
#define CGPIOD_OBJ_CLS_MASK             0x0000FF00 //
#define CGPIOD_OBJ_CLS_WBS              0x00001B00 // WBS = input + output + system
#define CGPIOD_OBJ_CLS_WBR              0x00001D00 // WBR = input + shutter + system
#define CGPIOD_OBJ_CLS_INPUT            0x00000100 // input
#define CGPIOD_OBJ_CLS_OUTPUT           0x00000200 // output
#define CGPIOD_OBJ_CLS_SHUTTER          0x00000400 // shutter
#define CGPIOD_OBJ_CLS_HBEAT            0x00000800 // heartbeat
#define CGPIOD_OBJ_CLS_SYSTEM           0x00001000 // system

#define CGPIOD_OBJ_NONE                 0x00000000 //
#define CGPIOD_OBJ_INPUT0               0x00000100 //
#define CGPIOD_OBJ_INPUT1               0x00000101 //
#define CGPIOD_OBJ_OUTPUT0              0x00000200 //
#define CGPIOD_OBJ_OUTPUT1              0x00000201 //
#define CGPIOD_OBJ_SHUTTER0             0x00000400 //
#define CGPIOD_OBJ_SYSTEM               0x00001000 //
#define CGPIOD_OBJ_SYSVERSION           0x00001000 //
#define CGPIOD_OBJ_SYSMEMORY            0x00001001 //
#define CGPIOD_OBJ_SYSEMUL              0x00011002 //
#define CGPIOD_OBJ_SYSMODE              0x00021003 //
#define CGPIOD_OBJ_SYSEFMT              0x00041004 //
#define CGPIOD_OBJ_SYSDISABLE           0x00081005 //
#define CGPIOD_OBJ_SYSENABLE            0x00081006 //
#define CGPIOD_OBJ_SYSREBOOT            0x00081007 //

//----------------------------------------------------------------------------
// system=getemul
// system/emul=?
// system/emul=0.ack
//----------------------------------------------------------------------------
#define CGPIOD_SYS_CMD_NONE                      0 // 
#define CGPIOD_SYS_CMD_VERSION                   1 // 
#define CGPIOD_SYS_CMD_MEMORY                    2 // 
#define CGPIOD_SYS_CMD_UPTIME                    3 // 
#define CGPIOD_SYS_CMD_EMUL                      4 // WBS/WBR
#define CGPIOD_SYS_CMD_MODE                      5 // standalone/networked/both
#define CGPIOD_SYS_CMD_EFMT                      6 // setefmt.0|1.ack
#define CGPIOD_SYS_CMD_DISABLE                   7 // 
#define CGPIOD_SYS_CMD_ENABLE                    8 // 
#define CGPIOD_SYS_CMD_REBOOT                    9 // reboot.ack

//----------------------------------------------------------------------------
// heartbeat definitions
//----------------------------------------------------------------------------
#define CGPIOD_HB_COUNT                          3 //
#define CGPIOD_HB0                               0 // 0.5s heartbeat
#define CGPIOD_HB0_PERIOD                      500 // 
#define CGPIOD_HB1                               1 // 1.0s heartbeat
#define CGPIOD_HB1_PERIOD                     1000 // 
#define CGPIOD_HB2                               2 // 2.0s heartbeat
#define CGPIOD_HB2_PERIOD                     2000 // 

#define CGPIOD_HB_EVT_EVEN                       0 // 
#define CGPIOD_HB_EVT_ODD                        1 // 

typedef struct {
  tUint32            msStart;                      // start time in ms
  tUint32            msPeriod;                     // period in ms
  tUint32            dwRepeat;                     // repeat times, 0=forever
  tUint32            dwCntr;                       // counter value
  tCChar*            szName;                       //
  } tGpiodHbeat;

//----------------------------------------------------------------------------
// input definitions
//----------------------------------------------------------------------------
#define CGPIOD_IN_COUNT                          2 //

#define CGPIOD_IN0_PIN                           0 // D3 
#define CGPIOD_IN1_PIN                           2 // D4 

#define CGPIOD_IN_TMR_DEBOUNCE                 100 // in ms
#define CGPIOD_IN_TMR_INGT1                   1000 //
#define CGPIOD_IN_TMR_INGT2                   2000 //

#define CGPIOD_IN_VAL_IN                         1 // input closed
#define CGPIOD_IN_VAL_OUT                        0 // input opened

#define CGPIOD_IN_POL_NORMAL                     0 //
#define CGPIOD_IN_POL_INVERT                     1 //

#define CGPIOD_IN_STATE_OUT                      0 // 
#define CGPIOD_IN_STATE_INGT0                    1 // 
#define CGPIOD_IN_STATE_INGT1                    2 // 
#define CGPIOD_IN_STATE_INGT2                    3 // 

#define CGPIOD_IN_EVT_INGT0                      2 // 
#define CGPIOD_IN_EVT_OUTLT1                     3 // 
#define CGPIOD_IN_EVT_INGT1                      4 // 
#define CGPIOD_IN_EVT_OUTGT1                     5 // 
#define CGPIOD_IN_EVT_INGT2                      6 // 
#define CGPIOD_IN_EVT_OUT                        7 // 

#define CGPIOD_IN_FLG_NONE              0x00000000 // 
#define CGPIOD_IN_FLG_MQTT_INGT0        0x00000004 // send event to MQTT broker
#define CGPIOD_IN_FLG_MQTT_OUTLT1       0x00000008 // send event to MQTT broker
#define CGPIOD_IN_FLG_MQTT_INGT1        0x00000010 // send event to MQTT broker
#define CGPIOD_IN_FLG_MQTT_OUTGT1       0x00000020 // send event to MQTT broker
#define CGPIOD_IN_FLG_MQTT_INGT2        0x00000040 // send event to MQTT broker
#define CGPIOD_IN_FLG_MQTT_OUT          0x00000080 // send event to MQTT broker
#define CGPIOD_IN_FLG_MQTT_ALL          0x000000FC // send all events to MQTT broker

typedef struct {
  tUint32            dwFlags;                      // evt routing flags
  tUint32            dwState;                      // input state
  tUint32            dwPin;                        // GPIO pin number
  tUint32            dwPol;                        // polarity 0=normal, 1=invert
  tUint32            dwVal;                        // stable pin value
  tUint32            tmrDebounce;                  // debounce timer
  tUint32            msDebounce;                   // debounce ref time
  tUint32            msState;                      // state ref time
  } tGpiodInput;

//----------------------------------------------------------------------------
// output definitions
//----------------------------------------------------------------------------
#define CGPIOD_OUT_COUNT                         2 // 

#define CGPIOD_OUT0_PIN                          5 // D1
#define CGPIOD_OUT1_PIN                          4 // D2

#define CGPIOD_OUT_POL_NORMAL                    0 // 
#define CGPIOD_OUT_POL_INVERT                    1 // 

#define CGPIOD_OUT_STATE_OFF                     0 // 
#define CGPIOD_OUT_STATE_ON                      1 // 

#define CGPIOD_OUT_CMD_NONE                      0 //
#define CGPIOD_OUT_CMD_STATUS                    1 //
#define CGPIOD_OUT_CMD_ON                        2 //
#define CGPIOD_OUT_CMD_OFF                       3 //
#define CGPIOD_OUT_CMD_ONLOCKED                  4 //
#define CGPIOD_OUT_CMD_OFFLOCKED                 5 //
#define CGPIOD_OUT_CMD_TOGGLE                    6 //
#define CGPIOD_OUT_CMD_UNLOCK                    7 //
#define CGPIOD_OUT_CMD_ONDELAYED                 8 //
#define CGPIOD_OUT_CMD_OFFDELAYED                9 //
#define CGPIOD_OUT_CMD_ONTIMED                  10 //
#define CGPIOD_OUT_CMD_OFFTIMED                 11 //
#define CGPIOD_OUT_CMD_TOGGLEDELAYED            12 //
#define CGPIOD_OUT_CMD_TOGGLETIMED              13 //
#define CGPIOD_OUT_CMD_LOCK                     14 //
#define CGPIOD_OUT_CMD_LOCKTIMED                15 //
#define CGPIOD_OUT_CMD_TIMESET                  16 //
#define CGPIOD_OUT_CMD_TIMEADD                  17 //
#define CGPIOD_OUT_CMD_TIMEABORT                18 //
#define CGPIOD_OUT_CMD_BLINK                    19 //

#define CGPIOD_OUT_EVT_ON                        2 //
#define CGPIOD_OUT_EVT_OFF                       3 //
#define CGPIOD_OUT_EVT_TIMEXP                 0xFD // time expired

#define CGPIOD_OUT_FLG_NONE             0x00000000 //
#define CGPIOD_OUT_FLG_MQTT_ON          0x00000004 //
#define CGPIOD_OUT_FLG_MQTT_OFF         0x00000008 //
#define CGPIOD_OUT_FLG_MQTT_TIMEXP      0x00000010 //
#define CGPIOD_OUT_FLG_MQTT_ALL         0x0000001C //
#define CGPIOD_OUT_FLG_LOCKED           0x40000000 //

typedef struct {
  tUint32            dwFlags;                      //
  tUint32            dwPin;                        // GPIO pin number
  tUint32            dwPol;                        // polarity 0=normal, 1=invert
  tUint32            dwState;                      //
  tUint32            dwCmd;                        // 
  tUint32            dwRun;                        // time 2
  } tGpiodOutput;

//----------------------------------------------------------------------------
// shutter definitions
//----------------------------------------------------------------------------
#define CGPIOD_UDM_COUNT                         1 //

#define CGPIOD_UDM0_PIN_UP                       5 // D1
#define CGPIOD_UDM0_PIN_DOWN                     4 // D2

#define CGPIOD_UDM_POL_NORMAL                    0 // 
#define CGPIOD_UDM_POL_INVERT                    1 // 

#define CGPIOD_UDM_RUN_DEF                   15000 // 15s

#define CGPIOD_UDM_STATE_STOP                    0 // 
#define CGPIOD_UDM_STATE_UP                      1 // 
#define CGPIOD_UDM_STATE_DOWN                    2 // 

#define CGPIOD_UDM_CMD_NONE                      0 //
#define CGPIOD_UDM_CMD_STATUS                    1 //
#define CGPIOD_UDM_CMD_STOP                      2 //
#define CGPIOD_UDM_CMD_TOGGLEUP                  3 //
#define CGPIOD_UDM_CMD_TOGGLEDOWN                4 //
#define CGPIOD_UDM_CMD_UP                        5 //
#define CGPIOD_UDM_CMD_DOWN                      6 //
#define CGPIOD_UDM_CMD_TIPUP                     7 //
#define CGPIOD_UDM_CMD_TIPDOWN                   8 //
#define CGPIOD_UDM_CMD_PRIOLOCK                  9 //
#define CGPIOD_UDM_CMD_PRIOUNLOCK               10 //
#define CGPIOD_UDM_CMD_LEARNON                  11 //
#define CGPIOD_UDM_CMD_LEARNOFF                 12 //
#define CGPIOD_UDM_CMD_PRIOSET                  13 //
#define CGPIOD_UDM_CMD_PRIORESET                14 //
#define CGPIOD_UDM_CMD_SENSROLUP                15 //
#define CGPIOD_UDM_CMD_SENSJALUP                16 //
#define CGPIOD_UDM_CMD_SENSROLDOWN              17 //
#define CGPIOD_UDM_CMD_SENSJALDOWN              18 //

#define CGPIOD_UDM_PRIO_LVL_0                    0 //
#define CGPIOD_UDM_PRIO_LVL_1                    1 //
#define CGPIOD_UDM_PRIO_LVL_2                    2 //
#define CGPIOD_UDM_PRIO_LVL_3                    3 //
#define CGPIOD_UDM_PRIO_LVL_4                    4 //
#define CGPIOD_UDM_PRIO_LVL_5                    5 //

#define CGPIOD_UDM_PRIO_MASK_NONE       0x00000000 //
#define CGPIOD_UDM_PRIO_MASK_0          0x00000001 //
#define CGPIOD_UDM_PRIO_MASK_1          0x00000002 //
#define CGPIOD_UDM_PRIO_MASK_2          0x00000004 //
#define CGPIOD_UDM_PRIO_MASK_3          0x00000008 //
#define CGPIOD_UDM_PRIO_MASK_4          0x00000010 //
#define CGPIOD_UDM_PRIO_MASK_5          0x00000020 //

#define CGPIOD_UDM_EVT_STOP                      1 //
#define CGPIOD_UDM_EVT_UPON                      2 //
#define CGPIOD_UDM_EVT_DOWNON                    3 //
#define CGPIOD_UDM_EVT_UPOFF                     4 //
#define CGPIOD_UDM_EVT_DOWNOFF                   5 //
#define CGPIOD_UDM_EVT_TIMEXP                    6 // timer expired

#define CGPIOD_UDM_FLG_NONE             0x00000000 //
#define CGPIOD_UDM_FLG_MQTT_STOP        0x00000002 //
#define CGPIOD_UDM_FLG_MQTT_UPON        0x00000004 //
#define CGPIOD_UDM_FLG_MQTT_DOWNON      0x00000008 //
#define CGPIOD_UDM_FLG_MQTT_UPOFF       0x00000010 //
#define CGPIOD_UDM_FLG_MQTT_DOWNOFF     0x00000020 //
#define CGPIOD_UDM_FLG_MQTT_TIMEXP      0x00000040 //
#define CGPIOD_UDM_FLG_MQTT_ALL         0x0000007C //
#define CGPIOD_UDM_FLG_LOCKED           0x40000000 //

typedef struct {
  tUint32            dwFlags;                      //
  tUint32            dwPinUp;                      // GPIO pin number
  tUint32            dwPinDown;                    // GPIO pin number
  tUint32            dwPol;                        // polarity 0=normal, 1=invert
  tUint32            dwRunDef;                     // default run time for standalone oper
  tUint32            dwState;                      // CGPIOD_UDM_STATE_
  tUint32            dwPrioLvl;                    // 0..5
  tUint32            dwPrioMask;                   // 0..63
  tUint32            dwCmd;                        // 
  tUint32            dwDelay;                      // delay time
  tUint32            dwRun;                        // run time
  tUint32            dwTip;                        // tip time
  } tGpiodShutter;

//----------------------------------------------------------------------------
// obj / parm [0]             [1]            [2] [3]
// output                     delay          run
// shutter    lock plvl pmask delay          run tip
// system     ack             emul mode efmt 
//----------------------------------------------------------------------------
typedef struct {
  tUint32            msNow;                        // 
  tUint32            dwObj;                        // 
  tUint32            dwCmd;                        // 
  tUint32            dwLock;                       //
  tUint32            dwPrioLvl;                    //
  tUint32            dwPrioMask;                   //
  tUint32            dwDelay;                      //
  tUint32            dwRun;                        //
  tUint32            dwTip;                        //
  tUint32            dwEmul;                       //
  tUint32            dwMode;                       //
  tUint32            dwEfmt;                       //
  } tGpiodCmd;

typedef struct {
  tUint32            msNow;                        // 
  tUint32            dwObj;                        // 
  tUint32            dwEvt;                        // 
  tCChar*            szEvt;                        //
  } tGpiodEvt;

//----------------------------------------------------------------------------
// callbacks
//----------------------------------------------------------------------------
void                 gpiodOnHttpConfig(HttpRequest &request, HttpResponse &response);
void                 gpiodOnMqttPublish(String topic, String message);

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class CGpiod {
 private:
  tUint32            m_dwError;                    //

  tUint32            m_dwEmul;                     //
  tUint32            m_dwMode;                     //
  tUint32            m_dwEfmt;                     //
  tCChar*            m_szCmdPfx;                   //
  tCChar*            m_szEvtPfx;                   //

  tGpiodHbeat        m_hbeat[CGPIOD_HB_COUNT];     // heartbeat counters
  tGpiodInput        m_input[CGPIOD_IN_COUNT];     //
  tGpiodOutput       m_output[CGPIOD_OUT_COUNT];   //
  tGpiodShutter      m_shutter[CGPIOD_UDM_COUNT];  // 
  CParse             m_parse;                      //
  Timer              m_timer;                      //
  Timer              m_timerMqtt;                  //


 public:
  //--------------------------------------------------------------------------
  void               begin();
  void               checkConnection();
//  void               notifyChange(String object, String value);
//  void               registerHttpHandlers(HttpServer &server);
//  void               registerCommandHandlers();

  //--------------------------------------------------------------------------
  //
  //--------------------------------------------------------------------------
  tUint32            _GetFlags(tUint32* pFlags, tUint32 dwFlags) { return *pFlags &   dwFlags; }
  tUint32            _SetFlags(tUint32* pFlags, tUint32 dwFlags) { return *pFlags |=  dwFlags; }
  tUint32            _RstFlags(tUint32* pFlags, tUint32 dwFlags) { return *pFlags &= ~dwFlags; }

  //--------------------------------------------------------------------------
  // gpiod.cpp
  //--------------------------------------------------------------------------
  tUint32            GetEmul()                     { return m_dwEmul; }
  tUint32            GetMode()                     { return m_dwMode; }
  tUint32            GetEfmt()                     { return m_dwEfmt; }

  tUint32            OnConfig();
  tUint32            OnInit();
  void               OnRun();
  tUint32            OnExit();

  tUint32            DoCmd(tGpiodCmd* pCmd);
  tUint32            DoEvt(tGpiodEvt* pEvt);
  void               _DoPublish(tUint32 fDup, tUint32 fQoS, tUint32 fRetain, tCChar* szObj, tCChar* szMsg);

  //--------------------------------------------------------------------------
  // gpiod_parse.cpp
  //--------------------------------------------------------------------------
  tUint32            ParseCmd(tGpiodCmd* pOut, tChar* pObj, tChar* pCmd, tUint32 dwMask1);
  tUint32            _parseEvtInput(tGpiodCmd* pOut);
  tUint32            _parseCmdOutput(tGpiodCmd* pOut);
  tUint32            _parseCmdShutter(tGpiodCmd* pOut);
  tUint32            _parseCmdSystem(tGpiodCmd* pOut);
  tUint32            _parseCmdParams(tGpiodCmd* pOut);

  //--------------------------------------------------------------------------
  // gpiod_print.cpp
  //--------------------------------------------------------------------------
  void               PrintEvt(tGpiodEvt* pEvt);
  void               PrintCmd(tGpiodCmd* pCmd);
  tCChar*            _printObj2String(tChar* pOut, tUint32 dwObj);
  tCChar*            _printObjEvt2String(tChar* pOut, tUint32 dwObj, tUint32 dwEvt);
  tCChar*            _printObjCmd2String(tChar* pOut, tUint32 dwObj, tUint32 dwCmd);
  tCChar*            _printVal2String(tChar* pOut, tUint32 dwVal);
  tCChar*            _printCmdParamVals(tChar* pOut, tUint32 cbOut, tGpiodCmd* pCmd);

 private:
  //--------------------------------------------------------------------------
  // gpiod_input.cpp
  //--------------------------------------------------------------------------
  tUint32            _inputOnConfig();
  tUint32            _inputOnInit();
  tUint32            _inputOnRun(tUint32 msNow);
  tUint32            _inputOnExit();
  tUint32            _inputGetPinVal(tGpiodInput* pObj, tUint32 msNow);

  //--------------------------------------------------------------------------
  // gpiod_output.cpp
  //--------------------------------------------------------------------------
  tUint32            _outputOnConfig();
  tUint32            _outputOnInit();
  tUint32            _outputOnRun(tUint32 msNow);
  tUint32            _outputOnExit();
//  tUint32            _outputOnHbTick(tGpiodEvt* pEvt);
  tUint32            _outputDoEvt(tGpiodEvt* pEvt);
  tUint32            _outputDoCmd(tGpiodCmd* pCmd);
  void               _outputSetState(tGpiodOutput* pObj, tUint32 dwState, tGpiodEvt* pEvt);

  //--------------------------------------------------------------------------
  // gpiod_shutter.cpp
  //--------------------------------------------------------------------------
  tUint32            _shutterOnConfig();
  tUint32            _shutterOnInit();
  tUint32            _shutterOnRun(tUint32 msNow);
  tUint32            _shutterOnExit();
  tUint32            _shutterDoEvt(tGpiodEvt* pEvt);
  tUint32            _shutterCheckPrio(tGpiodShutter* pObj, tGpiodCmd* pCmd);
  tUint32            _shutterDoCmd(tGpiodCmd* pCmd);
  void               _shutterSetState(tGpiodShutter* pObj, tUint32 dwState, tGpiodEvt* pEvt);

  //--------------------------------------------------------------------------
  // gpiod_system.cpp
  //--------------------------------------------------------------------------
  tUint32            _systemOnConfig();
  tUint32            _systemOnInit();
  tUint32            _systemOnRun(tUint32 msNow);
  tUint32            _systemOnExit();
  tUint32            _systemDoCmd(tGpiodCmd* pCmd);

  }; // CGpiod

extern CGpiod g_gpiod;


#endif // __cgpiod_hpp__
