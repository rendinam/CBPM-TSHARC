/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_proc_data.c                                      |
*                                                                       |
* Description  :  Sets up all the necessary structures for performing a |
*                 processed data acquisition and stores the             |
*                 configuration in a central, temporary master struct.  |
*                                                                       |
* Arguments    :                                                        |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"

int timing_adjust   = FALSE;

int cbpm_proc_data_prep(void) {
  
  char input;
  int  retval;

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;


  char text_type[7] = "PROMPT\0";
  char infostring[80];
  strcpy( infostring, "");
  strcpy( infostring, "********* PROCESSED DATA *********\n\0" );
  if (CTL_System.RunningMode == CBI_BACKEND_MODE) {
    if(send(CTL_System.mgmt_socket_IDs[0], text_type, strlen(text_type)+1, 0) == -1) {
      perror("send() error");
    }
    sleep(1);
    if(send(CTL_System.mgmt_socket_IDs[0], infostring, strlen(infostring)+1, 0) == -1) {
      perror("send() error");
    }
  } else {
    printf("%s", infostring);
  }

  int tsetup;
  tsetup = cbpm_measurement_tsetup();
  cbpm_query_species( tsetup );


  // Set bunch pattern ------------------------------------------
  //-------------------------------------------------------------
  // TODO: If this is being used to set up an orbit acquisition,
  // query the bunch to examine in terms of SPECIES/TRAIN/BUNCH,
  // otherwise, just ask for raw bunch bucket number(s) to 
  // describe a bunch pattern.
  //-------------------------------------------------------------
  cbpm_query_bunches(CTL_System.meas_info.p_bunch_pattern);


  // Initialize flags
  int use_enable_gate = FALSE;
  int ped_sub         = FALSE;
  int scale           = FALSE;
  int jitter_suppress = FALSE;
  
  use_enable_gate = cbpm_query_trigger(dp);

  
  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(6);
  
  prompts[0].supp_text_ptr    = NULL;
  prompts[0].prompt_text      = "Number of turns to acquire: ";
  prompts[0].data_type        = CBI_INT;
  prompts[0].valid_inputs     = NULL;
  prompts[0].map_out          = NULL;
  prompts[0].default_value    = 1024;
  prompts[0].destination      = &(dp->cbpm_cmd_params.num_turns);
  prompts[0].last_val_default = TRUE;
  
  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {TRUE, TRUE, FALSE, FALSE};

  prompts[1].supp_text_ptr    = NULL;
  prompts[1].prompt_text      = "Pedstal-subtract data [y/n]? ";
  prompts[1].data_type        = CBI_CHAR;
  prompts[1].valid_inputs     = valid_inputs;
  prompts[1].map_out          = map_out;
  prompts[1].default_value    = 'y';
  prompts[1].destination      = &(ped_sub);
  prompts[1].last_val_default = TRUE;

  prompts[2].supp_text_ptr    = NULL;
  prompts[2].prompt_text      = "Gain-scale data [y/n]? ";
  prompts[2].data_type        = CBI_CHAR;
  prompts[2].valid_inputs     = valid_inputs;
  prompts[2].map_out          = map_out;
  prompts[2].default_value    = 'y';
  prompts[2].destination      = &(scale);
  prompts[2].last_val_default = TRUE;

  prompts[3].supp_text_ptr    = NULL;
  prompts[3].prompt_text      = "Use JITTER SUPPRESSION calculation method [y/n]? ";
  prompts[3].data_type        = CBI_CHAR;
  prompts[3].valid_inputs     = valid_inputs;
  prompts[3].map_out          = map_out;
  prompts[3].default_value    = 'y';
  prompts[3].destination      = &(jitter_suppress);
  prompts[3].last_val_default = TRUE;

  prompts[4].supp_text_ptr    = NULL;
  prompts[4].prompt_text      = "Perform an automatic TIMING adjustment prior to acquiring [y/n]? ";
  prompts[4].data_type        = CBI_CHAR;
  prompts[4].valid_inputs     = valid_inputs;
  prompts[4].map_out          = map_out;
  prompts[4].default_value    = 'n';
  prompts[4].destination      = &(timing_adjust);
  prompts[4].last_val_default = TRUE;

  prompts[5].supp_text_ptr    = NULL;
  prompts[5].prompt_text      = "Perform a GAIN adjustment prior to acquiring   [y/n]? ";
  prompts[5].data_type        = CBI_CHAR;
  prompts[5].valid_inputs     = valid_inputs;
  prompts[5].map_out          = map_out;
  prompts[5].default_value    = 'n';
  prompts[5].destination      = &(dp->cbpm_cmd_params.gain_cal);
  prompts[5].last_val_default = TRUE;



  retval = CBI_PROMPTER();

  if (enable_time_scan_tests() && timing_adjust) {
    cbpm_time_scan_prompt_Time_Scan_Protection();
  }

  printf("Num turns = %d\n", dp->cbpm_cmd_params.num_turns );
  printf("ped sub   = %d\n", ped_sub );
  printf("g scale   = %d\n", scale );


  // Honor timing adjustment response
  if (timing_adjust) {
    dp->cbpm_cmd_params.delay_cal                  = CBI_TRUE;
    dp->cbpm_cmd_params.generate_delay_corrections = CBI_TRUE;
  } else {
    dp->cbpm_cmd_params.delay_cal = CBI_FALSE;
  }

  // Honor jitter suppression response
  dp->cbpm_cmd_params.avg_mode = PROC_SIMPLE_AVG_MODE;
  if (jitter_suppress) {
    dp->cbpm_cmd_params.avg_mode = PROC_JITTER_SUPPRESSION_MODE;
  }
  
  // Honor data correction response
  if (ped_sub) {
    dp->cbpm_cmd_params.scale_mode = PED_SUB_DATA;
  }
  if (scale) {
    dp->cbpm_cmd_params.scale_mode = SCALED_DATA;
  }
  if (ped_sub && scale) {
    dp->cbpm_cmd_params.scale_mode = PED_SUB_SCALED_DATA;
  }


  printf("Processed data buffer index will be reset to 0 for this command.\n");
  dp->cbpm_cmd_params.reset_proc_buf_idx = TRUE;


  // Set the maximum number of hanshake completion checks to make.
  // Only manifests if there is a problem that causes a handshake timeout.
  if (~timing_adjust) {
    cbi_set_handshake_tries( 80 );
  } else {
    cbi_set_handshake_tries( 120 );
  }


  printf("use_enable_gate = %d\n", use_enable_gate);

  if (use_enable_gate) {
    return CBI_F_SUCCESS_W_GATE;
  } else {
    return CBI_F_SUCCESS_NO_GATE;
  }

}




//--------------------------------------------------------------------
// Function to load command parameters harvested from the CESR MPM
// interface array and loaded into the CTL_CBPM_Intf_Params 
// management structure into the platform-specific communications 
// structure(s) needed for instructing an instrument to perform the
// acquisition.
//
// NOTE: MPM interface mode commands will only honor 14ns
//       timing setups for the 'PT' 
//  (Position monitor, TigerSHARC-based) platform..
//--------------------------------------------------------------------
int cbpm_proc_data_prep_from_iface(void) {
  
  char *func_name = (char *)__FUNCTION__;

  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  int use_enable_gate = TRUE;
  int turns;
  int BID, train, bunch, bucket;

  // Copy parameters from CESR MPM interface into appropriate
  // command parameter members.

  // CESR MPM Interface array provided paramters ------
  dp->cbpm_cmd_params.species            = CTL_CBPM_Intf_Params.species;

  //----------------------------------------------------------
  // Determine which buckets to enable for sampling based on 
  // the train/bunch and species values provided by the 
  // requesting application
  //----------------------------------------------------------
  BID = CTL_CBPM_Intf_Params.bunch_id[0];  // FIXME:  HARDCODED to 0 slot?
  cbi_clear_bp( dp->cbpm_cmd_params.bunch_pat );
  train = cbi_bid2train( BID );
  bunch = cbi_bid2bunch( BID );
  printf("Externally-provided train # = %d\n", train );
  printf("Externally-provided bunch # = %d\n", bunch );

  // FIXME: make tblock switchable
  bucket = (cbpm_TB_to_BPslot(train, bunch) * 2) + dp->cbpm_cmd_params.species;
  printf("              CBIC bucket # = %d\n", bucket );

  cbi_set_bunch( dp->cbpm_cmd_params.bunch_pat, bucket+1 );

  // Set the number of turns to acquire
  if ( (CTL_CBPM_Intf_Params.num_turns == CBI_UNINITIALIZED_PARAM) ||
       (CTL_CBPM_Intf_Params.num_turns < 1) || 
       (CTL_CBPM_Intf_Params.num_turns > CBPM_ADC_BUF_USE) ) {
    dp->cbpm_cmd_params.num_turns = 1024; 
  } else {
    dp->cbpm_cmd_params.num_turns = CTL_CBPM_Intf_Params.num_turns;
  }


  // Non-interface-provided parameters -------
  dp->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
  dp->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
  dp->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA;
  dp->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
  dp->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
  dp->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
  dp->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

  printf("Processed data buffer index will be reset to 0 for this command.\n");
  dp->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;


  // Set the maximum number of hanshake completion checks to make.
  // Only manifests if there is a problem that causes a handshake timeout.
  cbi_set_handshake_tries( 40 );

  return CBI_F_SUCCESS_W_GATE;

}





int cbpm_proc_data_post(int iidx) {

  FILE *fp;
  char filename[CBI_MAX_STRING_LENGTH]   = "";
  char full_fname[CBI_MAX_STRING_LENGTH] = "";


  int debug_level = CTL_System.debug_verbosity;

  //-------------------------------------------------------------------
  // Set to TRUE for PROC file writing for server-mode proc requests.
  //-------------------------------------------------------------------
  int write_proc_file = FALSE;


  //// Only write data file if NOT in server (control system interface) mode.
  ////   - or - if PROC file writing is explicitly requested by above variable.
  //// For CLOCK CHARACTERIZATION / ORBIT STABILITY WORK [instance (1/3)] ////
  if (write_proc_file == TRUE || CTL_System.InterfaceState != CBI_CTL_INTF_ENABLED) {
    fp = cbi_managed_file_open( filename, full_fname, iidx, PROC_DIR, PROC, CBI_SINGLE_FILE, FALSE );
    if (fp == NULL) {
      printf("Skipping post call due to file access error!\n");
      return CBI_F_FAILURE;
    }
  }
  //// Force true for CLOCK CHARACTERIZATION / ORBIT STABILITY  WORK ////


  int k;
  int entry;
  int tot_bunches;
  int chan = 0; // Only a single channel per timing block per card for BPMs.

  CBPM_DATA  *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  // Command is done.  Read back status and payload
  // Read back supplementary data only if in the appropriate debug level.
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TEMPERATURES_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  if (debug_level > CBI_DEBUG_LEVEL_2) {

    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CMD_PARAMS_TAG,      CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
  }

  // TODO: For now, every post call for a measurement that ultimately writes a data file will need a call to get
  //       the temperatures before producing that file.  Put this call to get needed file header values in a more
  //       central location.
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TEMPERATURES_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }

  // Essential measurement data
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_RAW_DATA_HEADER_TAG,  CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }

  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_BUF_HEADER_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_BUF_TAG,        CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }

  if (dp->cbpm_cmd_params.delay_cal && enable_time_scan_tests()) {
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TEST_RESULTS_TAG,    CTL_System.p_Module[iidx], CBI_FULL_STRUCT);	  
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_BTEST_RESULTS_TAG,    CTL_System.p_Module[iidx], CBI_FULL_STRUCT);	  
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
  }
  

  tot_bunches = dp->cbpm_raw_data_header.tot_bunches;

  //printf("Number of bunches requested  = %d\n", tot_bunches );
  //printf("Wrote to proc buf entry slot = %d\n\n", dp->cbpm_proc_buf_header.write_ptr[0] );

  int active_buf = dp->cbpm_proc_buf_header.active_buf;

  entry = 0;


  //// For CLOCK CHARACTERIZATION / ORBIT STABILITY  WORK [instance (2/3)] ////
  if (write_proc_file == TRUE || CTL_System.InterfaceState != CBI_CTL_INTF_ENABLED) {

    RDV3_INST_DATA hdata;
    cbpm_prep_instrument_header( &hdata, iidx );
    cbpmfio_rw_instrument_header(fp, CBPMFIO_WRITE);

    cbpm_print_proc_buf_entries( 0, iidx,  entry,  entry + (tot_bunches-1),  fp ); // Regular proc data acquisitions

    fclose(fp);
  }
  //// Force true for CLOCK CHARACTERIZATION WORK ////
  

  if (debug_level > CBI_DEBUG_LEVEL_2) {
    cbpm_print_cmd_params_struct(iidx, stdout);
  }


  if (debug_level > CBI_DEBUG_LEVEL_2) {
    // TESTING: Print out the threshold bitfields from the raw buf header for this acquisition
    // Don't enable gain calibration when testing this to prevent obfuscation of results.
    // FIXME: Move to stream_io routine.  Print to console and log to data file for each measurement.
    printf("LOW        zero bitfields: 0x%x   0x%x   0x%x   0x%x  \n",
       dp->cbpm_raw_data_header.ADC_zero_val_bitfields[0],
       dp->cbpm_raw_data_header.ADC_zero_val_bitfields[1],
       dp->cbpm_raw_data_header.ADC_zero_val_bitfields[2],
       dp->cbpm_raw_data_header.ADC_zero_val_bitfields[3] );
    printf("LOWER    thresh bitfields: 0x%x   0x%x   0x%x   0x%x  \n",
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

  ///// For CHARACTERIZATION / ORBIT STABILITY  WORK [instance (3/3)] ///
  if (write_proc_file == TRUE || CTL_System.InterfaceState != CBI_CTL_INTF_ENABLED) {
    printf("Full filename = %s\n", full_fname );
  }
  ///// CHARACTERIZATION WORK ///


  return CBI_F_SUCCESS;

}





// Post function used when a 3rd-party orbit measurement is requested.
// Does all a normal processed data acquisition does and then publishes
// results to the CESR database.
int cbpm_proc_data_post_publish(int iidx) {

  int retval;

  retval = cbpm_proc_data_post( iidx );

  if (retval == CBI_F_SUCCESS) {
    retval = cbpm_buttons2db( iidx );
  }

  return retval;

}



int cbpm_proc_data_final(void) {

  int retval, iidx;

  if (CTL_System.InterfaceState == CBI_CTL_INTF_ENABLED) {
    for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
      if (CTL_System.p_Module[iidx]->active == CBI_ACTIVE) {
	retval = cbpm_buttons2db( iidx );
      }
    }
  }

  if (timing_adjust && enable_time_scan_tests()) {
    cbpm_time_scan_summary(1); // in post, read in CBPM_BTEST_RESULTS_TAG struct. this function needs it.
  }
  
  cbpm_print_turn_count_summary();

  return CBI_F_SUCCESS;

}

