
//----------------------------------------------------------------------------
// cgpiod_parse.cpp : command parser
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
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
    // parse object or system command
    Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0010, "CGpiod::ParseCmd,obj=%s,cmd=%s,orig=%u,emul=%u",
                 pObj ? pObj : "", pCmd ? pCmd : "", dwOrig, dwEmul);
    memset(pOut, 0, sizeof(tGpiodCmd));
    m_parse.SetReservedIdents(g_gpiodParseObj);
    m_parse.SetString(pObj ? pObj : pCmd);
    if ((m_parse.NextToken(dwOrig, dwEmul) != CPARSE_TYPE_NODE) && (dwErr = XERROR_SYNTAX))
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
        dwErr = XERROR_SYNTAX;
        Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0100, "CGpiod::ParseCmd,err=%u,unknown object", dwErr);
        break;
      } // switch

    } while (FALSE);

  Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x9999, "CGpiod::ParseCmd,err=%u,obj=%08X,cmd=%08X,parms=%08X", 
               dwErr, pOut->dwObj, pOut->dwCmd, pOut->dwParms);
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
    m_parse.NextToken(pOut->dwOrig, xNum2BitMask(pOut->dwObj & CGPIOD_OBJ_NUM_MASK));

    if (m_parse.TType() == CPARSE_TYPE_NONE) {
      // revert to default status command
      pOut->dwCmd = CGPIOD_IN_CMD_STATUS;
      break;
      } // if

    else if (m_parse.TType() != CPARSE_TYPE_LEAF) {
      // syntax error
      dwErr = XERROR_SYNTAX;
      break;
      } // if

    // known command
    pOut->dwCmd = m_parse.TVal();

    // parse mandatory parms
    if (pOut->dwCmd & CGPIOD_CMD_PRM_MANDATORY) {
      } // CGPIOD_CMD_PRM_MANDATORY

    // parse optional parms
    if ((pOut->dwCmd & CGPIOD_CMD_PRM_OPTIONAL) && (m_parse.NextToken(0, 0) == CPARSE_TYPE_PERIOD)) {

      if (pOut->dwCmd & CGPIOD_IN_PRM_DEBOUNCE) { // debounce 0-3600 seconds in ms
        if (dwErr = m_parse.GetNumber(&pOut->parmsInput.dwDebounce, 50, 3600000)) break;
        pOut->dwParms |= CGPIOD_IN_PRM_DEBOUNCE;
        } // if

      } // CGPIOD_CMD_PRM_OPTIONAL

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

    if (m_parse.TType() == CPARSE_TYPE_NONE) {
      // revert to default status command
      pOut->dwCmd = CGPIOD_OUT_CMD_STATUS;
      break;
      } // if

    else if (m_parse.TType() != CPARSE_TYPE_LEAF) {
      // syntax error
      dwErr = XERROR_SYNTAX;
      break;
      } // if

    // known command
    pOut->dwCmd = m_parse.TVal();

    // parse mandatory parms
    if (pOut->dwCmd & CGPIOD_CMD_PRM_MANDATORY) {

      if (pOut->dwCmd & CGPIOD_OUT_PRM_DELAY) { // delay 1-3600 seconds
        if (dwErr = m_parse.GetNumber(&pOut->parmsOutput.dwDelay, 1, 3600)) break;
        pOut->dwParms |= CGPIOD_OUT_PRM_DELAY;
        } // if

      if (pOut->dwCmd & CGPIOD_OUT_PRM_RUN) { // run 1-3600 seconds
        if (dwErr = m_parse.GetNumber(&pOut->parmsOutput.dwRun, 1, 3600)) break;
        pOut->dwParms |= CGPIOD_OUT_PRM_RUN;
        } // if

      } // CGPIOD_CMD_PRM_MANDATORY

    // parse optional parms
    if ((pOut->dwCmd & CGPIOD_CMD_PRM_OPTIONAL) && (m_parse.NextToken(0, 0) == CPARSE_TYPE_PERIOD)) {

      if (pOut->dwCmd & CGPIOD_OUT_PRM_DEFRUN) { // run 0-3600 seconds
        if (dwErr = m_parse.GetNumber(&pOut->parmsOutput.dwRun, 0, 3600)) break;
        pOut->dwParms |= CGPIOD_OUT_PRM_DEFRUN;
        } // if

      } // CGPIOD_CMD_PRM_OPTIONAL

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
    m_parse.NextToken(pOut->dwOrig, xNum2BitMask(pOut->dwObj & CGPIOD_OBJ_NUM_MASK));

    if (m_parse.TType() == CPARSE_TYPE_NONE) {
      // revert to default status command
      pOut->dwCmd = CGPIOD_SHU_CMD_STATUS;
      break;
      } // if

    else if (m_parse.TType() != CPARSE_TYPE_LEAF) {
      // syntax error
      dwErr = XERROR_SYNTAX;
      break;
      } // if

    // known command
    pOut->dwCmd = m_parse.TVal();

    // parse mandatory parms
    if (pOut->dwCmd & CGPIOD_CMD_PRM_MANDATORY) {

      if (pOut->dwCmd & CGPIOD_SHU_PRM_PRIOMASK) { // prio-mask 0-63
        if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwPrioMask, 0, 63)) break;
        pOut->dwParms |= CGPIOD_SHU_PRM_PRIOMASK;
        } // if

      if (pOut->dwCmd & CGPIOD_SHU_PRM_PRIOLEVEL) { // prio-level 0-5
        if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwPrioLvl, 0, 5)) break;
        pOut->dwParms |= CGPIOD_SHU_PRM_PRIOLEVEL;
        } // if

      if (pOut->dwCmd & CGPIOD_SHU_PRM_PRIOLOCK) { // lock 0-1
        if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwLock, 0, 1)) break;
        pOut->dwParms |= CGPIOD_SHU_PRM_PRIOLOCK;
        } // if

      if (pOut->dwCmd & CGPIOD_SHU_PRM_DELAY) { // delay 0-3600 seconds
        if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwDelay, 0, 3600)) break;
        pOut->dwParms |= CGPIOD_SHU_PRM_DELAY;
        } // if

      if (pOut->dwCmd & CGPIOD_SHU_PRM_RUN) { // run 1-3600 seconds
        if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwRun, 1, 3600)) break;
        pOut->dwParms |= CGPIOD_SHU_PRM_RUN;
        } // if

      if (pOut->dwCmd & CGPIOD_SHU_PRM_TIP) { // tip 0-3600 1/10th seconds
        if (dwErr = m_parse.GetNumber(&pOut->parmsShutter.dwTip, 0, 3600)) break;
        pOut->dwParms |= CGPIOD_SHU_PRM_TIP;
        } // if

      } // CGPIOD_CMD_PRM_MANDATORY

    // parse optional parms
    if ((pOut->dwCmd & CGPIOD_CMD_PRM_OPTIONAL) && (m_parse.NextToken(0, 0) == CPARSE_TYPE_PERIOD)) {

      if (pOut->dwCmd & CGPIOD_SHU_PRM_DEFRUN) { // run 1-3600 seconds
        if (dwErr = m_parse.GetNumber(&pOut->parmsOutput.dwRun, 1, 3600)) break;
        pOut->dwParms |= CGPIOD_SHU_PRM_DEFRUN;
        } // if

      } // CGPIOD_CMD_PRM_OPTIONAL

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
    if (m_parse.NextToken(pOut->dwOrig, xNum2BitMask(pOut->dwObj & CGPIOD_OBJ_NUM_MASK)) != CPARSE_TYPE_LEAF) {
      // syntax error
      dwErr = XERROR_SYNTAX;
      break;
      } // if

    // known command
    pOut->dwCmd = m_parse.TVal();

    // parse mandatory parms
    if (pOut->dwCmd & CGPIOD_CMD_PRM_MANDATORY) {

      if (pOut->dwCmd & CGPIOD_TMR_PRM_DELAY) { // delay 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
        if (dwErr = m_parse.GetNumber(&pOut->parmsTimer.dwDelay, 1, 65535)) break;
        pOut->dwParms |= CGPIOD_TMR_PRM_DELAY;
        } // if

      if (pOut->dwCmd & CGPIOD_TMR_PRM_RUN) { // runtime 1-65535 seconds or 1/10th seconds (6535s or 65535 1/10th s)
        if (dwErr = m_parse.GetNumber(&pOut->parmsTimer.dwRun, 1, 65535)) break;
        pOut->dwParms |= CGPIOD_TMR_PRM_RUN;
        } // if

      } // CGPIOD_CMD_PRM_MANDATORY

    // parse optional parms
    if ((pOut->dwCmd & CGPIOD_CMD_PRM_OPTIONAL) && (m_parse.NextToken(0, 0) == CPARSE_TYPE_PERIOD)) {
      } // CGPIOD_CMD_PRM_OPTIONAL

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
    // command already parsed, copy it
    pOut->dwCmd = pOut->dwObj;

    // parse mandatory parms
    if (pOut->dwCmd & CGPIOD_CMD_PRM_MANDATORY) {
      
      } // CGPIOD_CMD_PRM_MANDATORY

    // parse optional parms
    if (pOut->dwCmd & CGPIOD_CMD_PRM_OPTIONAL) {

      if (pOut->dwCmd & CGPIOD_SYS_PRM_LOGLEVEL) { // .<loglevel>
        if (dwErr = m_parse.GetNumber(&pOut->parmsSystem.dwParm, 0, 0xFFFFFFFF)) break;
        pOut->dwParms |= CGPIOD_SYS_PRM_LOGLEVEL;
        } // if

      if (pOut->dwCmd & CGPIOD_SYS_PRM_EMUL) { // .<emul>
        if (dwErr = m_parse.GetNumber(&pOut->parmsSystem.dwParm, CGPIOD_EMUL_OUTPUT, CGPIOD_EMUL_SHUTTER)) break;
        pOut->dwParms |= CGPIOD_SYS_PRM_EMUL;
        } // if

      if (pOut->dwCmd & CGPIOD_SYS_PRM_MODE) { // .<mode>
        if (dwErr = m_parse.GetNumber(&pOut->parmsSystem.dwParm, CGPIOD_MODE_LOCAL, CGPIOD_MODE_BOTH)) break;
        pOut->dwParms |= CGPIOD_SYS_PRM_MODE;
        } // if

      if (pOut->dwCmd & CGPIOD_SYS_PRM_OFFON) { // .<lock> | .<disable>
        if (dwErr = m_parse.GetNumber(&pOut->parmsSystem.dwParm, 0, 1)) break;
        pOut->dwParms |= CGPIOD_SYS_PRM_OFFON;
        } // if

      if (pOut->dwCmd & CGPIOD_SYS_PRM_ACK) { // .ack
        m_parse.SkipSeparator(CPARSE_TYPE_PERIOD, FALSE);
        m_parse.SetReservedIdents(g_gpiodParseObjParm);

        if (m_parse.NextToken(pOut->dwOrig, pOut->dwObj & CGPIOD_OBJ_CLS_MASK) != CPARSE_TYPE_PARM) {
          dwErr = XERROR_SYNTAX;
          break;
          } // if

        pOut->parmsSystem.dwAck = m_parse.TVal();
        pOut->dwParms |= CGPIOD_SYS_PRM_ACK;
        } // if

      } // CGPIOD_CMD_PRM_OPTIONAL

    } while (FALSE);

  // correct error for absent optional parameter
  if (dwErr == XERROR_NO_DATA)
    dwErr = XERROR_SUCCESS;

  Debug.logTxt(CLSLVL_GPIOD_PARSE | 0x0000, "CGpiod::_parseCmdSystem,err=%u,obj=%08X,cmd=%08X,parms=%08X", 
               dwErr, pOut->dwObj, pOut->dwCmd, pOut->dwParms);
  return dwErr; 
  } // _parseCmdSystem

