/*!
*-------------------------------------------------------------------------+
* File         :  cbpm_measurement_tsetup.c                               |
*                                                                         |
* Description  :  Returns the timing setup value of the first active      |
*                 instrument in the list of currently online insturments. |
*                 All online and active instruments in a given            |
*                 application session should be in the same timing_setup. |
*                 Function is primarily useful for determining which      |
*                 which timing setup and timing mode to use when setting  |
*                 up for an action execution in a "_prep" function, and   |
*                 for providing the timing setup used for a given         |
*                 measurement when a data file header is written.         |
*                                                                         |
* Arguments    :  None                                                    |
*                                                                         |
* Author       :  M. Rendina                                              |
*-------------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_measurement_tsetup(void) {
  
  CBPM_DATA         *dp;

  int i, tsetup;
  for (i = 0; i < CBI_MAX_INSTRUMENTS; i++) {

    dp = CTL_System.p_Module[i]->dsp_data;

    if ( CTL_System.p_Module[i]->active == CBI_ACTIVE ) {

      tsetup = dp->cbpm_op_timing.active_timing_setup;

      break;
    }
  }
  
  return tsetup;

}


int cbpm_measurement_tmode(void) {
  
  int tsetup = cbpm_measurement_tsetup();

  switch (tsetup) { 
    
  case s4ns_P_VAR_G:
  case s4ns_P_FIX_G:
  case s4ns_E_VAR_G:
  case s4ns_E_FIX_G:
    return m4ns;
    break;
    
  case s14ns_VAR_G:
  case s14ns_FIX_G:
    return m14ns;
    break;
    
  default:
    return CBI_F_FAILURE;
  }
  
}
