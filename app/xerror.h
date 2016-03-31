   
//------------------------------------------------------------------------------
// xerror.h : common error codes
//
// Copyright (c) Jo Simons, 2005-2016, All Rights Reserved.
//
// dwError = 0xMMEEEEEE
//   MM = 00 : system
//        01 : SNMP
//------------------------------------------------------------------------------
#ifndef __xerror_h__
#define __xerror_h__

#include "stdafx.h"

tCChar* xError2String(tUint32 dwError);

#define XERROR_MOD_SYS                  0x00000000 //
//#define XERROR_MOD_SYS_MAX                      22 //

#define XERROR_SUCCESS                           0 // success
#define XERROR_INTERNAL                          1 // internal error cannot be resolved by user

#define XERROR_MEMORY                            2 // out of memory or buffer too small
#define XERROR_CREATE                            3 // error creating file or socket
#define XERROR_OPEN                              4 // error opening file or socket
#define XERROR_WRITE                             5 // error writing file or socket
#define XERROR_READ                              6 // error reading file or socket
#define XERROR_SEEK                              7 // error reading file or socket

#define XERROR_STATE                             8 // invalid state
#define XERROR_INPUT                             9 // invalid input
#define XERROR_DATA                             10 // invalid (response) data

#define XERROR_RESOURCES                        11 // no resources to complete operation
#define XERROR_NOT_FOUND                        12 // resource not found
#define XERROR_LICENSE                          13 // resource not licensed
#define XERROR_ACCESS                           14 // resource not accessible
#define XERROR_DISABLED                         15 // resource disabled                 
#define XERROR_BUSY                             16 // resource busy

#define XERROR_PENDING                          17 // request  pending
#define XERROR_TIMEOUT                          18 // request  timed out
#define XERROR_ABORTED                          19 // request  aborted

#define XERROR_NO_DATA                          20 // internal only
#define XERROR_CLOSED                           21 // file, socket or connection closed

// -----------------------------------------------------------------
// SNMP Error Codes
// -----------------------------------------------------------------
#define XERROR_MOD_SNMP                 0x01000000 //
//#define XERROR_MOD_SNMP_MAX                     19 //

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


/*
#define E000_SUCCESS                       0 //
#define E100_TRYING                      100 //
#define E182_QUEUED                      182 //
#define E183_SESSION_PROGRESS            183 //
#define E200_OK                          200 //
#define E400_BAD_REQUEST                 400 //
#define E401_UNAUTHORIZED                401 //
#define E402_PAYMENT_REQUIRED            402 //
#define E403_FORBIDDEN                   403 //
#define E404_NOT_FOUND                   404 //
#define E405_METHOD_NOT_ALLOWED          405 //
#define E406_NOT_ACCEPTABLE              406 //
#define E407_PROXY_AUTH_REQUIRED         407 //
#define E408_REQ_TIMEOUT                 408 //
#define E414_REQ_URI_TOO_LONG            414 //
#define E416_UNSUP_URI_SCHEME            416 //
#define E480_TEMP_UNAVAILABLE            480 //
#define E483_TOO_MANY_HOPS               483 //
#define E484_ADDR_INCOMPLETE             484 //
#define E485_AMBIGUOUS                   485 //
#define E486_BUSY_HERE                   486 //
#define E487_REQ_TERMINATED              487 //
#define E488_NOT_ACCEPTABLE_HERE         488 //
#define E491_REQ_PENDING                 491 //
#define E493_UNDECIPHERABLE              493 //
#define E500_SERVER_INT_ERROR            500 //
#define E501_NOT_IMPLEMENTED             501 //
#define E502_BAD_GATEWAY                 502 //
#define E503_SERVICE_UNAVAILABLE         503 //
#define E504_SERVER_TIMEOUT              504 //
#define E505_VERSION_NOT_SUPPORTED       505 //
#define E513_MSG_TOO_LARGE               513 //
#define E600_BUSY_EVERYWHERE             600 //
#define E603_DECLINE                     603 //
#define E604_DOES_NOT_EXIST              604 //
#define E606_NOT_ACCEPTABLE              606 //
*/

#endif // __xerror_h__

