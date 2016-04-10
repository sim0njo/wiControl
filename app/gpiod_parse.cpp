
//----------------------------------------------------------------------------
// cgpiod_parse.cpp : command parser
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
tUint32 CGpiod::ParseCmd(tGpiodCmd* pOut, tChar* pObj, tChar* pCmd, tUint32 dwMask1) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    // parse object
    m_parse.SetReservedIdents(g_gpiodParseObj);
    m_parse.SetString(pObj);
    if ((m_parse.NextToken(1, dwMask1) != CPARSE_TYPE_NODE) && (dwErr = XERROR_DATA))
      break;
      
    pOut->dwObj = m_parse.TVal();
//  g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,obj=%04X", pFunc, pOut->dwObj);

    // parse command and parms
    m_parse.SetString(pCmd);

    switch (pOut->dwObj & CGPIOD_OBJ_CLS_MASK) {
      case CGPIOD_OBJ_CLS_INPUT:   dwErr = _parseEvtInput(pOut);
        break;
      case CGPIOD_OBJ_CLS_OUTPUT:  dwErr = _parseCmdOutput(pOut);
        break;
      case CGPIOD_OBJ_CLS_SHUTTER: dwErr = _parseCmdShutter(pOut);
        break;
      case CGPIOD_OBJ_CLS_SYSTEM:  dwErr = _parseCmdSystem(pOut);
        break;
      default:
        break;
      } // switch

    } while (FALSE);

  return dwErr; 
  } // ParseCmd

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::_parseEvtInput(tGpiodCmd* pOut) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    m_parse.SetReservedIdents(g_gpiodParseEvtInput);
    if ((m_parse.NextToken(1, CGPIOD_OBJ_CLS_INPUT) != CPARSE_TYPE_LEAF) && (dwErr = XERROR_DATA))
      break;

    pOut->dwCmd = m_parse.TVal();
    dwErr = _parseCmdParams(pOut);
    } while (FALSE);

  return dwErr; 
  } // _parseEvtInput

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::_parseCmdOutput(tGpiodCmd* pOut) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    m_parse.SetReservedIdents(g_gpiodParseCmdOutput);
    if ((m_parse.NextToken(1, CGPIOD_OBJ_CLS_OUTPUT) != CPARSE_TYPE_LEAF) && (dwErr = XERROR_DATA))
      break;

    pOut->dwCmd = m_parse.TVal();
    dwErr = _parseCmdParams(pOut);
    } while (FALSE);

  return dwErr; 
  } // _parseCmdOutput

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
tUint32 CGpiod::_parseCmdShutter(tGpiodCmd* pOut) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    m_parse.SetReservedIdents(g_gpiodParseCmdShutter);
    if ((m_parse.NextToken(1, CGPIOD_OBJ_CLS_SHUTTER) != CPARSE_TYPE_LEAF) && (dwErr = XERROR_DATA))
      break;

    pOut->dwCmd = m_parse.TVal();
    dwErr = _parseCmdParams(pOut);
    } while (FALSE);

  return dwErr; 
  } // _parseCmdShutter

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::_parseCmdSystem(tGpiodCmd* pOut) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    // parse command
    m_parse.SetReservedIdents(g_gpiodParseCmdSystem);
    if ((m_parse.NextToken(1, CGPIOD_OBJ_CLS_SYSTEM) != CPARSE_TYPE_LEAF) && (dwErr = XERROR_DATA))
      break;

    pOut->dwCmd = m_parse.TVal();

    // if more then parse parms for set 
    if (m_parse.NextToken(0, 0) == CPARSE_TYPE_PERIOD) {
      _parseCmdParams(pOut);
      } // if

    } while (FALSE);

  return dwErr; 
  } // _parseCmdSystem

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::_parseCmdParams(tGpiodCmd* pOut) 
{
  tUint32 dwErr = XERROR_SUCCESS;
//tUint32 dwParm;

  do {
//  g_log.LogPrt(m_dwClsLvl | 0x0000, "%s,mask=0x%08X", pFunc, pOut->dwCmd & 0xFFFF0000);
    if ((pOut->dwCmd & 0xFFFF0000) == 0)
      break;

    if (pOut->dwCmd & 0x00010000) { // prio-mask 0-63
      if (dwErr = m_parse.GetNumber(&pOut->dwPrioMask, 0, 63)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,prio-mask=%u", pFunc, pOut->dwPrioMask);
      } // if

    if (pOut->dwCmd & 0x00020000) { // prio-level 0-5
      if (dwErr = m_parse.GetNumber(&pOut->dwPrioLvl, 0, 5)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,prio-lvl=%u", pFunc, pOut->dwPrioLvl);
      } // if

    if (pOut->dwCmd & 0x00040000) { // lock 0-1
      if (dwErr = m_parse.GetNumber(&pOut->dwLock, 0, 1)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,lock=%u", pFunc, pOut->dwLock);
      } // if

    if (pOut->dwCmd & 0x00180000) { // delay 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = m_parse.GetNumber(&pOut->dwDelay, 1, 65535)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,delay=%u", pFunc, pOut->dwDelay);
      } // if

//  if (pOut->dwCmd & 0x00600000) { // level 0-255
//    if (dwErr = m_parse.GetNumber(&pOut->dwParm[0], 0, 255)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,level=%u", pFunc, pOut->dwParm[0]);
//    } // if

    if (pOut->dwCmd & 0x01800000) { // runtime 1-160 seconds
      if (dwErr = m_parse.GetNumber(&pOut->dwRun, 1, 160)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,run=%u", pFunc, pOut->dwRun);
      } // if

    if (pOut->dwCmd & 0x06000000) { // runtime 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = m_parse.GetNumber(&pOut->dwRun, 1, 65535)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,run=%u", pFunc, pOut->dwRun);
      } // if

    if (pOut->dwCmd & 0x08000000) { // tiptime 1-65535 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = m_parse.GetNumber(&pOut->dwTip, 1, 65535)) break;
//    g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,tip=%u", pFunc, pOut->dwTip);
      } // if

    if (pOut->dwCmd & 0x10000000) { // <emul> 0|1
      if (dwErr = m_parse.GetNumber(&pOut->dwEmul, 0, 1)) break;
      } // if

    if (pOut->dwCmd & 0x20000000) { // <mode> 1|2|3
      if (dwErr = m_parse.GetNumber(&pOut->dwMode, 1, 3)) break;
      } // if

    if (pOut->dwCmd & 0x40000000) { // <efmt> 0|1
      if (dwErr = m_parse.GetNumber(&pOut->dwEfmt, 0, 1)) break;
      } // if

/*
    if (pOut->dwCmd & 0x80000000) { // ack
      m_parse.SetReservedIdents(g_ohcCmdParseParms);
      m_parse.SkipSeparator(CPARSE_TYPE_PERIOD, FALSE);

      if (m_parse.NextToken(1, ohcObjGrp2ObjMask(m_req.dwObj)) == CPARSE_TYPE_PARM)
        m_req.dwParm[0] = pParse->TVal();
      else 
        SetError(XERROR_INPUT);

      g_log.LogPrt(m_dwClsLvl | 0x0010, "%s,ack=%u", pFunc, m_req.dwParm[0]);
      } // if
*/
    } while (FALSE);

//  g_log.LogPrt(m_dwClsLvl | 0x9999, "%s,err=%u", pFunc, dwErr);
    return dwErr;
    } // _parseCmdParams

tParseRsvd g_gpiodParseObj[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//                    CGPIOD_OBJ_CLS                       0xPPPPMMMM                                   
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_NODE, 0x00000100, "input0",   },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_NODE, 0x00000101, "input1",   },
  { 0x00000001      , 0x00000201       , CPARSE_TYPE_NODE, 0x00000200, "output0",  },
  { 0x00000001      , 0x00000201       , CPARSE_TYPE_NODE, 0x00000201, "output1",  },
  { 0x00000001      , 0x00000402       , CPARSE_TYPE_NODE, 0x00000400, "shutter0", },
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_NODE, 0x00001000, "system",   },
                                                                         
  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",         },
  };

tParseRsvd g_gpiodParseEvtInput[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//in% wbs/wbr                                                PPPP      params                
//                          CC                                   EEEE  cmd/evt         
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000002, "ingt0",    },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000003, "outlt1",   },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000004, "ingt1",    },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000005, "outgt1",   },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000006, "ingt2",    },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000007, "out",      },

//fb% wbs                                                    PPPP      params                
//                                                               EEEE  cmd/evt         
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000002, "on",       },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000003, "off",      },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000004, "timexp",   },

//fb% wbr                                                    PPPP      params                
//                                                               EEEE  cmd/evt         
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000001, "stop",     },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000002, "upon",     },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000003, "downon",   },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000004, "upoff",    },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000005, "downoff",  },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000006, "timexp",   },

  { CPARSE_MASK_NONE, CPARSE_MASK_NONE , CPARSE_TYPE_NONE, 0x00000000, "",         },
  };

tParseRsvd g_gpiodParseCmdOutput[] = {
//  ModMask           ChnMask            CPARSE_TYPE_LEAF    PPPPMMMM
//output%: wbs             
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000001, "status",        },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000002, "on",            },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000003, "off",           },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000004, "onlocked",      },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000005, "offlocked",     },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000006, "toggle",        },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000007, "unlock",        },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00080008, "ondelayed",     }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00080009, "offdelayed",    }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0200000A, "ontimed",       }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0200000B, "offtimed",      }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0008000C, "toggledelayed", }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0200000D, "toggletimed",   }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0000000E, "lock",          },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0000000F, "locktimed",     },
  { 0x00000001      , 0x00000800       , CPARSE_TYPE_LEAF, 0x02000010, "timeset",       }, // 1-65535 s
  { 0x00000001      , 0x00000800       , CPARSE_TYPE_LEAF, 0x02000011, "timeadd",       }, // 1-65535 s
  { 0x00000001      , 0x00000800       , CPARSE_TYPE_LEAF, 0x00000012, "timeabort",     },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000013, "blink",         },

  { CPARSE_MASK_NONE, CPARSE_MASK_NONE , CPARSE_TYPE_NONE, 0x00000000, "",              },
  };

tParseRsvd g_gpiodParseCmdShutter[] = {
//  ModMask           ChnMask            CPARSE_TYPE_LEAF    PPPPMMMM
//shutter%: wbr
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000001, "status",          },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00020002, "stop",            }, // 0-5
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060003, "toggleUp",        }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060004, "toggleDown",      }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060005, "up",              }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060006, "down",            }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060007, "tipUp",           }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060008, "tipDown",         }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00010009, "priolock",        }, // 0-63
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0001000A, "priounlock",      }, // 0-63
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0000000B, "learnon",         },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0000000C, "learnoff",        },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0001000D, "prioset",         }, // 0-63
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0001000E, "prioreset",       }, // 0-63
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0416000F, "delayedup",       }, // 0-5/0-1/D1-65535/R1-65535
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0C160010, "tipdelayedup",    }, // 0-5/0-1/D1-65535/R1-65535/T1-65535
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04160011, "delayeddown",     }, // 0-5/0-1/D1-65535/R1-65535
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0C160012, "tipdelayeddown",  }, // 0-5/0-1/D1-65535/R1-65535/T1-65535
                                                                                             //         1/10s    1/10s    1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04000013, "timerondelayed",  }, // R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04000014, "timeroffdelayed", }, // R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04000015, "timerontimed",    }, // R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000016, "timerAbort",      }, //

  { CPARSE_MASK_NONE, CPARSE_MASK_NONE , CPARSE_TYPE_NONE, 0x00000000, "",                },
  };

tParseRsvd g_gpiodParseCmdSystem[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//                                                         0xPPPPMMMM                                   
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000001, "version",   },
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000002, "memory",    },
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000003, "uptime",    },
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000004, "emul",      }, // <emul>.ack
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000005, "mode",      }, // <mode>.ack
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000006, "efmt",      }, // <efmt>.ack
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000007, "disable",   }, // ack
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000008, "enable",    }, // ack
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000009, "reboot",    }, // ack
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000000, "output",    },
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000001, "shutter",   },
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000001, "standalone",},
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000002, "networked", },
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000003, "both",      },
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000000, "numerical", },
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000001, "textual",   } ,
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000001, "ack",       },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "", },
  };

