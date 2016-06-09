
//----------------------------------------------------------------------------
// cgpiod_parse_data.cpp : command parser data
//
// Copyright (c) Jo Simons, 2016-2016, All Rights Reserved.
//----------------------------------------------------------------------------
#include <gpiod.h>

tParseRsvd g_gpiodParseObj[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     CGPIOD_EMUL_%                        0x0000CCcc                                   
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000100, "in0",       },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000101, "in1",       },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000102, "in2",       },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000103, "in3",       },

  { 0x000000C0      , 0x00000001       , CPARSE_TYPE_NODE, 0x00000200, "out0",      },
  { 0x000000C0      , 0x00000001       , CPARSE_TYPE_NODE, 0x00000201, "out1",      },
  { 0x000000C0      , 0x00000001       , CPARSE_TYPE_NODE, 0x00000202, "out2",      },
  { 0x000000C0      , 0x00000001       , CPARSE_TYPE_NODE, 0x00000203, "out3",      },

  { 0x000000C0      , 0x00000002       , CPARSE_TYPE_NODE, 0x00000400, "out0",      },
  { 0x000000C0      , 0x00000002       , CPARSE_TYPE_NODE, 0x00000401, "out1",      },

  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000800, "tmr0",      },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000801, "tmr1",      },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000802, "tmr2",      },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00000803, "tmr3",      },

//  CGPIOD_ORIG_%     CGPIOD_EMUL_%                        0xppPPMMMM                                   
//{ 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00001000, "system",    },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00001001, "ping",      },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00001002, "version",   },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00001003, "memory",    },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x00001004, "uptime",    },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x01001005, "loglevel",  }, // <loglevel>
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x02001006, "emul",      }, // <emul>
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x04001007, "mode",      }, // <mode>
//{ 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x08001008, "efmt",      }, // <efmt>
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x10001009, "lock",      }, // <0|1>
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x1000100A, "disable",   }, // <0|1>
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x0080100B, "restart",   }, // ack
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_NODE, 0x0080100C, "save",      }, // ack
                                                                         
  { 0x10000000      , 0x00000000       , CPARSE_TYPE_NODE, 0x00002000, "hbeat",     }, // only for print

  { 0x10000000      , 0x00000000       , CPARSE_TYPE_NODE, 0x00004000, "led",       }, // only for print

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",          },
  };

tParseRsvd g_gpiodParseObjSta[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_CLS_%                                           0x000000SS  params                
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "out",       },
  { 0x00000100      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "in",        },

  { 0x00000A00      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "off",       },
  { 0x00000A00      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "on",        },

  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "stop",      },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "up",        },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000002, "down",      },

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

//{ 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "stop",      },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "upon",      },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000002, "downon",    },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000003, "upoff",     },
  { 0x80000400      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000004, "downoff",   },

  { 0x80000600      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000005, "timexp",    },
                                                                                  
  { 0x80000800      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000006, "timeroff",  },
  { 0x80000800      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000007, "timeron",   },
  { 0x80000800      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000008, "timerabort",},
                                                                                  
  { 0x00002000      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000000, "tickEven",  },
  { 0x00002000      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "tickOdd",   },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",          },
  };

tParseRsvd g_gpiodParseCmdInput[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     Num2Mask(cc)                         0x0000EEEE  params                
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000001, "status",    },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000002, "ingt0",     },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000003, "outlt1",    },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000004, "ingt1",     },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000005, "outgt1",    },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000006, "ingt2",     },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000007, "out",       },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x01000100, "debounce",  },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",          },
  };

tParseRsvd g_gpiodParseCmdOutput[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     Num2Mask(cc)                         0xPPppMMMM  params                
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000001, "status",         },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000002, "on",             },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000003, "off",            },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000004, "onlocked",       },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000005, "offlocked",      },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000006, "toggle",         },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000007, "unlock",         },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00010008, "ondelayed",      }, // 1-3600 s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00010009, "offdelayed",     }, // 1-3600 s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x0002000A, "ontimed",        }, // 1-3600 s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x0002000B, "offtimed",       }, // 1-3600 s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x0001000C, "toggledelayed",  }, // 1-3600 s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x0002000D, "toggletimed",    }, // 1-3600 s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x0000000E, "lock",           },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x0000000F, "locktimed",      },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00020010, "timeset",        }, // 1-3600 s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00020011, "timeadd",        }, // 1-3600 s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000012, "timeabort",      },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000017, "blink",          },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00020018, "blinktimed",     }, // 1-3600 s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x01000100, "deftime",        }, // 0-3600 s

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",               },
  };

tParseRsvd g_gpiodParseCmdShutter[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     Num2Mask(cc)                         0xPPppMMMM  params                
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00000001, "status",          },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00020002, "stop",            }, // 0-5
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00160003, "toggleUp",        }, // 0-5/0-1/R1-65535 1/10s
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00160004, "toggleDown",      }, // 0-5/0-1/R1-65535 1/10s
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00160005, "up",              }, // 0-5/0-1/R1-65535 1/10s
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00160006, "down",            }, // 0-5/0-1/R1-65535 1/10s
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00260007, "tipUp",           }, // 0-5/0-1/R1-65535 1/10s
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00260008, "tipDown",         }, // 0-5/0-1/R1-65535 1/10s
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x00010009, "priolock",        }, // 0-63
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0001000A, "priounlock",      }, // 0-63
//{ 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0000000B, "learnon",         },
//{ 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0000000C, "learnoff",        },
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0001000D, "prioset",         }, // 0-63
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x0001000E, "prioreset",       }, // 0-63
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x001E000F, "delayedup",       }, // 0-5/0-1/D1-65535/R1-65535
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x003E0010, "tipdelayedup",    }, // 0-5/0-1/D1-65535/R1-65535/T1-65535
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x001E0011, "delayeddown",     }, // 0-5/0-1/D1-65535/R1-65535
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x003E0012, "tipdelayeddown",  }, // 0-5/0-1/D1-65535/R1-65535/T1-65535
  { 0x000000C0      , 0x00000003       , CPARSE_TYPE_LEAF, 0x01000100, "deftime",         }, // 1-3600 s

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",                },
  };

tParseRsvd g_gpiodParseCmdTimer[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%     Num2Mask(cc)                         0xppPPMMMM  params                
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000001, "status",         },
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00010013, "ondelayed",      }, // 1-36000 1/10th s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00010014, "offdelayed",     }, // 1-36000 1/10th s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00020015, "ontimed",        }, // 1-36000 1/10th s
  { 0x000000C0      , 0x0000000F       , CPARSE_TYPE_LEAF, 0x00000016, "abort",          },

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",               },
  };

tParseRsvd g_gpiodParseCmdSystem[] = {
//  dwMask0           dwMask1            dwTType           dwTVal      szTVal
//  CGPIOD_ORIG_%                                          0xppPPMMMM                                   
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000001, "ping",      },
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000002, "version",   },
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000003, "memory",    },
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00000004, "uptime",    },
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x00800005, "restart",   }, // 
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x01000100, "loglevel",  }, // <loglevel>
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x02000200, "emul",      }, // <emul>
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x04000300, "mode",      }, // <mode>
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x08000400, "efmt",      }, // <efmt>
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x10000500, "lock",      }, // 
//{ 0x000000C0      , 0x00000001       , CPARSE_TYPE_LEAF, 0x10000600, "disable",   }, // 

  { 0x000000C0      , 0x00001000       , CPARSE_TYPE_PARM, 0x00000001, "ack",       }, // 

  { 0x00000000      , 0x00000000       , 0x00000000      , 0x00000000, "",          },
  };

