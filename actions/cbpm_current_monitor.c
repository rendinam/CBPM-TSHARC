/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_current_monitor.c                                |
*                                                                       |
* Description  :  Prep() sets up all the necessary structures for       |
*                 performing a continuous bunch-by-bunch current        |
*                 monitoring task in 4ns mode.                          |
*                 Post() handles the continuous execution of that task. |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"


#define CBPM_CURRMON_4NS_SPACING    4

int cbpm_do_current_monitor_loop(int iidx, int calibrating, int* species);
int cbpm_save_gains_to_temp(int iidx);


int cbpm_proc_data_currmon_prep(void) {

  char *func_name = (char *)__FUNCTION__;

  int  calradius, bucket, reg;
  char input;

  int ped_sub = CBI_FALSE;
  int scale   = CBI_FALSE;

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  // Set bunch pattern ------------------------------------------
  for (reg = 0; reg < 40; reg++) {
    dp->cbpm_cmd_params.bunch_pat[reg] = 0xFFFF;
  }

  // Set the trigger mask
  int use_enable_gate = FALSE;
  dp->cbpm_cmd_params.use_data_enable = CBI_FALSE;

  printf("Setting averaging mode to PROC_SIMPLE_AVG_MODE \n");
  dp->cbpm_cmd_params.avg_mode = PROC_SIMPLE_AVG_MODE;

  // Set the number of turns to acquire
  int turns;
  dp->cbpm_cmd_params.num_turns = 128;

  printf("Will acquire %d turns of data per measurement.\n", dp->cbpm_cmd_params.num_turns);

  //  No triggering
  dp->cbpm_cmd_params.use_data_enable = CBI_FALSE;

  // Instruct the instrument to continuously refresh all 
  // bunches as quickly as possible in its internal memory.
  printf("Continuous acquisition mode is enabled.\n");
  

  ped_sub = CBI_TRUE;
  scale   = CBI_TRUE;

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
  dp->cbpm_cmd_params.gain_cal  = CBI_TRUE;


  printf("Processed data buffer index will be reset to 0 for this command.\n");
  dp->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;


  if (use_enable_gate) {
    return CBI_F_SUCCESS_W_GATE;
  } else {
    return CBI_F_SUCCESS_NO_GATE;
  }

}




int cbi_exit_currmon_mode( void ) {

  int input = CBI_EMPTY_INPUT;

  while (TRUE) {
    input = cbi_get_char_local();
    if ((char)input == 'q') {
      continuous_mode = FALSE;
      break;
    }
  }

  return CBI_F_SUCCESS;
}





int cbpm_proc_data_currmon_post( int iidx ) {

  char *func_name = (char *)__FUNCTION__;

  int debug_level = CTL_System.debug_verbosity;
  
  int status = CBI_F_SUCCESS;

  //--------------------------------------------------------------
  // Also install the SIGPIPE signal handler when entering
  // continuous polling mode to allow for a semi-graceful
  // recovery from any potential socket/pipe errors that appear.
  //--------------------------------------------------------------
  if (signal (SIGPIPE, continuous_mode_sigpipe_handler) == SIG_IGN) {
    signal (SIGPIPE, SIG_IGN);
  }
  
  // Create thread to safely allow keyboard input without affecting CBInet operations
  // in progress.
  pthread_t  INPUT_thread;
  status = pthread_create( &(INPUT_thread), NULL, (void *)&(cbi_exit_currmon_mode), NULL );
  if (debug_level > CBI_DEBUG_LEVEL_1) {
    printf("INPUT_thread stat = %d\n", status);
  }

  int species = CBI_UNINITIALIZED_PARAM; 
  status = cbpm_do_current_monitor_loop(iidx, false, &species);
  
  //--------------------------------------------------------------
  // Disable current monitor-specific trapping of SIGPIPEs
  //--------------------------------------------------------------
  if (signal (SIGPIPE, std_sigpipe_handler) == SIG_IGN) {
    signal (SIGPIPE, SIG_IGN);
  }
  

  printf("\n\nPlease wait (3 sec)...");
  fflush(stdout);
  sleep(3);
  

  return CBI_F_SUCCESS;

}


int cbpm_proc_data_cal_currmon_post( int iidx ) {

  char *func_name = (char *)__FUNCTION__;
  
  int debug_level = CTL_System.debug_verbosity;

  CBPM_DATA          *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;  

  int status = CBI_F_SUCCESS;
  int species = CBI_UNINITIALIZED_PARAM; 
  char input;
  int stop_now = CBI_FALSE;

  continuous_mode = FALSE;
  cbpm_currmon_server = CBI_TRUE;

  int interactive_input = CBI_TRUE;

  float tolerance = 0.1;
  float currents[] =        {0.5, 0.6, 1.0, 1.1}; //The list of current to set machine to
  int gains[] =             {  5,   5,   5,   5}; //The gains associated with each current
  char measurement_mode[] = {'s', 'm', 's', 'm'}; //s corresponds to SETTING the gain, 
  //												m corresponds to measuring how good that the set value is
  int num_currents = sizeof(gains)/sizeof(int);

  int simulated = 1;
  
  //If Simulated = 1,
  //then simulate conditions to calibrate current monitor.
  //this is used to develop and test the function, when the machine is not available.
  //So the "simulated" arrays represent what the function would measure at each step of sim_time
  
  float simulated_cern[] =    {  0.50,  0.6,  2.0,  1.00,  1.1,  0.50,  0.60,  1.0,  1.10};
  float simulated_species[] = {  0.55,  0.7,  2.0,  1.05,  1.3,  0.45,  0.40,  0.95, 0.70};
  int   simulated_gain[] =    {     5,    5,    5,     5,    5,     5,     5,    5,     6};
  char  simulated_input[] =   {   'e',  'e',  'e',   'e',  'e',   'e',   'e',  'e',   'e'};
  int  sim_time = -1;
  int leng_of_sim = sizeof(simulated_cern)/sizeof(float);

  char* species_str[] = {"Positrons", "Electrons"};

  printf("\n");
  int card, tblock, curr_ind;
  float old_gain, new_gain;

  float cern, species_curr;
  int current_valid = CBI_TRUE;
  float target_current;

  for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
    
    for (curr_ind = 0; curr_ind< num_currents; curr_ind++) {
      
      target_current = currents[curr_ind];
      if (curr_ind == 0) {
	switch(tblock) {
	case (0) :
	  printf("Please put %5.2f mA of %s in ring.\n", target_current, species_str[0]);
	  break;
	case (1) :
	  if (stop_now ) break;
	  printf("Now dump the beam and inject %5.2f mA of %s.\n", target_current, species_str[1]);
	  break;
	}
      }
      else
	{
	  printf("Please increase the current to %5.2f mA of %s in ring.\n", target_current, species_str[tblock]);
	}
      
      
      if (!stop_now) {
	printf("When ready, press enter or any key, press q to abort> ");
	
	if (interactive_input) {
	  input = cbi_get_char();
	}
	
	if (simulated) {
	  sim_time++;
	  if (sim_time == leng_of_sim) {
	    stop_now = CBI_TRUE;
	    break;
	  }
	  
	  if (!interactive_input) {
	    input = simulated_input[sim_time];
	  }
	}
	
	printf("\n");
	if (input == 'q') {
	  stop_now = CBI_TRUE;
	  break;
	}
      }
      else break;
      
      
      current_valid = CBI_FALSE;
      do {
	//Now take measurement
	//status = cbpm_do_current_monitor_loop(iidx, true, &species);
	//printf("\n");
	
	if (simulated) dp->cbpm_op_gain.active_gain_settings[0][0] = simulated_gain[sim_time];
	printf("\n");
	
	
	//Read back CERN current
	cern = get_cern_current();
	if (simulated) cern = simulated_cern[sim_time];
	
	printf("Cern readback:    %5.2f mA\n", cern);
	
	//Test to see if have desired current
	int percentage = abs(target_current-cern)/cern;
	if (percentage <= tolerance) {
	  current_valid = CBI_TRUE;
	}
	else {
	  printf("\n*** Warning, the current requested was %5.2f mA and the CERN current read back %5.2f mA. \n", target_current, cern);
	  
	  if (cern < target_current) {
	    printf("Please inject up to %5.2f mA.\n", target_current);
	  }
	  else
	    printf("Please dump and inject to %5.2f mA.\n", target_current);
	  
	  printf("When ready, press enter or any key, press q to abort> ");
	  
	  if (interactive_input) {
	    input = cbi_get_char();
	  }
	  
	  if (simulated) {
	    sim_time++;
	    if (sim_time == leng_of_sim) {
	      stop_now = CBI_TRUE;
	      break;
	    }
	    
	    if (!interactive_input) {
	      input = simulated_input[sim_time];
	    }
	  }
	  
	  
	  printf("\n");
	  if (input == 'q') {
	    stop_now = CBI_TRUE;
	    break;
	  }
	  
	}
	
      } while (!current_valid && !stop_now);
      if (stop_now) break;
      
      //Read back from computed current from Current Monitor
      species_curr = get_species_current(species);
      //species_curr = cern / 1.1; //REMOVE LATER
      if (simulated) species_curr = simulated_species[sim_time];
      printf("Species readback: %5.2f mA\n", species_curr);
      
      //Determine current gain setting
      
      int gain_setting = dp->cbpm_op_gain.active_gain_settings[0][0];
      printf("Gain readback:     %i, gain expected: %i\n", gain_setting, gains[curr_ind]);
      if (gain_setting != gains[curr_ind] ) {
	printf("\n***** Error, the gain setting for this current should be %i, and it auto set to %i. \n", gains[curr_ind], gain_setting);
	printf("Aborting now... \n\n");
	stop_now = CBI_TRUE;
      }
      if (stop_now) break;
      
      if (measurement_mode[curr_ind] == 's') {
	float computed_new_gain_factor = cern/species_curr;
	printf("\n");
	for (card = 0; card < CBPM_MAX_CARDS; card++ ) {
	  old_gain = dp->cbpm_gain_config.gain_table[tblock][card][gain_setting];
	  new_gain = old_gain * computed_new_gain_factor;
	  
	  dp->cbpm_gain_config.gain_table[tblock][card][gain_setting] = new_gain;
	  printf("%i) Old gain: %5.3f, New gain: %5.3f\n", card, old_gain, new_gain);
	  
	  //TODO save gains back to instrument
	}
	printf("\n");
      }
      else {
	printf("Current error:\t\t\t\t %5.3f\n\n", cern-species_curr);
      }
    }
  }
  
  cbpm_save_gains_to_temp(iidx);
  
  return CBI_F_SUCCESS;
  
}


int cbpm_save_gains_to_temp(int iidx) {
  
  char *func_name = (char *)__FUNCTION__;

  CBPM_DATA          *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;  

  char* Name = CTL_System.p_Module[iidx]->det.name;
	
  int status = CBI_F_SUCCESS;

  FILE *fp_out;
  char filename[CBI_MAX_STRING_LENGTH]   = "";
  char full_fname[CBI_MAX_STRING_LENGTH] = "";	
	
  fp_out = cbi_managed_file_open( filename, full_fname, (int)NULL, PARAMS_DIR, PARAMETERS, CBI_SINGLE_FILE, FALSE );
  if (fp_out == NULL) {
    sprintf(message, "Open of instrument parameters file failed!");
    log_message( S_ERROR, func_name, message );
    return CBI_F_FAILURE;
  }
  
  cbi_write_param_file_header( fp_out, CTL_System.output_file_index, CBI_FALSE ); 

  printf("Name: %s\n", Name);
  printf("Filename  : %s\n", filename);
  printf("Full name : %s\n", full_fname);

  // Section open
  printf("Accessing file section >%s<\n", Name);
  faccess( FA_WRITE, fp_out, Name, NULL, 0, 0, 0, NULL);
  
  // Write gain table to intermediate file.
  status = faccess(FA_WRITE, fp_out, Name, "GAIN_TABLE_0", 4, CBPM_MAX_GAINS, FLOAT, dp->cbpm_gain_config.gain_table[0] );
  printf("write status = %d\n", status);
  status = faccess(FA_WRITE, fp_out, Name, "GAIN_TABLE_1", 4, CBPM_MAX_GAINS, FLOAT, dp->cbpm_gain_config.gain_table[1] );
  printf("write status = %d\n", status);
   
  printf("Stopping faccess() session...\n");
  faccess( FA_WRITE, fp_out, NULL, NULL, 0, 0, 0, NULL);

  if( fclose(fp_out) != 0) {
    perror("fclose: ");
  }      

  return status;
}


int cbpm_do_current_monitor_loop(int iidx, int calibrating, int* species) {
	
  char *func_name = (char *)__FUNCTION__;

  int debug_level = CTL_System.debug_verbosity;
  
  CBPM_DATA          *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  // For MPM addressing
  int  status, ele[2];
    
  cbi_copy_touched_structs( iidx, TOT_CBPM_COMM_STRUCTS );
  cbi_init_command( iidx, CURRENT_DATA_CMD );
  cbi_execute_command( iidx );
  int tot_bunches;

  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CMD_PARAMS_TAG,       CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_RAW_DATA_HEADER_TAG,  CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  tot_bunches = dp->cbpm_raw_data_header.tot_bunches;
  printf("Number of bunches requested = %d\n\n", tot_bunches );

  int counter = 0;

  int override  = FALSE;
  int first_pass = TRUE;

  cbpm_print_cmd_params_struct(iidx, stdout);

  continuous_mode = TRUE;
  
  
  // --- Status handling ---
  // Write current monitor server instance ID # to injection status/control MPM node.
  int currmon_ID = 100;  // FIXME centralize
  ele[0] = ele[1] = CBI_CURRMON_SERVER_ID_ELE;
  status = vmputn_c( CBI_INJ_CONTROL_NODE, ele[0], ele[1], &currmon_ID );


  int inj_handshake;
  int timing_mode = 4, timing_mode_request;
  int species_request;
  int old_gain_setting = dp->cbpm_op_gain.active_gain_settings[0][0];

  // Publish the species and bunch timing_mode indicators
  int timing_setup = dp->cbpm_op_timing.active_timing_setup;

  char *destination_node;

  int integrity_good;
  int tblock, card;
  int chan = 0; // for BPMs
  int retval;

  time_t      curtime;
  curtime = time(NULL);
  struct tm  *loctime;
  loctime = localtime(&curtime);

  //Wait for 3 seconds after starting the Instrument monitoring loop
  sleep(3);
	
  //----------------------------------------------
  // Drop into infinite loop of reading back the
  // sum-signal buffer rapidly...
  //----------------------------------------------
  do {

    //------------------------
    //  Control handling
    //------------------------
    // Obtain the timing_mode and species

    ele[0] = ele[1] = CBI_CURRMON_TIMING_MODE_ELE;
    status = vmgetn_c( CBI_INJ_CONTROL_NODE, ele[0], ele[1], &timing_mode_request );

    ele[0] = ele[1] = CBI_CURRMON_SPECIES_ELE;
    status = vmgetn_c( CBI_INJ_CONTROL_NODE, ele[0], ele[1], &species_request );

    // Only ever need to take action based on the species changing.  Timing mode 
    if (species_request != species) {

      if ( (species_request != POSITRONS) && (species_request != ELECTRONS) ) {
	sprintf(message, "Recieved invalid species request [%d] from injection software!  Exiting.", species_request );
	log_message( S_INFO, func_name, message );
	continuous_mode = CBI_FALSE;
	continue;
      }
      species       = species_request;

      dp->cbpm_op_timing.active_timing_setup = cbpm_currmon_modespec_to_tsetup( timing_mode, species );

      if (first_pass || !override) {

	first_pass = FALSE;

	//--------------------------------------------------------------
	// Stop current measurement.  Load new timing/species settings 
	// with locally cached values.  Restart current measurement.
	// Populate the CMD structure with the stopping condition and
	// send to instrument to break remote loop.
	//--------------------------------------------------------------
	printf("***********************************************\n");
	printf(" Changing configuration as per                 \n");
	printf("                   injection software request. \n");
	printf("   %s   \n", asctime(loctime) );
	printf("***********************************************\n");
	CTL_System.p_Module[iidx]->core->cbi_cmd.cmd       = CBI_IDLE_CMD;
	CTL_System.p_Module[iidx]->core->cbi_cmd.handshake = CBI_CMD_NEW_HANDSHAKE;
	
	status = cbi_gp_struct(WRITE, CTL_System.comm_method, CBI_CMD_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
	// Wait for instrument to drop out of its meaasurement loop.
	cbi_wait_for_command( iidx );
	printf("***********************************************\n");
	printf("  Waiting for measurement to halt..."); 
	sleep(1);
	printf("Done.\n");
	
	// Determine if the instrument's heartbeat is active before
	// dispatching the setup command.
	if ( cbi_check_heartbeat(iidx) ) {
	  
	  printf("***********************************************\n");
	  chan = 0; // For BPMs.
	  printf("                 Tblock_0    Tblock_1\n");
	  printf(" Signal Input : 0  1  2  3  0  1  2  3 \n");
	  printf("----------------------------------------\n");
	  printf(" Gain Setting : ");
	  for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
	    for (card = 0; card < CBPM_MAX_CARDS; card++) {
	      printf("%d  ", appconfig.default_gain_setting );
	      dp->cbpm_op_gain.active_gain_settings[card][tblock] = appconfig.default_gain_setting;
	    }
	  }
	  printf("\n");
	  cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_OP_GAIN_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
	  cbi_init_command( iidx, SET_GAINS_CMD );
	  cbi_execute_command( iidx );
	  cbi_wait_for_command( iidx );
	  cbpm_set_gains_post(iidx);
	  
	  printf("***********************************************\n");
	  cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_OP_TIMING_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
	  printf("  Setting timing setup to  %s                  \n",
		 tsetup_names[ dp->cbpm_op_timing.active_timing_setup ] );
	  printf("***********************************************\n");
	  CTL_System.p_Module[iidx]->core->cbi_cmd.cmd       = SET_TIMING_SETUP_CMD;
	  CTL_System.p_Module[iidx]->core->cbi_cmd.handshake = CBI_CMD_NEW_HANDSHAKE;
	  status = cbi_gp_struct(WRITE, CTL_System.comm_method, CBI_CMD_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
	  // Wait for instrument to drop out of its meaasurement loop.
	  cbi_wait_for_command( iidx );
	  cbpm_set_timing_setup_post(iidx);
	  
	  printf("***********************************************\n");
	  printf("REMINDER: Current monitor has an upper gain \n");
	  printf("          setting limit of  %d .\n", CBPM_MAX_CURRMON_GAIN_SETTING );
	  printf("GAIN SETTING = %d\n", dp->cbpm_op_gain.active_gain_settings[0][0]);
	  printf("***********************************************\n\n\n");
	  printf("***********************************************\n");
	  printf(" 4ns Bunch-by-bunch current monitor running... \n");
	  printf("***********************************************\n");
	  if (species == POSITRONS) {
	    destination_node = (char *)(&(CBI_POS_CURRMON_NODE));
	  } else {
	    destination_node = (char *)(&(CBI_ELE_CURRMON_NODE));
	  }
	  printf("\nType 'q' and then <ENTER> to exit current monitor mode: \n");
	  printf("Write counter (in \'%s\' element 1282):       ", destination_node);
	  fflush(stdout);
	  
	  // FIXME: TODO: Wipe the unsigned current array in the database for the species
	  // this has switched FROM.
	
	  
	} else {
	  
	  printf("ERROR: Instrument heartbeat not present!\n        Unable to change configuration.\n");
	  
	}

	// Display gain setting changes as they occur
	cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_GAIN_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
	if (dp->cbpm_op_gain.active_gain_settings[0][0] != old_gain_setting) {
	  printf("\n\nEnter 'q' to exit current monitor mode: \n");
	  sprintf(message, "Gain setting change: %d --> %d", old_gain_setting, dp->cbpm_op_gain.active_gain_settings[0][0]);
	  log_message(S_DINFO, func_name, message);
	  old_gain_setting = dp->cbpm_op_gain.active_gain_settings[0][0];
	}
	
	//---------------------------------
	// Restart the current monitoring
	//---------------------------------
	cbi_init_command( iidx, CURRENT_DATA_CMD );
	cbi_execute_command( iidx );
	

      } // endIF (first_pass || !override)
      
      // Display gain setting changes as they occur
      cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_GAIN_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
      if (dp->cbpm_op_gain.active_gain_settings[0][0] != old_gain_setting) {
	printf("\n\nEnter 'q' to exit current monitor mode: \n");
	sprintf(message, "Gain setting change: %d --> %d", old_gain_setting, dp->cbpm_op_gain.active_gain_settings[0][0]);
	log_message(S_DINFO, func_name, message);
	old_gain_setting = dp->cbpm_op_gain.active_gain_settings[0][0];
      }

    } // endIF   species_request   timing_mode_request



    //------------------------
    //  Status handling 
    //------------------------
    status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_SUM_DAT_BUF_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
    fflush(stdout);

    ////////////////////   DEBUGGING 
    ///////printf("  cbpm_proc_sum_dat_buf.fill_count = %d\n", dp->cbpm_proc_sum_dat_buf.fill_count );
    ////////////////////  /DEBUGGING

    retval = cbpm_currents2db( iidx );

    // Set handshake with injection software only if the database writes in the call above succeeded.
    if (retval == CBI_F_SUCCESS) {
      ele[0] = ele[1] = CBI_CURRMON_HANDSHAKE_ELE;
      status = vmgetn_c( CBI_INJ_CONTROL_NODE, ele[0], ele[1], &inj_handshake );
      if (inj_handshake != CBI_CURRMON_HANDSHAKE) {
	inj_handshake = CBI_CURRMON_HANDSHAKE;
	status = vmputn_c( CBI_INJ_CONTROL_NODE, ele[0], ele[1], &inj_handshake );
      }
    } else {
      printf("ERROR: Unable to write current measurement data or write counter to database.\n");
    }
    
    if (debug_level > CBI_DEBUG_LEVEL_2) {
      status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_RAW_DATA_HEADER_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
      printf("LOW        zero bitfields: 0x%x   0x%x   0x%x   0x%x  \n",
             dp->cbpm_raw_data_header.ADC_zero_val_bitfields[0],
             dp->cbpm_raw_data_header.ADC_zero_val_bitfields[1],
             dp->cbpm_raw_data_header.ADC_zero_val_bitfields[2],
             dp->cbpm_raw_data_header.ADC_zero_val_bitfields[3] );
      printf("LOWER      thresh bitfields: 0x%x   0x%x   0x%x   0x%x  \n",
             dp->cbpm_raw_data_header.ADC_lower_thresh_bitfields[0],
             dp->cbpm_raw_data_header.ADC_lower_thresh_bitfields[1],
             dp->cbpm_raw_data_header.ADC_lower_thresh_bitfields[2],
             dp->cbpm_raw_data_header.ADC_lower_thresh_bitfields[3] );
      printf("LOW        thresh bitfields: 0x%x   0x%x   0x%x   0x%x  \n",
             dp->cbpm_raw_data_header.ADC_low_thresh_bitfields[0],
             dp->cbpm_raw_data_header.ADC_low_thresh_bitfields[1],
             dp->cbpm_raw_data_header.ADC_low_thresh_bitfields[2],
             dp->cbpm_raw_data_header.ADC_low_thresh_bitfields[3] );
      printf("HIGH       thresh bitfields: 0x%x   0x%x   0x%x   0x%x  \n",
             dp->cbpm_raw_data_header.ADC_high_thresh_bitfields[0],
             dp->cbpm_raw_data_header.ADC_high_thresh_bitfields[1],
             dp->cbpm_raw_data_header.ADC_high_thresh_bitfields[2],
             dp->cbpm_raw_data_header.ADC_high_thresh_bitfields[3] );
      printf("SATURATION thresh bitfields: 0x%x   0x%x   0x%x   0x%x  \n",
             dp->cbpm_raw_data_header.ADC_saturation_bitfields[0],
             dp->cbpm_raw_data_header.ADC_saturation_bitfields[1],
             dp->cbpm_raw_data_header.ADC_saturation_bitfields[2],
             dp->cbpm_raw_data_header.ADC_saturation_bitfields[3] );
    }
    cbi_sleep(100);

  } while(continuous_mode); //endWHILE continuous_mode

  // Re-enable cursor
  printf("\033[?25h");
  
  //--------------------------------------------------------------
  // Readback loop was terminated locally, populate the
  // CMD structure with the stopping condition and
  // send to instrument to break remote loop.
  //--------------------------------------------------------------
  CTL_System.p_Module[iidx]->core->cbi_cmd.cmd       = CBI_IDLE_CMD;
  CTL_System.p_Module[iidx]->core->cbi_cmd.handshake = CBI_CMD_NEW_HANDSHAKE;
  
  status = cbi_gp_struct(WRITE, CTL_System.comm_method, CBI_CMD_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  
}
