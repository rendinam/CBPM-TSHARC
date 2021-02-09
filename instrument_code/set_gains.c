//-----------------------------------------------------------------------+
// File         :  set_gains.c                                           |
//                                                                       |
// Description  :  Uses the contents of the op_gain structure to look up |
//                 the gain code assigned to each channel in the device  |
//                 for the set of currently active gain settings.  The   |
//                 retrieved gain codes are then written to the hardware |
//                 to set the amplifier gains.                           |
//                                                                       |
// Prototype    :  int set_gain(int*)                                    |
//                                                                       |
// Returns      :  CBI_F_SUCCESS   - normal return                       |
//                 CBI_F_FAILURE   - problem configuring gains           |
//  TODO: Add value bounds checking to flag errors prior to attempting   |
//        to set gains.                                                  |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int set_gains( void ) {
  
  // Add function ID to the trace
  int FID = SET_GAINS_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  *value1_reg = SET_GAINS_ID;
  
  int gain_setting;
  int tblock, card; 
  
  int return_val;
  
  //---------------------------------------
  // Loop over all cards and set the gain
  //---------------------------------------
  
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
      gain_setting = INST_op_gain.active_gain_settings[card][tblock];
      *p_gain_regs[card][tblock] = INST_gain_config.gain_codes[gain_setting];
    }
  }
  
  //----------------------------
  // Decrement trace and return 
  //----------------------------
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
}










