/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_modespec_to_tsetup.c                             |
*                                                                       |
* Description  :  Appends a CBI_COMMAND command specification structure |
*                 to the central sequence queue of commands that gets   |
*                 iterated over when executing commands.                |
*                                                                       |
* Arguments    :  Pointer to string                                     |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_modegain_to_tsetup( int timing_mode, int gain_setting, int species ) {

  int timing_setup;

  if (gain_setting != 0) {

    if (timing_mode == m4ns) {
      if (species == POSITRONS) {
	timing_setup = s4ns_P_VAR_G;
      } else {
	timing_setup = s4ns_E_VAR_G;
      }
    } else if (timing_mode == m14ns) {
      timing_setup = s14ns_VAR_G;
    } else {
      timing_setup = CBI_UNINITIALIZED_PARAM;
    }

  } else {
   
    if (timing_mode == m4ns) {
      if (species == POSITRONS) {
	timing_setup = s4ns_P_FIX_G;
      } else {
	timing_setup = s4ns_E_FIX_G;
      }
    } else if (timing_mode == m14ns) {
      timing_setup = s14ns_FIX_G;
    } else {
      timing_setup = CBI_UNINITIALIZED_PARAM;
    }
 
  } //endIF gain_setting


  return timing_setup;
}






int cbpm_currmon_modespec_to_tsetup( int timing_mode, int species ) {

  int timing_setup;

  if (timing_mode == 4) {
    if (species == POSITRONS) {
      timing_setup = s4ns_P_VAR_G;
    } else {
      timing_setup = s4ns_E_VAR_G;
    }
  } else if (timing_mode == 14) {
    timing_setup = s14ns_VAR_G;
  } else {
    timing_setup = CBI_UNINITIALIZED_PARAM;
  }

  return timing_setup;
}

  


