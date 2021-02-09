//-----------------------------------------------------------------------+
// File         :  cbpm_set_timing_setup.c                               |
//                                                                       |
// Description  :  Pushes all the parameters associated with a given     |
//                 'timing setup' into the instrument and then applies   |
//                 them by running the instrument's SET_TIMING_SETUP     |
//                 command.                                              |
//                                                                       |
// Arguments    :  Instrument index                                      |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_set_timing_setup_prep( void ) {

  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  int tsetup, timing_setup, tmode, species;
  int retval;


  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(1);

  // Compose the menu of options as a single string.
  char supp_text[500];
  char temp[30];       // One line's worth of characters
  strcpy( supp_text, "" );
  for (tsetup = 0; tsetup < CBPM_MAX_TIMING_SETUPS; tsetup++) {
    sprintf(temp,
	   "%3d) %s\n",
	   tsetup,
	   tsetup_names[tsetup] );
    strcat( supp_text, temp );
  }

  int range[2] = {0, CBPM_TOTAL_TIMING_SETUPS-1};

  prompts[0].supp_text_ptr    = supp_text;
  prompts[0].prompt_text      = "Enter timing setup ";
  prompts[0].data_type        = CBI_INT;
  prompts[0].validator        = cbi_int_range_validator;
  prompts[0].valid_inputs     = range;
  prompts[0].default_value    = 2;
  prompts[0].destination      = &(dp->cbpm_op_timing.active_timing_setup);
  prompts[0].last_val_default = CBI_TRUE;

  //--------------------------------------------
  // Display prompts and process user input 
  //--------------------------------------------
  retval = CBI_PROMPTER();


  return retval;
 
  
}



int cbpm_set_timing_setup_post(int iidx) {

  int status, tblock, card;

  CBPM_DATA  *dp; 
  dp = CTL_System.p_Module[iidx]->dsp_data; 

  // Read back timing setup state from instrument.  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_TIMING_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  int tsetup = dp->cbpm_op_timing.active_timing_setup;
  printf("OP TIMING SETUP VALUE = %d [%s]\n", tsetup, tsetup_names[tsetup] );

  printf("Block delays = %d   %d\n",
	 dp->cbpm_timing_config.setups[tsetup].block_delays[0],
	 dp->cbpm_timing_config.setups[tsetup].block_delays[1]   );

  //---------------------------------------------------------------------
  // If timing setup is for a FIXED GAIN mode, force a transition to
  // that mode if not already in it.
  //  Note: Only checks the first op_gain entry, under the current
  //        assumption that all channels on all cards are always at the
  //        same gain setting.
  // If timing setup is for a VARIABLE GAIN mode, force a transition
  // to that mode with the application's default gain setting if not
  // already in it.
  //---------------------------------------------------------------------
  if ( tsetup == s4ns_P_FIX_G ||
       tsetup == s4ns_E_FIX_G ||
       tsetup == s14ns_FIX_G      )  {

    if(dp->cbpm_op_gain.active_gain_settings[0][0] != 0) {
      // Load up op_gain structure members:
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
	for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
	  dp->cbpm_op_gain.active_gain_settings[card][tblock] = 0;
	}
      }
      status = cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_OP_GAIN_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
      cbi_init_command( iidx, SET_GAINS_CMD );
      cbi_execute_command( iidx );
      cbi_wait_for_command( iidx );
      cbpm_set_gains_post( iidx );
    }
    
    
  } else {  // TODO: Disallow gain regime changes unless they are requested as part of a timing setup request.
            //   Fix for inconsistent gain settings when starting up.

    if(dp->cbpm_op_gain.active_gain_settings[0][0] <= 0 || dp->cbpm_op_gain.active_gain_settings[0][0] > CBPM_MAX_GAIN_SETTING) {
      // Load up op_gain structure members:
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
	for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
	  dp->cbpm_op_gain.active_gain_settings[card][tblock] = appconfig.default_gain_setting;
	}
      }
      status = cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_OP_GAIN_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
      cbi_init_command( iidx, SET_GAINS_CMD );
      cbi_execute_command( iidx );
      cbi_wait_for_command( iidx );
      cbpm_set_gains_post( iidx );
    }
    
  }
  
   return CBI_F_SUCCESS; 
  
 } 

