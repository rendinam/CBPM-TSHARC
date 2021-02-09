//-----------------------------------------------------------------------+
// File         :  cbpm_find_bunch.c                                     |
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


int cbpm_find_bunch_prep(void) {

  int i, value, retval;
  int tsetup, tblock, species;

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  printf("*********FIND BUNCH*****************\n");
 
  printf("Channel delays are being set on the instrument.\n");

  tsetup = cbpm_measurement_tsetup();

  char input;


  cbpm_query_species( tsetup );


  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(2);

  int valid_inputs[5] = {4, (int)'a', (int)'A', (int)'b', (int)'B'};

  prompts[0].supp_text_ptr    = NULL;
  prompts[0].prompt_text      = "Select the TIMING BLOCK to use (A,B): ";
  prompts[0].data_type        = CBI_CHAR;
  prompts[0].valid_inputs     = valid_inputs;
  prompts[0].map_out          = NULL;
  prompts[0].default_value    = 'y';
  prompts[0].destination      = &(dp->cbpm_cmd_params.tblock);
  prompts[0].last_val_default = FALSE;

  // Timing block determined by species request when in 14ns setups.
  // Skip prompt if in one of those setups.
  if (tsetup == s14ns_VAR_G || tsetup == s14ns_FIX_G) {
    prompts[0].active = FALSE;
  }


  int range[2] = {1, 10};
  prompts[1].supp_text_ptr    = NULL;
  prompts[1].prompt_text      = "Enter number of turns : ";
  prompts[1].data_type        = CBI_INT;
  prompts[1].validator        = cbi_int_range_validator;
  prompts[1].valid_inputs     = range;
  prompts[1].map_out          = NULL;
  prompts[1].default_value    = 2;
  prompts[1].destination      = &(dp->cbpm_cmd_params.num_turns);
  prompts[1].last_val_default = TRUE;


  retval = CBI_PROMPTER();

  cbi_clear_bp( dp->cbpm_cmd_params.bunch_pat );

  return F_SUCCESS;

}




int cbpm_find_bunch_post(int iidx) {


  FILE *fp;
  char filename[CBI_MAX_STRING_LENGTH]   = "";
  char full_fname[CBI_MAX_STRING_LENGTH] = "";

  fp = cbi_managed_file_open( filename, full_fname, iidx, TSCAN_DIR, FIND_BUNCH, CBI_MULTI_FILE, FALSE );


  int k;
  int tblock, species, num_turns, num_bunches, words, block_1_words;
  int num_buckets;
  int max_delay_steps;
  int species_offset;
  
  // Local pointers to manage copying of prep structs to real BPM structs
  CBPM_DATA  *dp;

  dp = CTL_System.p_Module[iidx]->dsp_data;

  // Read back the structures to obtain status and payload
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBI_STAT_TAG,       CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CMD_PARAMS_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_TIMING_TAG,  CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  cbpm_print_cmd_params_struct( iidx, stdout );

  num_turns = dp->cbpm_cmd_params.num_turns;
  tblock    = dp->cbpm_cmd_params.tblock;
  species   = dp->cbpm_cmd_params.species;

  int tsetup = dp->cbpm_op_timing.active_timing_setup;

  printf("num_turns   = %d\n", num_turns);

  if ( tsetup == s4ns_P_VAR_G || tsetup == s4ns_E_VAR_G ) {
    num_bunches     = CBPM_4NS_MAX_BUNCHES / 2;
    num_buckets     = CBPM_4NS_MAX_BUNCHES;
    max_delay_steps = CBPM_4NS_FIND_BUNCH_STEPS;
  } else {
    num_bunches     = CBPM_14NS_MAX_BUNCHES / 2;
    num_buckets     = CBPM_14NS_MAX_BUNCHES;
    max_delay_steps = CBPM_14NS_FIND_BUNCH_STEPS;
  }

  printf("num bunches = %d\n", num_bunches); 


  words = num_bunches * num_turns * max_delay_steps;
  printf("Number of words to retrieve = %d\n", words );


  dp->cbpm_adc_buffer0.raw_data[0] = 100;
  dp->cbpm_adc_buffer0.raw_data[1] = 200;
  dp->cbpm_adc_buffer0.raw_data[2] = 300;
  dp->cbpm_adc_buffer0.raw_data[3] = 400;
  dp->cbpm_adc_buffer0.raw_data[4] = 500;

  dp->cbpm_adc_buffer1.raw_data[0] = 100;
  dp->cbpm_adc_buffer1.raw_data[1] = 200;
  dp->cbpm_adc_buffer1.raw_data[2] = 300;
  dp->cbpm_adc_buffer1.raw_data[3] = 400;
  dp->cbpm_adc_buffer1.raw_data[4] = 500;


  if( tsetup == s14ns_VAR_G ) {
    if ( species == POSITRONS ) {
      printf("Retrieving block_0 data...\n");
      k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER0_TAG, CTL_System.p_Module[iidx], words);
      k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER2_TAG, CTL_System.p_Module[iidx], words);
      k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER4_TAG, CTL_System.p_Module[iidx], words);
      k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER6_TAG, CTL_System.p_Module[iidx], words);
    } else {
      printf("Retrieving block_1 data...\n");
      k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER1_TAG, CTL_System.p_Module[iidx], words);
      k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER3_TAG, CTL_System.p_Module[iidx], words);
      k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER5_TAG, CTL_System.p_Module[iidx], words);
      k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER7_TAG, CTL_System.p_Module[iidx], words);
    }
  } else {
    printf("Retrieving block_0 data...\n");
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER0_TAG, CTL_System.p_Module[iidx], words);
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER2_TAG, CTL_System.p_Module[iidx], words);
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER4_TAG, CTL_System.p_Module[iidx], words);
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER6_TAG, CTL_System.p_Module[iidx], words);
  }


  if (tsetup == s4ns_P_VAR_G || tsetup == s4ns_E_VAR_G) {

    printf("Maximum intensity signal found in bucket #  %d/%d   %d/%d\n",
	   dp->cbpm_cmd_params.bun_pat_offset,
	   num_buckets,
	   dp->cbpm_cmd_params.bun_pat_offset/2,
	   num_buckets/2  );

  } else { // 14ns mode

    if (species == POSITRONS) {
      species_offset = (dp->cbpm_cmd_params.bun_pat_offset/2);
    } else {
      species_offset = (dp->cbpm_cmd_params.bun_pat_offset - 1)/2;
    }
    printf("Maximum intensity signal found in bucket #  %d/%d  of this timing block\n",
	   (dp->cbpm_cmd_params.bun_pat_offset/2),
	   num_buckets );
  }
  
  

  // Write data to disk 
  cbpm_write_find_bunch_dat( iidx, fp, full_fname );
  
  char plottitle[CBI_MAX_STRING_LENGTH];
  
  if (tsetup == s4ns_P_VAR_G || tsetup == s4ns_E_VAR_G) {
    sprintf(plottitle, "Find Bunch Scan  --  Bucket %d/%d  --  %s (%s) ",
	    species_offset,
	    num_buckets,
	    CTL_System.p_Module[iidx]->comm.ethernet.hostname,
	    CTL_System.p_Module[iidx]->comm.ethernet.IP_address);
    
  } else {  // 14ns
    sprintf(plottitle, "Find Bunch Scan  --  Bucket %d/%d (%d plot)  --  %s (%s) ",
	    dp->cbpm_cmd_params.bun_pat_offset,
	    num_buckets,
	    dp->cbpm_cmd_params.bun_pat_offset/2,
	    CTL_System.p_Module[iidx]->comm.ethernet.hostname,
	    CTL_System.p_Module[iidx]->comm.ethernet.IP_address);
  }
  
  char dattitle[CBPM_MAX_CARDS][CBI_MAX_STRING_LENGTH] = {"Card_0", "Card_1", "Card_2", "Card_3"};
  
  // Plot the data for review
  /*PLOT_PARAMS pparams;
  pparams.major_ticks = 1;
  gp_plot( full_fname, 1, 5, pparams, plottitle, dattitle );*/

  
  return CBI_F_SUCCESS;

}

