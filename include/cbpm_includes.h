#ifndef CBPM_INCLUDES_H
#define CBPM_INCLUDES_H
//-----------------------------------------------------------------------+
// File         :  cbpm_includes.h                                       |
//                                                                       |
// Description  :  Aggregates all common platform-specific (BPM-TSHARC)  |
//                 header file references into one include file for easy |
//                 use.                                                  |
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

//System includes
//-------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <signal.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>


// ACC-release includes
//-------------------------------------
#include "CESR_platform.h"  // Platform macro definitions
#include "utilities.h"      //
#include "messages.h"       // Status and logging message capabilities


// Beam Instrumentation Core
//-------------------------------------
#include "cbi_core_includes.h"


// Instrument-platform-specific library includes (BPM-TSHARC)
//-------------------------------------
#include "cbpm_inst_includes.h"
#include "cbpm_protos.h"

#include "cbpm_types_a.h"   // Communications structure typedefs
#include "cbpm_tags_a.h"    // "TAG"s for referring to communications structures.
#include "cbpm_protos_a.h"  // Communications type conversion prototypes

#include "cbpm_constants.h" // Architecture-dependent constants
#include "cbpm_registers.h" // Architecture-dependent registers and memory blocks
#include "cbpm_ctl_types.h" // Control system-only structure definitions
#include "cbpm_names.h"     // Patform-dependent string constants

#ifndef CBPM_MATLAB
#include "cbpm_command_specs.h"
#endif

#include "cbpm_control_system.h"



#if defined (__cplusplus)
}
#endif

#endif //CBPM_INCLUDES_H
