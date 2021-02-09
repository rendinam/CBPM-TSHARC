//------------------------------------------------------------------------
// File         :  acquire_all_pedestals.c
//
// Description  :  Acquire a full pedestal table (entries for all gain
//                 settings in all gain regimes) for each supported
//                 timing setup.
//------------------------------------------------------------------------

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int acquire_all_pedestals( void ) {
	
  //-------------------------------------
  // Set routine ID in the trace
  //-------------------------------------
  int FID = ACQUIRE_ALL_PEDESTALS_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  volatile int *p_error = INST_cmd.error;     
  
  int tsetup, card, tblock;
  int retval;
  
  
  // Save the gain setting used when entering this function.
  // NOTE: This only checks  CARD 0 -- BLOCK A.
  int original_gain_setting = INST_op_gain.active_gain_settings[0][0];
  
  // Save the timing setup active when entering this function.
  int original_timing_setup = INST_op_timing.active_timing_setup;
  
 
  //-----------------------------------------------------------------
  // Visit each supported timing setup and perform a 
  // pedestal acquisition procedure.
  //-----------------------------------------------------------------
  for (tsetup = 0; tsetup < CBPM_TOTAL_TIMING_SETUPS; tsetup++) {
    
    INST_op_timing.active_timing_setup = tsetup;
    retval = set_timing_setup();
    
    if (retval == CBI_F_FAILURE) {
      cbpm_set_error(ACQ_ALL_PED_SET_TIMING_SETUP_ERR, p_error);
      INST_stat.num_levels--;
      return CBI_F_FAILURE;
    }
    
    retval = pedestal_cal();
    
    if (retval == CBI_F_FAILURE) {
      cbpm_set_error(ACQ_ALL_PED_PED_CAL_ERR, p_error);
      //INST_stat.num_levels--;
      //return CBI_F_FAILURE;
    }    
    
    
  } //endFOR tsetup
  
  
  // Restore timing setup to original value.
  INST_op_timing.active_timing_setup = original_timing_setup;
  retval = set_timing_setup();    		
  
  
  // Restore gain setting to original value.
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
      INST_op_gain.active_gain_settings[card][tblock] = original_gain_setting;
    }
  }
  set_gains();
  
  
  //---------------------------------------
  // Decrement the trace and return status
  //---------------------------------------
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
  
}

