//-----------------------------------------------------------------------+
// File         :  cbpm_find_bTMD_prep.c                                 |
//                                                                       |
// Description  :  Prompt user for this action's parameters,             |
//                 populate temporary structure in preparation of        |
//                 copying to all active BPM's structure sets.           |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_find_cTMD_post(int iidx) {

  int status;
  CBPM_DATA         *dp;

  dp = CTL_System.p_Module[iidx]->dsp_data;

  printf("Bunch pattern offset BEFORE cTMD setting = %d\n", dp->cbpm_cmd_params.bun_pat_offset );
  
  // Read back timing config.  Apply adjustments on top of these values.
  status=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TIMING_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  int new_cTMD;
  int tsetup = dp->cbpm_op_timing.active_timing_setup;
  int tblock = dp->cbpm_cmd_params.tblock;

  printf("BP offset = %d\n", dp->cbpm_timing_config.setups[tsetup].BP_offsets[tblock] );
  new_cTMD = (dp->cbpm_timing_config.setups[tsetup].BP_offsets[tblock] - 20) / 10.5;

  if (new_cTMD > 60) {
    new_cTMD = 60;
  }
  dp->cbpm_timing_config.setups[tsetup].cTMD = new_cTMD;

  printf("   cTMD      = %d\n", dp->cbpm_timing_config.setups[tsetup].cTMD );

  // Send adjusted value to instrument
  status=cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_TIMING_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  // Apply this change
  cbi_init_command( iidx, SET_TIMING_SETUP_CMD );
  cbi_execute_command( iidx );
  cbi_wait_for_command( iidx );


  return CBI_F_SUCCESS;

}


