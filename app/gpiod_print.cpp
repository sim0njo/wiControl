
//----------------------------------------------------------------------------
// cgpiod_print.hpp : command printer
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
//#include <SmingCore/SmingCore.h>
//#include <SmingCore/Debug.h>
#include <gpiod.h>

//--------------------------------------------------------------------------
// print event for debug purposes
//--------------------------------------------------------------------------
void CGpiod::PrintEvt(tGpiodEvt* pEvt) 
{
  tChar str[64], str1[32], str2[32];

  if (pEvt->szEvt)
    gsprintf(str, "CGpiod::PrintEvt,%s.%s", _printObj2String(str1, pEvt->dwObj), pEvt->szEvt);
  else 
    gsprintf(str, "CGpiod::PrintEvt,%s.%s", _printObj2String(str1, pEvt->dwObj), _printObjEvt2String(str2, pEvt->dwObj, pEvt->dwEvt));

  Debug.println(str);
  } // PrintEvt

//--------------------------------------------------------------------------
// print command for debug purposes
//--------------------------------------------------------------------------
void CGpiod::PrintCmd(tGpiodCmd* pCmd) 
{
  tChar     str[64], str1[32], str2[32], str3[32];

  gsprintf(str, "CGpiod::PrintCmd,%s.%s%s", _printObj2String(str1, pCmd->dwObj), 
           _printObjCmd2String(str2, pCmd->dwObj, pCmd->dwCmd), _printCmdParamVals(str3, sizeof(str2), pCmd));
  Debug.println(str);
  } // ParseCmd

//----------------------------------------------------------------------------
// print obj name or obj-id in hex if not found
//----------------------------------------------------------------------------
tCChar* CGpiod::_printObj2String(tChar* pOut, tUint32 dwObj) 
{
  tUint32    dwErr = XERROR_SUCCESS;
  tParseRsvd *pRsvd = g_gpiodParseObj;

  for ( ; pRsvd->dwMask0; pRsvd++)
    if (pRsvd->dwTVal == dwObj) { 
      gstrcpy(pOut, pRsvd->szTVal);
      return pOut; 
      } //

  gsprintf(pOut, "%08X", dwObj);
  return pOut; 
  } // _printObj2String

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tCChar* CGpiod::_printObjEvt2String(tChar* pOut, tUint32 dwObj, tUint32 dwEvt) 
{
  tUint32    dwErr  = XERROR_SUCCESS;
  tParseRsvd *pRsvd = g_gpiodParseEvtInput;
//tParseRsvd *pRsvd = (dwObj & CGPIOD_OBJ_CLS_INPUT)   ? g_gpiodParseEvtInput  :
//                    (dwObj & CGPIOD_OBJ_CLS_OUTPUT)  ? g_gpiodParseCmdOutput  :
//                    (dwObj & CGPIOD_OBJ_CLS_SHUTTER) ? g_gpiodParseCmdShutter : g_gpiodParseCmdSystem;

  for ( ; pRsvd->dwMask0; pRsvd++)
    if (((pRsvd->dwMask1 & CGPIOD_OBJ_CLS_MASK) &  (dwObj & CGPIOD_OBJ_CLS_MASK)) &&
        ((pRsvd->dwTVal  & CGPIOD_OBJ_EVT_MASK) == (dwEvt & CGPIOD_OBJ_EVT_MASK))) { 
      gstrcpy(pOut, pRsvd->szTVal);
      return pOut; 
      } // if

  gsprintf(pOut, "%08X", dwEvt);
  return pOut; 
  } // _printObjEvt2String

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tCChar* CGpiod::_printObjCmd2String(tChar* pOut, tUint32 dwObj, tUint32 dwCmd) 
{
  tUint32    dwErr  = XERROR_SUCCESS;
  tParseRsvd *pRsvd = (dwObj & CGPIOD_OBJ_CLS_INPUT)   ? g_gpiodParseEvtInput  :
                      (dwObj & CGPIOD_OBJ_CLS_OUTPUT)  ? g_gpiodParseCmdOutput  :
                      (dwObj & CGPIOD_OBJ_CLS_SHUTTER) ? g_gpiodParseCmdShutter : g_gpiodParseCmdSystem;

  for ( ; pRsvd->dwMask0; pRsvd++)
    if (((pRsvd->dwMask1 & CGPIOD_OBJ_CLS_MASK) &  (dwObj & CGPIOD_OBJ_CLS_MASK)) &&
        ((pRsvd->dwTVal  & CGPIOD_OBJ_CMD_MASK) == (dwCmd & CGPIOD_OBJ_CMD_MASK))) {
      gstrcpy(pOut, pRsvd->szTVal);
      return pOut; 
      } // if

  gsprintf(pOut, "%08X", dwCmd);
  return pOut; 
  } // _printObjCmd2String

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tCChar* CGpiod::_printVal2String(tChar* pOut, tUint32 dwVal) 
{
  gsprintf(pOut, "%u", dwVal);
  return pOut;
  } // _printVal2String

//----------------------------------------------------------------------------
// print all parameters to output buffer
//----------------------------------------------------------------------------
tCChar* CGpiod::_printCmdParamVals(tChar* pOut, tUint32 cbOut, tGpiodCmd* pCmd) 
{
  tChar str[32];

  do {
    *pOut = '\0';

    if ((pCmd->dwCmd & 0xFFFF0000) == 0)
      break;

    if (pCmd->dwCmd & 0x00010000) { // prio-mask 0-63
      gsprintf(str, ".%u", pCmd->dwPrioMask);
      xstrcatn(pOut, cbOut, str, 0);
      } // if

    if (pCmd->dwCmd & 0x00020000) { // prio-level 0-5
      gsprintf(str, ".%u", pCmd->dwPrioLvl);
      xstrcatn(pOut, cbOut, str, 0);
      } // if

    if (pCmd->dwCmd & 0x00040000) { // lock 0-1
      gsprintf(str, ".%u", pCmd->dwLock);
      xstrcatn(pOut, cbOut, str, 0);
      } // if

    if (pCmd->dwCmd & 0x00180000) { // delay 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
      gsprintf(str, ".%u", pCmd->dwDelay);
      xstrcatn(pOut, cbOut, str, 0);
      } // if

//  if (pCmd->dwCmd & 0x00600000) { // level 0-255 for dimmer
//    gsprintf(str, ".%u", pCmd->dwLevel);
//    xstrcatn(pOut, cbOut, str, 0);
//    } // if

    if (pCmd->dwCmd & 0x01800000) { // runtime 1-160 seconds
      gsprintf(str, ".%u", pCmd->dwRun);
      xstrcatn(pOut, cbOut, str, 0);
      } // if

    if (pCmd->dwCmd & 0x06000000) { // runtime 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
      gsprintf(str, ".%u", pCmd->dwRun);
      xstrcatn(pOut, cbOut, str, 0);
      } // if

    if (pCmd->dwCmd & 0x08000000) { // tiptime 1-65535 1/10th seconds (6535s or 65535 1/10th s)
      gsprintf(str, ".%u", pCmd->dwTip);
      xstrcatn(pOut, cbOut, str, 0);
      } // if
/*
    if (pCmd->dwCmd & COHCCMD_PARM_MOD_ACK) { // ack
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

    return pOut;
    } // _printCmdParams

