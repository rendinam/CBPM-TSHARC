//-----------------------------------------------------------------------+
// File         : cbpm_push_full_config.c                                |
//                                                                       |
// Description  : Simply copies the resident 'op_timing',                |
//                'timing_config', 'gain_config', and 'pedestal_config'  |
//                structures to an instrument.  Typically used to        |
//                the operational state of an instrument to what it was  |
//                immediately prior to a failure event of some kind.     |
//                                                                       |
// Arguments    :  Instrument index value                                |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_push_full_config_post( int iidx ) {

  int k, retval = CBI_F_SUCCESS;
  
  CBPM_DATA  *dp; 
  dp = CTL_System.p_Module[iidx]->dsp_data; 

  // Load into instrument the structure containing the curently operational timing setup value.
  printf("OP TIMING.active_timing_setup = %d [%s]\n", dp->cbpm_op_timing.active_timing_setup, tsetup_names[dp->cbpm_op_timing.active_timing_setup] );
  k = cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_OP_TIMING_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k != CBI_F_SUCCESS) {
    retval = CBI_F_COMM_ERROR;
  }


  // Load into instrument the structure containing all cached timing paramters
  k = cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_TIMING_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k != CBI_F_SUCCESS) {
    retval = CBI_F_COMM_ERROR;
  }


  // Load into instrument the structure containing all cached gain paramters
  k = cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_GAIN_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k != CBI_F_SUCCESS) {
    retval = CBI_F_COMM_ERROR;
  }


  // Load into instrument the structure containing all cached pedestal paramters
  k = cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_PEDESTAL_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k != CBI_F_SUCCESS) {
    retval = CBI_F_COMM_ERROR;
  }


  // Load into instrument the structure containing all cached betatron phase measurement paramters
  k = cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_PHASE_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k != CBI_F_SUCCESS) {
    retval = CBI_F_COMM_ERROR;
  }


  // Load into instrument the structure containig all code parameters
  k = cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_CODE_PARAMS_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k != CBI_F_SUCCESS) {
    retval = CBI_F_COMM_ERROR;
  }


  return retval;

}

