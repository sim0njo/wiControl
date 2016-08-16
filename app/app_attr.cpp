
//----------------------------------------------------------------------------
// app_attr.cpp : application attributes
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include "app.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tCChar* CApplication::GetStrAttr(tCChar* szAttr) 
{
//if      (!strcmp(szAttr, "nodeId"))
//  return m_nodeId.c_str();

//else if (!strcmp(szAttr, "cpuBoost"))
//  return m_strUser.c_str();

  return "";
  } //

tUint32 CApplication::GetNumAttr(tCChar* szAttr) 
{
  if      (!strcmp(szAttr, "cpuBoost"))
    return m_cpuBoost;

//else if (!strcmp(szAttr, "mqttPktRx"))
//  return m_dwPktRx;

  return 0;
  } //

