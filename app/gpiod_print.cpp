
//----------------------------------------------------------------------------
// cgpiod_print.hpp : command printer
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

//--------------------------------------------------------------------------
// print event for debug purposes
//--------------------------------------------------------------------------
void CGpiod::PrintEvt(tGpiodEvt* pEvt, tUint32 dwClsLvl, tCChar* szPfx) 
{
  tChar  str1[16], str2[16];
  tCChar *szTopic = pEvt->szTopic ? pEvt->szTopic : "system";

  if      (pEvt->dwObj == CGPIOD_OBJ_SYSTEM)
    Debug.logTxt(dwClsLvl, "%s,%s.%s", szPfx, szTopic, pEvt->szEvt);

  else if (pEvt->szEvt)
    Debug.logTxt(dwClsLvl, "%s,%s.%s", szPfx, _printObj2String(str1, pEvt->dwObj), pEvt->szEvt);

  else 
    Debug.logTxt(dwClsLvl, "%s,%s.%s", szPfx, 
                 _printObj2String(str1, pEvt->dwObj), _printObjEvt2String(str2, pEvt->dwObj, pEvt->dwEvt));

  } // PrintEvt

//--------------------------------------------------------------------------
// print command for debug purposes
//--------------------------------------------------------------------------
void CGpiod::PrintCmd(tGpiodCmd* pCmd, tUint32 dwClsLvl, tCChar* szPfx) 
{
  tChar  str1[16], str2[16], str3[32];

  Debug.logTxt(dwClsLvl, "%s,%s.%s%s", szPfx, _printObj2String(str1, pCmd->dwObj),
               _printObjCmd2String(str2, pCmd->dwObj, pCmd->dwCmd), _printCmdParamVals(str3, sizeof(str2), pCmd));
  } // PrintCmd

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
// convert object status to string
//----------------------------------------------------------------------------
tCChar* CGpiod::_printObjSta2String(tChar* pOut, tUint32 dwObj, tUint32 dwSta) 
{
  tParseRsvd *pRsvd = g_gpiodParseObjSta;

  for ( ; pRsvd->dwMask0; pRsvd++)
    if (((pRsvd->dwMask0 & CGPIOD_OBJ_CLS_MASK) &  (dwObj & CGPIOD_OBJ_CLS_MASK)) &&
        ( pRsvd->dwTVal                         ==  dwSta                       )) { 
      gstrcpy(pOut, pRsvd->szTVal);
      return pOut; 
      } // if

  gsprintf(pOut, "%08X", dwSta);
  return pOut; 
  } // _printObjSta2String

//----------------------------------------------------------------------------
// convert object event to string
//----------------------------------------------------------------------------
tCChar* CGpiod::_printObjEvt2String(tChar* pOut, tUint32 dwObj, tUint32 dwEvt) 
{
  tParseRsvd *pRsvd = g_gpiodParseObjEvt;

  for ( ; pRsvd->dwMask0; pRsvd++)
    if (((pRsvd->dwMask0 & CGPIOD_OBJ_CLS_MASK) &  (dwObj & CGPIOD_OBJ_CLS_MASK)) &&
        ( pRsvd->dwTVal                         ==  dwEvt                       )) { 
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
  tParseRsvd *pRsvd = (dwObj & CGPIOD_OBJ_CLS_INPUT)   ? g_gpiodParseCmdInput   :
                      (dwObj & CGPIOD_OBJ_CLS_OUTPUT)  ? g_gpiodParseCmdOutput  :
                      (dwObj & CGPIOD_OBJ_CLS_SHUTTER) ? g_gpiodParseCmdShutter : g_gpiodParseCmdSystem;

  for ( ; pRsvd->dwMask0; pRsvd++)
    if (
//      ((pRsvd->dwMask1 & CGPIOD_OBJ_CLS_MASK) &  (dwObj & CGPIOD_OBJ_CLS_MASK)) &&
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

    switch (pCmd->dwObj & CGPIOD_OBJ_CLS_MASK) {
      case CGPIOD_OBJ_CLS_INPUT:   
        break;

      case CGPIOD_OBJ_CLS_OUTPUT:  
      case CGPIOD_OBJ_CLS_TIMER:  
        if (pCmd->dwCmd & 0x00080000) { // delay 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
          gsprintf(str, ".%u", pCmd->parmsOutput.dwDelay);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        if (pCmd->dwCmd & 0x02000000) { // runtime 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
          gsprintf(str, ".%u", pCmd->parmsOutput.dwRun);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        break;

      case CGPIOD_OBJ_CLS_SHUTTER: 
        if (pCmd->dwCmd & 0x00010000) { // prio-mask 0-63
          gsprintf(str, ".%u", pCmd->parmsShutter.dwPrioMask);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        if (pCmd->dwCmd & 0x00020000) { // prio-level 0-5
          gsprintf(str, ".%u", pCmd->parmsShutter.dwPrioLvl);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        if (pCmd->dwCmd & 0x00040000) { // lock 0-1
          gsprintf(str, ".%u", pCmd->parmsShutter.dwLock);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        if (pCmd->dwCmd & 0x00100000) { // delay 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
          gsprintf(str, ".%u", pCmd->parmsShutter.dwDelay);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        if (pCmd->dwCmd & 0x04000000) { // runtime 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
          gsprintf(str, ".%u", pCmd->parmsShutter.dwRun);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        if (pCmd->dwCmd & 0x08000000) { // tiptime 1-65535 1/10th seconds (6535s or 65535 1/10th s)
          gsprintf(str, ".%u", pCmd->parmsShutter.dwTip);
          xstrcatn(pOut, cbOut, str, 0);
          } // if
        break;

//    case CGPIOD_OBJ_CLS_DIMMER:  
//      if (pCmd->dwCmd & 0x00600000) { // level 0-255 for dimmer
//        gsprintf(str, ".%u", pCmd->dwLevel);
//        xstrcatn(pOut, cbOut, str, 0);
//        } // if

//      if (pCmd->dwCmd & 0x01800000) { // runtime 1-160 seconds
//        gsprintf(str, ".%u", pCmd->dwRun);
//        xstrcatn(pOut, cbOut, str, 0);
//        } // if

//      break;

      case CGPIOD_OBJ_CLS_SYSTEM:  
        if (pCmd->dwCmd & 0x00010000) { // loglevel
          gsprintf(str, ".0x%08X", pCmd->parmsSystem.dwParm);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        if (pCmd->dwCmd & 0x00020000) { // emul
          gsprintf(str, ".%u", pCmd->parmsSystem.dwParm);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        if (pCmd->dwCmd & 0x00040000) { // mode
          gsprintf(str, ".%u", pCmd->parmsSystem.dwParm);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        if (pCmd->dwCmd & 0x00080000) { // efmt
          gsprintf(str, ".%u", pCmd->parmsSystem.dwParm);
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        if (pCmd->dwCmd & 0x80000000) { // ack
          gsprintf(str, ".ack");
          xstrcatn(pOut, cbOut, str, 0);
          } // if

        break;

      } // switch

    } while (FALSE);

    return pOut;
    } // _printCmdParams

