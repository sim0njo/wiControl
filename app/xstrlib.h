
//----------------------------------------------------------------------------
// xstrlib.h
//
// Copyright (c) Jo Simons, 2005-2015, All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef __xstrlib_h__
#define __xstrlib_h__

#include "xstdafx.h"

tInt32   xstrfmta(tChar** pp, tInt32 nLen, tChar* pFmt, ...);
tChar*   xstrndup(tCChar* pStr, tUint32 cbStr);
tChar*   xstrdup(tCChar* pStr);
tChar*   xstrlwr(tChar* pStr);
size_t   xstrnlen(tCChar* pStr, size_t dwMax);
tChar*   xstrcpyn(tChar* pDst, size_t cbDst, tCChar* pSrc, tBool* pbError);
tChar*   xstrncpy(tChar* pDst, size_t cbDst, tCChar* pSrc, size_t cbSrc, tBool* pbError);
tChar*   xstrcatn(tChar* pDst, size_t cbDst, tCChar* pSrc, tBool* pbError);
tInt32   xstrnicmp(tCChar* pStr1, tCChar* pStr2, size_t cbLen);
tInt32   xstricmp(tCChar* pStr1, tCChar* pStr2);

tUint32  xstrNextArgVal(tChar** ppStr, tChar cSep1, tChar cSep2, tChar** ppArg, tChar** ppVal);
tUint32  xstrNextArg(tChar** ppStr, tChar cSep1, tChar** ppArg);
tUint32  xstrToUint32(tCChar* pVal);
tUint32  xstrToUint32(tUint32& dwVal, tCChar* pVal);
tUint32  xstrHexToUint8(tUint8* pOut, tChar* pIn);
tChar*   xstrUint8ToHex(tChar* pOut, tUint8* pIn, tUint32 cbIn);

tCChar*  xstrBitMask2Str(tChar* pOut, tUint32 cbOut, tUint32 dwMask);
        
#endif // __xstrlib_h__
