/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_cont_proc_data.c                                 |
*                                                                       |
* Description  :  Sets up all the necessary structures for performing a |
*                 continuous bunch-by-bunch current monitoring task in  |
*                 4ns mode.                                             |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_cont_proc_data_prep( void ) {

  char *func_name = (char *)__FUNCTION__;

  int  calradius, bucket, reg;

  int ped_sub = CBI_FALSE;
  int scale   = CBI_FALSE;

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;


  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  int retval, input = FALSE;
  CBI_INIT_PROMPTS(1);


  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {TRUE, TRUE, FALSE, FALSE};
  prompts[0].supp_text_ptr    = NULL;
  prompts[0].prompt_text      = "\n\nWould you like begin continuous position monitoring for all active instruments? (y/n): ";
  prompts[0].data_type        = CBI_CHAR;
  prompts[0].valid_inputs     = valid_inputs;
  prompts[0].map_out          = map_out;
  prompts[0].default_value    = 'n';
  prompts[0].destination      = &(input);
  prompts[0].last_val_default = FALSE;


  retval = CBI_PROMPTER();

  if (input == FALSE) {
    printf("Continuous monitoring mode aborted.\n");
    return CBI_F_FAILURE;
  }


  int species, tsetup, tblock;
  tsetup = cbpm_measurement_tsetup();

  int use_enable_gate = FALSE;
  use_enable_gate = cbpm_query_trigger(dp);

  //printf("Setting hardware   TRIGGER MASK A  (60Hz) in command parameters...\n\n");
  //dp->cbpm_cmd_params.use_data_enable  = TRUE;
  //dp->cbpm_cmd_params.trig_mask        = TRIG_MASK_A;

  if (tsetup == s4ns_P_VAR_G) {
    printf("Timing setup is for 4ns POSITRONS.\n");
    species = POSITRONS;
  } else if (tsetup == s4ns_E_VAR_G) {
    printf("Timing setup is for 4ns ELECTRONS.\n");
    species = ELECTRONS;
  } else {
    printf("Enter species [POSITRONS=0, ELECTRONS=1]    : ");
    species = cbi_get_int();
    tblock  = species;
  }
  dp->cbpm_cmd_params.species       = species;

  // Set bunch pattern to standard single bunch Train-1, Bunch-1 ----------
  cbi_clear_bp( dp->cbpm_cmd_params.bunch_pat );
  cbi_set_bunch( dp->cbpm_cmd_params.bunch_pat, species+1 );

  // Set the trigger mask
  dp->cbpm_cmd_params.use_data_enable = CBI_FALSE;

  printf("Setting averaging mode to SIMPLE_AVG_MODE \n");
  dp->cbpm_cmd_params.avg_mode = PROC_SIMPLE_AVG_MODE;

  // Set the number of turns to acquire (tune for desired performance)
  int turns;
  printf("Will acquire 128 turns of data per measurement.\n");
  dp->cbpm_cmd_params.num_turns = 128;


  //  No triggering
  dp->cbpm_cmd_params.use_data_enable = CBI_FALSE;

  // Instruct the instrument to continuously refresh all 
  // bunches as quickly as possible in its internal memory.
  printf("Continuous acquisition mode is enabled.\n");


  if (ped_sub) {
    dp->cbpm_cmd_params.scale_mode = PED_SUB_DATA;
  }
  if (scale) {
    dp->cbpm_cmd_params.scale_mode = SCALED_DATA;
  }
  if (ped_sub && scale) {
    dp->cbpm_cmd_params.scale_mode = PED_SUB_SCALED_DATA;
  }

  dp->cbpm_cmd_params.delay_cal = CBI_FALSE;
  //dp->cbpm_cmd_params.gain_cal  = CBI_TRUE;


  printf("Processed data buffer index will be reset to 0 for this command.\n");
  dp->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;


  if (use_enable_gate) {
    return CBI_F_SUCCESS_W_GATE;
  } else {
    return CBI_F_SUCCESS_NO_GATE;
  }

}




int cbpm_cont_proc_data_post( int iidx ) {

   char *func_name = (char *)__FUNCTION__; 

   // Start continuous data acquisition on instrument 
   cbi_copy_touched_structs( iidx, TOT_CBPM_COMM_STRUCTS ); 
   cbi_init_command( iidx, CONTINUOUS_PROC_CMD ); 
   cbi_execute_command( iidx ); 

   return CBI_F_SUCCESS;

}




int cbpm_cont_proc_data_read( void ) {

  char *func_name = (char *)__FUNCTION__;

  int debug_level = CTL_System.debug_verbosity;

  CBPM_DATA          *dp;

  int status, iidx;

  int tot_bunches;
  int counter = 0;

  //---------------------------------------------------------------------
  // Override the SIGINT signal handler when entering continuous
  //  mode to allow breaking out of server mode and back to application.
  //---------------------------------------------------------------------
  if (signal (SIGINT, continuous_mode_sigint_handler) == SIG_IGN) {
    signal (SIGINT, SIG_IGN);
  }

  //--------------------------------------------------------------
  // Also install the SIGPIPE signal handler when entering
  // continuous polling mode to allow for a semi-graceful
  // recovery from any potential socket/pipe errors that appear.
  //--------------------------------------------------------------
  if (signal (SIGPIPE, continuous_mode_sigpipe_handler) == SIG_IGN) {
    signal (SIGPIPE, SIG_IGN);
  }


  int species = CBI_UNINITIALIZED_PARAM;

  // Publish the species and bunch timing_mode indicators
  int timing_setup = dp->cbpm_op_timing.active_timing_setup;

  int integrity_good;
  int tblock, card;
  int chan = 0; // for BPMs
  int retval;

  // For speed, only grab the first proc buf entry that holds the single bunch measurement results.
  int num_words = sizeof(CBPM_PROC_DATA);

  continuous_mode = TRUE;

  //----------------------------------------------
  // Drop into infinite loop of reading back the
  // sum-signal buffer rapidly...
  //----------------------------------------------
  while (continuous_mode) {

    printf("\n");

    for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
      if ( CTL_System.p_Module[iidx]->active == CBI_ACTIVE ) {

	dp = CTL_System.p_Module[iidx]->dsp_data;

	// Retrieve the first entry in the processed data buffer.
	status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_BUF_TAG, CTL_System.p_Module[iidx], num_words);
	if (status == CBI_F_FAILURE) {
	  sprintf(message, "Readback failed for instrument %s!\n", CTL_System.p_Module[iidx]->comm.ethernet.hostname );
	  log_message(S_ERROR, func_name, message);
	}

/* 	printf("[%s]   %6.4f  %6.4f  %6.4f  %6.4f \n", */
/* 	       CTL_System.p_Module[iidx]->comm.ethernet.hostname, */
/* 	       dp->cbpm_proc_buf.proc_data[0].button[1], */
/* 	       dp->cbpm_proc_buf.proc_data[0].button[2], */
/* 	       dp->cbpm_proc_buf.proc_data[0].button[0], */
/* 	       dp->cbpm_proc_buf.proc_data[0].button[3]  ); */
	if (iidx % 50 == 0) {
	  printf("\n");
	}
	printf(".");
	fflush(stdout);
	

	retval = cbpm_buttons2db( iidx );
	
	//cbi_sleep(1);
	if (continuous_mode == FALSE) {
	  break;
	}

	cbi_sleep(50);

      } //endIF active
    } //endFOR iidx

  } //endWHILE continuous_mode

  // Re-enable cursor
  //printf("\033[?25h");
  

  //--------------------------------------------------------------
  // Readback loop was terminated locally, populate the
  // CMD structure with the stopping condition and
  // send to instrument to break remote loop.
  //--------------------------------------------------------------
  printf("Sending IDLE command to terminate remote loop...\n");
  
  for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
    if ( CTL_System.p_Module[iidx]->active == CBI_ACTIVE ) {
      printf("resetting %d...\n", iidx+1);
      cbi_init_command( iidx, CBI_IDLE_CMD ); 
      cbi_execute_command( iidx ); 
      cbi_sleep(10);
    }
  }
  
  
  //--------------------------------------------------------------
  // Disable current monitor-specific trapping of SIGPIPEs
  //--------------------------------------------------------------
  if (signal (SIGPIPE, std_sigpipe_handler) == SIG_IGN)
    signal (SIGPIPE, SIG_IGN);
  
  
  //--------------------------------------------------------------
  // Re-instate the standard keyboard interrupt signal handling
  // to exit application.
  //--------------------------------------------------------------
  if (signal (SIGINT, std_sigint_handler) == SIG_IGN)
    signal (SIGINT, SIG_IGN);
  
  
  return CBI_F_SUCCESS;

}


