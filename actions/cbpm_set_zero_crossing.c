/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_set_zero_crossing.c                              |
*                                                                       |
* Description  :  Shifts all block timing values (0.4ns) LATER          |
*                 from where the waveform peak is normally sampled to   |
*                 where the waveform crosses the zero-amplitide axis.   |
*                 This can be used for clock diagnostics.               |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_set_zero_crossing_prep(void) {
  
  int input;
  int status, inst, card, chan = 0;
  int TSETUP;

  TSETUP = cbpm_measurement_tsetup();
  printf("Timing setup is presently: %d [%s]\n", TSETUP, tsetup_names[TSETUP] );

  CBPM_DATA *dp, *dpt;
  dpt = CTL_System.p_Module_temp->dsp_data;


  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(1);


  int retval;
  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {CBI_TRUE, CBI_TRUE, CBI_FALSE, CBI_FALSE};
  prompts[0].supp_text_ptr    = "\n\n\n***********************************************************\n"
    " This shifts the waveform sample point on \n"
    " BLOCK A and B for all active instruments 0.4ns (40 computer units) \n"
    " later in time to examine the waveform zero-crossing point.\n\n"
    " This is only accurate for the FIXED GAIN amplifier mode.\n\n"
    " The undo this change, you must restart the program.\n"
    "************************************************************\n";
  prompts[0].prompt_text      = "Do you wish to proceed [y/n]? ";
  prompts[0].data_type        = CBI_CHAR;
  prompts[0].valid_inputs     = valid_inputs;
  prompts[0].map_out          = map_out;
  prompts[0].default_value    = 'n';
  prompts[0].destination      = &(input);
  prompts[0].last_val_default = CBI_TRUE;

  
  retval = CBI_PROMPTER();


  if (input == TRUE ) {

    for (inst = 0; inst < num_online(); inst++) {
      if (CTL_System.p_Module[inst]->active == CBI_ACTIVE) {

	dp = CTL_System.p_Module[inst]->dsp_data;
	dp->cbpm_timing_config.setups[TSETUP].block_delays[0] += 40;
	dp->cbpm_timing_config.setups[TSETUP].block_delays[1] += 40;

	// Send updated timing parameters to instrument
	printf("Sending updated parameters...\n");
	status=cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_TIMING_CONFIG_TAG, CTL_System.p_Module[inst], CBI_FULL_STRUCT);
	
      }
    } // endFOR inst

    dpt->cbpm_op_timing.active_timing_setup = TSETUP;
    
  } else {
    printf(" \n\n Action aborted. \n\n");
    return CBI_F_FAILURE;
  }

  return CBI_F_SUCCESS;

}
