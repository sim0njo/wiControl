
//----------------------------------------------------------------------------
// cgpiod.hpp
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//
// in8  -> D3, out0 -> D8
// in9  -> D4, out1 -> D7
// in10 -> D1, out2 -> D6
// in11 -> D2, out3 -> D5
//             out6 -> timer
//             out7 -> timer
//             led0 -> D0
//----------------------------------------------------------------------------
#ifndef __cgpiod_hpp__
#define __cgpiod_hpp__

#include <mqtt.h>
#include <xstdafx.h>
#include <xerror.h>
#include <cparse.hpp>
#include <app_clslevels.h>

//----------------------------------------------------------------------------
extern tParseRsvd    g_gpiodParseObj[];
extern tParseRsvd    g_gpiodParseObjSta[];
extern tParseRsvd    g_gpiodParseObjEvt[];
extern tParseRsvd    g_gpiodParseCmdInput[];
extern tParseRsvd    g_gpiodParseCmdTimer[];
extern tParseRsvd    g_gpiodParseCmdOutput[];
extern tParseRsvd    g_gpiodParseCmdShutter[];
extern tParseRsvd    g_gpiodParseCmdSystem[];

//----------------------------------------------------------------------------
void                 gpiodOnHttpConfig(HttpRequest &request, HttpResponse &response);
void                 gpiodOnMqttPublish(tChar* szTopic, tChar* szMsg);

//----------------------------------------------------------------------------
#define CGPIOD_VERSION                   "4.0.0.0" //                                   
#define CGPIOD_DATE                       __DATE__ //

#define CGPIOD_CMD_PFX                       "cmd" //
#define CGPIOD_CFG_PFX                       "cfg" //
#define CGPIOD_EVT_PFX                       "evt" //
#define CGPIOD_STA_PFX                       "sta" //
#define CGPIOD_BOO_PFX                       "boo" //

#define CGPIOD_FLG_NONE                 0x00000000 // 
#define CGPIOD_FLG_LOCK                 0x00000001 // 
#define CGPIOD_FLG_DISABLE              0x00000002 // 

#define CGPIOD_EMUL_NONE                         0 // 
#define CGPIOD_EMUL_OUTPUT                       1 // emulate output0-1
#define CGPIOD_EMUL_SHUTTER                      2 // emulate shutter0

#define CGPIOD_MODE_NONE                         0 // 
#define CGPIOD_MODE_STANDALONE                   1 // handle events locally
#define CGPIOD_MODE_MQTT                         2 // publish events
#define CGPIOD_MODE_BOTH                         3 // do both

#define CGPIOD_EFMT_NONE                         0 // 
#define CGPIOD_EFMT_NUMERICAL                    1 // 
#define CGPIOD_EFMT_TEXTUAL                      2 // 

#define CGPIOD_SEC_2_MSEC                     1000 // multiplier for second to ms
#define CGPIOD_DECISEC_2_MSEC                  100 // multiplier for decisecond to ms

//----------------------------------------------------------------------------
#define CGPIOD_ORIG_INPUT               0x80000001 // 
#define CGPIOD_ORIG_OUTPUT              0x80000002 // 
#define CGPIOD_ORIG_SHUTTER             0x80000004 // 
#define CGPIOD_ORIG_SYSTEM              0x80000008 // 
#define CGPIOD_ORIG_TIMER               0x80000010 // 
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
#define CGPIOD_OBJ_CLS_TIMER            0x00000800 // timer   outputs
#define CGPIOD_OBJ_CLS_SYSTEM           0x00001000 // system
#define CGPIOD_OBJ_CLS_HBEAT            0x00002000 // heartbeat
#define CGPIOD_OBJ_CLS_LED              0x00004000 // led

#define CGPIOD_OBJ_CLS_WBS              0x00001B00 // WBS = input + output  + timer + system
#define CGPIOD_OBJ_CLS_WBR              0x00001D00 // WBR = input + shutter + timer + system

#define CGPIOD_CMD_PRM_OPTIONAL         0xFF000000 // optional parms mask
#define CGPIOD_CMD_PRM_MANDATORY        0x00FF0000 // mandatory parms mask

#define CGPIOD_CMD_RSP_AUTO             0x000000FF // cmd with auto response

//----------------------------------------------------------------------------
// system=ping   
// system=loglevel [.<loglevel>.ack]
// system=emul     [.<emul>.ack]
//----------------------------------------------------------------------------
#define CGPIOD_OBJ_SYSTEM               0x00001000 // system

#define CGPIOD_SYS_CMD_NONE                      0 // 
#define CGPIOD_SYS_CMD_PING                      1 // 
#define CGPIOD_SYS_CMD_VERSION                   2 // 
#define CGPIOD_SYS_CMD_MEMORY                    3 // 
#define CGPIOD_SYS_CMD_UPTIME                    4 // 
#define CGPIOD_SYS_CMD_RESTART                   5 // restart.ack
#define CGPIOD_SYS_CMD_LOGLEVEL         0x00000100 // 
#define CGPIOD_SYS_CMD_EMUL             0x00000200 // WBS/WBR
#define CGPIOD_SYS_CMD_MODE             0x00000300 // standalone/networked/both
#define CGPIOD_SYS_CMD_EFMT             0x00000400 // setefmt.0|1.ack
#define CGPIOD_SYS_CMD_LOCK             0x00000500 // 
#define CGPIOD_SYS_CMD_DISABLE          0x00000600 // 

#define CGPIOD_SYS_PRM_LOGLEVEL         0x01000000 //
#define CGPIOD_SYS_PRM_EMUL             0x02000000 //
#define CGPIOD_SYS_PRM_MODE             0x04000000 //
#define CGPIOD_SYS_PRM_EFMT             0x08000000 //
#define CGPIOD_SYS_PRM_OFFON            0x10000000 //
#define CGPIOD_SYS_PRM_ACK              0x00800000 //

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
// led definitions
//----------------------------------------------------------------------------
#define CGPIOD_LED_PIN                          16 // D0
#define CGPIOD_LED_BLINKTIME                 10000 // 10s

#define CGPIOD_LED_CMD_NONE                      0 //
#define CGPIOD_LED_CMD_ON                        2 //
#define CGPIOD_LED_CMD_OFF                       3 //
#define CGPIOD_LED_CMD_ONTIMED                  10 //
#define CGPIOD_LED_CMD_BLINK                    19 // extended
#define CGPIOD_LED_CMD_BLINKTIMED               20 // extended

//----------------------------------------------------------------------------
// input definitions
//----------------------------------------------------------------------------
#define CGPIOD_IN_COUNT                          4 //

#define CGPIOD_IN0                               0 //
#define CGPIOD_IN1                               1 //
#define CGPIOD_IN2                               2 //
#define CGPIOD_IN3                               3 //

#define CGPIOD_IN0_PIN                           0 // D3 x
#define CGPIOD_IN1_PIN                           2 // D4 x
#define CGPIOD_IN2_PIN                           5 // D1 x
#define CGPIOD_IN3_PIN                           4 // D2 x

#define CGPIOD_IN_TMR_DEBOUNCE                 100 // in ms
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
#define CGPIOD_IN_CMD_DEBOUNCE          0x00000100 // 

#define CGPIOD_IN_PRM_DEBOUNCE          0x01000000 // optional

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
  tUint32            dwFlags;                      // evt MQTT routing flags
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

#define CGPIOD_OUT0_PIN                         15 // D8 
#define CGPIOD_OUT1_PIN                         13 // D7 
#define CGPIOD_OUT2_PIN                         12 // D6
#define CGPIOD_OUT3_PIN                         14 // D5

#define CGPIOD_OUT_RUN_DEF                       0 // infinite

#define CGPIOD_OUT_STATE_OFF                     0 // 
#define CGPIOD_OUT_STATE_ON                      1 // 

#define CGPIOD_OUT_CMD_NONE                      0 // out0-1
#define CGPIOD_OUT_CMD_STATUS                    1 // out0-1
#define CGPIOD_OUT_CMD_ON                        2 // out0-1
#define CGPIOD_OUT_CMD_OFF                       3 // out0-1
#define CGPIOD_OUT_CMD_ONLOCKED                  4 // out0-1
#define CGPIOD_OUT_CMD_OFFLOCKED                 5 // out0-1
#define CGPIOD_OUT_CMD_TOGGLE                    6 // out0-1
#define CGPIOD_OUT_CMD_UNLOCK                    7 // out0-1
#define CGPIOD_OUT_CMD_ONDELAYED                 8 // out0-1
#define CGPIOD_OUT_CMD_OFFDELAYED                9 // out0-1
#define CGPIOD_OUT_CMD_ONTIMED                  10 // out0-1
#define CGPIOD_OUT_CMD_OFFTIMED                 11 // out0-1
#define CGPIOD_OUT_CMD_TOGGLEDELAYED            12 // out0-1
#define CGPIOD_OUT_CMD_TOGGLETIMED              13 // out0-1
#define CGPIOD_OUT_CMD_LOCK                     14 // out0-1
#define CGPIOD_OUT_CMD_LOCKTIMED                15 // out0-1
#define CGPIOD_OUT_CMD_TIMESET                  16 // out0-1
#define CGPIOD_OUT_CMD_TIMEADD                  17 // out0-1
#define CGPIOD_OUT_CMD_TIMEABORT                18 // out0-1
#define CGPIOD_OUT_CMD_BLINK                    23 // extended
#define CGPIOD_OUT_CMD_BLINKTIMED               24 // extended
#define CGPIOD_OUT_CMD_EMULTIME         0x00000100 // extended

#define CGPIOD_OUT_PRM_DELAY            0x00010000 //
#define CGPIOD_OUT_PRM_RUN              0x00020000 //
#define CGPIOD_OUT_PRM_EMULTIME         0x01000000 // optional

#define CGPIOD_OUT_EVT_OFF                       0 //
#define CGPIOD_OUT_EVT_ON                        1 //
#define CGPIOD_OUT_EVT_TIMEXP                    5 // time ended

#define CGPIOD_OUT_FLG_NONE             0x00000000 //
#define CGPIOD_OUT_FLG_MQTT_OFF         0x00000001 //
#define CGPIOD_OUT_FLG_MQTT_ON          0x00000002 //
#define CGPIOD_OUT_FLG_MQTT_TIMEXP      0x00000020 //
#define CGPIOD_OUT_FLG_MQTT_ALL         0x00000023 //
#define CGPIOD_OUT_FLG_LOCKED           0x20000000 //

typedef struct {
  tUint32            dwFlags;                      //
  tUint32            dwPin;                        // GPIO pin number
  tUint32            dwPol;                        // polarity 0=normal, 1=invert
  tUint32            dwRunDef;                     // default run time for standalone oper
  tUint32            dwState;                      //
  tUint32            dwCmd;                        // 
  tUint32            dwRun;                        // time 2
  } tGpiodOutput;

//----------------------------------------------------------------------------
// shutter definitions
//----------------------------------------------------------------------------
#define CGPIOD_UDM_COUNT                         2 //

#define CGPIOD_UDM0                              0 //
#define CGPIOD_UDM1                              1 //

#define CGPIOD_UDM0_PIN_UP                      15 // out0 D8 
#define CGPIOD_UDM0_PIN_DOWN                    13 // out1 D7
#define CGPIOD_UDM1_PIN_UP                      12 // out2 D6
#define CGPIOD_UDM1_PIN_DOWN                    14 // out3 D5

#define CGPIOD_UDM_RUN_DEF                   30000 // 30s

#define CGPIOD_UDM_STATE_STOP                    0 // 
#define CGPIOD_UDM_STATE_UP                      1 // 
#define CGPIOD_UDM_STATE_DOWN                    2 // 

#define CGPIOD_UDM_CMD_NONE                      0 //
#define CGPIOD_UDM_CMD_STATUS                    1 // out0-1
#define CGPIOD_UDM_CMD_STOP                      2 // out0-1
#define CGPIOD_UDM_CMD_TOGGLEUP                  3 // out0-1
#define CGPIOD_UDM_CMD_TOGGLEDOWN                4 // out0-1
#define CGPIOD_UDM_CMD_UP                        5 // out0-1
#define CGPIOD_UDM_CMD_DOWN                      6 // out0-1
#define CGPIOD_UDM_CMD_TIPUP                     7 // out0-1
#define CGPIOD_UDM_CMD_TIPDOWN                   8 // out0-1
#define CGPIOD_UDM_CMD_PRIOLOCK                  9 // out0-1
#define CGPIOD_UDM_CMD_PRIOUNLOCK               10 // out0-1
#define CGPIOD_UDM_CMD_LEARNON                  11 // out0-1
#define CGPIOD_UDM_CMD_LEARNOFF                 12 // out0-1
#define CGPIOD_UDM_CMD_PRIOSET                  13 // out0-1
#define CGPIOD_UDM_CMD_PRIORESET                14 // out0-1
#define CGPIOD_UDM_CMD_SENSROLUP                15 // out0-1
#define CGPIOD_UDM_CMD_SENSJALUP                16 // out0-1
#define CGPIOD_UDM_CMD_SENSROLDOWN              17 // out0-1
#define CGPIOD_UDM_CMD_SENSJALDOWN              18 // out0-1
#define CGPIOD_UDM_CMD_EMULTIME         0x00000100 // extended

#define CGPIOD_UDM_PRM_PRIOMASK         0x00010000 //
#define CGPIOD_UDM_PRM_PRIOLEVEL        0x00020000 //
#define CGPIOD_UDM_PRM_PRIOLOCK         0x00040000 //
#define CGPIOD_UDM_PRM_DELAY            0x00080000 //
#define CGPIOD_UDM_PRM_RUN              0x00100000 //
#define CGPIOD_UDM_PRM_TIP              0x00200000 //
#define CGPIOD_UDM_PRM_EMULTIME         0x01000000 // optional

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

//#define CGPIOD_UDM_EVT_STOP                      0 //
#define CGPIOD_UDM_EVT_UPON                      1 //
#define CGPIOD_UDM_EVT_DOWNON                    2 //
#define CGPIOD_UDM_EVT_UPOFF                     3 //
#define CGPIOD_UDM_EVT_DOWNOFF                   4 //
#define CGPIOD_UDM_EVT_TIMEXP                    5 // time ended
#define CGPIOD_UDM_EVT_TIMEROFF                  6 // 
#define CGPIOD_UDM_EVT_TIMERON                   7 // 
#define CGPIOD_UDM_EVT_TIMERABORT                8 // 

#define CGPIOD_UDM_FLG_NONE             0x00000000 //
#define CGPIOD_UDM_FLG_MQTT_STOP        0x00000001 //
#define CGPIOD_UDM_FLG_MQTT_UPON        0x00000002 //
#define CGPIOD_UDM_FLG_MQTT_DOWNON      0x00000004 //
#define CGPIOD_UDM_FLG_MQTT_UPOFF       0x00000008 //
#define CGPIOD_UDM_FLG_MQTT_DOWNOFF     0x00000010 //
#define CGPIOD_UDM_FLG_MQTT_TIMEXP      0x00000020 //
#define CGPIOD_UDM_FLG_MQTT_TIMEROFF    0x00000040 //
#define CGPIOD_UDM_FLG_MQTT_TIMERON     0x00000080 //
#define CGPIOD_UDM_FLG_MQTT_TIMERABORT  0x00000100 //
#define CGPIOD_UDM_FLG_MQTT_ALL         0x000001FF //
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
// timer definitions
//----------------------------------------------------------------------------
#define CGPIOD_TMR_COUNT                         4 // 

#define CGPIOD_TMR_STATE_OFF                     0 // 
#define CGPIOD_TMR_STATE_ON                      1 // 

#define CGPIOD_TMR_CMD_NONE                      0 //
#define CGPIOD_TMR_CMD_STATUS                    1 //
#define CGPIOD_TMR_CMD_TIMERONDELAYED           19 //
#define CGPIOD_TMR_CMD_TIMEROFFDELAYED          20 //
#define CGPIOD_TMR_CMD_TIMERONTIMED             21 //
#define CGPIOD_TMR_CMD_TIMERABORT               22 //

#define CGPIOD_TMR_PRM_DELAY            0x00010000 //
#define CGPIOD_TMR_PRM_RUN              0x00020000 //

#define CGPIOD_TMR_EVT_TIMEROFF                  6 // 
#define CGPIOD_TMR_EVT_TIMERON                   7 // 
#define CGPIOD_TMR_EVT_TIMERABORT                8 // 

#define CGPIOD_TMR_FLG_MQTT_TIMEROFF    0x00000040 //
#define CGPIOD_TMR_FLG_MQTT_TIMERON     0x00000080 //
#define CGPIOD_TMR_FLG_MQTT_TIMERABORT  0x00000100 //
#define CGPIOD_TMR_FLG_MQTT_ALL         0x000001C0 //

typedef struct {
  tUint32            dwFlags;                      //
  tUint32            dwState;                      //
  tUint32            dwCmd;                        // 
  tUint32            dwRun;                        //
  } tGpiodTimer;  

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
      tUint32        dwDelay;                      //
      tUint32        dwRun;                        //
      } parmsTimer;
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
  tCChar*            szTopic;                      //
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
  tUint32            m_dwEmul;                     //
  tUint32            m_dwMode;                     //
  tUint32            m_dwEfmt;                     //

  tGpiodOutput       m_led;                        // system led
  tGpiodHbeat        m_hbeat;                      // heartbeat counter

  tGpiodInput        m_input[CGPIOD_IN_COUNT];     // in8-11
  tGpiodOutput       m_output[CGPIOD_OUT_COUNT];   // out0-3
  tGpiodShutter      m_shutter[CGPIOD_UDM_COUNT];  // out0-1
  tGpiodTimer        m_timer[CGPIOD_TMR_COUNT];    // out6-7

  CParse             m_parse;                      //
  Timer              m_timerRoot;                  //
  Timer              m_timerMqtt;                  //

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
  // calculate timer end time in ms, will be at least 1ms
  //--------------------------------------------------------------------------
  tUint32            SetTimerSec(tUint32 msNow, tUint32 dwSec) {
    return (msNow + (dwSec * CGPIOD_SEC_2_MSEC)) | 1; 
    } //

  tUint32            SetTimerDeciSec(tUint32 msNow, tUint32 dwDeciSec) {
    return (msNow + (dwDeciSec * CGPIOD_DECISEC_2_MSEC)) | 1; 
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
  tUint32            GetEmul()                     { return m_dwEmul; }
  tUint32            GetMode()                     { return m_dwMode; }
  tUint32            GetEfmt()                     { return m_dwEfmt; }

  tUint32            OnConfig();
  tUint32            OnInit();
  void               OnRun();
  tUint32            OnExit();

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
  tUint32            _parseCmdTimer(tGpiodCmd* pOut);
  tUint32            _parseCmdSystem(tGpiodCmd* pOut);

  //--------------------------------------------------------------------------
  // gpiod_print.cpp
  //--------------------------------------------------------------------------
  void               PrintEvt(tGpiodEvt* pEvt, tUint32 dwClsLvl, tCChar* szPfx);
  void               PrintCmd(tGpiodCmd* pCmd, tUint32 dwClsLvl, tCChar* szPfx);
  tCChar*            _printObj2String(tChar* pOut, tUint32 dwObj);
  tCChar*            _printObjSta2String(tChar* pOut, tUint32 dwObj, tUint32 dwSta);
  tCChar*            _printObjEvt2String(tChar* pOut, tUint32 dwObj, tUint32 dwEvt);
  tCChar*            _printObjCmd2String(tChar* pOut, tUint32 dwObj, tUint32 dwCmd);
  tCChar*            _printVal2String(tChar* pOut, tUint32 dwVal);
  tCChar*            _printCmdParamVals(tChar* pOut, tUint32 cbOut, tGpiodCmd* pCmd);

 private:
  //--------------------------------------------------------------------------
  //
  //--------------------------------------------------------------------------
  tUint32            _GetFlags(tUint32* pFlags, tUint32 dwFlags) { return *pFlags &   dwFlags; }
  tUint32            _SetFlags(tUint32* pFlags, tUint32 dwFlags) { return *pFlags |=  dwFlags; }
  tUint32            _RstFlags(tUint32* pFlags, tUint32 dwFlags) { return *pFlags &= ~dwFlags; }

  //--------------------------------------------------------------------------
  // gpiod_input.cpp
  //--------------------------------------------------------------------------
  tUint32            _inputOnConfig();
  tUint32            _inputOnInit();
  tUint32            _inputOnRun(tUint32 msNow);
  tUint32            _inputOnExit();
  tUint32            _inputDoCmd(tGpiodCmd* pCmd);
  tUint32            _inputGetPinVal(tGpiodInput* pObj, tUint32 msNow);

  //--------------------------------------------------------------------------
  // gpiod_timer.cpp
  //--------------------------------------------------------------------------
  tUint32            _timerOnConfig();
  tUint32            _timerOnInit();
  tUint32            _timerOnRun(tUint32 msNow);
  tUint32            _timerOnExit();
  tUint32            _timerDoEvt(tGpiodEvt* pEvt);
  tUint32            _timerDoCmd(tGpiodCmd* pCmd);
  void               _timerSetState(tGpiodTimer* pObj, tUint32 dwState, tGpiodEvt* pEvt);

  //--------------------------------------------------------------------------
  // gpiod_output.cpp
  //--------------------------------------------------------------------------
  tUint32            _outputOnConfig();
  tUint32            _outputOnInit();
  tUint32            _outputOnRun(tUint32 msNow);
  tUint32            _outputOnExit();
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
  tUint32            _shutterDoCmd(tGpiodCmd* pCmd);
  tUint32            _shutterCheckPrio(tGpiodShutter* pObj, tGpiodCmd* pCmd);
  void               _shutterSetState(tGpiodShutter* pObj, tUint32 dwState, tGpiodEvt* pEvt);

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
