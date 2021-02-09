//-----------------------------------------------------------------------+
// File         :  cbpm_phase_turn_offset.c                              |
//                                                                       |
// Description  :  Determines the turns offset value to use in           |
//                 calculating betatron phase measurement coefficients   |
//                 based on the provided species and bunch index for the |
//                 given instrument.                                     |
//                                                                       |
// Arguments    :  iidx    - instrument index                            |
//                 species -                                             |
//                 bunch   - bunch index value                           |
//                                                                       |
// Return Value :  Status - F_SUCCESS/F_FAILURE                          |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_phase_turn_offset( int iidx, int species, int bunch ) {

  CBPM_CONTROL   *cp;
  cp = CTL_System.p_Module[iidx]->control;

  int offset = CBI_UNINITIALIZED_PARAM;

  if ( bunch < cp->config_params.phase_jump_boundaries[species][0] ) {
    offset = cp->config_params.phase_turn_offsets[species][0];
  } else {
    offset = cp->config_params.phase_turn_offsets[species][1];
  }
  
  return offset;

}


int cbpm_phase_wait_value( int iidx, int species, int bunch ) {

  CBPM_CONTROL   *cp;
  cp = CTL_System.p_Module[iidx]->control;

  int wait_val = CBI_UNINITIALIZED_PARAM;

  if ( bunch < cp->config_params.phase_jump_boundaries[species][0] ) {
    wait_val = cp->config_params.phase_wait_values[species][0];
  } else {
    wait_val = cp->config_params.phase_wait_values[species][1];
  }
  
  return wait_val;

}
