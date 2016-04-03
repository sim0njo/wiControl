
//--------------------------------------------------------------------
// xstdafx.h : standard include file
//
// Copyright (c) Jo Simons, 2005-2016, All Rights Reserved.
//
// Version numbering: 4.x.y.z -> beta  version, 0=official version
//                    | | +----> minor version
//                    | +------> major version
//                    +--------> indicates new xDvpt framework used
//
//------------------------------------------------------------------------------
#ifndef __xstdafx_h__
#define __xstdafx_h__

#ifdef LINUX
  #include  <sys/types.h>
  #include  <sys/timeb.h>
  #include  <sys/time.h>
  #include  <sys/stat.h>
  #include  <sys/param.h>
  #include  <sys/signal.h>
//#include  <signal.h>
//#include  <sys/wait.h>
  #include  <limits.h>
  #include  <stdio.h>
  #include  <stdlib.h>
  #include  <semaphore.h>
  #include  <unistd.h>
  #include  <sys/socket.h>
  #include  <sys/select.h>
  #include  <netinet/in.h>
  #include  <arpa/inet.h>
  #include  <netdb.h>
  #include  <pthread.h>
  #include  <time.h>
  #include  <fcntl.h>
  #include  <errno.h>
  #include  <dirent.h>
  #include  <sys/ioctl.h>
#endif /* LINUX */

/********************************** Includes **********************************/

#include  <ctype.h>
#include  <stdarg.h>
#include  <string.h>

//--------------------------------------------------------------------
// global defines
//--------------------------------------------------------------------
#define szCRLF                        "\r\n" //
#define szEMPTY                           "" //
#define NELEMENTS(a) (sizeof(a) / sizeof((a)[0]))

typedef const char                         tCChar; //
typedef char                                tChar; //
typedef signed char                         tInt8; //
typedef unsigned char                      tUint8; //
typedef short                              tInt16; //
typedef unsigned short                    tUint16; //
typedef int                                tInt32; //
typedef long                                tLong; //
typedef unsigned long                      tUlong; //
typedef unsigned int                      tUint32; //
typedef unsigned int                        tBool; //
typedef unsigned int                      tIpAddr; //
typedef double                            tDouble; //
typedef float                              tFloat; //
typedef long long                          tInt64; //
typedef unsigned long long                tUint64; //

#ifndef xmax
#define xmax(a,b)           (((a) > (b)) ? (a) : (b))
#endif

#ifndef xmin
#define xmin(a,b)           (((a) < (b)) ? (a) : (b))
#endif

#define xNBR_2_BITPOS(dwNbr)           ((dwNbr < 32) ? (0x01  << dwNbr) : 0)

#define MAKE_tUint32(a, b, c, d) \
  ((tUint32) ( ((tUint8)a) | (((tUint32)(tUint8)b) << 8) | (((tUint32)(tUint8)c) << 16) | (((tUint32)(tUint8)d) << 24) ))
#define MAKE_tUint16(a, b) \
  ((tUint16) ( ((tUint8)a) | (((tUint16)(tUint8)b) << 8) )

#ifdef LINUX

#define MAKEDWORD(a, b)     ((tUint32)(((tUint16)(a)) | ((tUint32)((tUint16)(b))) << 16))
#define LOWORD(l)           ((tUint16)(l))
#define HIWORD(l)           ((tUint16)(((tUint32)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((tUint8)(w))
#define HIBYTE(w)           ((tUint8)(((tUint16)(w) >> 8) & 0xFF))
#endif

/*
 *  "Boolean" constants
 */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
 *  The following include has to be after the unicode defines.  By putting it
 *  here, many modules in various parts of the tree are cleaner.
 */
//#if (defined (LITTLEFOOT) && defined (INMEM))
//  #include  "lf/inmem.h"
//#endif /* LITTLEFOOT && INMEM */

#define gmemcmp     memcmp
#define gmemcpy     memcpy
#define gmemset     memset

#define gasctime    asctime
#define gsprintf    sprintf
#define gprintf     printf
#define gfprintf    fprintf
#define gsscanf     sscanf
#define gvsprintf   vsprintf

#define gstrcpy     strcpy
#define gstrncpy    strncpy
#define gstrncat    strncat
#define gstrlen     strlen
#define gstrcat     strcat
#define gstrcmp     strcmp
#define gstrncmp    strncmp

#define gstrchr     strchr
#define gstrrchr    strrchr
#define gstrtok     strtok
#define gstrnset    strnset
#define gstrrchr    strrchr
#define gstrspn     strspn
#define gstrcspn    strcspn
#define gstrstr     strstr
#define gstrtol     strtol

#define gstrlwr     xstrlwr
#define gstrnlen    xstrnlen
#define gstrcatn    xstrcatn
#define gstrcpyn    xstrcpyn
#define gstricmp    xstricmp
#define gstrnicmp   xstrnicmp

#define gtolower    tolower
#define gtoupper    toupper
#define gisspace    isspace
#define gisdigit    isdigit
#define gisxdigit   isxdigit
#define gisalnum    isalnum
#define gisalpha    isalpha
#define gisupper    isupper
#define gislower    islower
#define gatoi       atoi

#endif // __xstdafx_h__ 

/******************************************************************************/

