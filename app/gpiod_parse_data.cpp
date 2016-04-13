
//----------------------------------------------------------------------------
// cgpiod_parse_data.cpp : command parser data
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

tParseRsvd g_gpiodParseObj[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//                    CGPIOD_OBJ_CLS                       0xPPPPMMMM                                   
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_NODE, 0x00000100, "input0",   },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_NODE, 0x00000101, "input1",   },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_NODE, 0x00000200, "output0",  },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_NODE, 0x00000201, "output1",  },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_NODE, 0x00000400, "shutter0", },
  { 0x00000001      , 0x00000800       , CPARSE_TYPE_NODE, 0x00000800, "hbeat0",   },
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_NODE, 0x00001000, "system",   },
                                                                         
  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",         },
  };

tParseRsvd g_gpiodParseObjEvt[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//in% wbs/wbr                                                PPPP      params                
//                          CC                                   EEEE  cmd/evt         
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000002, "ingt0",    },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000003, "outlt1",   },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000004, "ingt1",    },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000005, "outgt1",   },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000006, "ingt2",    },
  { 0x00000001      , 0x00000100       , CPARSE_TYPE_LEAF, 0x00000007, "out",      },

//fb% wbs                                                    PPPP      params                
//                                                               EEEE  cmd/evt         
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000002, "on",       },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000003, "off",      },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000004, "timexp",   },

//fb% wbr                                                    PPPP      params                
//                                                               EEEE  cmd/evt         
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000001, "stop",     },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000002, "upon",     },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000003, "downon",   },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000004, "upoff",    },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000005, "downoff",  },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000006, "timexp",   },

  { 0x00000001      , 0x00000800       , CPARSE_TYPE_LEAF, 0x00000000, "tickEven", },
  { 0x00000001      , 0x00000800       , CPARSE_TYPE_LEAF, 0x00000001, "tickOdd",  },

  { CPARSE_MASK_NONE, CPARSE_MASK_NONE , CPARSE_TYPE_NONE, 0x00000000, "",         },
  };

tParseRsvd g_gpiodParseCmdOutput[] = {
//  ModMask           ChnMask            CPARSE_TYPE_LEAF    PPPPMMMM
//output%: wbs             
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000001, "status",        },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000002, "on",            },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000003, "off",           },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000004, "onlocked",      },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000005, "offlocked",     },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000006, "toggle",        },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000007, "unlock",        },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00080008, "ondelayed",     }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00080009, "offdelayed",    }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0200000A, "ontimed",       }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0200000B, "offtimed",      }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0008000C, "toggledelayed", }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0200000D, "toggletimed",   }, // 1-65535 s
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0000000E, "lock",          },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x0000000F, "locktimed",     },
  { 0x00000001      , 0x00000800       , CPARSE_TYPE_LEAF, 0x02000010, "timeset",       }, // 1-65535 s
  { 0x00000001      , 0x00000800       , CPARSE_TYPE_LEAF, 0x02000011, "timeadd",       }, // 1-65535 s
  { 0x00000001      , 0x00000800       , CPARSE_TYPE_LEAF, 0x00000012, "timeabort",     },
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000013, "blink",         },

  { CPARSE_MASK_NONE, CPARSE_MASK_NONE , CPARSE_TYPE_NONE, 0x00000000, "",              },
  };

tParseRsvd g_gpiodParseCmdShutter[] = {
//  ModMask           ChnMask            CPARSE_TYPE_LEAF    PPPPMMMM
//shutter%: wbr
  { 0x00000001      , 0x00000200       , CPARSE_TYPE_LEAF, 0x00000001, "status",          },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00020002, "stop",            }, // 0-5
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060003, "toggleUp",        }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060004, "toggleDown",      }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060005, "up",              }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060006, "down",            }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060007, "tipUp",           }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04060008, "tipDown",         }, // 0-5/0-1/R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00010009, "priolock",        }, // 0-63
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0001000A, "priounlock",      }, // 0-63
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0000000B, "learnon",         },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0000000C, "learnoff",        },
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0001000D, "prioset",         }, // 0-63
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0001000E, "prioreset",       }, // 0-63
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0416000F, "delayedup",       }, // 0-5/0-1/D1-65535/R1-65535
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0C160010, "tipdelayedup",    }, // 0-5/0-1/D1-65535/R1-65535/T1-65535
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04160011, "delayeddown",     }, // 0-5/0-1/D1-65535/R1-65535
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x0C160012, "tipdelayeddown",  }, // 0-5/0-1/D1-65535/R1-65535/T1-65535
                                                                                             //         1/10s    1/10s    1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04000013, "timerondelayed",  }, // R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04000014, "timeroffdelayed", }, // R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x04000015, "timerontimed",    }, // R1-65535 1/10s
  { 0x00000001      , 0x00000400       , CPARSE_TYPE_LEAF, 0x00000016, "timerAbort",      }, //

  { CPARSE_MASK_NONE, CPARSE_MASK_NONE , CPARSE_TYPE_NONE, 0x00000000, "",                },
  };

tParseRsvd g_gpiodParseCmdSystem[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//                                                         0xPPPPMMMM                                   
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000001, "version",   },
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000002, "memory",    },
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000003, "uptime",    },
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000004, "emul",      }, // <emul>.ack
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000005, "mode",      }, // <mode>.ack
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000006, "efmt",      }, // <efmt>.ack
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000007, "disable",   }, // ack
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000008, "enable",    }, // ack
  { 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x80000009, "reboot",    }, // ack
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000000, "output",    },
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000001, "shutter",   },
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000001, "standalone",},
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000002, "networked", },
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000003, "both",      },
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000000, "numerical", },
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000001, "textual",   } ,
//{ 0x00000001      , 0x00001000       , CPARSE_TYPE_LEAF, 0x00000001, "ack",       },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "", },
  };

