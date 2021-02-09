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


int cbpm_find_bTMD_prep(void) {

  int i, value;
  int species;

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  printf("********* FIND BLOCK TURN MARKER DELAY **********\n");

  printf("Will use cached channel delays.\n");

  // Populate/initialize the relevant fields in the cmd_params structure
  
  int tblock;
  printf("Checking timing setup of the first active device.\n");
  printf("Startup checks shall have ensured that all active\n");
  printf("devices are configured for the same timing setup.\n\n");

  int tsetup;
  tsetup = cbpm_measurement_tsetup();
  
  int DefaultValue = 0;

  if (tsetup == s4ns_P_VAR_G || tsetup == s4ns_P_FIX_G ) {
    printf("Timing setup is for %s POSITRONS.\n", tsetup_names[tsetup] );
    species = POSITRONS;
    printf("Enter timing block [0 = A,  1 = B] : ");
    value = cbi_get_int();
    if (value == CBI_EMPTY_INPUT) {
      value = DefaultValue;
    }
    tblock = value;
  } else if (tsetup == s4ns_E_VAR_G || tsetup == s4ns_E_FIX_G ) {
    printf("Timing setup is for %s ELECTRONS.\n", tsetup_names[tsetup] );
    species = ELECTRONS;
    printf("Enter timing block [0 = A,  1 = B] : ");
    value = cbi_get_int();
    if (value == CBI_EMPTY_INPUT) {
      value = DefaultValue;
    }
    tblock = value;
  } else {
    printf("Enter species [POSITRONS=0, ELECTRONS=1]    : ");
    species = cbi_get_int();
    if (species == CBI_EMPTY_INPUT) {
      species = DefaultValue;
    }
    tblock  = species;  // FIXME: Will eventually want to be able to swap this on demand.
  }
  
  dp->cbpm_cmd_params.species          = species;
  dp->cbpm_cmd_params.tblock           = tblock;
  printf("TBLOCK = %d\n", tblock);
  
  int turns;
  printf("Setting number of turns to 2\n");
  turns = 2;
  dp->cbpm_cmd_params.num_turns = turns;
  
  cbi_clear_bp( dp->cbpm_cmd_params.bunch_pat );
  
  return F_SUCCESS;
  
}





int cbpm_find_bTMD_post(int iidx) {
  
  int k;
  int species, card;
  
  CBPM_DATA         *dp;

  dp = CTL_System.p_Module[iidx]->dsp_data;

  // Read back the structures to obtain status and payload
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBI_STAT_TAG,            CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CMD_PARAMS_TAG,      CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TIMING_CONFIG_TAG,   CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_TIMING_TAG,       CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  species = dp->cbpm_cmd_params.species;

  int tblock = dp->cbpm_cmd_params.tblock;
  int tsetup = dp->cbpm_op_timing.active_timing_setup;

  printf(" \n\nSettings determined for TIMING BLOCK %d:\n", tblock);
  printf("   b_p_offset = %d\n",   dp->cbpm_timing_config.setups[tsetup].BP_offsets[tblock] );
  printf("   cTMD       = %d\n",   dp->cbpm_timing_config.setups[tsetup].cTMD );
  printf("   bTMD       = %d\n",   dp->cbpm_timing_config.setups[tsetup].bTMDs[tblock]);
  printf("   Initial CD = %d\n\n", dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][0] );

  return CBI_F_SUCCESS;

}


