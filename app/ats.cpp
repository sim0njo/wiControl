
//----------------------------------------------------------------------------
// ats.cpp : automatic test system interface
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <HTTP.h>
#include <gpiod.h>
#include <ats.h>

//----------------------------------------------------------------------------
// HTTP config callback
//----------------------------------------------------------------------------
void                 atsOnHttpQuery(HttpRequest &request, HttpResponse &response)
{
  tUint32 dw, dwCnt;
  CAtsReq req;

  if (!g_http.isHttpClientAllowed(request, response))
    return;

  String strCcmd = (request.getRequestMethod() == RequestMethod::POST) ? request.getPostParameter("ccmd") :
                                                                         request.getQueryParameter("ccmd");
  req.DoCcmd(strCcmd);

  // send response
  response.sendString(req.m_strTerse.c_str());
  } // atsOnHttpQuery

//----------------------------------------------------------------------------
// ccmd=<obj> *[.<parm>] *[;<obj> *[.<parm>]]
//----------------------------------------------------------------------------
void CAtsReq::DoCcmd(String strCcmd)
{ 
  Vector<String> cmdToken;
  tInt32         n, numToken = splitString(strCcmd, ';' , cmdToken);

  tUint32   cbCcmd = strCcmd.length();
  tUint32   msNow = millis(), dwErr = XERROR_SUCCESS;
  tGpiodCmd cmd = { 0 };

  do {
    // handle each command
    for (n = 0; n < numToken; n++) {
      tChar sz[cbCcmd + 11];
      gstrcpy(sz, cmdToken[n].c_str());

      // parse object, cmd and optional parms
      if (g_gpiod.ParseCmd(&cmd, 0, sz, CGPIOD_ORIG_HTTP, g_appCfg.gpiodEmul)) {
        Debug.logTxt(CLSLVL_ATS | 0x0300, "CAtsReq::DoCmd,ParseCmd() failed,dropping");
        dwErr = XERROR_SYNTAX;
        } // if
      else {
        cmd.msNow = msNow;
        dwErr = g_gpiod.DoCmd(&cmd);
        } // else

      if (dwErr) {
        gsprintf(sz, "-%u", dwErr);
        _AppendTerse(sz);
        } //
      else {
        gsprintf(sz, "%u", cmd.dwRsp);
        _AppendTerse(sz);
        } // else

      } // for

    } while (0);

  } // CAtsReq::DoCmd

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CAtsReq::_AppendTerse(tChar* pStr)
{
  if (m_strTerse.length())
    m_strTerse += ";";

  m_strTerse += pStr;
  } // CAtsReq::_AppendTerse

