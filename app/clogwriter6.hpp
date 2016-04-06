//----------------------------------------------------------------------------
// clogwriter6.hpp : generic logger module
//
// Copyright (c) Jo Simons, 2005-2015, All Rights Reserved.
//
// 20150813, v6.0.0.0, separate config for console/file/syslog
//
// dwMsg:32 = 0xCCLLssss
//   CC   class 0-15, class 15 = for xxLibs objects, index into m_dwLvl[]
//   LL   level 0-31, matches bit in m_dwLvl mask
//   ssss sequence-nr 0-65535
//----------------------------------------------------------------------------
#ifndef __clogwriter6_hpp__
#define __clogwriter6_hpp__

#include "xstdafx.h"
#include "xstrlib.h"

// internal defines
#define _CLOG_CLS_MAX                    0x10000000 //
#define _CLOG_CLS_NBR                            16 //
#define _CLOG_LVL_MASK                           31 //
#define _CLOG_CLS_LVL_MASK               0xF0E00000 //
#define _CLOG_MAX_BUF                           512 //

class CLogWriter6 {
 private:
  tUint32            m_dwFormat;                   // format flags
  tUint32            m_dwLvl[_CLOG_CLS_NBR];       //
  tChar              m_str[_CLOG_MAX_BUF];         // for LogPrt method

 public:
  //----------------------------------------------------------------------------
  // constructor & destructor
  //----------------------------------------------------------------------------
  CLogWriter6() {
    m_dwFormat  = CLOG_FMT_TSTAMP | CLOG_FMT_MSGID | CLOG_FMT_CRLF | CLOG_FMT_BINASC;
    memset(m_dwLvl, 0, sizeof(m_dwLvl));
    } // CLogWriter6

  ~CLogWriter6() { }

  //----------------------------------------------------------------------------
  // get/set output format
  //----------------------------------------------------------------------------
  tUint32            GetFormat()                   { return m_dwFormat; };
  tUint32            SetFormat(tUint32 dwFormat)   { return m_dwFormat = dwFormat & CLOG_FMT_MASK; };

  //----------------------------------------------------------------------------
  // get/set class levels
  //----------------------------------------------------------------------------
  tUint32            GetClsLevels(tUint32 dwCls)      { 
    return (dwCls < _CLOG_CLS_MAX) ?  m_dwLvl[dwCls >> 24]           : 0; 
    } // GetClsLevels

  tUint32            SetClsLevels(tUint32 dwCls, tUint32 dwLvls) {
    return (dwCls < _CLOG_CLS_MAX) ? (m_dwLvl[dwCls >> 24] = dwLvls) : 0; 
    } // SetClsLevels

  tUint32            HasClsLevels()                { 
    for (tUint32 dwCls = CLOG_CLS_0; dwCls < _CLOG_CLS_MAX; dwCls += CLOG_CLS_1)
      if (m_dwLvl[dwCls >> 24]) return 1;

    return 0;
    } // HasClsLevels

  //----------------------------------------------------------------------------
  // log textual data
  //----------------------------------------------------------------------------
  void               LogPrt(tUint32 dwMsg, tCChar* pFmt, ...) {
    tChar   *pOut;
    va_list argList;

    // exit if level not supported for class
    if ((dwMsg & _CLOG_CLS_LVL_MASK) || !(m_dwLvl[dwMsg >> 24] & (0x1 << ((dwMsg >> 16) & _CLOG_LVL_MASK))))
      return;

    // first compose logline leader
    pOut = _Leader(m_str, dwMsg);
    va_start(argList, pFmt);
    m_vsnprintf(pOut, _CLOG_MAX_BUF - gstrlen(m_str) - 1, pFmt, argList);
    va_end(argList);
    _Write(m_str);
    } // LogPrt

  //----------------------------------------------------------------------------
  // log binary data
  //----------------------------------------------------------------------------
  void               LogBin(tUint32 dwMsg, tUint32 dwIndent, const void *pBin, tUint32 cbBin, tCChar* pFmt, ...) {
    tUint8  *pByt = (tUint8*) pBin;
    tChar   *pHex, *pAsc, *pOut;
    tUint32 dw;
    va_list argList;

    // exit if level not supported for class
    if ((dwMsg & _CLOG_CLS_LVL_MASK) || !(m_dwLvl[dwMsg >> 24] & (0x1 << ((dwMsg >> 16) & _CLOG_LVL_MASK))))
      return;

    // first compose logline leader
    pOut = _Leader(m_str, dwMsg);
    va_start(argList, pFmt);
    m_vsnprintf(pOut, _CLOG_MAX_BUF - gstrlen(m_str) - 1, pFmt, argList);
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
    }; // LogBin

 private:
  //----------------------------------------------------------------------------
  // support functions
  //----------------------------------------------------------------------------
  void               _NibbleToHex(tUint8 byNibble, tChar **ppHex) {
    **ppHex = (byNibble < 0x0A) ? byNibble + '0' : byNibble + '7';
    (*ppHex)++;
    }; // _NibbleToHex

  void               _ByteToHex(tUint8 byByte, tChar **ppHex) {
    _NibbleToHex(byByte / 16, ppHex);
    _NibbleToHex(byByte % 16, ppHex);
    }; // _ByteToHex

  void               _ByteToAsc(tUint8 byByte, tChar **ppAsc) {
    **ppAsc = ((byByte < 32) || (byByte > 127)) ? '.' : byByte;
    (*ppAsc)++;
    }; // _ByteToAsc

  //----------------------------------------------------------------------------
  // build logline leader <pri> date_time msgid
  //----------------------------------------------------------------------------
  tChar*             _Leader(tChar *pOut, tUint32 dwMsg) {
    if (m_dwFormat & CLOG_FMT_MSGID) {
      // add msgid
      gsprintf(pOut, "%08X,", dwMsg);
      pOut +=strlen(pOut);
      } // if

    return pOut;
    }; // _Leader

  //----------------------------------------------------------------------------
  // write msg to output streams
  //----------------------------------------------------------------------------
  void               _Write(tChar* pStr) {
    // handle console logging
    if (m_dwFormat & CLOG_FMT_CRLF)
      Debug.println(pStr);
    else
      Debug.print(pStr);
    }; // _Write

  }; // CLogWriter6

#endif // __clogwriter6_hpp__
