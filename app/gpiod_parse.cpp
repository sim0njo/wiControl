
//----------------------------------------------------------------------------
// cgpiod_parse.cpp : command parser
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

//--------------------------------------------------------------------------
// parse command
//   pObj = 0                         | <obj>
//   pCmd = <obj>.<method> *[.<parm>] | <method> *[.<parm>]
//--------------------------------------------------------------------------
tUint32 CGpiod::ParseCmd(tGpiodCmd* pOut, tChar* pObj, tChar* pCmd, tUint32 dwOrig, tUint32 dwEmul) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    // parse object
//  if (pObj)
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "CGpiod::ParseCmd,obj=%s", pObj);

//  if (pCmd)
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0020, "CGpiod::ParseCmd,cmd=%s", pCmd);

//  Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0030, "CGpiod::ParseCmd,orig=%u,emul=%u", dwOrig, dwEmul);
    memset(pOut, 0, sizeof(tGpiodCmd));
    m_parse.SetReservedIdents(g_gpiodParseObj);
    m_parse.SetString(pObj ? pObj : pCmd);
    if ((m_parse.NextToken(dwOrig, dwEmul) != CPARSE_TYPE_NODE) && (dwErr = XERROR_INPUT))
      break;
      
    pOut->dwOrig = dwOrig;
    pOut->dwObj  = m_parse.TVal();

    // parse command and parms
    if (pObj)
      m_parse.SetString(pCmd);
    else
      m_parse.SkipSeparator(CPARSE_TYPE_PERIOD, 0);

    switch (pOut->dwObj & CGPIOD_OBJ_CLS_MASK) {
      case CGPIOD_OBJ_CLS_INPUT:   dwErr = _parseCmdInput(pOut);
        break;
      case CGPIOD_OBJ_CLS_OUTPUT:  dwErr = _parseCmdOutput(pOut);
        break;
      case CGPIOD_OBJ_CLS_SHUTTER: dwErr = _parseCmdShutter(pOut);
        break;
      case CGPIOD_OBJ_CLS_TIMER:   dwErr = _parseCmdTimer(pOut);
        break;
      case CGPIOD_OBJ_CLS_SYSTEM:  dwErr = _parseCmdSystem(pOut);
        break;
      default:
        break;
      } // switch

    } while (FALSE);

//Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x9999, "CGpiod::ParseCmd,err=%u", dwErr);
  return dwErr; 
  } // ParseCmd

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tUint32 CGpiod::_parseCmdInput(tGpiodCmd* pOut) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    m_parse.SetReservedIdents(g_gpiodParseCmdInput);
    if ((m_parse.NextToken(pOut->dwOrig, xNum2BitMask(pOut->dwObj & CGPIOD_OBJ_NUM_MASK)) != CPARSE_TYPE_LEAF) && (dwErr = XERROR_DATA))
      break;

    pOut->dwCmd = m_parse.TVal();
    } while (FALSE);

  return dwErr; 
  } // _parseCmdInput

//----------------------------------------------------------------------------
// parse output command
//----------------------------------------------------------------------------
tUint32 CGpiod::_parseCmdOutput(tGpiodCmd* pOut) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    // parse command
    m_parse.SetReservedIdents(g_gpiodParseCmdOutput);
    m_parse.NextToken(pOut->dwOrig, xNum2BitMask(pOut->dwObj & CGPIOD_OBJ_NUM_MASK));

    if      (m_parse.TType() == CPARSE_TYPE_NONE)
      // no more token, revert to status command
      pOut->dwCmd = CGPIOD_OUT_CMD_STATUS;

    else if (m_parse.TType() == CPARSE_TYPE_LEAF)
      // known command
      pOut->dwCmd = m_parse.TVal();

    else {
      // input error
      dwErr = XERROR_INPUT;
      break;
      } // else

    // parse parameters
    if (pOut->dwCmd & 0x00080000) { // delay 1-3600 seconds
      if (dwErr = m_parse.GetNumber(&pOut->parmsOutput.dwDelay, 1, 3600)) break;
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "%s,delay=%u", pFunc, pOut->parmsOutput.dwDelay);
      } // if

    if (pOut->dwCmd & 0x02000000) { // run 1-3600 seconds
      if (dwErr = m_parse.GetNumber(&pOut->parmsOutput.dwRun, 1, 3600)) break;
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "%s,run=%u", pFunc, pOut->parmsOutput.dwRun);
      } // if

    } while (FALSE);

  return dwErr; 
  } // _parseCmdOutput

//--------------------------------------------------------------------------
// parse shutter command
//--------------------------------------------------------------------------
tUint32 CGpiod::_parseCmdShutter(tGpiodCmd* pOut) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    // parse command
    m_parse.SetReservedIdents(g_gpiodParseCmdShutter);
    if ((m_parse.NextToken(pOut->dwOrig, xNum2BitMask(pOut->dwObj & CGPIOD_OBJ_NUM_MASK)) != CPARSE_TYPE_LEAF) && (dwErr = XERROR_DATA))
      break;

    pOut->dwCmd = m_parse.TVal();

    // parse parameters
    if (pOut->dwCmd & 0x00010000) { // prio-mask 0-63
      if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwPrioMask, 0, 63)) break;
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "%s,prio-mask=%u", pFunc, pOut->parmsShutter.dwPrioMask);
      } // if

    if (pOut->dwCmd & 0x00020000) { // prio-level 0-5
      if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwPrioLvl, 0, 5)) break;
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "%s,prio-lvl=%u", pFunc, pOut->parmsShutter.dwPrioLvl);
      } // if

    if (pOut->dwCmd & 0x00040000) { // lock 0-1
      if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwLock, 0, 1)) break;
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "%s,lock=%u", pFunc, pOut->parmsShutter.dwLock);
      } // if

    if (pOut->dwCmd & 0x00100000) { // delay 0-3600 seconds
      if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwDelay, 0, 3600)) break;
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "%s,delay=%u", pFunc, pOut->parmsShutter.dwDelay);
      } // if

    if (pOut->dwCmd & 0x04000000) { // run 1-3600 seconds
      if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwRun, 1, 3600)) break;
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "%s,run=%u", pFunc, pOut->parmsShutter.dwRun);
      } // if

    if (pOut->dwCmd & 0x08000000) { // tip 0-3600 1/10th seconds
      if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwTip, 0, 3600)) break;
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "%s,tip=%u", pFunc, pOut->parmsShutter.dwTip);
      } // if

    } while (FALSE);

  return dwErr; 
  } // _parseCmdShutter

//----------------------------------------------------------------------------
// parse timer command
//----------------------------------------------------------------------------
tUint32 CGpiod::_parseCmdTimer(tGpiodCmd* pOut) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    // parse command
    m_parse.SetReservedIdents(g_gpiodParseCmdTimer);
    if ((m_parse.NextToken(pOut->dwOrig, xNum2BitMask(pOut->dwObj & CGPIOD_OBJ_NUM_MASK)) != CPARSE_TYPE_LEAF) && (dwErr = XERROR_DATA))
      break;

    pOut->dwCmd = m_parse.TVal();

    // parse parameters
    if (pOut->dwCmd & 0x00080000) { // delay 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = m_parse.GetNumber(&pOut->parmsTimer.dwDelay, 1, 65535)) break;
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "%s,delay=%u", pFunc, pOut->parmsOutput.dwDelay);
      } // if

    if (pOut->dwCmd & 0x02000000) { // runtime 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = m_parse.GetNumber(&pOut->parmsTimer.dwRun, 1, 65535)) break;
//    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "%s,run=%u", pFunc, pOut->parmsOutput.dwRun);
      } // if

    } while (FALSE);

  return dwErr; 
  } // _parseCmdTimer

//----------------------------------------------------------------------------
// parse system command
//----------------------------------------------------------------------------
tUint32 CGpiod::_parseCmdSystem(tGpiodCmd* pOut) 
{
  tUint32 dwErr = XERROR_SUCCESS;

  do {
    // parse command
    m_parse.SetReservedIdents(g_gpiodParseCmdSystem);
    if ((m_parse.NextToken(pOut->dwOrig, 1) != CPARSE_TYPE_LEAF) && (dwErr = XERROR_DATA))
      break;

    pOut->dwCmd = m_parse.TVal();

    // if more then parse parms for set 
    if (m_parse.NextToken(0, 0) == CPARSE_TYPE_PERIOD) {

      // parse parameters
      if (pOut->dwCmd & 0x00010000) { // loglevel
        if (dwErr = m_parse.GetNumber(&pOut->parmsSystem.dwParm, 0, 0xFFFFFFFF)) break;
        pOut->dwParms |= 0x00010000;
        } // if

      if (pOut->dwCmd & 0x00020000) { // emul
        if (dwErr = m_parse.GetNumber(&pOut->parmsSystem.dwParm, CGPIOD_EMUL_OUTPUT, CGPIOD_EMUL_SHUTTER)) break;
        pOut->dwParms |= 0x00020000;
        } // if

      if (pOut->dwCmd & 0x00040000) { // mode
        if (dwErr = m_parse.GetNumber(&pOut->parmsSystem.dwParm, CGPIOD_MODE_STANDALONE, CGPIOD_MODE_BOTH)) break;
        pOut->dwParms |= 0x00040000;
        } // if

      if (pOut->dwCmd & 0x00080000) { // efmt
        if (dwErr = m_parse.GetNumber(&pOut->parmsSystem.dwParm, CGPIOD_EFMT_NUMERICAL, CGPIOD_EFMT_TEXTUAL)) break;
        pOut->dwParms |= 0x00080000;
        } // if

      if (pOut->dwCmd & 0x00100000) { // lock | disable
        if (dwErr = m_parse.GetNumber(&pOut->parmsSystem.dwParm, 0, 1)) break;
        pOut->dwParms |= 0x00100000;
        } // if

      if (pOut->dwCmd & 0x00200000) { // pwm
        if (dwErr = m_parse.GetNumber(&pOut->parmsSystem.dwParm, 0, 0xFFFFFFFF)) break;
        pOut->dwParms |= 0x00200000;
        } // if

      if (pOut->dwCmd & 0x80000000) { // ack
        m_parse.SkipSeparator(CPARSE_TYPE_PERIOD, FALSE);

        if (m_parse.NextToken(pOut->dwOrig, pOut->dwObj & CGPIOD_OBJ_CLS_MASK) == CPARSE_TYPE_PARM)
          pOut->parmsSystem.dwAck = m_parse.TVal();
        else 
          dwErr = XERROR_INPUT;

        } // if
      } // if

    } while (FALSE);

  return dwErr; 
  } // _parseCmdSystem

