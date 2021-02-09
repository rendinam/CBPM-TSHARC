/*!
*-------------------------------------------------------------------------+
* File         :  cbpm_measurement_species.c                              |
*                                                                         |
* Description  :  Returns the species value of the most recent            |
*                 measurement.  Meant to give lower-level libraries       |
*                 access to the species identifier for the measurement.   |
*                                                                         |
* Arguments    :  None                                                    |
*                                                                         |
* Author       :  M. Rendina                                              |
*-------------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_measurement_species(void) {
  
  int tsetup  = CTL_System.meas_info.tsetup;
  int species = CBI_UNINITIALIZED_PARAM;
                
  switch (tsetup) {
  case s4ns_P_FIX_G:
  case s4ns_P_VAR_G:
    species = POSITRONS;
    break;
  case s4ns_E_FIX_G:
  case s4ns_E_VAR_G:
    species = ELECTRONS;
    break;
  case s14ns_FIX_G:
  case s14ns_VAR_G:
    species = DUAL;
    break;
  }
  
  return species;

}
