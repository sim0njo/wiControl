
//----------------------------------------------------------------------------
// clogwriter.hpp : generic logger module
//
// Copyright (c) Jo Simons, 2005-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef __clogwriter_hpp__
#define __clogwriter_hpp__

#include "xstdafx.h"

// internal defines
#define CLogLvl2Mask(dwLvl)          ((tUint32)0x01 << (((tUint32)dwLvl >> 16) &  31))

#define CLOG_CLSLVL_NONE                0xFFFF0000 // 0

#define CLOG_CLS_0                      0x00000000 // 0
#define CLOG_CLS_1                      0x01000000 // 1
#define CLOG_CLS_2                      0x02000000 // 2
#define CLOG_CLS_3                      0x03000000 // 3
#define CLOG_CLS_4                      0x04000000 // 4
#define CLOG_CLS_5                      0x05000000 // 5
#define CLOG_CLS_6                      0x06000000 // 6
#define CLOG_CLS_7                      0x07000000 // 7
#define CLOG_CLS_8                      0x08000000 // 8
#define CLOG_CLS_9                      0x09000000 // 9
#define CLOG_CLS_A                      0x0A000000 // 10
#define CLOG_CLS_B                      0x0B000000 // 11
#define CLOG_CLS_C                      0x0C000000 // 12
#define CLOG_CLS_D                      0x0D000000 // 13
#define CLOG_CLS_E                      0x0E000000 // 14
#define CLOG_CLS_F                      0x0F000000 // 15 used for xxlibs components
#define CLOG_CLS_MAX                    0x10000000 //
#define CLOG_CLS_NBR                            16 //

#define CLOG_LVL_00                     0x00000000 // 0
#define CLOG_LVL_01                     0x00010000 // 1
#define CLOG_LVL_02                     0x00020000 // 2
#define CLOG_LVL_03                     0x00030000 // 3
#define CLOG_LVL_04                     0x00040000 // 4
#define CLOG_LVL_05                     0x00050000 // 5
#define CLOG_LVL_06                     0x00060000 // 6
#define CLOG_LVL_07                     0x00070000 // 7
#define CLOG_LVL_08                     0x00080000 // 8
#define CLOG_LVL_09                     0x00090000 // 9
#define CLOG_LVL_0A                     0x000A0000 // 10
#define CLOG_LVL_0B                     0x000B0000 // 11
#define CLOG_LVL_0C                     0x000C0000 // 12
#define CLOG_LVL_0D                     0x000D0000 // 13
#define CLOG_LVL_0E                     0x000E0000 // 14
#define CLOG_LVL_0F                     0x000F0000 // 15
#define CLOG_LVL_10                     0x00100000 // 16
#define CLOG_LVL_11                     0x00110000 // 17
#define CLOG_LVL_12                     0x00120000 // 18
#define CLOG_LVL_13                     0x00130000 // 19
#define CLOG_LVL_14                     0x00140000 // 20
#define CLOG_LVL_15                     0x00150000 // 21
#define CLOG_LVL_16                     0x00160000 // 22
#define CLOG_LVL_17                     0x00170000 // 23
#define CLOG_LVL_18                     0x00180000 // 24
#define CLOG_LVL_19                     0x00190000 // 25
#define CLOG_LVL_1A                     0x001A0000 // 26
#define CLOG_LVL_1B                     0x001B0000 // 27
#define CLOG_LVL_INFO                   0x001C0000 // 28 1C
#define CLOG_LVL_WARN                   0x001D0000 // 29 1D
#define CLOG_LVL_ERROR                  0x001E0000 // 30 1E
#define CLOG_LVL_CRIT                   0x001F0000 // 31 1F

#define CLOG_FMT_MSGID                  0x00000001 //
#define CLOG_FMT_CRLF                   0x00000002 //
#define CLOG_FMT_BINASC                 0x00000004 //
#define CLOG_FMT_MASK                   0x00000007 //

#define CLOG_MAX_BUF                           512 //


class CLogWriter {
 private:
  tUint32            m_dwFormat;                   // format flags
  tUint32            m_dwLvl[CLOG_CLS_NBR];        //
  tChar              m_str[CLOG_MAX_BUF];          // for LogPrt method

 public:
  //----------------------------------------------------------------------------
  // constructor & destructor
  //----------------------------------------------------------------------------
  CLogWriter();
  ~CLogWriter();

  //----------------------------------------------------------------------------
  // get/set output format
  //----------------------------------------------------------------------------
  tUint32            GetFormat();
  tUint32            SetFormat(tUint32 dwFormat);

  //----------------------------------------------------------------------------
  // get/set class levels
  //----------------------------------------------------------------------------
  tUint32            GetClsLevels(tUint32 dwCls);
  tUint32            SetClsLevels(tUint32 dwCls, tUint32 dwLvls);
  tUint32            HasClsLevels();

  //----------------------------------------------------------------------------
  // log textual data
  //----------------------------------------------------------------------------
  void               LogPrt(tUint32 dwMsg, tCChar* pFmt, ...);

  //----------------------------------------------------------------------------
  // log binary data
  //----------------------------------------------------------------------------
  void               LogBin(tUint32 dwMsg, tUint32 dwIndent, const void *pBin, tUint32 cbBin, tCChar* pFmt, ...);

 private:
  //----------------------------------------------------------------------------
  // support functions
  //----------------------------------------------------------------------------
  void               _NibbleToHex(tUint8 byNibble, tChar **ppHex);
  void               _ByteToHex(tUint8 byByte, tChar **ppHex);
  void               _ByteToAsc(tUint8 byByte, tChar **ppAsc);

  //----------------------------------------------------------------------------
  // build logline leader <pri> date_time msgid
  //----------------------------------------------------------------------------
  tChar*             _Leader(tChar *pOut, tUint32 dwMsg);

  //----------------------------------------------------------------------------
  // write msg to output streams
  //----------------------------------------------------------------------------
  void               _Write(tChar* pStr);

  }; // CLogWriter

extern CLogWriter    g_log;

#endif // __clogwriter_hpp__
