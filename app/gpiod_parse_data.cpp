
//----------------------------------------------------------------------------
// cgpiod_parse_data.cpp : command parser data
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

tParseRsvd g_gpiodParseObj[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     CGPIOD_EMUL_%                        0x0000CCcc                                   
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000100, "in0",      },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000101, "in1",      },
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_NODE, 0x00000200, "out0",     },
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_NODE, 0x00000201, "out1",     },
  { 0x00000018      , 0x00000002       , CPARSE_TYPE_NODE, 0x00000400, "out0",     },
  { 0x10000000      , 0x00000000       , CPARSE_TYPE_NODE, 0x00000800, "hbeat0",   }, // only for print
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_NODE, 0x00001000, "system",   },
                                                                         
  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",         },
  };

tParseRsvd g_gpiodParseObjEvt[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_CLS_%                                           0x0000EEEE  params                
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000002, "ingt0",    },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000003, "outlt1",   },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000004, "ingt1",    },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000005, "outgt1",   },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000006, "ingt2",    },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000007, "out",      },

  { 0x80000200      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000002, "on",       },
  { 0x80000200      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000003, "off",      },
  { 0x80000200      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000004, "timexp",   },

  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "stop",     },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000002, "upon",     },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000003, "downon",   },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000004, "upoff",    },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000005, "downoff",  },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000006, "timexp",   },

  { 0x00000800      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "tickEven", },
  { 0x00000800      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "tickOdd",  },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",         },
  };

tParseRsvd g_gpiodParseCmdInput[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     Num2Mask(cc)                         0x0000EEEE  params                
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000002, "ingt0",    },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000003, "outlt1",   },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000004, "ingt1",    },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000005, "outgt1",   },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000006, "ingt2",    },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000007, "out",      },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",         },
  };

tParseRsvd g_gpiodParseCmdOutput[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     Num2Mask(cc)                         0xPPPPMMMM  params                
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000001, "status",         },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000002, "on",             },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000003, "off",            },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000004, "onlocked",       },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000005, "offlocked",      },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000006, "toggle",         },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000007, "unlock",         },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00080008, "ondelayed",      }, // 1-65535 s
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00080009, "offdelayed",     }, // 1-65535 s
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0200000A, "ontimed",        }, // 1-65535 s
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0200000B, "offtimed",       }, // 1-65535 s
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0008000C, "toggledelayed",  }, // 1-65535 s
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0200000D, "toggletimed",    }, // 1-65535 s
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0000000E, "lock",           },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0000000F, "locktimed",      },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x02000010, "timeset",        }, // 1-65535 s
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x02000011, "timeadd",        }, // 1-65535 s
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000012, "timeabort",      },
  { 0x00000018      , 0x00000000       , CPARSE_TYPE_LEAF, 0x00000013, "timerondelayed", },
  { 0x00000018      , 0x00000000       , CPARSE_TYPE_LEAF, 0x00000014, "timeroffdelayed",},
  { 0x00000018      , 0x00000000       , CPARSE_TYPE_LEAF, 0x00000015, "timerontimed",   },
  { 0x00000018      , 0x00000000       , CPARSE_TYPE_LEAF, 0x00000016, "timerabort",     },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000017, "blink",          },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x02000018, "blinktimed",     }, // 1-65535 s

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",         },
  };

tParseRsvd g_gpiodParseCmdShutter[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     Num2Mask(cc)                         0xPPPPMMMM  params                
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "status",          },
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00020002, "stop",            }, // 0-5
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x04060003, "toggleUp",        }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x04060004, "toggleDown",      }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x04060005, "up",              }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x04060006, "down",            }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x04060007, "tipUp",           }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x04060008, "tipDown",         }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00010009, "priolock",        }, // 0-63
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x0001000A, "priounlock",      }, // 0-63
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x0000000B, "learnon",         },
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x0000000C, "learnoff",        },
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x0001000D, "prioset",         }, // 0-63
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x0001000E, "prioreset",       }, // 0-63
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x0416000F, "delayedup",       }, // 0-5/0-1/D1-65535/R1-65535
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x0C160010, "tipdelayedup",    }, // 0-5/0-1/D1-65535/R1-65535/T1-65535
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x04160011, "delayeddown",     }, // 0-5/0-1/D1-65535/R1-65535
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x0C160012, "tipdelayeddown",  }, // 0-5/0-1/D1-65535/R1-65535/T1-65535
                                                                                             //         1/10s    1/10s    1/10s
  { 0x00000018      , 0x00000000       , CPARSE_TYPE_LEAF, 0x04000013, "timerondelayed",  }, // R1-65535 1/10s
  { 0x00000018      , 0x00000000       , CPARSE_TYPE_LEAF, 0x04000014, "timeroffdelayed", }, // R1-65535 1/10s
  { 0x00000018      , 0x00000000       , CPARSE_TYPE_LEAF, 0x04000015, "timerontimed",    }, // R1-65535 1/10s
  { 0x00000018      , 0x00000000       , CPARSE_TYPE_LEAF, 0x00000016, "timerAbort",      }, //

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",         },
  };

tParseRsvd g_gpiodParseCmdSystem[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%                                          0xPPPPMMMM                                   
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "ping",      },
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000002, "version",   },
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000003, "memory",    },
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000004, "uptime",    },
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x80010005, "loglevel",  }, // <loglevel>.ack
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x80020006, "emul",      }, // <emul>.ack
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x80040007, "mode",      }, // <mode>.ack
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x80080008, "efmt",      }, // <efmt>.ack
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x80100009, "lock",      }, // .ack
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x8010000A, "disable",   }, // .ack
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_LEAF, 0x8000000B, "restart",   }, // .ack

  { 0x00000018      , 0x00001000       , CPARSE_TYPE_PARM, 0x00000001, "ack",       },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "", },
  };

