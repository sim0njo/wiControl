
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
    m_parse.SetReservedIdents(g_gpiodParseObjEvt);
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
//  Debug.LogTxt(m_dwClsLvl | 0x0000, "%s,mask=0x%08X", pFunc, pOut->dwCmd & 0xFFFF0000);
    if ((pOut->dwCmd & 0xFFFF0000) == 0)
      break;

    if (pOut->dwCmd & 0x00010000) { // prio-mask 0-63
      if (dwErr = m_parse.GetNumber(&pOut->dwPrioMask, 0, 63)) break;
//    Debug.LogTxt(m_dwClsLvl | 0x0010, "%s,prio-mask=%u", pFunc, pOut->dwPrioMask);
      } // if

    if (pOut->dwCmd & 0x00020000) { // prio-level 0-5
      if (dwErr = m_parse.GetNumber(&pOut->dwPrioLvl, 0, 5)) break;
//    Debug.LogTxt(m_dwClsLvl | 0x0010, "%s,prio-lvl=%u", pFunc, pOut->dwPrioLvl);
      } // if

    if (pOut->dwCmd & 0x00040000) { // lock 0-1
      if (dwErr = m_parse.GetNumber(&pOut->dwLock, 0, 1)) break;
//    Debug.LogTxt(m_dwClsLvl | 0x0010, "%s,lock=%u", pFunc, pOut->dwLock);
      } // if

    if (pOut->dwCmd & 0x00180000) { // delay 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = m_parse.GetNumber(&pOut->dwDelay, 1, 65535)) break;
//    Debug.LogTxt(m_dwClsLvl | 0x0010, "%s,delay=%u", pFunc, pOut->dwDelay);
      } // if

//  if (pOut->dwCmd & 0x00600000) { // level 0-255
//    if (dwErr = m_parse.GetNumber(&pOut->dwParm[0], 0, 255)) break;
//    Debug.LogTxt(m_dwClsLvl | 0x0010, "%s,level=%u", pFunc, pOut->dwParm[0]);
//    } // if

    if (pOut->dwCmd & 0x01800000) { // runtime 1-160 seconds
      if (dwErr = m_parse.GetNumber(&pOut->dwRun, 1, 160)) break;
//    Debug.LogTxt(m_dwClsLvl | 0x0010, "%s,run=%u", pFunc, pOut->dwRun);
      } // if

    if (pOut->dwCmd & 0x06000000) { // runtime 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = m_parse.GetNumber(&pOut->dwRun, 1, 65535)) break;
//    Debug.LogTxt(m_dwClsLvl | 0x0010, "%s,run=%u", pFunc, pOut->dwRun);
      } // if

    if (pOut->dwCmd & 0x08000000) { // tiptime 1-65535 1/10th seconds (6535s or 65535 1/10th s)
      if (dwErr = m_parse.GetNumber(&pOut->dwTip, 1, 65535)) break;
//    Debug.LogTxt(m_dwClsLvl | 0x0010, "%s,tip=%u", pFunc, pOut->dwTip);
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

      Debug.LogTxt(m_dwClsLvl | 0x0010, "%s,ack=%u", pFunc, m_req.dwParm[0]);
      } // if
*/
    } while (FALSE);

//  Debug.LogTxt(m_dwClsLvl | 0x9999, "%s,err=%u", pFunc, dwErr);
    return dwErr;
    } // _parseCmdParams

