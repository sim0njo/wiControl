
//----------------------------------------------------------------------------
// network_attr.cpp : network configuration
//
// Copyright (c) Jo Simons, 2015-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include "network.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tCChar* CNetwork::GetStrAttr(tCChar* szAttr) 
{
  if      (!strcmp(szAttr, "staSSID"))
    return m_staSSID.c_str();

  else if (!strcmp(szAttr, "staPswd"))
    return m_staPswd.c_str();

  else if (!strcmp(szAttr, "staAddr"))
    return WifiStation.getIP().toString().c_str();

  else if (!strcmp(szAttr, "staMask"))
    return WifiStation.getNetworkMask().toString().c_str();

  else if (!strcmp(szAttr, "staGtwy"))
    return WifiStation.getNetworkGateway().toString().c_str();

  return "";
  } //

tUint32 CNetwork::GetNumAttr(tCChar* szAttr) 
{
  if      (!strcmp(szAttr, "apMode"))
    return m_apMode;

  else if (!strcmp(szAttr, "staDHCP"))
    return m_staDHCP;

  return 0;
  } //

