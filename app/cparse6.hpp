
//------------------------------------------------------------------------------
// cparse6.hpp : generic parser object
//
// 20130122, v6.0.0.0, added SkipSeparator, GetNumber and DoOperNum
//
// Copyright (c) Jo Simons, 2005-2016, All Rights Reserved.
//------------------------------------------------------------------------------
#ifndef __cparse6_hpp__
#define __cparse6_hpp__

#include "xstdafx.h"
#include "xerror.h"
#include "xstrlib.h"

#define CPARSE_CLSLVL_DBG CLOG_CLS_F | CLOG_LVL_02 //

// alpha-numerical selector flags
#define CPARSE_ALPHANUM_NONE            0x00000000 //
#define CPARSE_ALPHANUM_USCORE          0x00000001 // '_'
#define CPARSE_ALPHANUM_DOLLAR          0x00000002 // '$'
#define CPARSE_ALPHANUM_PERIOD          0x00000004 // '.'
#define CPARSE_ALPHANUM_DASH            0x00000008 // '-'
#define CPARSE_ALPHANUM_COLON           0x00000010 // ':'
#define CPARSE_ALPHANUM_SEMICOLON       0x00000020 // ';'
#define CPARSE_ALPHANUM_DQUOTE          0x00000040 // '"'
#define CPARSE_ALPHANUM_SPACE           0x00000080 // ' '
#define CPARSE_ALPHANUM_ASTERISK        0x00000100 // '*'
#define CPARSE_ALPHANUM_QUESTION        0x00000200 // '?'
#define CPARSE_ALPHANUM_DIGIT           0x00000400 // '0'..'9'
#define CPARSE_ALPHANUM_ALL             0x000000FF //

// token types
#define CPARSE_TYPE_MASK                0x000F0000 //

#define CPARSE_TYPE_NONE                0x00000000 // no more tokens
#define CPARSE_TYPE_NUMBER              0x00000001 // 123
#define CPARSE_TYPE_STRING              0x00000002 // "text"
#define CPARSE_TYPE_IDENTIFIER          0x00000003 // identifier
#define CPARSE_TYPE_NODE                0x00000004 //
#define CPARSE_TYPE_LEAF                0x00000005 //
#define CPARSE_TYPE_PARM                0x00000006 //
//#define CPARSE_TYPE_RESERVED            0x00000007 // reserved strings

#define CPARSE_TYPE_OPER_MASK           0x0000FF00 //
#define CPARSE_TYPE_OPER                0x00030000 //  

#define CPARSE_TYPE_OPER_UNI            0x00010000 // unary operator

#define CPARSE_TYPE_NOTOPER             0x00000100 // ! ~
#define CPARSE_TYPE_OPER_LOGNOT         0x00010101 // ! logical not
#define CPARSE_TYPE_OPER_BITNOT         0x00010102 // ~ bitwise not

#define CPARSE_TYPE_OPER_BI             0x00020000 // binary operator

#define CPARSE_TYPE_MULOPER             0x00020200 // * / %
#define CPARSE_TYPE_OPER_MUL            0x00020201 // *
#define CPARSE_TYPE_OPER_DIV            0x00020202 // /
#define CPARSE_TYPE_OPER_MOD            0x00020203 // %
                        
#define CPARSE_TYPE_ADDOPER             0x00020400 // + -
#define CPARSE_TYPE_OPER_ADD            0x00020401 // + add or unary plus
#define CPARSE_TYPE_OPER_SUB            0x00020402 // - subtract or unary minus

#define CPARSE_TYPE_SHOPER              0x00020800 // << >>
#define CPARSE_TYPE_OPER_SHL            0x00020801 // <<
#define CPARSE_TYPE_OPER_SHR            0x00020802 // >>

#define CPARSE_TYPE_CMPOPER             0x00021000 // < <= > >= == <>
#define CPARSE_TYPE_OPER_LT             0x00021001 // <
#define CPARSE_TYPE_OPER_LE             0x00021002 // <=
#define CPARSE_TYPE_OPER_GT             0x00021003 // >
#define CPARSE_TYPE_OPER_GE             0x00021004 // >=
#define CPARSE_TYPE_OPER_EQ             0x00021005 // ==
#define CPARSE_TYPE_OPER_NE             0x00021006 // != <> ><

#define CPARSE_TYPE_ANDOPER             0x00022000 // & ^ | && ||
#define CPARSE_TYPE_OPER_BITAND         0x00022001 // &
#define CPARSE_TYPE_OPER_XOR            0x00022002 // ^
#define CPARSE_TYPE_OPER_BITOR          0x00022003 // |
#define CPARSE_TYPE_OPER_LOGAND         0x00022004 // &&
#define CPARSE_TYPE_OPER_LOGOR          0x00022005 // ||

#define CPARSE_TYPE_DELIMITER           0x00040000 // delimiter
#define CPARSE_TYPE_LQUOTE              0x00040000 // `
#define CPARSE_TYPE_RQUOTE              0x00040001 // '
#define CPARSE_TYPE_AT                  0x00040002 // @
#define CPARSE_TYPE_HASH                0x00040003 // #
#define CPARSE_TYPE_DOLLAR              0x00040004 // $
#define CPARSE_TYPE_LBRACKET            0x00040005 // (
#define CPARSE_TYPE_RBRACKET            0x00040006 // )
#define CPARSE_TYPE_USCORE              0x00040007 // _
#define CPARSE_TYPE_ASSIGN              0x00040008 // =
#define CPARSE_TYPE_LBRACE              0x00040009 // {
#define CPARSE_TYPE_RBRACE              0x0004000A // }
#define CPARSE_TYPE_LSQUARE             0x0004000B // [
#define CPARSE_TYPE_RSQUARE             0x0004000C // ]
#define CPARSE_TYPE_BSLASH              0x0004000D // backslash
#define CPARSE_TYPE_COLON               0x0004000E // :
#define CPARSE_TYPE_SEMICOLON           0x0004000F // ;
#define CPARSE_TYPE_COMMA               0x00040010 // ,
#define CPARSE_TYPE_PERIOD              0x00040011 // .
#define CPARSE_TYPE_QUESTION            0x00040012 // ?

//#define CPARSE_FLAG_TOKEN               0x00000001 // token available
//#define CPARSE_FLAG_MODE1               0x00000002 // mode1 operation selection
#define CPARSE_MASK_ALL                 0xFFFFFFFF //
#define CPARSE_MASK_NONE                0x00000000 //
//efine CPARSE_MASK_ROOT                0x00000001 //
#define CPARSE_MASK_NODE                0x00000001 //
#define CPARSE_MASK_LEAF                0x00000002 //
#define CPARSE_MASK_PARM                0x00000004 //

typedef struct {
  tUint32            dwMask0;                      // I
  tUint32            dwMask1;                      // I
  tUint32            dwTType;                      // O
  tUint32            dwTVal;                       // O
  tCChar*            szTVal;                       // I
  } tParse6Rsvd;


class CParse6 {
 private:
  tUint32            m_dwAlphaNumMask;                // 
  tParse6Rsvd*       m_pRsvd;                      // list of reserved words
  tChar*             m_pStr;                       // string to parse
  tChar              m_cSepa;                      // saved separator character
  tUint32            m_cbSepa;                     // 
  tUint32            m_dwTType;                    // token type
  tUint32            m_dwTVal;                     // token value
  tChar*             m_pTStr;                      // token string

 public:
  //----------------------------------------------------------------------------
  // constructor & destructor
  //----------------------------------------------------------------------------
  CParse6() {
    m_pRsvd    = 0;
    m_pStr     = 0;
    m_cSepa    = 0;
    m_cbSepa   = 0;
    SetAlphaNumMask(CPARSE_ALPHANUM_USCORE);
    }; // CParse6

         ~CParse6()                   { }

  tUint32 TType()                     { return m_dwTType;             }
  tUint32 TVal()                      { return m_dwTVal;              }
  tChar*  TStr()                      { return m_pTStr;               }

  /*------------------------------------------------------------------*/
  /* set list of reserved words                                       */
  /*------------------------------------------------------------------*/
  void SetAlphaNumMask(tUint32 dwAlNumMask) { m_dwAlphaNumMask = dwAlNumMask; }

  /*------------------------------------------------------------------*/
  /* set list of reserved words                                       */
  /*------------------------------------------------------------------*/
  void SetReservedIdents(tParse6Rsvd* pRsvd) { m_pRsvd  = pRsvd;  }
   
  /*------------------------------------------------------------------*/
  /* set string to parse                                              */
  /*------------------------------------------------------------------*/
  tCChar* GetString()            { return m_pStr; }
  void    SetString(tChar* pStr) {
    m_pStr   = pStr;
    m_cSepa  = 0;
    m_pTStr  = 0;
    m_cbSepa = 0;
    }; // SetString

  /*------------------------------------------------------------------*/
  /* restore previous token                                           */
  /*------------------------------------------------------------------*/
  void RestoreToken() {
    // restore separator if needed
    if (m_cSepa) {
      *m_pStr  = m_cSepa;
      m_cSepa  = 0;
      m_pStr  += m_cbSepa;
      m_cbSepa = 0;
    } // if

    if (m_pTStr) m_pStr = m_pTStr;
    }; // RestoreToken

  /*------------------------------------------------------------------*/
  /* return next token                                                */
  /*------------------------------------------------------------------*/
  tUint32 NextToken(tUint32 dwMask0, tUint32 dwMask1) {
    tCChar *pFunc = "CParse5::NextToken";

    // restore separator if needed
    if (m_cSepa) {
      *m_pStr  = m_cSepa;
      m_cSepa  = 0;
      m_pStr  += m_cbSepa;
      m_cbSepa = 0;
      } // if

    // skip leading spaces, CR, LF and comments
    while (*m_pStr && _IsSkipChar(&m_pStr));

    // initialise token
    m_dwTType = CPARSE_TYPE_NONE;
    m_dwTVal  = 0;
    m_pTStr   = m_pStr;

    // handle mode0
    switch (*m_pStr) {
      case '\0': break;

      case '!':
        m_dwTType = CPARSE_TYPE_OPER_LOGNOT;
        if (*(++m_pStr) != '=') break;

        m_dwTType = CPARSE_TYPE_OPER_NE;
        m_pStr++;
        break;

      case '~':
        m_dwTType = CPARSE_TYPE_OPER_BITNOT;
        m_pStr++;
        break;

      case '*': case '/': case '%':
        m_dwTType = (*m_pStr == '*') ? CPARSE_TYPE_OPER_MUL :
                    (*m_pStr == '%') ? CPARSE_TYPE_OPER_MOD : CPARSE_TYPE_OPER_DIV;
        m_pStr++;
        break;

      case '-': case '+':
        m_dwTType = (*m_pStr == '-') ? CPARSE_TYPE_OPER_SUB : CPARSE_TYPE_OPER_ADD;
        m_pStr++;
        break;

      case '<':
        m_dwTType = CPARSE_TYPE_OPER_LT;
        m_pStr++;

        if (*m_pStr == '<') {
          m_dwTType = CPARSE_TYPE_OPER_SHL;
          m_pStr++;
          break;
          } // if

        if (*m_pStr == '=') {
          m_dwTType = CPARSE_TYPE_OPER_LE;
          m_pStr++;
          break;
          } // if

        break;

      case '>':
        m_dwTType = CPARSE_TYPE_OPER_GT;
        m_pStr++;

        if (*m_pStr == '>') {
          m_dwTType = CPARSE_TYPE_OPER_SHR;
          m_pStr++;
          break;
          } // if

        if (*m_pStr == '=') {
          m_dwTType = CPARSE_TYPE_OPER_GE;
          m_pStr++;
          break;
          } // if

        break;

      case '=':
        m_dwTType = CPARSE_TYPE_ASSIGN;
        if (*(++m_pStr) != '=') break;
       
        m_dwTType = CPARSE_TYPE_OPER_EQ;
        m_pStr++;
        break;

      case '&':
        m_dwTType = CPARSE_TYPE_OPER_BITAND;
        if (*(++m_pStr) != '&') break;

        m_dwTType = CPARSE_TYPE_OPER_LOGAND;
        m_pStr++;
        break;

      case '|':
        m_dwTType = CPARSE_TYPE_OPER_BITOR;
        if (*(++m_pStr) != '|') break;

        m_dwTType = CPARSE_TYPE_OPER_LOGOR;
        m_pStr++;
        break;

      case '^':
        m_dwTType = CPARSE_TYPE_OPER_XOR;
        m_pStr++;
        break;

      case '`':
        m_dwTType = CPARSE_TYPE_LQUOTE;
        m_pStr++;
        break;

      case '@':
        m_dwTType = CPARSE_TYPE_AT;
        m_pStr++;
        break;

      case '#':
        m_dwTType = CPARSE_TYPE_HASH;
        m_pStr++;
        break;

      case '$':
        m_dwTType = CPARSE_TYPE_DOLLAR;
        m_pStr++;
        break;

      case '(':
        m_dwTType = CPARSE_TYPE_LBRACKET;
        m_pStr++;
        break;

      case ')':
        m_dwTType = CPARSE_TYPE_RBRACKET;
        m_pStr++;
        break;

      case '_':
        m_dwTType = CPARSE_TYPE_USCORE;
        m_pStr++;
        break;

      case '{':
        m_dwTType = CPARSE_TYPE_LBRACE;
        m_pStr++;
        break;

      case '}':
        m_dwTType = CPARSE_TYPE_RBRACE;
        m_pStr++;
        break;

      case '[':
        m_dwTType = CPARSE_TYPE_LSQUARE;
        m_pStr++;
        break;

      case ']':
        m_dwTType = CPARSE_TYPE_RSQUARE;
        m_pStr++;
        break;

      case '\\':
        m_dwTType = CPARSE_TYPE_BSLASH;
        m_pStr++;
        break;

      case ':':
        m_dwTType = CPARSE_TYPE_COLON;
        m_pStr++;
        break;

      case ';':
        m_dwTType = CPARSE_TYPE_SEMICOLON;
        m_pStr++;
        break;

      case '\'':
        m_dwTType = CPARSE_TYPE_RQUOTE;
        m_pStr++;
        break;

      case ',':
        m_dwTType = CPARSE_TYPE_COMMA;
        m_pStr++;
        break;

      case '.':
        m_dwTType = CPARSE_TYPE_PERIOD;
        m_pStr++;
        break;

      case '?':
        m_dwTType = CPARSE_TYPE_QUESTION;
        m_pStr++;
        break;

      case '\"':
        // collect quoted string
        m_dwTType = CPARSE_TYPE_STRING;
        m_pTStr++;

        while (*(++m_pStr)) { // "abc"
//        if ((*m_pStr == '\"') && ( (*(++m_pStr) == 0) || (*m_pStr != '\"') ) ) break;
          if (*m_pStr == '\"') {
            if (*(m_pStr + 1) != '\"') break;
            m_pStr++;
          } // if
        } // while
        
        if (*m_pStr) {
          m_cSepa  = *m_pStr;
          *m_pStr  = 0;
          m_cbSepa = 1;
        } // if

        break;

      default:
/*
        if (parse.GetNumber() == CPARSE_TYPE_NUMBER) {
          m_cSepa = *m_pStr;
          *m_pStr = 0;
          break;
          } // if
*/
        if (gisdigit(*m_pStr)) {
          // collect number
          m_dwTType = CPARSE_TYPE_NUMBER;
          
          // handle hexadecimal format 0x.....
          if ((*m_pStr == '0') && ((*(m_pStr + 1) == 'x') || (*(m_pStr + 1) == 'X'))) {
            for (m_pStr += 2; isxdigit(*m_pStr); ) {
              m_dwTVal = (m_dwTVal * 16) + _Hex2Val(*m_pStr++);
              } // for

            m_cSepa = *m_pStr;
            *m_pStr = 0;
            break;
            } // if

          // handle decimal format
          while (*m_pStr && gisdigit(*m_pStr)) {
            m_dwTVal = (m_dwTVal * 10) + *m_pStr - '0';
            m_pStr++;
            } // while

          m_cSepa = *m_pStr;
          *m_pStr = 0;
          break;
          } // if isdigit

        if (!_IsAlphaNum(*m_pStr)) break;

        // collect identifier
        m_dwTType = CPARSE_TYPE_IDENTIFIER;
        while (_IsAlphaNum(*m_pStr)) { *m_pStr++; }
        m_cSepa = *m_pStr;
        *m_pStr = 0;
                
        // handle reserved word checking
        _Translate(dwMask0, dwMask1);
        break;
      } // switch

    if (m_dwTType & (CPARSE_TYPE_OPER | CPARSE_TYPE_DELIMITER)) {
      m_cSepa  = *m_pStr;
      *m_pStr  = 0;
      } // if

//  g_log.LogPrt(CPARSE_CLSLVL_DBG | 0x9999, "%s,ttype=0x%08X,tval=%u/%s", pFunc, m_dwTType, m_dwTVal, m_pTStr);
    return m_dwTType;
    } ; // NextToken

//--------------------------------------------------------------------
// collect decimal/hexadecimal number
//--------------------------------------------------------------------
tUint32 GetNumber() 
{
  if (gisdigit(*m_pStr)) {
    // collect number
    m_dwTType = CPARSE_TYPE_NUMBER;
          
    // handle hexadecimal format 0x.....
    if ((*m_pStr == '0') && ((*(m_pStr + 1) == 'x') || (*(m_pStr + 1) == 'X'))) {
      for (m_pStr += 2; gisxdigit(*m_pStr); ) {
        m_dwTVal = (m_dwTVal * 16) + _Hex2Val(*m_pStr++);
        } // for

//    m_cSepa = *m_pStr;
//    *m_pStr = 0;
      } // if

    else {
      // handle decimal format
      while (*m_pStr && gisdigit(*m_pStr)) {
        m_dwTVal = (m_dwTVal * 10) + *m_pStr - '0';
        m_pStr++;
        } // while

//    m_cSepa = *m_pStr;
//    *m_pStr = 0;
      } // else

    } // if isdigit
  else
    m_dwTType = CPARSE_TYPE_NONE;
            
  return m_dwTType;
  } // GetNumber

//--------------------------------------------------------------------
// read optional/mandatory separator character '.'                      
//--------------------------------------------------------------------
tUint32 SkipSeparator(tUint32 dwTType, tBool bRequired) 
{
  // skip optional period separator
  if (NextToken(0, 0) == dwTType)
    return XERROR_SUCCESS;

  if (bRequired)
    return XERROR_INPUT;

  RestoreToken();
  return XERROR_SUCCESS; 
  } // SkipSeparator

//--------------------------------------------------------------------
// read [<separator>] <number> within range(dwLo, dwHi)                             
//--------------------------------------------------------------------
tUint32 GetNumber(tUint32* pdwNum, tUint32 dwLo, tUint32 dwHi)
{
  // skip optional separator
  SkipSeparator(CPARSE_TYPE_PERIOD, FALSE);

  if (NextToken(0, 0) != CPARSE_TYPE_NUMBER)
    return XERROR_INPUT;

  if ((m_dwTVal < dwLo) || (m_dwTVal > dwHi)) 
    return XERROR_INPUT;

  if (pdwNum)
    *pdwNum = m_dwTVal;

  return XERROR_SUCCESS; 
  } // GetNumber

//--------------------------------------------------------------------
// read number within mask                                          
//--------------------------------------------------------------------
tUint32 GetNumberMask(tUint32* pdwNum, tUint32 dwMask) 
{
  // skip optional separator
  SkipSeparator(CPARSE_TYPE_PERIOD, FALSE);

  if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) 
    return XERROR_INPUT;

  if ((m_dwTVal < 0) || (m_dwTVal > 31)) 
    return XERROR_INPUT;

  if (!(dwMask & (0x1 << m_dwTVal))) 
    return XERROR_INPUT;

  if (pdwNum)
    *pdwNum = m_dwTVal;

  return XERROR_SUCCESS; 
  } // GetNumberMask

//--------------------------------------------------------------------
// handle arithmetic: dwIO = dwIO *{<oper> <num>}                                          
//--------------------------------------------------------------------
tUint32 DoOperNum(tUint32* pdwNum)
{
  tCChar  *pFunc = "CParse5::DoOperNum";
  tUint32 dwOper, dwErr = XERROR_SUCCESS;

  // clear result
//g_log.LogPrt(CPARSE_CLSLVL_DBG | 0x0000, "%s,dwNum=%u", pFunc, *pdwNum);
  
  do {
    // exit if no binary operator
    if (!((dwOper = NextToken(0, 0)) & CPARSE_TYPE_OPER_BI))
      break;

    // exit with error if no parameter for operator
    if ((NextToken(0, 0) != CPARSE_TYPE_NUMBER) && (dwErr = XERROR_INPUT))
      break;

    DoOper(pdwNum, dwOper, m_dwTVal);
    } while (TRUE);

  RestoreToken();
//g_log.LogPrt(CPARSE_CLSLVL_DBG | 0x9999, "%s,dwNum=%u,err=%u", pFunc, *pdwNum, dwErr);
  return dwErr;
  } // DoOperNum

//----------------------------------------------------------------------------
// *pdwVal1 = *pdwVal1 <dwOper> dwVal2
//----------------------------------------------------------------------------
tUint32 DoOper(tUint32* pdwVal1, tUint32 dwOper, tUint32 dwVal2)
{
  switch (dwOper) {
    case CPARSE_TYPE_OPER_MUL:    *pdwVal1  *= dwVal2;
      break;
            
    case CPARSE_TYPE_OPER_DIV: // ok
      if (dwVal2)
        *pdwVal1 /= dwVal2;
//    else
//      divide by zero !!!
      break;
            
    case CPARSE_TYPE_OPER_MOD: // use %%   %25
      if (dwVal2)
        *pdwVal1 %= dwVal2;
//    else
//      modulus by zero !!!
      break;
            
    case CPARSE_TYPE_OPER_ADD:    *pdwVal1  += dwVal2; // use %2b
      break;
            
    case CPARSE_TYPE_OPER_SUB:    *pdwVal1  -= dwVal2;
      break;
           
    case CPARSE_TYPE_OPER_SHL:    *pdwVal1 <<= dwVal2;
      break;
           
    case CPARSE_TYPE_OPER_SHR:    *pdwVal1 >>= dwVal2;
      break;

    case CPARSE_TYPE_OPER_LT:     *pdwVal1   = (*pdwVal1 <  dwVal2) ? 1 : 0;
      break;

    case CPARSE_TYPE_OPER_LE:     *pdwVal1   = (*pdwVal1 <= dwVal2) ? 1 : 0;
      break;

    case CPARSE_TYPE_OPER_GT:     *pdwVal1   = (*pdwVal1 >  dwVal2) ? 1 : 0;
      break;

    case CPARSE_TYPE_OPER_GE:     *pdwVal1   = (*pdwVal1 >= dwVal2) ? 1 : 0;
      break;

    case CPARSE_TYPE_OPER_EQ:     *pdwVal1   = (*pdwVal1 == dwVal2) ? 1 : 0;
      break;

    case CPARSE_TYPE_OPER_NE:     *pdwVal1   = (*pdwVal1 != dwVal2) ? 1 : 0;
      break;

    case CPARSE_TYPE_OPER_BITAND: *pdwVal1  &= dwVal2; // use %26
      break;
            
    case CPARSE_TYPE_OPER_XOR:    *pdwVal1  ^= dwVal2;
      break;
        
    case CPARSE_TYPE_OPER_BITOR:  *pdwVal1  |= dwVal2;
      break;
           
    case CPARSE_TYPE_OPER_LOGAND: *pdwVal1   = (*pdwVal1 && dwVal2) ? 1 : 0;
      break;
            
    case CPARSE_TYPE_OPER_LOGOR:  *pdwVal1   = (*pdwVal1 || dwVal2) ? 1 : 0;
      break;
           
    } // switch

  return XERROR_SUCCESS;
  } // DoOper

/*------------------------------------------------------------------*/
/* handle arithmetic: dwIO = dwIO *{<oper> <num>}                   */
/*------------------------------------------------------------------*/
tUint32 DoOperNum2(tChar* pOut, tUint32 cbOut, tUint32 dwNum)
{
  tCChar  *pFunc = "DoOperNum";
  tUint32 bCont = 1;

//g_log.LogPrt(CPARSE_CLSLVL_DBG | 0x0000, "%s,dwNum=%u", pFunc, dwNum);
  do {
    // clear result
    if (pOut)
      *pOut = '\0';
  
    switch (NextToken(0, 0)) {
      case CPARSE_TYPE_OPER_ADD: // ok
        // collect value and perform operator
        if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) break;
        dwNum += m_dwTVal;
        break;
            
      case CPARSE_TYPE_OPER_SUB: // ok
        // collect value and perform operator
        if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) break;
        dwNum -= m_dwTVal;
        break;
           
      case CPARSE_TYPE_OPER_MUL: // ok
        // collect value and perform operator         
        if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) break;
        dwNum *= m_dwTVal;
        break;
            
      case CPARSE_TYPE_OPER_DIV: // ok
        // collect value and perform operator
        if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) break;
        if (m_dwTVal)
          dwNum /= m_dwTVal;
//      else
//        g_log.LogPrt(CPARSE_CLSLVL_DBG | 0x9999, "%s,divide by zero", pFunc);
        break;
            
      case CPARSE_TYPE_OPER_MOD: // use %%   %25
        // collect value and perform operator
        if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) break;
        if (m_dwTVal)
          dwNum %= m_dwTVal;
//      else
//        g_log.LogPrt(CPARSE_CLSLVL_DBG | 0x9999, "%s,modulus by zero", pFunc);
        break;
            
      case CPARSE_TYPE_OPER_BITAND: // use %26
        // collect value and perform operator
        if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) break;
        dwNum &= m_dwTVal;
        break;
            
      case CPARSE_TYPE_OPER_XOR: // ok
        // collect value and perform operator
        if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) break;
        dwNum ^= m_dwTVal;
        break;
        
      case CPARSE_TYPE_OPER_BITOR: // ok
        // collect value and perform operator
        if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) break;
        dwNum |= m_dwTVal;
        break;
           
      case CPARSE_TYPE_OPER_SHL: // ok
        // collect value and perform operator
        if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) break;
        if (m_dwTVal)
          dwNum <<= m_dwTVal;
        break;
           
      case CPARSE_TYPE_OPER_SHR: // ok
        // collect value and perform operator
        if (NextToken(0, 0) != CPARSE_TYPE_NUMBER) break;
        if (m_dwTVal)
          dwNum >>= m_dwTVal;
        break;

      default: // done
        RestoreToken();
        bCont = 0;
        break;
      } // switch
    } while (bCont);

  if (pOut) {
    if (cbOut > sizeof("1234567890"))
      gsprintf(pOut, "%u", dwNum);
//  else
//    g_log.LogPrt(CPARSE_CLSLVL_DBG | 0x9990, "%s,not enough memory to store results", pFunc);
  } // if

//g_log.LogPrt(CPARSE_CLSLVL_DBG | 0x9999, "%s,dwNum=%u", pFunc, dwNum);
  return dwNum;
  } // DoOperNum

 private:
  /*------------------------------------------------------------------*/
  /* support functions                                                */
  /*------------------------------------------------------------------*/
  tUint32 _Hex2Val(tChar ch) {
    return ((ch >= '0') && (ch <= '9')) ? ch - '0' :
           ((ch >= 'A') && (ch <= 'Z')) ? ch - '7' :
           ((ch >= 'a') && (ch <= 'z')) ? ch - 'W' : 0;
    } // _Hex2Val

  //--------------------------------------------------------------------------
  tUint32 _IsSkipChar(tChar** ppStr) {
    // skip leading spaces, CR and LF
    if ((*(*ppStr) == ' ') || (*(*ppStr) == '\r') || (*(*ppStr) == '\n')) {
      (*ppStr)++;
      return 1;
      } // if

    if (!strncmp(*ppStr, "//", 2) || (*(*ppStr) == '#')) {
      // skip comment till end of line
      for ((*ppStr) += 2; *(*ppStr) && (*(*ppStr) != '\r') && (*(*ppStr) != '\n'); (*ppStr)++);
      return 1;
      } // if

    if (!strncmp(*ppStr, "/*", 2)) {
      // skip till "*/"
      for ((*ppStr) += 2; *(*ppStr) && strncmp(*ppStr, "*/", 2); (*ppStr)++);
      if (*(*ppStr)) (*ppStr) += 2;
      return 1;
      } // if

    return 0;
    } // _IsSkipChar

  //--------------------------------------------------------------------------
  // is character alphanumeric ?
  //--------------------------------------------------------------------------
  tBool    _IsAlphaNum(tChar ch) {
    // adjusted for Sming
//  if (gisalnum(ch))                                                   return TRUE;
//    if (isAlphaNumeric(ch))                                             return TRUE;
    if (isAlpha(ch))                                                    return TRUE;
    if (isDigit(ch))                                                    return TRUE;
    
    if ((m_dwAlphaNumMask & CPARSE_ALPHANUM_USCORE)    && (ch == '_'))  return TRUE;

    if ((m_dwAlphaNumMask & CPARSE_ALPHANUM_DOLLAR)    && (ch == '$'))  return TRUE;

    if ((m_dwAlphaNumMask & CPARSE_ALPHANUM_PERIOD)    && (ch == '.'))  return TRUE;
                                               
    if ((m_dwAlphaNumMask & CPARSE_ALPHANUM_DASH)      && (ch == '-'))  return TRUE;

    if ((m_dwAlphaNumMask & CPARSE_ALPHANUM_COLON)     && (ch == ':'))  return TRUE;

    if ((m_dwAlphaNumMask & CPARSE_ALPHANUM_SEMICOLON) && (ch == ';'))  return TRUE;

    if ((m_dwAlphaNumMask & CPARSE_ALPHANUM_DQUOTE)    && (ch == '\"')) return TRUE;

    if ((m_dwAlphaNumMask & CPARSE_ALPHANUM_SPACE)     && (ch == ' '))  return TRUE;

    if ((m_dwAlphaNumMask & CPARSE_ALPHANUM_ASTERISK)  && (ch == '*'))  return TRUE;

    if ((m_dwAlphaNumMask & CPARSE_ALPHANUM_QUESTION)  && (ch == '?'))  return TRUE;

    return FALSE; 
    } // _IsAlphaNum
                                   
  /*------------------------------------------------------------------*/
  /* translate token string into reserved word                        */
  /*------------------------------------------------------------------*/
  tBool    _Translate(tUint32 dwMask0, tUint32 dwMask1) {
    tParse6Rsvd *pRsvd;
    tUint32     dwTVal = 0, dwTType = 0;

    if (!dwMask0 || !dwMask1) return FALSE;

    for (pRsvd = m_pRsvd; pRsvd && pRsvd->dwTType; pRsvd++) {
      if ((pRsvd->dwMask0 & dwMask0) &&
          (pRsvd->dwMask1 & dwMask1) &&
          !gstricmp(pRsvd->szTVal, m_pTStr)) {

        m_dwTType = pRsvd->dwTType;
        m_dwTVal = pRsvd->dwTVal;
        return TRUE;
        } // if
      } // for

    return FALSE;
    } // _Translate

  }; // CParse6

#endif // __cparse6_hpp__
