//-----------------------------------------------------------------------+
// File         : cbpm_reread_params.c                                   |
//                                                                       |
// Description  : Rereads in the detector and instrument parameters.     |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Stedinger                                          |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_reread_params( void ) {

  int k, retval = CBI_F_SUCCESS;

  //---------------------------------------------------------------------
  // Read in operational parameters for each instrument verfied as being
  // online and associate the assigned detectors with each unit.
  //    Read in and store separate timing values for each available
  //    gain regime.
  //---------------------------------------------------------------------
  printf("\n\n\n----------------------------------------------------------------------\n");
  printf("Reading general instrument parameters for all requested instruments...\n");
  printf("----------------------------------------------------------------------\n");
  // FIXME: TODO: Allow selection of instrument-specific file reading routine here
  retval = cbpm_read_detector_params( appconfig.det_param_file );  
  fflush(stdout);
  
  if (retval != CBI_F_SUCCESS) {
    return retval;
  }
  
  printf("\n\n\n----------------------------------------------------------------------\n");
  printf("Reading instrument-specific parameters for all requested instruments...\n");
  printf("----------------------------------------------------------------------\n");
  // FIXME: TODO: Allow selection of instrument-specific file reading routine here
  retval = cbpm_read_instrument_params( appconfig.inst_param_file ); 
  
  if (retval != CBI_F_SUCCESS) {
    return retval;
  }  

  printf("\n\n\n----------------------------------------------------------------------\n");
  printf("Loading the Instrument Code parameters to all requested instruments...\n");
  printf("----------------------------------------------------------------------\n");
  retval = cbpm_load_instr_code_params();
  fflush(stdout);

  return retval;

}
