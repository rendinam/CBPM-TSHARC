//-----------------------------------------------------------------------+
// File         : cbpm_set_gains.c                                       |
//                                                                       |
// Description  : Sets the gain of the front-end card amplifiers by      |
//                sending configuration data to and then running the     |
//                instrument's on-board gain setting routine.            |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_set_gains_prep(void) {

  int card, tblock, gain_setting;

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;


  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  int retval, input = CBI_FALSE;
  CBI_INIT_PROMPTS(1);
  
  char prompt[100];
  sprintf(prompt, "Enter a gain setting index between 1 and %d inclusive ", CBPM_MAX_GAIN_SETTING );

  int range[2] = {1, CBPM_MAX_GAIN_SETTING};
  prompts[0].supp_text_ptr    = "\n*********** SET GAIN *****************\n"
                                " All channels will be set to the same gain.\n";
  prompts[0].prompt_text      = prompt;
  prompts[0].data_type        = CBI_INT;
  prompts[0].valid_inputs     = range;
  prompts[0].validator        = cbi_int_range_validator;
  prompts[0].default_value    = 4;
  prompts[0].destination      = &(gain_setting);
  prompts[0].last_val_default = CBI_FALSE;


  retval = CBI_PROMPTER();


  // Load up temporary cmd param structure member
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
      dp->cbpm_op_gain.active_gain_settings[card][tblock] = gain_setting;
    }
  }

  //return F_SUCCESS;
  return retval;

}




int cbpm_set_gains_post( int iidx ) {
  
  int stat;

  CBPM_DATA  *dptr;

  dptr      = CTL_System.p_Module[iidx]->dsp_data;

  stat = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_GAIN_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  printf("gain setting = %d  (gain code = %d)\n", 
	 dptr->cbpm_op_gain.active_gain_settings[0][0],
	 dptr->cbpm_gain_config.gain_codes[ dptr->cbpm_op_gain.active_gain_settings[0][0] ]  );

  return CBI_F_SUCCESS;
  
}
