#ifndef CBPM_INST_INCLUDES_H
#define CBPM_INST_INCLUDES_H
//-----------------------------------------------------------------------+
// File         :  cbpm_inst_includes.h                                  |
//                                                                       |
// Description  :  Aggregates all common header file references needed   |
//                 by the code that runs on a BPM into one include file  |
//                 for easy use.                                         |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

//-----------------------------------------------------------------------+
//  The following provides for C++ compatibility (ie, C++ routines can   |
//  explicitly use this include file).                                   |
//-----------------------------------------------------------------------+
#if defined (__cplusplus)
extern "C" {
#endif


#ifdef CBPM_INST_PLATFORM
  #include <defts101.h>              // DSP development environment header
  #include <sysreg.h>                //
  #include <builtins.h>              //
  #include <stdlib.h>                //
  #include <time.h>                  //
  #include <math.h>                  //    
  #include <sysreg.h>                //
  #include <signal.h>                //
  #include "cbpm_registers.h"
  #include "cbpm_system.h"
  #include "cbpm_commstructs_a.h"
#endif 

#include "dig_board_6048-113.h"

#include "cbi_core_inst_includes.h"
#include "cbpm_trace.h"
#include "cbpm_constants.h"
#include "cbpm_types_a.h"
#include "cbpm_versions.h"



#if defined (__cplusplus)
}
#endif

#endif //CBPM_INST_INCLUDES_H
