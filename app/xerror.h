   
//------------------------------------------------------------------------------
// xerror.h : common error codes
//
// Copyright (c) Jo Simons, 2005-2016, All Rights Reserved.
//
// dwError = 0xMMEEEEEE
//   MM = 00 : generic
//        01 : SNMP
//        02 : HTTP
//------------------------------------------------------------------------------
#ifndef __xerror_h__
#define __xerror_h__

#include "xstdafx.h"

tCChar* xError2String(tUint32 dwError);


#define XERROR_                         0x00000000 //
#define XERROR_SUCCESS                           0 // success
#define XERROR_INTERNAL                          1 // internal error cannot be resolved by user
#define XERROR_MEMORY                            2 // out of memory or buffer too small
#define XERROR_CREATE                            3 // error creating file or socket
#define XERROR_OPEN                              4 // error opening file or socket
#define XERROR_WRITE                             5 // error writing file or socket
#define XERROR_READ                              6 // error reading file or socket
#define XERROR_SEEK                              7 // error reading file or socket
#define XERROR_STATE                             8 // invalid state
#define XERROR_SYNTAX                            9 // invalid syntax
#define XERROR_SEMANTIC                         10 // invalid semantic
#define XERROR_RESPONSE                         11 // invalid response
#define XERROR_RESOURCES                        12 // no resources to complete operation
#define XERROR_NOT_FOUND                        13 // resource not found
#define XERROR_LICENSE                          14 // resource not licensed
#define XERROR_ACCESS                           15 // resource not accessible
#define XERROR_DISABLED                         16 // resource disabled                 
#define XERROR_BUSY                             17 // resource busy
#define XERROR_PENDING                          18 // request  pending
#define XERROR_TIMEOUT                          19 // request  timed out
#define XERROR_ABORTED                          20 // request  aborted
#define XERROR_NO_DATA                          21 // internal only
#define XERROR_CLOSED                           22 // file, socket or connection closed
//#define XERROR_MOD_SYS_MAX                      23 //

// -----------------------------------------------------------------
// SNMP Error Codes
// -----------------------------------------------------------------
#define XERROR_SNMP_                    0x01000000 //
#define XERROR_SNMP_SUCCESS             0x00000000 //
#define XERROR_SNMP_TOO_BIG             0x01000001 //
#define XERROR_SNMP_NO_SUCH_NAME        0x01000002 //
#define XERROR_SNMP_BAD_VALUE           0x01000003 //
#define XERROR_SNMP_READ_ONLY           0x01000004 //
#define XERROR_SNMP_GEN_ERR             0x01000005 //
#define XERROR_SNMP_NO_ACCESS           0x01000006 //
#define XERROR_SNMP_WRONG_TYPE          0x01000007 //
#define XERROR_SNMP_WRONG_LENGTH        0x01000008 //
#define XERROR_SNMP_WRONG_ENCODING      0x01000009 //
#define XERROR_SNMP_WRONG_VALUE         0x0100000A //
#define XERROR_SNMP_NO_CREATION         0x0100000B //
#define XERROR_SNMP_INCONSIST_VALUE     0x0100000C //
#define XERROR_SNMP_RESOURCE_UNAVAIL    0x0100000D //
#define XERROR_SNMP_COMMIT_FAILED       0x0100000E //
#define XERROR_SNMP_UNDO_FAILED         0x0100000F //
#define XERROR_SNMP_AUTHORIZATION       0x01000010 //
#define XERROR_SNMP_NOT_WRITABLE        0x01000011 //
#define XERROR_SNMP_INCONSIST_NAME      0x01000012 //
//#define XERROR_MOD_SNMP_MAX                     19 //


// -----------------------------------------------------------------
// HTTP Error Codes
// -----------------------------------------------------------------
#define XERROR_HTTP_                    0x02000000 //
#define XERROR_HTTP_SUCCESS             0x02000000 // 0
#define XERROR_HTTP_TRYING              0x02000064 // 100 
#define XERROR_HTTP_QUEUED              0x020000B6 // 182 
#define XERROR_HTTP_SESSION_PROGRESS    0x020000B7 // 183 
#define XERROR_HTTP_OK                  0x020000C8 // 200 
#define XERROR_HTTP_BAD_REQUEST         0x02000190 // 400 
#define XERROR_HTTP_UNAUTHORIZED        0x02000191 // 401 
#define XERROR_HTTP_PAYMENT_REQUIRED    0x02000192 // 402 
#define XERROR_HTTP_FORBIDDEN           0x02000193 // 403 
#define XERROR_HTTP_NOT_FOUND           0x02000194 // 404 
#define XERROR_HTTP_METHOD_NOT_ALLOWED  0x02000195 // 405 
#define XERROR_HTTP_NOT_ACCEPTABLE      0x02000196 // 406 
#define XERROR_HTTP_PROXY_AUTH_REQUIRED 0x02000197 // 407 
#define XERROR_HTTP_REQ_TIMEOUT         0x02000198 // 408 
#define XERROR_HTTP_REQ_URI_TOO_LONG    0x0200019E // 414 
#define XERROR_HTTP_UNSUP_URI_SCHEME    0x020001A0 // 416 
#define XERROR_HTTP_TEMP_UNAVAILABLE    0x020001E0 // 480 
#define XERROR_HTTP_TOO_MANY_HOPS       0x020001E3 // 483 
#define XERROR_HTTP_ADDR_INCOMPLETE     0x020001E4 // 484 
#define XERROR_HTTP_AMBIGUOUS           0x020001E5 // 485 
#define XERROR_HTTP_BUSY_HERE           0x020001E6 // 486 
#define XERROR_HTTP_REQ_TERMINATED      0x020001E7 // 487 
#define XERROR_HTTP_NOT_ACCEPTABLE_HERE 0x020001E8 // 488 
#define XERROR_HTTP_REQ_PENDING         0x020001EB // 491 
#define XERROR_HTTP_UNDECIPHERABLE      0x020001ED // 493 
#define XERROR_HTTP_SERVER_INT_ERROR    0x020001F4 // 500 
#define XERROR_HTTP_NOT_IMPLEMENTED     0x020001F5 // 501 
#define XERROR_HTTP_BAD_GATEWAY         0x020001F6 // 502 
#define XERROR_HTTP_SERVICE_UNAVAILABLE 0x020001F7 // 503 
#define XERROR_HTTP_SERVER_TIMEOUT      0x020001F8 // 504 
#define XERROR_HTTP_VERSION_NOT_SUPP    0x020001F9 // 505 
#define XERROR_HTTP_MSG_TOO_LARGE       0x02000201 // 513 
//#define XERROR_HTTP_BUSY_EVERYWHERE     0x02000258 // 600 
//#define XERROR_HTTP_DECLINE             0x0200025B // 603 
//#define XERROR_HTTP_DOES_NOT_EXIST      0x0200025C // 604 
//#define XERROR_HTTP_NOT_ACCEPTABLE      0x0200025E // 606 

#endif // __xerror_h__

