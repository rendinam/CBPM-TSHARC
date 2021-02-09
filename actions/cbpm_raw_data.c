/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_raw_data.c                                       |
*                                                                       |
* Description  :  Routines employed in setting up and executing a raw   |
*                 (turn-by-turn) BPM data acquisition.                  |
*                                                                       |
* Arguments    :  None                                                  |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"

int timing_adjust_for_raw   = FALSE;

int cbpm_raw_data_prep(void) {

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  char input, retval;

  printf("********* RAW DATA *********\n");

  cbpm_query_pattern_type(CTL_System.meas_info.p_bunch_pattern);

  // Determine if the trigger mask is needed
  int use_enable_gate = CBI_FALSE;
  use_enable_gate = cbpm_query_trigger(dp);


  printf("Will provide PEDESTAL-SUBTRACTED and GAIN-SCALED data.\n");
  dp->cbpm_cmd_params.scale_mode = PED_SUB_SCALED_DATA;


  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(2);

  int range[2] = {1, 320000};
  prompts[0].supp_text_ptr    = NULL;
  prompts[0].prompt_text      = "Number of turns to acquire: ";
  prompts[0].data_type        = CBI_INT;
  prompts[0].valid_inputs     = range;
  prompts[0].default_value    = 1024;
  prompts[0].destination      = &(dp->cbpm_cmd_params.num_turns);
  prompts[0].last_val_default = FALSE;


  int valid_inputs[5] = {4, (int)'y', (int)'Y', 
                            (int)'n', (int)'N'  };
  int map_out[4]      = {     TRUE,     TRUE,    FALSE,    FALSE};

  prompts[1].supp_text_ptr    = NULL;
  prompts[1].prompt_text      = "Perform an automatic TIMING adjustment as part of acquisition?  [y/n]";
  prompts[1].data_type        = CBI_CHAR;
  prompts[1].valid_inputs     = valid_inputs;
  prompts[1].map_out          = map_out;
  prompts[1].default_value    = 'n';
  prompts[1].destination      = &(timing_adjust_for_raw);
  prompts[1].last_val_default = FALSE;


  retval = CBI_PROMPTER();
  
  if (enable_time_scan_tests() && timing_adjust_for_raw) {
    cbpm_time_scan_prompt_Time_Scan_Protection();
  }  

  fflush(stdout);

  dp->cbpm_cmd_params.scale_mode = PED_SUB_SCALED_DATA;
  dp->cbpm_cmd_params.gain_cal   = TRUE;
  
  // Honor timing adjustment response
  if (timing_adjust_for_raw) {
    dp->cbpm_cmd_params.delay_cal                  = CBI_TRUE;
    dp->cbpm_cmd_params.generate_delay_corrections = CBI_TRUE;
  } else {
    dp->cbpm_cmd_params.delay_cal = CBI_FALSE;
  } 

  if (use_enable_gate == CBI_TRUE) {
    return CBI_F_SUCCESS_W_GATE;
  } else {
    return CBI_F_SUCCESS_NO_GATE;
  }

}



int cbpm_raw_data_prep_from_iface( void ) {

  char *func_name = (char *)__FUNCTION__;


  //////////////// multibunch mode local toggle /////////////////
  int multibunch = TRUE;
  ///////////////////////////////////////////////////////////////

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
  FILE *fp;
  int BP_array[4][10];
  int row, col;
  int   retcode;
  char *category   = "multibunch_for_servers";
  char  pat_idx[5] = {"1"};
  int *pattern_array = CTL_System.meas_info.p_bunch_pattern;

  if (multibunch == TRUE) {

    fp = fopen( "/nfs/cesr/online/instr/CBPM/ops/multibunch_server_pattern.cfg", "r" );
    retcode = faccess( FA_READ, fp, category, pat_idx, 4, 10, HEXINT, BP_array );
    fclose(fp);

    printf("\n Bunch Pattern Representation: \n");
    
    int i = 0;
    for ( row = 0; row < 4; row++) {
      for (col = 0; col < 10; col++) {
	printf("%x ", BP_array[row][col] );
	pattern_array[i] = BP_array[row][col];
	i++;
      }
      printf("\n");
    }
    printf("\n");
    
  } else {
    
    BID = CTL_CBPM_Intf_Params.bunch_id[0];  // FIXME:  HARDCODED to 0 slot?
    cbi_clear_bp( dp->cbpm_cmd_params.bunch_pat );

    // NEW - Copy each word of the bunch pattern array to the temporary bunch
    //       pattern.
    train = cbi_bid2train( BID );
    bunch = cbi_bid2bunch( BID );
    printf("Externally-provided train # = %d\n", train );
    printf("Externally-provided bunch # = %d\n", bunch );

    // FIXME: make tblock switchable
    bucket = (cbpm_TB_to_BPslot(train, bunch) * 2) + dp->cbpm_cmd_params.species;
    printf("              CBIC bucket # = %d\n", bucket );

    cbi_set_bunch( dp->cbpm_cmd_params.bunch_pat, bucket+1 );

  } //endIF multibunch


  // Set the number of turns to acquire
  if ( (CTL_CBPM_Intf_Params.num_turns == CBI_UNINITIALIZED_PARAM) ||
       (CTL_CBPM_Intf_Params.num_turns < 1) || 
       (CTL_CBPM_Intf_Params.num_turns > CBPM_ADC_BUF_USE) ) {
    dp->cbpm_cmd_params.num_turns = 1024; 
  } else {
    dp->cbpm_cmd_params.num_turns = CTL_CBPM_Intf_Params.num_turns;
  }

  // Non-interface-provided parameters -------
  dp->cbpm_diagnostic.sample_at_zero_crossing    = CBI_FALSE;
  dp->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
  /////// 14-Jun-2-11  Changed to "B" to get around pinger (mis?)-firing at 30Hz
  ///////              instead of 60Hz.
  //////dp->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
  dp->cbpm_cmd_params.trig_mask                  = TRIG_MASK_B;
  dp->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA;
  dp->cbpm_cmd_params.delay_cal                  = CBI_FALSE;
  dp->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
  dp->cbpm_cmd_params.gain_cal                   = CBI_TRUE;

  // Set the maximum number of hanshake completion checks to make.
  // Only manifests if there is a problem that causes a handshake timeout.
  if (~timing_adjust_for_raw) {
    cbi_set_handshake_tries( 40 );
  } else {
    cbi_set_handshake_tries( 60 );
  }


  return CBI_F_SUCCESS_W_GATE;

}





int cbpm_raw_data_post(int iidx) {

  FILE *fp;
  char filename[CBI_MAX_STRING_LENGTH]   = "";
  char full_fname[CBI_MAX_STRING_LENGTH] = "";

  int k;
  int species, num_turns, block_0_words, block_1_words;

  CBPM_DATA  *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  //------------------------------------------
  // Read back parameter updates and payload.
  //------------------------------------------
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CMD_PARAMS_TAG,      CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_FIND_DELAY_OUT_TAG,  CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_RAW_DATA_HEADER_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }
  // TODO: For now, every post call for a measurement that ultimately writes a data file will need a call to get
  //       the temperatures before producing that file.  Put this call to get needed file header values in a more
  //       central location.
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TEMPERATURES_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }
  
  if (timing_adjust_for_raw && enable_time_scan_tests()) {
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TEST_RESULTS_TAG,    CTL_System.p_Module[iidx], CBI_FULL_STRUCT);	  
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_BTEST_RESULTS_TAG,    CTL_System.p_Module[iidx], CBI_FULL_STRUCT);	  
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
  }  

  /*if (dp->cbpm_raw_data_header.num_BP_bits_wrapped[0] > 0) {
    printf("%s \n",
	   CTL_System.p_Module[iidx]->det.location );
  }*/

  num_turns     = dp->cbpm_raw_data_header.num_turns;

  // Instrument is always configured to grab one extra turn of data than what was requested,
  // to accommodate 14ns wrapped bunch patterns, so always read back that extra turn as well.
  block_0_words = dp->cbpm_raw_data_header.num_bunches[0] * (num_turns+1);
  block_1_words = dp->cbpm_raw_data_header.num_bunches[1] * (num_turns+1);

  //printf("Number of words to retrieve from block_0 = %d\n", block_0_words );
  //printf("Number of words to retrieve from block_1 = %d\n", block_1_words );
  fflush(stdout);

  if (block_0_words > 0) {
    //printf("Retrieving block_0 data...\n");
    fflush(stdout);
    k=cbi_gp_struct(READ, ETHERNET, CBPM_ADC_BUFFER0_TAG, CTL_System.p_Module[iidx], block_0_words);
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
    k=cbi_gp_struct(READ, ETHERNET, CBPM_ADC_BUFFER2_TAG, CTL_System.p_Module[iidx], block_0_words);
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
    k=cbi_gp_struct(READ, ETHERNET, CBPM_ADC_BUFFER4_TAG, CTL_System.p_Module[iidx], block_0_words);
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
    k=cbi_gp_struct(READ, ETHERNET, CBPM_ADC_BUFFER6_TAG, CTL_System.p_Module[iidx], block_0_words);
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
  }

  if (block_1_words > 0) {
    //printf("Retrieving block_1 data...\n");
    fflush(stdout);
    k=cbi_gp_struct(READ, ETHERNET, CBPM_ADC_BUFFER1_TAG, CTL_System.p_Module[iidx], block_1_words);
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
    k=cbi_gp_struct(READ, ETHERNET, CBPM_ADC_BUFFER3_TAG, CTL_System.p_Module[iidx], block_1_words);
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
    k=cbi_gp_struct(READ, ETHERNET, CBPM_ADC_BUFFER5_TAG, CTL_System.p_Module[iidx], block_1_words);
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
    k=cbi_gp_struct(READ, ETHERNET, CBPM_ADC_BUFFER7_TAG, CTL_System.p_Module[iidx], block_1_words);
    if (k == CBI_F_FAILURE) {
      return CBI_F_COMM_ERROR;
    }
  }

  //printf("Retrieving phase word data...\n");
  fflush(stdout);
  k=cbi_gp_struct(READ, ETHERNET, CBPM_PH_WORD_BUF_TAG, CTL_System.p_Module[iidx], num_turns);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }

  species       = dp->cbpm_cmd_params.species;

  fflush(stdout);

  
  return CBI_F_SUCCESS;

}

int cbpm_raw_data_final(void) {

  cbpm_write_all_raw_data();

  if (timing_adjust_for_raw && enable_time_scan_tests()) {
    cbpm_time_scan_summary(1); // in post, read in CBPM_BTEST_RESULTS_TAG struct. this function needs it.
  }
  
  return CBI_F_SUCCESS;
}
