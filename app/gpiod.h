
//----------------------------------------------------------------------------
// cgpiod.h
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//
// 4I4O-ed01    4I4O-ed02    4I4O-ed03    RGB-ed1
// in0   D3/G0  in0   D3/G0  in0   D2/G4  -
// in1   D4/G2  in1   D4/G2  in1   D1/G5  -
// in2   D1/G5  in2   D1/G5  in2   D4/G2  -
// in3   D2/G4  in3   D2/G4  in3   D3/G0  -
// out0->D8/G15 out0->D8/G15 out0->D8/G15 -
// out1  D7/G13 out1  D7/G13 out1  D6/G12 -
// out2  D6/G12 out2  D6/G12 out2  D7/G13 -
// out3  D5/G14 out3  D5/G14 out3  D5/G14 -
// led   D0/G16 led   D0/G16 -            -
// -            -            -            r ->D7/G13
// -            -            -            g ->D6/G12
// -            -            -            b ->D5/G14
// -            -            -            ww  D2/G4
// -            -            -            cw  D1/G5
//
//----------------------------------------------------------------------------
#ifndef __cgpiod_hpp__
#define __cgpiod_hpp__

#include <mqtt.h>
#include <xstdafx.h>
#include <xerror.h>
#include <cparse.hpp>
#include <app_clslevels.h>
#include <application.h>

//----------------------------------------------------------------------------
extern tParseRsvd    g_gpiodParseObj[];
extern tParseRsvd    g_gpiodParseObjParm[];
extern tParseRsvd    g_gpiodParseObjSta[];
extern tParseRsvd    g_gpiodParseObjEvt[];
extern tParseRsvd    g_gpiodParseCmdInput[];
extern tParseRsvd    g_gpiodParseCmdOutput[];
extern tParseRsvd    g_gpiodParseCmdShutter[];

//----------------------------------------------------------------------------
void                 gpiodOnHttpConfig(HttpRequest &request, HttpResponse &response);
void                 gpiodOnMqttPublish(tChar* szTopic, tChar* szMsg);

//----------------------------------------------------------------------------
#define CGPIOD_VERSION                   "4.0.1.0" //                                   
#define CGPIOD_DATE                       __DATE__ //
#define CGPIOD_PERIOD_NETWCHECK               1000 //

#define CGPIOD_CMD_PFX                       "cmd" //
#define CGPIOD_EVT_PFX                       "evt" //
#define CGPIOD_STA_PFX                       "sta" //

#define CGPIOD_EMUL_NONE                         0 //
#define CGPIOD_EMUL_OUTPUT                       1 // emulate outputs
#define CGPIOD_EMUL_SHUTTER                      2 // emulate shutters
#define CGPIOD_EMUL_RGB                          4 // emulate RGB/WW/CW

#define CGPIOD_MODE_NONE                         0 // 
#define CGPIOD_MODE_LOCAL                        1 // handle cmds/events locally
#define CGPIOD_MODE_MQTT                         2 // allow remote system to send cmds/receive events
#define CGPIOD_MODE_BOTH                         3 // combination of both

#define CGPIOD_LOCK_FALSE                        0 // 
#define CGPIOD_LOCK_TRUE                         1 // 

#define CGPIOD_DISABLE_FALSE                     0 // 
#define CGPIOD_DISABLE_TRUE                      1 // 

#define CGPIOD_SEC_2_MSEC                     1000 // multiplier for second to ms
#define CGPIOD_DECISEC_2_MSEC                  100 // multiplier for decisecond to ms

//----------------------------------------------------------------------------
#define CGPIOD_ORIG_INPUT               0x80000001 // 
#define CGPIOD_ORIG_OUTPUT              0x80000002 // 
#define CGPIOD_ORIG_SHUTTER             0x80000004 // 
#define CGPIOD_ORIG_RGB                 0x80000008 // 
#define CGPIOD_ORIG_SYSTEM              0x80000010 // 
#define CGPIOD_ORIG_CLI                 0x00000020 // 
#define CGPIOD_ORIG_HTTP                0x00000040 // 
#define CGPIOD_ORIG_MQTT                0x80000080 // 
#define CGPIOD_ORIG_INTERNAL            0x40000000 // 

#define CGPIOD_ORIG_FLG_PUBLISH         0x80000000 // for which origs to publish status

//----------------------------------------------------------------------------
#define CGPIOD_IO_DIR_INPUT                      0 // 
#define CGPIOD_IO_DIR_OUTPUT                     1 // 

#define CGPIOD_IO_POL_NORMAL                     0 //
#define CGPIOD_IO_POL_INVERT                     1 //

//      CGPIOD_EVT_                     0x0000NNNN //
#define CGPIOD_EVT_NUM_MASK             0x0000FFFF //

//      CGPIOD_CMD_                     0xPPPPNNNN //
#define CGPIOD_CMD_NUM_MASK             0x0000FFFF //
#define CGPIOD_CMD_PRM_MASK             0xFFFF0000 //

//      CGPIOD_OBJ_                     0x0000CCNN //
#define CGPIOD_OBJ_NUM_MASK             0x000000FF //
#define CGPIOD_OBJ_CLS_MASK             0x0000FF00 //

#define CGPIOD_OBJ_CLS_INPUT            0x00000100 // input
#define CGPIOD_OBJ_CLS_OUTPUT           0x00000200 // regular outputs
#define CGPIOD_OBJ_CLS_SHUTTER          0x00000400 // shutter outputs
#define CGPIOD_OBJ_CLS_RGB              0x00000800 // rgbwc

//#define CGPIOD_OBJ_CLS_TIMER            0x00000800 // timer   outputs
#define CGPIOD_OBJ_CLS_SYSTEM           0x00001000 // system
#define CGPIOD_OBJ_CLS_HBEAT            0x00002000 // heartbeat

#define CGPIOD_OBJ_CLS_WBS              0x00001300 // WBS = input + output  + system
#define CGPIOD_OBJ_CLS_WBR              0x00001500 // WBR = input + shutter + system

#define CGPIOD_CMD_PRM_OPTIONAL         0xFF000000 // optional parms mask
#define CGPIOD_CMD_PRM_MANDATORY        0x00FF0000 // mandatory parms mask

#define CGPIOD_CMD_RSP_AUTO             0x00001000 // cmd with auto response

//----------------------------------------------------------------------------
// system=ping   
// system=loglevel [.<loglevel>.ack]
// system=emul     [.<emul>.ack]
//----------------------------------------------------------------------------
#define CGPIOD_OBJ_SYSTEM               0x00001000 // system

#define CGPIOD_SYS_CMD_NONE             0x00001000 // 
#define CGPIOD_SYS_CMD_PING             0x00001001 // 
#define CGPIOD_SYS_CMD_VERSION          0x00001002 // 
#define CGPIOD_SYS_CMD_MEMORY           0x00001003 // 
#define CGPIOD_SYS_CMD_UPTIME           0x00001004 // 
#define CGPIOD_SYS_CMD_LOGLEVEL         0x00001005 // 
#define CGPIOD_SYS_CMD_EMUL             0x00001006 // WBS/WBR
#define CGPIOD_SYS_CMD_MODE             0x00001007 // standalone/networked/both
#define CGPIOD_SYS_CMD_LOCK             0x00001008 // 
#define CGPIOD_SYS_CMD_DISABLE          0x00001009 // 
#define CGPIOD_SYS_CMD_RESTART          0x0000100A // restart.ack
#define CGPIOD_SYS_CMD_SAVE             0x0000100B // save.ack

#define CGPIOD_SYS_PRM_LOGLEVEL         0x01000000 //
#define CGPIOD_SYS_PRM_EMUL             0x02000000 //
#define CGPIOD_SYS_PRM_MODE             0x04000000 //
#define CGPIOD_SYS_PRM_OFFON            0x08000000 //
#define CGPIOD_SYS_PRM_ACK              0x10000000 //

//----------------------------------------------------------------------------
// heartbeat definitions
//----------------------------------------------------------------------------
#define CGPIOD_HB_PERIOD                      1000 // 
#define CGPIOD_HB_EVT_EVEN                       0 // 
#define CGPIOD_HB_EVT_ODD                        1 // 

typedef struct {
  tUint32            msStart;                      // start time in ms
  tUint32            msPeriod;                     // period in ms
  tUint32            dwRepeat;                     // repeat times, 0=forever
  tUint32            dwCntr;                       // counter value
  } tGpiodHbeat;

//----------------------------------------------------------------------------
// input definitions
//----------------------------------------------------------------------------
#define CGPIOD_IN_COUNT                          4 //

#define CGPIOD_IN0                               0 //     
#define CGPIOD_IN1                               1 //     
#define CGPIOD_IN2                               2 //     
#define CGPIOD_IN3                               3 //     

#ifdef TOPOLOGY_4I4O_ED03
#define CGPIOD_IN0_PIN                           4 // D2/4
#define CGPIOD_IN1_PIN                           5 // D1/5
#define CGPIOD_IN2_PIN                           2 // D4/2
#define CGPIOD_IN3_PIN                           0 // D3/0
#else  // ed01 and ed02
#define CGPIOD_IN0_PIN                           0 // D3/0
#define CGPIOD_IN1_PIN                           2 // D4/2
#define CGPIOD_IN2_PIN                           5 // D1/5
#define CGPIOD_IN3_PIN                           4 // D2/4
#endif

#define CGPIOD_IN_DEBOUNCE                     100 // in ms

#define CGPIOD_IN_TMR_INGT1                   1000 //
#define CGPIOD_IN_TMR_INGT2                   2000 //

#define CGPIOD_IN_VAL_IN                         1 // input closed
#define CGPIOD_IN_VAL_OUT                        0 // input opened

#define CGPIOD_IN_STATE_OUT                      0 // 
#define CGPIOD_IN_STATE_INGT0                    1 // 
#define CGPIOD_IN_STATE_INGT1                    2 // 
#define CGPIOD_IN_STATE_INGT2                    3 // 

#define CGPIOD_IN_CMD_STATUS                     1 // 
#define CGPIOD_IN_CMD_INGT0                      2 // 
#define CGPIOD_IN_CMD_OUTLT1                     3 // 
#define CGPIOD_IN_CMD_INGT1                      4 // 
#define CGPIOD_IN_CMD_OUTGT1                     5 // 
#define CGPIOD_IN_CMD_INGT2                      6 // 
#define CGPIOD_IN_CMD_OUT                        7 // 
#define CGPIOD_IN_CMD_DEBOUNCE                   8 // 

#define CGPIOD_IN_PRM_DEBOUNCE          0x01000000 // optional

#define CGPIOD_IN_EVT_INGT0                      2 // 
#define CGPIOD_IN_EVT_OUTLT1                     3 // 
#define CGPIOD_IN_EVT_INGT1                      4 // 
#define CGPIOD_IN_EVT_OUTGT1                     5 // 
#define CGPIOD_IN_EVT_INGT2                      6 // 
#define CGPIOD_IN_EVT_OUT                        7 // 

typedef struct {
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
#define CGPIOD_OUT_COUNT                         4 // 

#define CGPIOD_OUT0                              0 //
#define CGPIOD_OUT1                              1 //
#define CGPIOD_OUT2                              2 //
#define CGPIOD_OUT3                              3 //

#ifdef TOPOLOGY_4I4O_ED03
#define CGPIOD_OUT0_PIN                         15 // D8 
#define CGPIOD_OUT1_PIN                         12 // D6 
#define CGPIOD_OUT2_PIN                         13 // D7
#define CGPIOD_OUT3_PIN                         14 // D5
#else  // ed01 and ed02
#define CGPIOD_OUT0_PIN                         15 // D8 
#define CGPIOD_OUT1_PIN                         13 // D7 
#define CGPIOD_OUT2_PIN                         12 // D6
#define CGPIOD_OUT3_PIN                         14 // D5
#endif

#define CGPIOD_OUT_DEF_RUN                       0 // infinite

#define CGPIOD_OUT_STATE_OFF                     0 // 
#define CGPIOD_OUT_STATE_ON                      1 // 

#define CGPIOD_OUT_LOCK_OFF                      0 // 
#define CGPIOD_OUT_LOCK_ON                       1 // 

#define CGPIOD_OUT_CMD_NONE             0x00000000 // out0-1
#define CGPIOD_OUT_CMD_STATUS           0x00001001 // out0-1
#define CGPIOD_OUT_CMD_OFF              0x00001002 // out0-1
#define CGPIOD_OUT_CMD_ON               0x00001003 // out0-1
#define CGPIOD_OUT_CMD_TOGGLE           0x00001004 // out0-1
#define CGPIOD_OUT_CMD_BLINK            0x00001005 // extended
#define CGPIOD_OUT_CMD_LOCK             0x00000006 // out0-1
#define CGPIOD_OUT_CMD_DEFRUN           0x00000007 // extended

#define CGPIOD_OUT_PRM_LOCK             0x00010000 // mandatory
#define CGPIOD_OUT_PRM_DELAY            0x00020000 // mandatory
#define CGPIOD_OUT_PRM_RUN              0x00040000 // mandatory
#define CGPIOD_OUT_PRM_OLOCK            0x01000000 // optional
#define CGPIOD_OUT_PRM_DEFRUN           0x02000000 // optional

#define CGPIOD_OUT_FLG_NONE             0x00000000 //
#define CGPIOD_OUT_FLG_LOCKED           0x20000000 //

typedef struct {
  tUint32            dwFlags;                      //
  tUint32            dwPin;                        // GPIO pin number
  tUint32            dwPol;                        // polarity 0=normal, 1=invert
  tUint32            dwRunDef;                     // default run time for standalone oper
  tUint32            dwState;                      //
  tUint32            dwCmd;                        // 
  tUint32            dwLock;                       // 
  tUint32            dwDelay;                      // 
  tUint32            dwRun;                        // time 2
  } tGpiodOutput;

//----------------------------------------------------------------------------
// shutter definitions
//----------------------------------------------------------------------------
#define CGPIOD_SHU_COUNT                         2 //

#define CGPIOD_SHU0                              0 //
#define CGPIOD_SHU1                              1 //

#ifdef TOPOLOGY_4I4O_ED03
#define CGPIOD_SHU0_PIN_DOWN                    15 // out0 D8 
#define CGPIOD_SHU0_PIN_UP                      12 // out1 D6
#define CGPIOD_SHU1_PIN_DOWN                    13 // out2 D7
#define CGPIOD_SHU1_PIN_UP                      14 // out3 D5
#else  // ed01 and ed02
#define CGPIOD_SHU0_PIN_DOWN                    15 // out0 D8 
#define CGPIOD_SHU0_PIN_UP                      13 // out1 D7
#define CGPIOD_SHU1_PIN_DOWN                    12 // out2 D6
#define CGPIOD_SHU1_PIN_UP                      14 // out3 D5
#endif

#define CGPIOD_SHU_DEF_RUN                      30 // 30s
#define CGPIOD_SHU_DEF_DELAY                   200 // 200ms

#define CGPIOD_SHU_STATE_STOP                    0 // 
#define CGPIOD_SHU_STATE_DOWN                    1 // 
#define CGPIOD_SHU_STATE_UP                      2 // 

#define CGPIOD_SHU_CMD_NONE                      0 //
#define CGPIOD_SHU_CMD_STATUS                    1 // out0-1
#define CGPIOD_SHU_CMD_STOP                      2 // out0-1
#define CGPIOD_SHU_CMD_UP                        3 // out0-1
#define CGPIOD_SHU_CMD_DOWN                      4 // out0-1
#define CGPIOD_SHU_CMD_TOGGLEUP                  5 // out0-1
#define CGPIOD_SHU_CMD_TOGGLEDOWN                6 // out0-1
#define CGPIOD_SHU_CMD_TIPUP                     7 // out0-1
#define CGPIOD_SHU_CMD_TIPDOWN                   8 // out0-1
#define CGPIOD_SHU_CMD_PRIOLOCK                  9 // out0-1
#define CGPIOD_SHU_CMD_PRIOUNLOCK               10 // out0-1
#define CGPIOD_SHU_CMD_PRIOSET                  11 // out0-1
#define CGPIOD_SHU_CMD_PRIORESET                12 // out0-1
#define CGPIOD_SHU_CMD_DEFRUN                   13 // extended
#define CGPIOD_SHU_CMD_LEARNON                  14 // out0-1
#define CGPIOD_SHU_CMD_LEARNOFF                 15 // out0-1

#define CGPIOD_SHU_PRM_PRIOMASK         0x00010000 //
#define CGPIOD_SHU_PRM_PRIOLEVEL        0x00020000 //
#define CGPIOD_SHU_PRM_PRIOLOCK         0x00040000 //
#define CGPIOD_SHU_PRM_DELAY            0x00080000 //
#define CGPIOD_SHU_PRM_RUN              0x00100000 //
#define CGPIOD_SHU_PRM_TIP0             0x00200000 //
#define CGPIOD_SHU_PRM_TIP1             0x00400000 //
#define CGPIOD_SHU_PRM_DEFRUN           0x01000000 // optional

#define CGPIOD_SHU_PRIO_LVL_0                    0 //
#define CGPIOD_SHU_PRIO_LVL_1                    1 //
#define CGPIOD_SHU_PRIO_LVL_2                    2 //
#define CGPIOD_SHU_PRIO_LVL_3                    3 //
#define CGPIOD_SHU_PRIO_LVL_4                    4 //
#define CGPIOD_SHU_PRIO_LVL_5                    5 //

#define CGPIOD_SHU_PRIO_MASK_NONE       0x00000000 //
#define CGPIOD_SHU_PRIO_MASK_0          0x00000001 //
#define CGPIOD_SHU_PRIO_MASK_1          0x00000002 //
#define CGPIOD_SHU_PRIO_MASK_2          0x00000004 //
#define CGPIOD_SHU_PRIO_MASK_3          0x00000008 //
#define CGPIOD_SHU_PRIO_MASK_4          0x00000010 //
#define CGPIOD_SHU_PRIO_MASK_5          0x00000020 //

#define CGPIOD_SHU_FLG_NONE             0x00000000 //
#define CGPIOD_SHU_FLG_LOCKED           0x40000000 //

typedef struct {
  tUint32            dwFlags;                      //
  tUint32            dwPinUp;                      // GPIO pin number
  tUint32            dwPinDown;                    // GPIO pin number
  tUint32            dwPol;                        // polarity 0=normal, 1=invert
  tUint32            dwRunDef;                     // default run time for standalone oper
  tUint32            dwState;                      // CGPIOD_SHU_STATE_
  tUint32            dwPrioLvl;                    // 0..5
  tUint32            dwPrioMask;                   // 0..63
  tUint32            dwCmd;                        // 
  tUint32            dwDelay1;                     // delay time
  tUint32            dwRun;                        // run time
  tUint32            dwDelay2;                     // delay time between run and tip
  tUint32            dwTip;                        // tip time
  } tGpiodShutter;

//----------------------------------------------------------------------------
typedef struct {
  tUint32            msNow;                        // I
  tUint32            dwOrig;                       // I
  tUint32            dwObj;                        // I
  tUint32            dwCmd;                        // I
  tUint32            dwParms;                      // I
  tUint32            dwError;                      // O
  tUint32            dwRsp;                        // O

  union {
    struct {
      tUint32        dwDebounce;                   //
      } parmsInput; 
    struct {
      tUint32        dwLock;                       //
      tUint32        dwDelay;                      //
      tUint32        dwRun;                        //
      } parmsOutput;
    struct {
      tUint32        dwPrioMask;                   //
      tUint32        dwPrioLvl;                    //
      tUint32        dwLock;                       //
      tUint32        dwDelay;                      //
      tUint32        dwRun;                        //
      tUint32        dwTip;                        //
      } parmsShutter;
    struct {
      tUint32        dwParm;                       //
      tUint32        dwAck;                        //
      } parmsSystem;
    };

  } tGpiodCmd;

typedef struct {
  tUint32            msNow;                        // 
  tUint32            dwOrig;                       // I
  tUint32            dwObj;                        // 
  tCChar*            szObj;                        //
  tUint32            dwEvt;                        // 
  tCChar*            szEvt;                        //
  } tGpiodEvt;

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class CGpiod {
 private:
  tUint32            m_dwError;                    //
  tUint32            m_dwFlags;                    //
  tUint32            m_dwTicks = 0;                //

  tGpiodHbeat        m_hbeat;                      // heartbeat counter

  tGpiodInput        m_input[CGPIOD_IN_COUNT];     // in0-3
  tGpiodOutput       m_output[CGPIOD_OUT_COUNT];   // out0-3
  tGpiodShutter      m_shutter[CGPIOD_SHU_COUNT];  // out0-1

  CParse             m_parse;                      //

 public:
  //--------------------------------------------------------------------------
  void               begin();
  void               checkConnection();

  //--------------------------------------------------------------------------
  //
  //--------------------------------------------------------------------------
  tUint32            GetFlags(tUint32 dwFlags)     { return m_dwFlags &   dwFlags; }
  tUint32            SetFlags(tUint32 dwFlags)     { return m_dwFlags |=  dwFlags; }
  tUint32            RstFlags(tUint32 dwFlags)     { return m_dwFlags &= ~dwFlags; }

  //--------------------------------------------------------------------------
  //
  //--------------------------------------------------------------------------
  tUint32            GetFlags(tUint32* pFlags, tUint32 dwFlags) { return *pFlags &   dwFlags; }
  tUint32            SetFlags(tUint32* pFlags, tUint32 dwFlags) { return *pFlags |=  dwFlags; }
  tUint32            RstFlags(tUint32* pFlags, tUint32 dwFlags) { return *pFlags &= ~dwFlags; }
  tUint32            HasFlags(tUint32* pFlags, tUint32 dwFlags) { return ((*pFlags & dwFlags) == dwFlags) ? 1 : 0; }

  //--------------------------------------------------------------------------
  // calculate timer end time in ms, will be at least 1ms
  //--------------------------------------------------------------------------
  tUint32            SetTimerSec(tUint32 msNow, tUint32 dwSec) {
    return (msNow + (dwSec * CGPIOD_SEC_2_MSEC)) | 1; 
    } //

  tUint32            SetTimerDeciSec(tUint32 msNow, tUint32 dwDeciSec) {
    return (msNow + (dwDeciSec * CGPIOD_DECISEC_2_MSEC)) | 1; 
    } //

  tUint32            SetTimerMilliSec(tUint32 msNow, tUint32 dwMilliSec) {
    return (msNow + dwMilliSec) | 1; 
    } //

  //--------------------------------------------------------------------------
  // check if timer is expired
  //--------------------------------------------------------------------------
  tUint32            ChkTimer(tUint32 msNow, tUint32 msTimer) {
    // handle clock wrap
    if ((msTimer > ESP8266_MILLIS_MID) && (msNow < ESP8266_MILLIS_MID))
      msNow += ESP8266_MILLIS_MAX;
    
    return (msNow > msTimer) ? 1 : 0;
    } //

  //--------------------------------------------------------------------------
  // gpiod.cpp
  //--------------------------------------------------------------------------
  tUint32            OnConfig();
  tUint32            OnInit();
  void               OnRun();
  tUint32            OnExit();

  tUint32            GetState(tUint32 dwObj);

  tUint32            DoCmd(tGpiodCmd* pCmd);
  tUint32            DoEvt(tGpiodEvt* pEvt);
  tUint32            DoSta(tGpiodEvt* pEvt);

  //--------------------------------------------------------------------------
  // gpiod_parse.cpp
  //--------------------------------------------------------------------------
  tUint32            ParseCmd(tGpiodCmd* pOut, tChar* pObj, tChar* pCmd, tUint32 dwMask0, tUint32 dwMask1);
  tUint32            _parseCmdInput(tGpiodCmd* pOut);
  tUint32            _parseCmdOutput(tGpiodCmd* pOut);
  tUint32            _parseCmdShutter(tGpiodCmd* pOut);
  tUint32            _parseCmdRgb(tGpiodCmd* pOut);
  tUint32            _parseCmdTimer(tGpiodCmd* pOut);
  tUint32            _parseCmdSystem(tGpiodCmd* pOut);

  //--------------------------------------------------------------------------
  // gpiod_print.cpp
  //--------------------------------------------------------------------------
  tCChar*            PrintObj2String(tChar* pOut, tUint32 dwObj);
  tCChar*            PrintObjSta2String(tChar* pOut, tUint32 dwObj, tUint32 dwSta);
  tCChar*            PrintObjEvt2String(tChar* pOut, tUint32 dwObj, tUint32 dwEvt);
  tCChar*            PrintObjCmd2String(tChar* pOut, tUint32 dwObj, tUint32 dwCmd);
  tCChar*            PrintVal2String(tChar* pOut, tUint32 dwVal);
  tCChar*            PrintCmdParamVals(tChar* pOut, tUint32 cbOut, tGpiodCmd* pCmd);

 private:
  //--------------------------------------------------------------------------
  // gpiod_input.cpp
  //--------------------------------------------------------------------------
  tUint32            _inputOnConfig();
  tUint32            _inputOnInit();
  tUint32            _inputOnRun(tUint32 msNow);
  tUint32            _inputOnExit();
  tUint32            _inputDoCmd(tGpiodCmd* pCmd);
  tUint32            _inputGetState(tUint32 dwObj);
  tUint32            _inputGetPinVal(tGpiodInput* pObj, tUint32 msNow);

  //--------------------------------------------------------------------------
  // gpiod_output.cpp
  //--------------------------------------------------------------------------
  tUint32            _outputOnConfig();
  tUint32            _outputOnInit();
  tUint32            _outputOnRun(tUint32 msNow);
  tUint32            _outputOnExit();
  tUint32            _outputDoEvt(tGpiodEvt* pEvt);
  tUint32            _outputDoCmd(tGpiodCmd* pCmd);

  tUint32            _outputGetLock(tUint32 dwObj);
  tUint32            _outputSetLock(tGpiodOutput* pObj, tUint32 dwLock, tGpiodEvt* pEvt);

  tUint32            _outputGetState(tUint32 dwObj);
  tUint32            _outputSetState(tGpiodOutput* pObj, tUint32 dwState, tGpiodEvt* pEvt);

  //--------------------------------------------------------------------------
  // gpiod_shutter.cpp
  //--------------------------------------------------------------------------
  tUint32            _shutterOnConfig();
  tUint32            _shutterOnInit();
  tUint32            _shutterOnRun(tUint32 msNow);
  tUint32            _shutterOnExit();
  tUint32            _shutterDoEvt(tGpiodEvt* pEvt);
  tUint32            _shutterDoCmd(tGpiodCmd* pCmd);
  tUint32            _shutterCheckPrio(tGpiodShutter* pObj, tGpiodCmd* pCmd);
  tUint32            _shutterGetState(tUint32 dwObj);
  tUint32            _shutterSetState(tGpiodShutter* pObj, tUint32 dwState, tGpiodEvt* pEvt);

  //--------------------------------------------------------------------------
  // gpiod_system.cpp
  //--------------------------------------------------------------------------
  tUint32            _systemOnConfig();
  tUint32            _systemOnInit();
  tUint32            _systemOnRun(tUint32 msNow);
  tUint32            _systemOnExit();
  tUint32            _systemDoEvt(tGpiodEvt* pEvt);
  tUint32            _systemDoCmd(tGpiodCmd* pCmd);

  //--------------------------------------------------------------------------
  // gpiod_io.cpp
  //--------------------------------------------------------------------------
  void               _ioSetPinDir(tInt32 pinNum, tUint32 pinDir);
  void               _ioSetPinPullup(tInt32 pinNum, tUint32 bPullup);
  void               _ioSetPinVal(tUint32 pinNum, tUint32 pinVal);
  tUint32            _ioGetPinVal(tUint32 pinNum);

  }; // CGpiod

extern CGpiod g_gpiod;

#endif // __cgpiod_hpp__
