
// ---------------------------------------------------------------------------
// xstrlib.cpp : cross-platform string library
//
// Copyright (c) Jo Simons, 2005-2015, All Rights Reserved.
// ---------------------------------------------------------------------------
//#include <SmingCore/SmingCore.h>
#include "xstrlib.h"
#include "xerror.h"


//----------------------------------------------------------------------------
// convert bitmask into string
//   0x000300F1 -> 0,4-7,16-17
//----------------------------------------------------------------------------
tCChar*  xstrBitMask2Str(tChar* pOut, tUint32 cbOut, tUint32 dwMask) {
  tInt32 n, nRanges = 0, nFrom = -1, nTo = -1;
  tChar  str[32];

  for (*pOut = '\0', n = 0; n < 31; n++) {
    if ((nFrom < 0) && (dwMask & (0x1 << n))) {
      nFrom = nTo = n;
      continue;
      } // if

    if (dwMask & (0x1 << n)) {
      nTo = n;
      continue;
      } 

    if (nFrom >= 0) {
      // append range
      if (nFrom == nTo) 
        gsprintf(str, nRanges ? ",%d" : "%d", nFrom);
      else    
        gsprintf(str, nRanges ? ",%d-%d" : "%d-%d", nFrom, nTo);
          
      xstrcatn(pOut, cbOut, str, 0);
      nFrom = nTo = -1;
      nRanges++;
      } // if
      
    } // for

  return pOut;
  } // xstrBitMask2Str 

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tInt32 xstrfmta(tChar** ppStr, tInt32 nLen, tChar* pFmt, ...)
{
  va_list argList;
	tInt32	nRet = 0;
  tChar   *pStr;

	va_start(argList, pFmt);

	if ((pStr = (tChar*) malloc(nLen)))
    nRet = gvsnprintf(pStr, nLen, pFmt, argList);

	va_end(argList);
	if (ppStr)
    *ppStr = pStr;

  return nRet;
  } // xstrfmta

//----------------------------------------------------------------------------
// duplicate string
//----------------------------------------------------------------------------
tChar* xstrndup(tCChar* pStr, tUint32 cbStr)
{
	tChar	 *pstr = 0;

	if (pStr && cbStr && (pstr = (tChar*) malloc(cbStr + 1))) {
		gmemcpy(pstr, pStr, cbStr);
    *(pstr + cbStr) = '\0';
    } // if

	return pstr;
  } // xstrndup

//----------------------------------------------------------------------------
// duplicate string
//----------------------------------------------------------------------------
tChar* xstrdup(tCChar* pStr)
{
	tChar	 *pstr;
	tInt32 nLen;

	if (pStr == NULL) pStr = "";

	nLen = gstrlen(pStr) + 1;
	if ((pstr = (tChar*) malloc(nLen)))
		gstrcpy(pstr, pStr);

	return pstr;
  } // xstrdup

//----------------------------------------------------------------------------
// convert to lower case
//----------------------------------------------------------------------------
tChar* xstrlwr(tChar* pStr)
{
	tChar *pstr = pStr;
  for (; *pstr; pstr++) *pstr = tolower(*pstr);

  return pStr;
  } // xstrlwr
             
//----------------------------------------------------------------------------
// return size of pStr, limited to dwMax
//----------------------------------------------------------------------------
size_t xstrnlen(tCChar* pStr, size_t dwMax)
{
	return min(gstrlen(pStr), dwMax);
  } // xstrnlen
             
//----------------------------------------------------------------------------
// copy src to dst always zero terminated, returns dst, reports overflow error
//----------------------------------------------------------------------------
tChar* xstrcpyn(tChar* pDst, size_t cbDst, tCChar* pSrc, tUint32* pdwError)
{
  register tChar       *pdst = pDst;
  register const tChar *psrc = (pSrc) ? pSrc : "";
  register size_t      cbdst = cbDst;

  // clear error
  if (pdwError)
    *pdwError = XERROR_SUCCESS;

  // copy string
  for ( ; *psrc && (cbdst > 1); *pdst++ = *psrc++, cbdst--);
  
  // make sure it is terminated
  *pdst = '\0';

  // report error
  if (*psrc && pdwError)
    *pdwError = XERROR_RESOURCES;

  return pDst;  
  } // xstrcpyn

//----------------------------------------------------------------------------
// copy src to dst always zero terminated, returns dst, reports overflow error
//----------------------------------------------------------------------------
tChar* xstrncpy(tChar* pDst, size_t cbDst, tCChar* pSrc, size_t cbSrc, tUint32* pdwError)
{
  register tChar       *pdst = pDst;
  register const tChar *psrc = (pSrc) ? pSrc : "";
  register size_t      cbdst = cbDst;

  // clear error
  if (pdwError)
    *pdwError = XERROR_SUCCESS;

  // copy string
  for ( ; *psrc && cbSrc && (cbdst > 1); *pdst++ = *psrc++, cbdst--, cbSrc--);
  
  // make sure it is terminated
  *pdst = '\0';

  // report error
  if (*psrc && pdwError)
    *pdwError = XERROR_RESOURCES;

  return pDst;  
  } // xstrncpy

//----------------------------------------------------------------------------
// concatenate src to dst, always zero terminate
//----------------------------------------------------------------------------
tChar* xstrcatn(tChar* pDst, size_t cbDst, tCChar* pSrc, tUint32* pdwError)
{
  register tChar       *pdst = pDst;
  register const tChar *psrc = pSrc;
  register size_t      cbdst = cbDst;
  size_t               cb    = gstrlen(pDst);

  // clear error
  if (pdwError)
    *pdwError = XERROR_SUCCESS;

  do {
    // dst already overflowed
    if (cb >= cbdst)
      break;

    // append src, leave room for terminating zero
    for (cbdst -= cb, pdst += cb; *psrc && (cbdst > 1); *pdst++ = *psrc++, cbdst--);
    
    // zero terminate
    *pdst = '\0';
    } while (FALSE);

  // report error
  if (*psrc && pdwError)
    *pdwError = XERROR_RESOURCES;

  return pDst;
  } // xstrcatn

//----------------------------------------------------------------------------
// case insensitive compare strings
//----------------------------------------------------------------------------
tInt32 xstrnicmp(tCChar* pStr1, tCChar* pStr2, size_t cbLen)
{
  register tChar c1 = 0, c2 = 0;
  register tInt32 cblen = cbLen;

  do {
    c1 = tolower((tChar) *pStr1++);
    c2 = tolower((tChar) *pStr2++);
        
    if (c1 == '\0')
      break;

    if ((--cblen) <= 0)
      break;

    } while (c1 == c2);

  return (c1 - c2);
  } // xstrnicmp

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
tInt32 xstricmp(tCChar* pStr1, tCChar* pStr2)
{
  register tChar c1 = 0, c2 = 0;

  do {
    c1 = tolower((tChar) *pStr1++);
    c2 = tolower((tChar) *pStr2++);
        
    if (c1 == '\0')
      break;

    } while (c1 == c2);

  return (c1 - c2);
  } // xstrnicmp

//----------------------------------------------------------------------------
// convert decimal ascii number to tUint32
//----------------------------------------------------------------------------
tUint32 xstrToUint32(tCChar* pVal)
{
  register tUint32 dwRet = 0;
 
  for ( ; gisdigit(*pVal); pVal++) 
    dwRet = (dwRet * 10) + (*pVal - '0');

  return dwRet;
  } // xstrToUint32

tUint32 xstrToUint32(tUint32& dwVal, tCChar* pVal)
{
  for (dwVal = 0; gisdigit(*pVal); pVal++) 
    dwVal = (dwVal * 10) + (*pVal - '0');

  return (*pVal) ? XERROR_INPUT : XERROR_SUCCESS;
  } // xstrToUint32

//----------------------------------------------------------------------------
// destructive parse of string to return next arg/val pair
//   *ppStr = [<arg>[<sep1><val>] *[<sep2><arg>[<sep1><val>]]]
//
// returns 1 if arg/val found, else 0
//----------------------------------------------------------------------------
tUint32 xstrNextArgVal(tChar** ppStr, tChar cSep1, tChar cSep2, tChar** ppArg, tChar** ppVal)
{
  tUint32 dwRet = 0;
  tChar   *pArg = 0, *pVal = 0;

  do {
    // exit if no or no more input
    if (!*ppStr || !*(*ppStr))
      break;

    // skip leading spaces
    while (*(*ppStr) == ' ') (*ppStr)++;

    // isolate <arg>[<sep1><val>]
    for (pArg = *ppStr; *(*ppStr); (*ppStr)++) {
      if (*(*ppStr) == cSep2) {
        *(*ppStr)++ = '\0';
        break;
        } // if
      }  // for

    // split <arg>[<sep1><val]
    for (pVal = pArg; *pVal; pVal++) {
      if (*pVal == cSep1) {
        *pVal++ = '\0';
        break;
        } // if
      } // for

    if (ppArg) *ppArg = pArg;
    if (ppVal) *ppVal = pVal;
    dwRet = 1;
    } while (FALSE);

  return dwRet;
  } // xstrNextArgVal

//----------------------------------------------------------------------------
// destructive parse of query string to return next arg
//   *ppQuery = [<arg> *[<sep1><arg>]]
//
// returns 1 if arg found, else 0
//----------------------------------------------------------------------------
tUint32 xstrNextArg(tChar** ppStr, tChar cSep1, tChar** ppArg)
{
  tUint32 dwRet = 0;
  tChar   *pArg = 0;

  do {
    // exit if no or no more query input
    if (!*ppStr || !*(*ppStr))
      break;

    // skip leading spaces
    while (*(*ppStr) == ' ') (*ppStr)++;

    // isolate <arg>
    for (pArg = *ppStr; *(*ppStr); (*ppStr)++) {
      if (*(*ppStr) == cSep1) {
        *(*ppStr)++ = '\0';
        break;
        } // if
      }  // for

    if (ppArg) *ppArg = pArg;
    dwRet = 1;
    } while (FALSE);

  return dwRet;
  } // xstrNextArg

//----------------------------------------------------------------------------
// convert hex string to tUint8 array  XXXXXX
//----------------------------------------------------------------------------
tUint32 _xstrHexNibble(tChar ch) {
  return ((ch >= '0') && (ch <= '9')) ? ch - '0' :
         ((ch >= 'A') && (ch <= 'Z')) ? ch - '7' :
         ((ch >= 'a') && (ch <= 'z')) ? ch - 'W' : -1;
  } // _xstrHexNibble

tUint32 _xstrHexOctet(tChar** ppIn) {
  return (_xstrHexNibble(*(*ppIn)++) << 4) + _xstrHexNibble(*(*ppIn)++);
  } // _HexOctet2Val

//----------------------------------------------------------------------------
// convert hex string to tUint8 array, return number of bytes in output, 0=error
//----------------------------------------------------------------------------
tUint32 xstrHexToUint8(tUint8* pOut, tChar* pIn) {
  tUint32 cbRet = 0, dwLen, dw;

  do {
    // must be even length
    if ((dwLen = gstrlen(pIn)) % 2) break;

    for (dwLen /= 2; cbRet < dwLen; cbRet++) { 
      if ((dw = _xstrHexOctet(&pIn)) < 256)
        *pOut++ = (tUint8) dw;
      else {
        cbRet = 0;
        break;
        } // else
      } // for

    } while (FALSE);

  return cbRet;
  } // xstrHexToUint8

//----------------------------------------------------------------------------
// convert tUint8 array to hex string
//----------------------------------------------------------------------------
tChar* xstrUint8ToHex(tChar* pOut, tUint8* pIn, tUint32 cbIn) {
  tChar *pstr = pOut;

  for (; cbIn--; pstr += 2)
    gsprintf(pstr, "%02X", *pIn++);

  return pOut;
  } // xstrUint8ToHex

