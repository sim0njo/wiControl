
//----------------------------------------------------------------------------
// cgpiod_parse_data.cpp : command parser data
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

tParseRsvd g_gpiodParseObj[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     CGPIOD_EMUL_%                        0x0000CCcc                                   
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000100, "in0",       },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000101, "in1",       },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000102, "in2",       },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000103, "in3",       },

  { 0x00000018      , 0x00000001       , CPARSE_TYPE_NODE, 0x00000200, "out0",      },
  { 0x00000018      , 0x00000001       , CPARSE_TYPE_NODE, 0x00000201, "out1",      },

  { 0x00000018      , 0x00000002       , CPARSE_TYPE_NODE, 0x00000400, "out0",      },

  { 0x10000000      , 0x00000000       , CPARSE_TYPE_NODE, 0x00000800, "hbeat0",    }, // only for print

  { 0x00000010      , 0x00000003       , CPARSE_TYPE_NODE, 0x00001000, "system",    },
                                                                         
  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",          },
  };

tParseRsvd g_gpiodParseObjSta[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_CLS_%                                           0x000000SS  params                
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "out",       },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "in",        },

  { 0x00000200      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "off",       },
  { 0x00000200      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "on",        },

  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "stop",      },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "upon",      },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000002, "downon",    },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",          },
  };

tParseRsvd g_gpiodParseObjEvt[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_CLS_%                                           0x0000EEEE  params                
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000002, "ingt0",     },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000003, "outlt1",    },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000004, "ingt1",     },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000005, "outgt1",    },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000006, "ingt2",     },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000007, "out",       },

  { 0x80000200      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "off",       },
  { 0x80000200      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "on",        },

  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "stop",      },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "upon",      },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000002, "downon",    },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000003, "upoff",     },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000004, "downoff",   },

  { 0x80000600      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000005, "timexp",    },
  { 0x80000600      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000006, "timeroff",  },
  { 0x80000600      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000007, "timeron",   },
  { 0x80000600      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000008, "timerabort",},
                                                                                  
  { 0x00000800      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "tickEven",  },
  { 0x00000800      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "tickOdd",   },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",          },
  };

tParseRsvd g_gpiodParseCmdInput[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     Num2Mask(cc)                         0x0000EEEE  params                
  { 0x00000018      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000001, "status",    },
  { 0x00000018      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000002, "ingt0",     },
  { 0x00000018      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000003, "outlt1",    },
  { 0x00000018      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000004, "ingt1",     },
  { 0x00000018      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000005, "outgt1",    },
  { 0x00000018      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000006, "ingt2",     },
  { 0x00000018      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000007, "out",       },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",          },
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
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000013, "blink",          },
  { 0x00000018      , 0x00000003       , CPARSE_TYPE_LEAF, 0x02000014, "blinktimed",     }, // 1-65535 s

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",               },
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

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",                },
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

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",          },
  };

