//----------------------------------------------------------------------------
// CLogWriter.cpp : generic logger module
//
// Copyright (c) Jo Simons, 2005-2015, All Rights Reserved.
//
// dwMsg:32 = 0xCCLLssss
//   CC   class 0-15, class 15 = for xxLibs objects, index into m_dwLvl[]
//   LL   level 0-31, matches bit in m_dwLvl mask
//   ssss sequence-nr 0-65535
//----------------------------------------------------------------------------
#include "clogwriter.hpp"
#include "xstrlib.h"
#include <SmingCore/Debug.h>

#define CLOG_CLS_LVL_MASK               0x0F1FFFFF //
#define CLOG_LVL_MASK                   0x001F0000 //


CLogWriter           g_log;


  //----------------------------------------------------------------------------
  // constructor & destructor
  //----------------------------------------------------------------------------
  CLogWriter::CLogWriter() {
    m_dwFormat  = CLOG_FMT_MSGID | CLOG_FMT_CRLF | CLOG_FMT_BINASC;
    memset(m_dwLvl, 0, sizeof(m_dwLvl));
    } // CLogWriter

  CLogWriter::~CLogWriter() { }

  //----------------------------------------------------------------------------
  // get/set output format
  //----------------------------------------------------------------------------
  tUint32 CLogWriter::GetFormat()                  { return m_dwFormat; }
  tUint32 CLogWriter::SetFormat(tUint32 dwFormat)  { return m_dwFormat = dwFormat & CLOG_FMT_MASK; }

  //----------------------------------------------------------------------------
  // get/set class levels
  //----------------------------------------------------------------------------
  tUint32 CLogWriter::GetClsLevels(tUint32 dwCls)  { 
    return (dwCls < CLOG_CLS_MAX) ?  m_dwLvl[dwCls >> 24]           : 0; 
    } // GetClsLevels

  tUint32 CLogWriter::SetClsLevels(tUint32 dwCls, tUint32 dwLvls) {
    return (dwCls < CLOG_CLS_MAX) ? (m_dwLvl[dwCls >> 24] = dwLvls) : 0; 
    } // SetClsLevels

  tUint32 CLogWriter::HasClsLevels()               { 
    for (tUint32 dwCls = CLOG_CLS_0; dwCls < CLOG_CLS_MAX; dwCls += CLOG_CLS_1)
      if (m_dwLvl[dwCls >> 24]) return 1;

    return 0;
    } // HasClsLevels

  //----------------------------------------------------------------------------
  // log textual data
  //----------------------------------------------------------------------------
  void    CLogWriter::LogPrt(tUint32 dwMsg, tCChar* pFmt, ...) {
    tChar   *pOut;
    va_list argList;

    do {
      // exit if level not supported for class
      if ((dwMsg & ~CLOG_CLS_LVL_MASK) || !(m_dwLvl[dwMsg >> 24] & (0x1 << ((dwMsg & CLOG_LVL_MASK) >> 16))))
        break;

      // first compose logline leader
      pOut = _Leader(m_str, dwMsg);
      va_start(argList, pFmt);
      if (gvsnprintf(pOut, CLOG_MAX_BUF - gstrlen(m_str) - 1, pFmt, argList) < 0)
        break;

      va_end(argList);
      _Write(m_str);
      } while (FALSE);

    } // LogPrt

  //----------------------------------------------------------------------------
  // log binary data
  //----------------------------------------------------------------------------
  void    CLogWriter::LogBin(tUint32 dwMsg, tUint32 dwIndent, const void *pBin, tUint32 cbBin, tCChar* pFmt, ...) {
    tUint8  *pByt = (tUint8*) pBin;
    tChar   *pHex, *pAsc, *pOut;
    tUint32 dw;
    va_list argList;

    do {
      // exit if level not supported for class
      if ((dwMsg & ~CLOG_CLS_LVL_MASK) || !(m_dwLvl[dwMsg >> 24] & (0x1 << ((dwMsg & CLOG_LVL_MASK) >> 16))))
        break;

      // first compose logline leader
      pOut = _Leader(m_str, dwMsg);
      va_start(argList, pFmt);
      if (gvsnprintf(pOut, CLOG_MAX_BUF - gstrlen(m_str) - 1, pFmt, argList) < 0)
        break;

      va_end(argList);
      _Write(m_str);

      while (cbBin) {
        memset(pOut, 0x20, 256);
        pHex = &pOut[dwIndent % 64];
        pAsc = pHex + 65;

        for (dw = (32 < cbBin) ? 32 : cbBin; dw; dw--, cbBin--) {
          if (m_dwFormat & CLOG_FMT_BINASC) _ByteToAsc(*pByt, &pAsc);
          _ByteToHex(*pByt++, &pHex);
          } // for

        *pAsc = '\0';
        _Write(m_str);
        } // while
      } while (FALSE);

    } // LogBin

  //----------------------------------------------------------------------------
  // support functions
  //----------------------------------------------------------------------------
  void    CLogWriter::_NibbleToHex(tUint8 byNibble, tChar **ppHex) {
    **ppHex = (byNibble < 0x0A) ? byNibble + '0' : byNibble + '7';
    (*ppHex)++;
    } // _NibbleToHex

  void    CLogWriter:: _ByteToHex(tUint8 byByte, tChar **ppHex) {
    _NibbleToHex(byByte / 16, ppHex);
    _NibbleToHex(byByte % 16, ppHex);
    } // _ByteToHex

  void    CLogWriter:: _ByteToAsc(tUint8 byByte, tChar **ppAsc) {
    **ppAsc = ((byByte < 32) || (byByte > 127)) ? '.' : byByte;
    (*ppAsc)++;
    } // _ByteToAsc

  //----------------------------------------------------------------------------
  // build logline leader: msgid
  //----------------------------------------------------------------------------
  tChar*  CLogWriter::_Leader(tChar *pOut, tUint32 dwMsg) {
    if (m_dwFormat & CLOG_FMT_MSGID) {
      // add msgid
      gsprintf(pOut, "%08X,", dwMsg);
      pOut +=strlen(pOut);
      } // if

    return pOut;
    } // _Leader

  //----------------------------------------------------------------------------
  // write msg to output streams
  //----------------------------------------------------------------------------
  void    CLogWriter::_Write(tChar* pStr) {
    // handle console logging
    if (m_dwFormat & CLOG_FMT_CRLF)
      Debug.println(pStr);
    else
      Debug.print(pStr);
    }; // _Write

