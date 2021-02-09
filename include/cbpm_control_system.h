#ifndef CBPM_CONTROL_SYSTEM_H
#define CBPM_CONTROL_SYSTEM_H

//-----------------------------------------------------------------------+
// File         :  cbpm_control_system.h                                 |
//                                                                       |
// Description  :  This header file defines the Fast BPM system config-  |
//                 uration data structures.   These data structures      |
//                 contain configuration and calibration information     |
//                 for the new BPM readout system.                       |
//                                                                       |
// Author       :  M. Palmer                                             |
//-----------------------------------------------------------------------+


//-----------------------------------------------------------------------+
//  The following provides for C++ compatibility (ie, C++ routines can   |
//  explicitly use this include file).                                   |
//-----------------------------------------------------------------------+
#if defined (__cplusplus)
extern "C" {
#endif


//-----------------------------------------------------------------------+
// The following header files are those required by this header file.    |
//-----------------------------------------------------------------------+
#include "cbi_constants.h"
#include "cbpm_ctl_types.h"


#if defined (__cplusplus)
}
#endif


#endif // CBPM_CONTROL_SYSTEM_H





//-----------------------------------------------------------------------+
// Declare global structures required for the BPM instrument-specific    |
// control system functionality.                                         |
//-----------------------------------------------------------------------+
#if defined (CBPM_NO_EXTERN)


// These structures hold the complete set of communications data structures.
// Those include data sets and control/parameter information for individual 
// commands and state information for each BPM.    
CBPM_DATA           cbpm_comm_structs[CBI_MAX_INSTRUMENTS];
CBPM_CONTROL        tsharc_ctl[CBI_MAX_INSTRUMENTS];
CBPM_TBT_DATA       cbpm_tbt_data[CBI_MAX_INSTRUMENTS];

CBPM_DATA           cbpm_comm_structs_temp;
CBPM_CONTROL        tsharc_ctl_temp;
CBPM_TBT_DATA       cbpm_tbt_data_temp;
CBPM_POS_DATA       cbpm_pos_data_temp;
CBPM_TBT_ANALYSIS   cbpm_tbt_analysis_temp;


// Holding area for data support values
struct data_spec    dspec;

float supp_plotvals[8];
int   continuous_mode     = FALSE;
// Global to allow breaking out of current monitor mode
int   cbpm_currmon_server = FALSE;



#else



extern CBPM_DATA           cbpm_comm_structs[CBI_MAX_INSTRUMENTS];
extern CBPM_CONTROL        tsharc_ctl[CBI_MAX_INSTRUMENTS];
extern CBPM_TBT_DATA       cbpm_tbt_data[CBI_MAX_INSTRUMENTS];

extern CBPM_DATA           cbpm_comm_structs_temp;
extern CBPM_CONTROL        tsharc_ctl_temp;
extern CBPM_TBT_DATA       cbpm_tbt_data_temp;
extern CBPM_POS_DATA       cbpm_pos_data_temp;
extern CBPM_TBT_ANALYSIS   cbpm_tbt_analysis_temp;

///extern CBPM_ORBIT_DATA     orbit_data_block[CBI_MAX_INSTRUMENTS];
///extern CBPM_ORBIT_STAT     orbit_stat_block[CBI_MAX_INSTRUMENTS];


extern struct data_spec   dspec;

extern float supp_plotvals[8];
extern int   continuous_mode;
extern int   cbpm_currmon_server;



#endif
