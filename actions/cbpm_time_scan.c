//-----------------------------------------------------------------------+
// File         :  cbpm_time_scan.c                                      |
//                                                                       |
// Description  :  simple prep) Prompts for necessary parameters, uses   |
//                 sane defaults for others to allow for simple, quick   |
//                 time scans to be run with minimal interaction.        |
//                                                                       |
//                 post() Handles reading back the time scan data and    |
//                 plotting the results.                                 |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina, M. Stedinger                              |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"

//Prototypes for encoding and decoding
#include "cbpm_test_functions.h"

int fitter_test(int iidx);
void CheckValidity(int species, int *status, CBPM_DATA *dp, CBPM_CONTROL *cp, int rollback_changes);
int PerformTest(int index, int addr, float value, int StatusArray[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS], CBPM_DATA *dp);
int PerformTest2Input(int index, int addr, float value1, float value2, int StatusArray[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS], CBPM_DATA *dp);
int  DetermineStatus(int Status[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS]);
int DisplayStatus(int Array[CBPM_FIND_DELAYS_HIST_LENGTH][CBPM_MAX_ACQ_CONTROLLERS], CBPM_DATA *dp, int showFullArray, int showingInstr);

float StatusVals[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS];
float StatusErrs[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS];
float StatusWarns[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS];

#define CBPM_CHAN_TO_CHAN_DRIFT_THRESH        4
#define CBPM_CHAN_DELAY_DRIFT_THRESH          40

int prevStatuses[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS];
int block_delays_uninitialized = CBI_FALSE;
int Using_Parallel_Post;
int debug_level;


//By default, show plot, like from Time-In, if don't want plot, like when calling from interface,
//waveform_data is explictily set to false, and in the normal prep it is a prompt.
int waveform_data = CBI_TRUE;

int printf_level = 1;

//*********************************************************************************************************************
int enable_time_scan_tests(void)
{
  return CBI_TRUE;
}
//*********************************************************************************************************************

//Function called conditionally, if need to query whether to show plot or not
int prompt_for_waveform_data(void) {
	
  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;
	
  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {TRUE, TRUE, FALSE, FALSE};
	
  CBI_INIT_PROMPTS(1);
  
  prompts[0].supp_text_ptr    = NULL;
  if ((dp->cbpm_code_params.execution_flags[DSP_DEBUG_LEVEL] < CBI_DEBUG_LEVEL_2) 
      && dp->cbpm_cmd_params.generate_delay_corrections
      && debug_level == CBI_DEBUG_LEVEL_0) {
    prompts[0].prompt_text      = "Would you like to see a waveform plot [y/n]? (use -d 1 or higher to get 4 card plots) ";
  }
  else
  {
    prompts[0].prompt_text      = "Would you like to see a waveform plot [y/n]? ";
  }	  	
  prompts[0].data_type        = CBI_CHAR;
  prompts[0].valid_inputs     = valid_inputs;
  prompts[0].map_out          = map_out;
  prompts[0].default_value    = 'n';
  prompts[0].destination      = &(waveform_data);
  prompts[0].last_val_default = TRUE;
  
  return CBI_PROMPTER();	  
}

//Function called when generate_delays is true
int prompt_print_level(void) {
	
  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;	

  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(1);
  int retval;
	  
  int i = 0;
  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {TRUE, TRUE, FALSE, FALSE};
	  
  prompts[i].supp_text_ptr    = NULL;
  prompts[i].prompt_text      = "Printf level (0-6) ? ";
  prompts[i].data_type        = CBI_INT;
  prompts[i].default_value    = 2;
  prompts[i].destination      = &(printf_level);
  prompts[i].last_val_default = TRUE;
  i++; 
  
  retval = CBI_PROMPTER(); 
}

int cbpm_time_scan_prompt_Time_Scan_Protection(void)
{
  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;	


  cbpm_copy_inst_params_for_prep(dp);  


  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(1);
  int retval;
  
  int i = 0;
  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {TRUE, TRUE, FALSE, FALSE};
	  
  prompts[i].supp_text_ptr    = NULL;
  prompts[i].prompt_text      = "Override Time Scan Protection ? ";
  prompts[i].data_type        = CBI_CHAR;
  prompts[i].valid_inputs     = valid_inputs;
  prompts[i].map_out          = map_out;  
  prompts[i].default_value    = 'n';
  prompts[i].destination      = &(dp->cbpm_code_params.execution_flags[OVERRIDE_TIMING_ROLLBACK]);
  prompts[i].last_val_default = TRUE;
  i++; 
  
  retval = CBI_PROMPTER();
  
  return retval;
}

int prompt_Data_Enable(void) {
	
  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;	

  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(1);
  int retval;
  
  int i = 0;
  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {TRUE, TRUE, FALSE, FALSE};
	  
  prompts[i].supp_text_ptr    = NULL;
  prompts[i].prompt_text      = "Use Hardware Data Enable [y/n]? ";
  prompts[i].data_type        = CBI_CHAR;
  prompts[i].valid_inputs     = valid_inputs;
  prompts[i].map_out          = map_out;
  prompts[i].default_value    = 'y';
  prompts[i].destination      = &(dp->cbpm_find_delay_params.use_data_enable);
  prompts[i].last_val_default = TRUE;
  i++;
	
  retval = CBI_PROMPTER();
  
  return retval;  
}

int cbpm_time_scan_prep(void) {

  int i, value, species;
  int gstep, cdelay_low;
  int bunch;
  int tblock = -1;
  int retval;
  
  #include "cbpm_instrument_code_param.h"	

  Using_Parallel_Post = cbpm_commands[cbi_command_idx_from_name(cbpm_commands, "TIME SCAN")].parallel_post;  
  debug_level  = CTL_System.debug_verbosity;      

  int tsetup;
  tsetup = cbpm_measurement_tsetup();

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  printf("************ TIME SCAN *************\n");
  
  cbpm_query_species(tsetup);
  species = dp->cbpm_cmd_params.species;


  // Set bunch pattern ------------------------------------------
  bunch = cbpm_query_bunch(tsetup, species, dp->cbpm_cmd_params.bunch_pat );



  // Instruct DSP to perform a timing calibration ---------------
  char input;
  dp->cbpm_cmd_params.delay_cal = TRUE;

  // No GAIN calibration
  dp->cbpm_cmd_params.gain_cal = FALSE;
 
  retval = F_SUCCESS;

  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(1);
  i = 0;

  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {TRUE, TRUE, FALSE, FALSE};
  prompts[i].supp_text_ptr    = NULL;
  prompts[i].prompt_text      = "Generate delay corrections [y/n]? ";
  prompts[i].data_type        = CBI_CHAR;
  prompts[i].valid_inputs     = valid_inputs;
  prompts[i].map_out          = map_out;
  prompts[i].default_value    = 'y';
  prompts[i].destination      = &(dp->cbpm_cmd_params.generate_delay_corrections);
  prompts[i].last_val_default = TRUE;
  i++;
  
  retval = CBI_PROMPTER();
  
  // Triggering  -----------------------------------
  dp->cbpm_cmd_params.trig_mask       = 0;
  
  cbpm_copy_inst_params_for_prep(dp);  

  if (dp->cbpm_cmd_params.generate_delay_corrections) {
    if (enable_time_scan_tests()) {
      prompt_print_level();
      cbpm_time_scan_prompt_Time_Scan_Protection(); // includes: cbpm_copy_inst_params_for_prep(dp);  
    }
    
	  if (CTL_System.self_trigger_enable) {
    	prompt_Data_Enable();
	  } else {
      dp->cbpm_find_delay_params.use_data_enable = CBI_FALSE;
    }
  }
  
  /*if (dp->cbpm_find_delay_params.use_data_enable) {
	  printf("WILL use data enable hardware trigger for time scan.\n");
  } 
  else {
	  printf("Not using data enable  trigger for time scan.\n");
  }   
  
    printf("Not using 60Hz, line-synchronous hardware trigger for time scan.\n");
  dp->cbpm_cmd_params.use_data_enable = CBI_FALSE;
  dp->cbpm_cmd_params.trig_mask       = 0;
  retval = F_SUCCESS;
*/
  
  /*if (CTL_System.self_trigger_enable) {
  	dp->cbpm_cmd_params.use_data_enable = TRUE;
    dp->cbpm_find_delay_params.use_data_enable;
  }  */

  if (!Using_Parallel_Post) {
    if (dp->cbpm_cmd_params.generate_delay_corrections) {
      prompt_for_waveform_data();
    } else {
      waveform_data = 1; // If no delay corrections, then always plot, otherwise there is no point in running the Time Scan function.
    }
  }

  printf("Will average \'fitter\' data over %d turns.\n\n", CBPM_FIT_TURNS);
  
  int timing_mode;
  switch (tsetup) {
   	
  case s4ns_P_VAR_G:
  case s4ns_P_FIX_G:
  case s4ns_E_VAR_G:
  case s4ns_E_FIX_G:
    timing_mode = m4ns;
    break;
   	   
  case s14ns_VAR_G:
  case s14ns_FIX_G:
    timing_mode = m14ns;
    break;
   	   
  default:
    printf("************************************ ERROR Unknown timing setup: %i *** \n",tsetup);
    return CBI_F_FAILURE;
  }   
  
  int fallback_span;
  
  if (timing_mode == m4ns) {
    fallback_span = 800;
  }
  else
    fallback_span = 1400;
		
  dp->cbpm_code_params.tests[TEST_BLOCK_DELAY_TOO_CLOSE_TO_HIGH_EDGE].warning_threshold = fallback_span - tests[TEST_BLOCK_DELAY_TOO_CLOSE_TO_HIGH_EDGE].warning_threshold;
  dp->cbpm_code_params.tests[TEST_BLOCK_DELAY_TOO_CLOSE_TO_HIGH_EDGE].error_threshold = fallback_span - tests[TEST_BLOCK_DELAY_TOO_CLOSE_TO_HIGH_EDGE].error_threshold;  
  dp->cbpm_code_params.tests[TEST_BLOCK_DELAY_TOO_CLOSE_TO_HIGH_EDGE].enable = CBI_TRUE;

  // Store the parameters needed for formatting the datafile in an external struct
  dspec.species         = species;
  dspec.bunch           = bunch;

  //If using parallel post, then get a new file index number ONCE and use it for all the BPMs for a given time scan command
  if (Using_Parallel_Post) {
  	printf("USING PARALLEL POST\n");
    int file_idx     = cbi_new_file_idx(TSCAN_DIR, TRUE);
    CTL_System.output_file_index = file_idx;
  }
  
  return retval;
}





int cbpm_time_scan_prep_from_iface( void ) {

  char *func_name = (char *)__FUNCTION__;

  debug_level  = CTL_System.debug_verbosity;        

  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  int use_enable_gate = TRUE;
  int turns;
  int BID, train, bunch;
  
  // CESR MPM Interface array provided paramters ------
  dp->cbpm_cmd_params.species            = CTL_CBPM_Intf_Params.species;

  //----------------------------------------------------------
  // Determine which bunches to enable for sampling based on 
  // the train/bunch and species values provided by the 
  // requesting application
  //----------------------------------------------------------
  BID = CTL_CBPM_Intf_Params.bunch_id[0];        // FIXME:  HARDCODED to 0 for now.
  // New method: obtain the entire bunch pattern from the Intf_Params array.
  //             Load it into the temporary bunch pattern.
  cbi_clear_bp( dp->cbpm_cmd_params.bunch_pat );
  train = cbi_bid2train( BID );
  bunch = cbi_bid2bunch( BID );
  printf("Externally-provided train # = %d\n", train );
  printf("Externally-provided bunch # = %d\n", bunch );

  // FIXME: make tblock switchable
  bunch = (cbpm_TB_to_BPslot(train, bunch) * 2) + dp->cbpm_cmd_params.species;
  printf("              CBIC bunch # = %d\n", bunch );
  
  cbi_set_bunch( dp->cbpm_cmd_params.bunch_pat, bunch+1 );

  dp->cbpm_cmd_params.num_turns                  = 50; //CTL_CBPM_Intf_Params.num_turns;

  // Non-interface-provided parameters -------
  dp->cbpm_cmd_params.use_data_enable            = CBI_FALSE;
  dp->cbpm_cmd_params.trig_mask                  = 0;
  dp->cbpm_cmd_params.scale_mode                 = PED_SUB_DATA;
  dp->cbpm_cmd_params.delay_cal                  = CBI_TRUE; 
  dp->cbpm_cmd_params.generate_delay_corrections = CBI_TRUE;
  dp->cbpm_cmd_params.gain_cal                   = CBI_FALSE;
  dp->cbpm_cmd_params.avg_mode                   = PROC_SIMPLE_AVG_MODE;

  // Set the maximum number of hanshake completion checks to make.
  // Only manifests if there is a problem that causes a handshake timeout.
  cbi_set_handshake_tries( 40 );

  //If using parallel post, then get a new file index number ONCE and use it for all the BPMs for a given time scan command  
  if (Using_Parallel_Post) {
  	printf("USING PARALLEL POST\n");
    int file_idx     = cbi_new_file_idx(TSCAN_DIR, TRUE);
    CTL_System.output_file_index = file_idx;
  }
  
  //When calling time scan from interface, don't display plot.
  waveform_data = CBI_FALSE;

  return CBI_F_SUCCESS;
}




int cbpm_time_scan_post(int iidx) {

  int i, k, blockstep, card;
  int species;
  int cdelay_low;
  int pre_cds[CBPM_MAX_CARDS]; // channel delays fed to command execution
  
  CBPM_DATA     *dp;
  CBPM_CONTROL  *cp;

  // Assign pointers
  dp      = CTL_System.p_Module[iidx]->dsp_data;
  cp      = CTL_System.p_Module[iidx]->control;

  int tblock;
  int init_gd_count, GD_radius;
  
  debug_level  = CTL_System.debug_verbosity;        

  FILE *fp;
  char filename[CBI_MAX_STRING_LENGTH]   = "";
  char full_fname[CBI_MAX_STRING_LENGTH] = "";


  // Command is done, read back all relevant structs to check status and receive payload  
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_TIMING_TAG,         CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  int tsetup = dp->cbpm_op_timing.active_timing_setup;

  //-----------------------------------------------------------
  // Save copy of timing config structure for comparison after 
  // any timing correction is performed.
  //-----------------------------------------------------------
  cp->config_params.timing_config.setups[tsetup] = dp->cbpm_timing_config.setups[tsetup];

  //TODO read back fewer communication structures - just the ones needed for that output displayed or plotted

  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CMD_PARAMS_TAG,        CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBI_STAT_TAG,               CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  int BlockStep;
  int NUM_VALS_TO_RETRIEVE;

  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_FIND_DELAY_PARAMS_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_FIND_DELAY_OUT_TAG,    CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CODE_PARAMS_TAG,    CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TEST_RESULTS_TAG,    CTL_System.p_Module[iidx], CBI_FULL_STRUCT);  
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_RAW_DATA_HEADER_TAG,   CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
 
  //don't do this in parallel mode cause don't need it and it can slow it down
  if ((waveform_data == CBI_TRUE) && (Using_Parallel_Post == CBI_FALSE)) {
    printf("Obtaining waveform data...\n");

    if (dp->cbpm_raw_data_header.num_bunches[0] != 0) {
      tblock = 0;
          } else if (dp->cbpm_raw_data_header.num_bunches[1] != 0) {
      tblock = 1;
          } else {
      if (debug_level >= CBI_DEBUG_LEVEL_4) {
        printf("Timing block could not be determined!\n");
      }//if
    }//if
      
    switch (CTL_System.p_Module[iidx]->core->cbi_stat.state) {
    case (m4ns):
      GD_radius = 400;
      break;
    case (m14ns):
      GD_radius = 700;
      break;
    }
    init_gd_count = dp->cbpm_timing_config.setups[tsetup].block_delays[tblock] - GD_radius;
    BlockStep = CBPM_BLOCK_STEP;

    NUM_VALS_TO_RETRIEVE = (dp->cbpm_find_delay_out.max_block_delay - init_gd_count)/BlockStep;
    if (debug_level >= CBI_DEBUG_LEVEL_2) {
      printf("VALS TO RETRIEVE = %d\n", NUM_VALS_TO_RETRIEVE);
    }
    NUM_VALS_TO_RETRIEVE = GD_radius * 2 * CBPM_SCAN_TURNS;
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER0_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER1_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER2_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER3_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER4_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER5_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER6_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
    k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER7_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
  }

  //Testing: this statement is only outside the if block to get data for Control side Verification of CheckValidity  
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CHAN_CAL_WORK_TAG,     CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  // Data found in the following structures is required for calculation / display below.
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_DIAGNOSTIC_TAG,        CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_GAIN_TAG,           CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TIMING_CONFIG_TAG,     CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  //printf("\n\n\n ************************ \n printf_level: %i \n *********************** \n\n\n", printf_level);
  
  if ((dp->cbpm_cmd_params.generate_delay_corrections) 
  && (printf_level > 0)) {
	
    cbi_gp_struct(READ, CTL_System.comm_method, CBPM_BFIND_DELAY_OUT_TAG,    CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
    cbi_gp_struct(READ, CTL_System.comm_method, CBPM_BTEST_RESULTS_TAG,    CTL_System.p_Module[iidx], CBI_FULL_STRUCT);	  
  }  

  if (Using_Parallel_Post) {
    int start_delays[4];
    for (int card = 0; card < CBPM_MAX_CARDS; card++ ) {
      start_delays[card] = dp->cbpm_diagnostic.pre_pf_offset_chan_delays[card];
    }

    FILE *ccdat_fp;
    char ccdatfile[CBI_MAX_STRING_LENGTH]      = ""; // Slot to hold generated file name
    char full_ccdatname[CBI_MAX_STRING_LENGTH] = "";
    ccdat_fp = cbi_managed_file_open_fixedindex( ccdatfile, full_ccdatname, iidx, TSCAN_DIR, CHAN_CAL);
    //printf("ccdatfile: %s\n", ccdatfile);
    cbpm_write_chan_cal_parallelpost(iidx, ccdat_fp, full_ccdatname, start_delays, 0, dp->cbpm_find_delay_out.peaks); //
 }

  return CBI_F_SUCCESS;
}



int cbpm_time_scan_summary( int printf_level_in ) {
	
	int chipdelaychanged = FALSE;
	printf_level = printf_level_in;

  int  inst, card;
  int  tsetup, tblock;
  int  chan = 0; // For BPMs
  int  diff_from_c0_before[CBPM_MAX_CARDS];
  int  diff_from_c0_after[CBPM_MAX_CARDS], before, after;
  int  ch_delay_changes[CBPM_MAX_CARDS];
  int  badcount = 0;
  int  ch_to_ch_test_failed = CBI_FALSE;


  CBPM_DATA         *dp; 
  CBPM_CONTROL      *cp;

  printf("\n\n\n   Timing Adjustment Summary:               \n\n");
  printf("                          Card-to-card change      Absolute change on each card   \n");
  printf("                      (diff from c0 before/after)                                      \n");
  printf("BAD                                                   c0     c1     c2    c3 \n");
  printf("------------------------------------------------------------------------------------\n");


  for (inst = 0; inst < num_online(); inst++) {
    if (CTL_System.p_Module[inst]->active == CBI_ACTIVE) {

      char delay_drift_problem_indicator = ' ';    	
      ch_to_ch_test_failed = CBI_FALSE;

      dp = CTL_System.p_Module[inst]->dsp_data;
      cp = CTL_System.p_Module[inst]->control;
      
      tsetup = dp->cbpm_op_timing.active_timing_setup;

      if (dp->cbpm_raw_data_header.num_bunches[0] != 0) {
        tblock = 0;
            } else if (dp->cbpm_raw_data_header.num_bunches[1] != 0) {
        tblock = 1;
            } else {
        if (debug_level >= CBI_DEBUG_LEVEL_4) {
          printf("Timing block could not be determined!\n");
        }//if
      }//if

      int status = CBI_F_SUCCESS;
      int species = dp->cbpm_cmd_params.species;
      
      char *test_numbers = "  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25\n";
	  char *test_numbers2= "    0       1       2       3       4       5       6       7       8       9      10      11      12      13      14      15      16      17      18      19      20      21      22      23      24      25\n";
	  char *valuesFormat = "%7.1f ";

      if ((dp->cbpm_cmd_params.generate_delay_corrections) 
	  && (printf_level >= 3)) {
      	

	printf("\nRound(New Peaks)\n   ");
	for(int j = 0; j<CBPM_MAX_ACQ_CONTROLLERS; j++) {
	  printf("%i, ", (int) roundf(dp->cbpm_find_delay_out.peaks[j]));
	}
	printf("\n\n");
	
	/*printf("\nOld BlockDelays\n\t");
	printf("%4i, ",dp->cbpm_timing_config.setups[tsetup].block_delays[tblock]);
	printf("\n");	

	printf("\nOld Chipvalues\n   ");
	for(int j = 0; j<CBPM_MAX_CARDS; j++) {
	  printf("%4i, ",cp->config_params.timing_config.setups[tsetup].chan_delays[tblock][j]);
	}
	printf("\n\n");*/
	
	printf("Offsets hist\n");
	for(int i = 0; i< dp->cbpm_find_delay_out.num_retries+3; i++) {
	  printf("%i: ",i);
	  for(int j = 0; j<CBPM_MAX_ACQ_CONTROLLERS; j++) {
	    printf("%5i, ",dp->cbpm_bfind_delay_out.offsetshist[i][j]);
	  }
	  printf("\n");
	}
	
	printf("\nPeaks hist\n");
	for(int i = 0; i< dp->cbpm_find_delay_out.num_retries+3; i++) {
	  printf("%i: ",i);
	  for(int j = 0; j<CBPM_MAX_ACQ_CONTROLLERS; j++) {
	    //printf("%4i, ",dp->cbpm_bfind_delay_out.peakshist[i][j]);
	    printf("%5.1f, ",dp->cbpm_bfind_delay_out.peakshist[i][j]);
	  }
	  printf("\n");
	}
	printf("\n");
	
	printf("Chips hist = Offsets + Peaks, and Block hist\n");
	for(int i = 0; i< dp->cbpm_find_delay_out.num_retries+3; i++) {
	  printf("%i: ",i);
	  for(int j = 0; j<CBPM_MAX_ACQ_CONTROLLERS; j++) {
	  	if (i< dp->cbpm_find_delay_out.num_retries+3-1) {
	      printf("%5i, ",dp->cbpm_bfind_delay_out.chipshist[i][j]);
	  	}
	  	else {
	      printf("%5i, ",0);
	  	}	  	 
	  }
	  printf("    %4i ",dp->cbpm_bfind_delay_out.blockhist[i]);
	  
	  if (!dp->cbpm_bfind_delay_out.rollbackhist[i] && (i!=0) && (i!=dp->cbpm_find_delay_out.num_retries+3-1)) {
	  	printf("  accepted");
	  }
	  
	  printf("\n");
	}
	
	  }
	  
/*if ((dp->cbpm_cmd_params.generate_delay_corrections) 
&& (printf_level > 0)) {

  printf("\nChips hist = Offsets + Peaks, and Block hist float\n");
  for(int i = 0; i< dp->cbpm_find_delay_out.num_retries+3; i++) {
    printf("%i: ",i);

    for(int j = 0; j<CBPM_MAX_ACQ_CONTROLLERS; j++) {
      if (i< dp->cbpm_find_delay_out.num_retries+3-1) {
        printf("%5.1f, ",dp->cbpm_bfind_delay_out.peakshist[i][j]+dp->cbpm_bfind_delay_out.offsetshist[i][j] - dp->cbpm_bfind_delay_out.chipshist[0][j]);
      }
      else {
        printf("%5i, ",0);
      }	  	 
    }
    printf("    %4i ",dp->cbpm_bfind_delay_out.blockhist[i]);

    if (!dp->cbpm_bfind_delay_out.rollbackhist[i] && (i!=0) && (i!=dp->cbpm_find_delay_out.num_retries+3-1)) {
    printf("  accepted");
    }

    printf("\n");
  }
}*/
	  
  if ((dp->cbpm_cmd_params.generate_delay_corrections) 
  && (printf_level >= 3)) {
	//__________________________________________	 
	    
	int cardDelays[4];
	int i;
	for (i = 0; i<4; i++) {
	  cardDelays[i] = dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][i];
	} 
	  
	int timing_mode;
	switch (tsetup) {
   	
	case s4ns_P_VAR_G:
	case s4ns_P_FIX_G:
	case s4ns_E_VAR_G:
	case s4ns_E_FIX_G:
	  timing_mode = m4ns;
	  break;
   	   
	case s14ns_VAR_G:
	case s14ns_FIX_G:
	  timing_mode = m14ns;
	  break;
   	   
	default:
	  printf("************************************ ERROR Unknown timing setup: %i *** \n",tsetup);
	  return CBI_F_FAILURE;
	}   
  
	int fallback_span;
  
	if (timing_mode == m4ns) {
	  fallback_span = 800;
	}
	else
	  fallback_span = 1400;	  
	  	  
	for (i = 0; i<4; i++) {
	  dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][i] = cardDelays[i];
	} 

	//___________________________________________	

	/*printf("\nNew BlockDelays\n\t");
	printf("%4i, ",dp->cbpm_timing_config.setups[tsetup].block_delays[tblock]);
	printf("\n");*/	
	
	printf("\nOld Chipvalues, and Old BlockDelay\n   ");
	for(int j = 0; j<CBPM_MAX_ACQ_CONTROLLERS; j++) {
	  printf("%4i, ", cp->config_params.timing_config.setups[tsetup].chan_delays[tblock][j]);
	}
	printf("    %4i ",dp->cbpm_bfind_delay_out.blockhist[0]); //dp->cbpm_timing_config.setups[tsetup].block_delays[tblock]);
	printf("\n\n");
	
	
	printf("New Chipvalues, and New BlockDelay\n   ");
	for(int j = 0; j<CBPM_MAX_ACQ_CONTROLLERS; j++) {
	  printf("%4i, ", dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][j]);
	}
	printf("    %4i ",dp->cbpm_timing_config.setups[tsetup].block_delays[tblock]);
	printf("\n\n");
  
	int chan = 0;
	//int status = CBI_F_SUCCESS;

	printf("ReTries: %i\n",dp->cbpm_find_delay_out.num_retries);
	printf("Test Results:  %i = Uninitialized, %i = Success, %i = Warning, %i = Error\n", CBPM_TEST_ENUM_UNINITIALIZED, CBPM_TEST_ENUM_SUCCESS, CBPM_TEST_ENUM_WARNING, CBPM_TEST_ENUM_ERROR);
	printf("Try  Result :  %i = SUCCESS, %i = WARNING, %i = FAILURE\n", CBI_F_SUCCESS, CBI_F_WARNING, CBI_F_FAILURE);

	//						  0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9   0   1   2
	//printf(              "\n  4   4   4   4   1   1   1   4   4   1   4   4   4   4   4   4   4   4   4   4   4   1   4\n");

	int testsums[CBPM_NUM_TESTS];
	int testsumoffsets[CBPM_NUM_TESTS] = {0, 0, 0, 0, 3, 3, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 3};
	int cardsums[CBPM_MAX_CARDS];
	int cardsumoffsets[CBPM_MAX_CARDS] = {1, 5, 5, 5};
	for(int i = 1; i< dp->cbpm_find_delay_out.num_retries+3; i++) {

	  if (dp->cbpm_btest_results.Statuseshist[i][0][0] == 0) {
	  	 printf("\tIteration #%i not run.\n", i);
	  	 continue;
	  }
	  printf("\nInstrument Test Results History %i Result:\t%i\tAcceptTiming:\t%i\tRollback:\t%4i\n",i, dp->cbpm_btest_results.statushist[i], dp->cbpm_bfind_delay_out.timingUpdateIsGoodhist[i], dp->cbpm_bfind_delay_out.rollbackhist[i]);
	  printf(test_numbers);
	  
		for(int test = 0; test < CBPM_NUM_TESTS; test++) {
			testsums[test] = 0;
		}
		  
	  for(int card= 0; card < CBPM_MAX_CARDS; card++) {
	  	cardsums[card] = 0;
	    for(int test = 0; test < CBPM_NUM_TESTS; test++) {
	      cardsums[card] += dp->cbpm_btest_results.Statuseshist[i][test][card]-1;
	      testsums[test] += dp->cbpm_btest_results.Statuseshist[i][test][card]-1;
	      
	      printf("%3i ",dp->cbpm_btest_results.Statuseshist[i][test][card]-1);
	    }//for
	    
	    //printf("    %3i / %3i ",cardsums[card]+cardsumoffsets[card], idealcardsums[card]);
	    if (dp->cbpm_btest_results.Statuseshist[i][0][0] != 0) {
	      printf(" %3i",cardsums[card]+cardsumoffsets[card]);
	    }
	    printf("\n");
	  }//for
	
	  if (dp->cbpm_btest_results.Statuseshist[i][0][0] != 0) {
		printf("\n");
		for(int test = 0; test < CBPM_NUM_TESTS; test++) {
			printf("%3i ",testsums[test]+testsumoffsets[test]);
		}
		printf("\n");
	  }
	  
	}//endfor

      }//if generate

      if (printf_level >= 2) {
	printf("\n-----------------------------------------------------------\n");
	printf("Instrument Test Results\n");
      }
	
      //if (printf_level >= 1) {

	if (enable_time_scan_tests() && printf_level >= 1) {
    DisplayStatus(dp->cbpm_test_results.Statuses, dp, printf_level>4, CBI_TRUE);
  }
	
	/*printf(test_numbers);
	for(int card= 0; card < CBPM_MAX_CARDS; card++) {
	  for(int test = 0; test < CBPM_NUM_TESTS; test++) {
	    printf("%3i ",dp->cbpm_test_results.Statuses[test][card]-1);
	  }//for
	  printf("\n");
	}//for
	printf("\n");*/
	  
      //}//if printf_level 1
	
  if (printf_level >= 2) {
	  
	/*printf("Instrument Enable Values\n");
	printf("    ");
	for(int test = 0; test < CBPM_NUM_TESTS; test++) {
	  printf("%i        ",dp->cbpm_code_params.tests[test].enable);
	}
	printf("\n\n");*/
	  int start;
    if (printf_level >= 3) {
       start = 1;
     } else {
       start = dp->cbpm_find_delay_out.num_retries+2 - 1;
     }
	  
    for(int i = start; i< dp->cbpm_find_delay_out.num_retries+2; i++) {
      printf("Instrument Test Values Step %i\n",i);
      
      /*if (dp->cbpm_btest_results.Statuseshist[i][0][0] == 0) {
         printf("\tIteration #%i not run.\n", i);
         continue;
      }*/
          
      printf(test_numbers2);
      for(int card= 0; card < CBPM_MAX_CARDS; card++) {
        for(int test = 0; test < CBPM_NUM_TESTS; test++) {
          printf(valuesFormat,dp->cbpm_btest_results.StatusVals[i][test][card]);
        }
        printf("\n");
      }	  
      printf("\n"); 
    }
	
  }//if printf_level 3

  if (printf_level >= 3) {
	  	
	//printf("\nDetermine whether to accept new timings or not\n");
	int addr;
	int timingUpdateIsGood = 1;
  
	for (card = 0; card<CBPM_MAX_CARDS; card++) {
	    addr = (card * CBPM_MAX_CARD_ADCS_PER_BLOCK) + chan;	

      for(int test = 0; test < CBPM_NUM_TESTS; test++) {
        if (dp->cbpm_code_params.tests[test].required_for_update_acceptance) {
          timingUpdateIsGood = timingUpdateIsGood && (dp->cbpm_test_results.Statuses[test][addr] < CBPM_TEST_ENUM_ERROR);
        }
      }//fot test
      
	    //timingUpdateIsGood = timingUpdateIsGood && (dp->cbpm_test_results.Statuses[TEST_SIGNAL_TOO_LOW][addr] < CBPM_TEST_ENUM_ERROR);
	    //printf("%i\n",dp->cbpm_test_results.Statuses[SIGNAL_TOO_LOW][addr]);  

	    //timingUpdateIsGood = timingUpdateIsGood && (dp->cbpm_test_results.Statuses[TEST_BEFORE_ADJUSTTIMING_ADC_RMS_TOO_BIG][addr] < CBPM_TEST_ENUM_ERROR);
	    //printf("%i\n",dp->cbpm_test_results.Statuses[BEFORE_ADJUSTTIMING_ADC_RMS_TOO_BIG][addr]);  

	    //timingUpdateIsGood = timingUpdateIsGood && (dp->cbpm_test_results.Statuses[TEST_AFTER_ADJUSTTIMING_ADC_RMS_TOO_BIG][addr] < CBPM_TEST_ENUM_ERROR);
	    //printf("%i\n",dp->cbpm_test_results.Statuses[AFTER_ADJUSTTIMING_ADC_RMS_TOO_BIG][addr]);  
	}//for card
      	   

	//int rollback_changes = !timingUpdateIsGood && !dp->cbpm_code_params.execution_flags[OVERRIDE_TIMING_ROLLBACK];
	

	  
	  
	/*if (rollback_changes) {
	  //dp->cbpm_code_params.tests[OFFSET_TOO_CLOSE_TO_HIGH_EDGE].enable = 0;
	  //dp->cbpm_code_params.tests[OFFSET_TOO_CLOSE_TO_LOW_EDGE].enable = 0;
	  //dp->cbpm_code_params.tests[CHIPS_TOO_FAR_APART].enable = 0;
	  dp->cbpm_code_params.tests[FITVALUE_TOO_CLOSE_TO_HIGH_EDGE].enable = 0;
	  dp->cbpm_code_params.tests[FITVALUE_TOO_CLOSE_TO_LOW_EDGE].enable = 0;
	}//if*/
		  	  
	CheckValidity(species, &status, dp, cp, 0);
	
	printf("\nLocal Warn Values:\n");
	printf(test_numbers2);
	for(int card= 0; card < CBPM_MAX_CARDS; card++) {
	  for(int test = 0; test < CBPM_NUM_TESTS; test++) {
	    printf(valuesFormat,StatusWarns[test][card]);
	  }
	  printf("\n");
	}	  
	  

	printf("\nLocal Err Values:\n");
	printf(test_numbers2);
	for(int card= 0; card < CBPM_MAX_CARDS; card++) {
	  for(int test = 0; test < CBPM_NUM_TESTS; test++) {
	    printf(valuesFormat,StatusErrs[test][card]);
	  }
	  printf("\n");
	}	  
	
	if (printf_level >= 4) {
	  printf("\ntimingUpdateIsGood = %i\n",timingUpdateIsGood);  
	  //printf("rollback_changes = %i\n",rollback_changes);  
	}	  
	  
	//_____________________________________________
	  
	  
	//if (rollback_changes) {
	//  printf("\n\n **************************** \n Warning, peak will be zero and others too, since rollback ocurred. \n ******************* \n\n");
	//}
	    	  
	//____________________________________	  

	printf("\nLocal Test Results: %i\n", status);
	/*printf(test_numbers);
	for(int card= 0; card < CBPM_MAX_CARDS; card++) {
	  for(int test = 0; test < CBPM_NUM_TESTS; test++) {
	    printf("%3i ",dp->cbpm_test_results.Statuses[test][card]-1);
	  }
	  printf("\n");
	}*/
	
	DisplayStatus(dp->cbpm_test_results.Statuses, dp, printf_level>5, CBI_FALSE);
	
  }

  if (printf_level >= 5) {			
	  
	printf("\nLocal Test Values:\n");
	printf(test_numbers2);
	for(int card= 0; card < CBPM_MAX_CARDS; card++) {
	  for(int test = 0; test < CBPM_NUM_TESTS; test++) {
	    printf(valuesFormat,StatusVals[test][card]);
	  }
	  printf("\n");
	}
	
  }
  if (printf_level >= 4) {		
		  
      }//printf_level 4
	
      if (printf_level >= 3) {	

	printf("--------------------------------------------------------------\n");
	printf("\nDifference between Test Results on Instrument and Local\n");
	printf(test_numbers);
	for(int card= 0; card < CBPM_MAX_CARDS; card++) {
	  for(int test = 0; test < CBPM_NUM_TESTS; test++) {
	    printf("%3i ",dp->cbpm_test_results.Statuses[test][card] - prevStatuses[test][card]);
	  }
	  printf("\n");
	}
	printf("\n");

	
      }//printf_level 3
  
      // Calculate card-to-card differences
      for (int card = 1; card < CBPM_MAX_CARDS; card++ ) {
	
	diff_from_c0_before[card] = cp->config_params.timing_config.setups[tsetup].chan_delays[tblock][0] -
	  cp->config_params.timing_config.setups[tsetup].chan_delays[tblock][card];
	
	diff_from_c0_after[card] = dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][0] -
	  dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][card];
	
      }//for
      
      // Calculate absolute differences
      for (int card = 0; card < CBPM_MAX_CARDS; card++ ) {
	
	before = cp->config_params.timing_config.setups[tsetup].chan_delays[tblock][card];
	after  = dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][card];
	
	ch_delay_changes[card] = after-before;
      }

      int min_start_delay = dp->cbpm_diagnostic.pre_pf_offset_chan_delays[0];
      int max_start_delay = min_start_delay;
      for (int card = 0; card<4; card++) {

	int delay = dp->cbpm_diagnostic.pre_pf_offset_chan_delays[card];
	if (delay<min_start_delay) {
	  min_start_delay = delay; }
	if (max_start_delay<delay) {
	  max_start_delay = delay; }
      }

      for (int card = 1; card < CBPM_MAX_CARDS; card++) {
	// If the card-to-card channel delays have drited by more than the threshold value,
	// flag instrument as bad.
	if ( abs(diff_from_c0_after[card]-diff_from_c0_before[card]) > CBPM_CHAN_TO_CHAN_DRIFT_THRESH ) {
	  delay_drift_problem_indicator = 'X';
	  badcount++;
	  ch_to_ch_test_failed = CBI_TRUE;
	  break;
	}//if
      }//for
      
      if (!ch_to_ch_test_failed) {
	for (int card = 0; card < CBPM_MAX_CARDS; card++ ) {
	  // If the absolute before --> after change in channel delays exceeds the threshold,
	  // flag instrument as bad.
	  if ( abs(ch_delay_changes[card]) > CBPM_CHAN_DELAY_DRIFT_THRESH ) {
	    delay_drift_problem_indicator = 'X';
	    badcount++;
	  }
	  break;
	}//for
      }//if
      
      printf("[%c]  %5s (%s)   ", 
	     delay_drift_problem_indicator, 
	     CTL_System.p_Module[inst]->det.location, 
	     CTL_System.p_Module[inst]->comm.ethernet.hostname );
      
      // Display card-to-card differences
      for (int card = 1; card < CBPM_MAX_CARDS; card++ ) {
		printf(" %+6d", diff_from_c0_after[card]-diff_from_c0_before[card]);
      }
      printf("    ");
      // Display absolute differences for each card, before-->after.
      for (int card = 0; card < CBPM_MAX_CARDS; card++ ) {
		printf(" %+6d", ch_delay_changes[card] );
		chipdelaychanged = chipdelaychanged || (abs(ch_delay_changes[card]) > 0);	
      }
      printf("\n");
      
    if (printf_level >= 6) {  
      printf("\n\nExecution flags\n");
      for (int i = 0; i< CBPM_NUM_EXECUTION_FLAGS; i++) {
        printf("%7i ", i);
      }  
      printf("\n");  
      for (int i = 0; i< CBPM_NUM_EXECUTION_FLAGS; i++) {
        printf("%7i ", dp->cbpm_code_params.execution_flags[i]);
      }
      printf("\n");
    }        
      
      if ((printf_level >= 3)) {
	    printf("\n\n******************************************************************************************************************************************************************\n\n");      
      }
      
    } // endIf active
  } // endFOR inst
  
  
    if (badcount > 0) {
      printf("    ***************************************************************\n");
      printf("    *                           WARNING                           *\n");
      printf("    * Large timing adjustments were performed on %2d Instruments!*\n", badcount );
      printf("    *                (See summary above for details.)             *\n");
      printf("    ***************************************************************\n\n");
    
    
  }
  
  return chipdelaychanged;
}




int cbpm_time_scan_final( void ) {

  cbi_close_sockets();

  if (!Using_Parallel_Post) {

    int i, k, blockstep;
    int species;
    int cdelay_low;
    int pre_cds[CBPM_MAX_CARDS]; // channel delays fed to command execution
  
    CBPM_DATA     *dp;
    CBPM_CONTROL    *cp;


    int tblock;
    int init_gd_count;

    int chan = 0; // for BPMs
    int iidx;

    FILE *fp;
    char filename[CBI_MAX_STRING_LENGTH]   = "";
    char full_fname[CBI_MAX_STRING_LENGTH] = "";

    for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
      if (CTL_System.p_Module[iidx]->active == CBI_ACTIVE) {


	// Clear filename strings for the managed file open below
	strcpy( filename, "" );
	strcpy( full_fname, "" );

	// Assign pointers
	dp      = CTL_System.p_Module[iidx]->dsp_data;
	cp      = CTL_System.p_Module[iidx]->control;
      
      
	species = dp->cbpm_cmd_params.species;
      
	int tsetup = dp->cbpm_op_timing.active_timing_setup;
      
      
	if (dp->cbpm_raw_data_header.num_bunches[0] != 0) {
	  tblock = 0;
	} else if (dp->cbpm_raw_data_header.num_bunches[1] != 0) {
	  tblock = 1;
	} else {
	  if (debug_level >= CBI_DEBUG_LEVEL_4) {
	    printf("Timing block could not be determined!\n");
	  }
	  return F_FAILURE;
	}
      
	if (debug_level >= CBI_DEBUG_LEVEL_4) {
	  printf("TBLOCK = %d\n", tblock);
	}
      
	if (debug_level >= CBI_DEBUG_LEVEL_4) {
	  cbpm_print_cmd_params_struct(iidx, stdout);
	}
      
      
	int bit, bunch, numbunches;
	bunch = -1;
      
	int GD_radius;
	if ( CTL_System.p_Module[iidx]->core->cbi_stat.state == m4ns ) {
	  if (debug_level >= CBI_DEBUG_LEVEL_4) {
	    printf("Scanning for high bit in __4ns__ pattern\n");
	  }
	  numbunches = 640;
	  GD_radius  = 400;
	} else if( CTL_System.p_Module[iidx]->core->cbi_stat.state == m14ns ) {
	  if (debug_level >= CBI_DEBUG_LEVEL_4) {
	    printf("Scanning for high bit in __14ns__ pattern\n");
	  }
	  numbunches = 183;
	  GD_radius  = 700;
	}
      
      
	for (bit = 0; bit < CBPM_MAX_BUNCHES; bit++) {
	  if ( dp->cbpm_cmd_params.rot_bunch_pat[bit] == 1 ) {
	    bunch = bit;
	    break;
	  }
	}
	if (debug_level >= CBI_DEBUG_LEVEL_4) {
	  printf("Bunch index in rotated pattern: %d\n", bunch);
	}
      
      
      
	// Gather up plotting parameters that were specified in the prep phase
	species    = dspec.species;
	bunch      = dspec.bunch;

	if (debug_level >= CBI_DEBUG_LEVEL_4) {
	  printf("Bunch from _prep(): %d\n", bunch);
	}
      
      
	int start_delays[4];
	if (debug_level >= CBI_DEBUG_LEVEL_4) {
	  printf("START DELAYS:  ");
	}
	for (int card = 0; card < CBPM_MAX_CARDS; card++ ) {
	  start_delays[card] = dp->cbpm_diagnostic.pre_pf_offset_chan_delays[card];
	  if (debug_level >= CBI_DEBUG_LEVEL_4) {
	    printf("C%d: %d \t", card, start_delays[card]);
	  }
	}
	if (debug_level >= CBI_DEBUG_LEVEL_4) {
	  printf("\n");
	}      
      
	//--------------------------------------------------------
	// Print channel delay change summary for the instrument.
	//--------------------------------------------------------
	int before, after;
	//printf("%s  --  ", 	   CTL_System.p_Module[iidx]->comm.ethernet.hostname );
	for (int card = 0; card < CBPM_MAX_CARDS; card++ ) {
	  before = cp->config_params.timing_config.setups[tsetup].chan_delays[tblock][card];
	  after = dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][card];
	  //printf("c%d: %+d     ", card, after-before);
	
	}
	//printf("\n");
        
      
	float GD = (float)dp->cbpm_timing_config.setups[tsetup].block_delays[tblock]/100;
      
	float p7 =  (float)dp->cbpm_diagnostic.pre_pf_peak_block_delays[0];
	float p8 =  (float)dp->cbpm_diagnostic.pre_pf_peak_block_delays[1];
	float p9 =  (float)dp->cbpm_diagnostic.pre_pf_peak_block_delays[2];
	float p10 = (float)dp->cbpm_diagnostic.pre_pf_peak_block_delays[3];
      
	if (debug_level >= CBI_DEBUG_LEVEL_3) {
	  printf("d0: %2f  window_begins: %2f \n",   p7, p7 - CBPM_CAL_RADIUS );
	  printf("d1: %2f  window_begins: %2f \n",   p8, p8 - CBPM_CAL_RADIUS );
	  printf("d2: %2f  window_begins: %2f \n",   p9, p9 - CBPM_CAL_RADIUS );
	  printf("d3: %2f  window_begins: %2f \n",   p10, p10 - CBPM_CAL_RADIUS );
	}
      
	if ( waveform_data == CBI_TRUE ) {
	
	  // Set up to plot waveform data -------------------------------------------
	  char tmptitle[CBI_MAX_STRING_LENGTH];
	  sprintf(tmptitle, "Timing Scan: Bunch #%d (%d-turn avg) -- %s (%s)-- %s -- Sample @ X=%2.2f",
		  bunch, 
		  CBPM_SCAN_TURNS, 
		  CTL_System.p_Module[iidx]->comm.ethernet.hostname,
		  CTL_System.p_Module[iidx]->comm.ethernet.IP_address,
		  CTL_System.p_Module[iidx]->det.location, 
		  GD );
	  char titles[CBPM_MAX_CARDS][CBI_MAX_STRING_LENGTH];

	  //----------------------------------------------------------------
	  // Annotate series titles with channel delays used for this scan
	  //----------------------------------------------------------------
	  for (int card = 0; card < CBPM_MAX_CARDS; card++) {
	    pre_cds[card] = dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][card];
	  }
	  int op_gain_setting;
	  for (int card = 0; card < CBPM_MAX_CARDS; card++) {
	    op_gain_setting = dp->cbpm_op_gain.active_gain_settings[card][tblock];
	    sprintf(titles[card], "Card_%d  gain setting:[%d] CD:[%d]", 
		    card, 
		    op_gain_setting,
		    pre_cds[card] );
	  }

	  if (debug_level >= CBI_DEBUG_LEVEL_4) {
	    printf(" COMMON TMD = %d\n", cp->config_params.timing_config.setups[tsetup].cTMD );
	  }


	  PLOT_PARAMS pparams;
	  pparams.major_ticks = 1;



	  supp_plotvals[0] = dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][0];
	  supp_plotvals[1] = dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][1];
	  supp_plotvals[2] = dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][2];
	  supp_plotvals[3] = dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][3];
	  supp_plotvals[4] = 0;
	  supp_plotvals[5] = 0;
	  supp_plotvals[6] = 0;
	  supp_plotvals[7] = 0;


	  ////printf("Determined maximum block delay of : %d\n", dp->cbpm_find_delay_out.max_block_delay );
	  init_gd_count = dp->cbpm_timing_config.setups[tsetup].block_delays[tblock] - GD_radius;

	  // Prevent negative GD time values on X-axis.
	  if (init_gd_count < 0) {
	    init_gd_count = 0;
	  }


		// Dump data to disk  // FIXME- re-enable!
		fp = cbi_managed_file_open( filename, full_fname, iidx, TSCAN_DIR, TIME_SCAN, CBI_MULTI_FILE, FALSE );
		RDV3_INST_DATA hdata;
		cbpm_prep_instrument_header( &hdata, iidx );
		cbpmfio_rw_instrument_header(fp, CBPMFIO_WRITE);
		fprintf(fp, "\n"); 
		cbpm_write_tscan_data( iidx, init_gd_count, fp, filename );
		fflush(fp);


	  //printf("\n\n\n CLOSING SOCKET \n\n\n");
	  //cbi_close_socket( iidx );  // NEW

	  gp_plot( full_fname, 1, 5, pparams, tmptitle, titles ); 

	  if (dp->cbpm_cmd_params.generate_delay_corrections) {

	    // Only dump the delay calibration data and error values if delay corrections
	    // were requested.

	    FILE *ccdat_fp;
	    char ccdatfile[CBI_MAX_STRING_LENGTH]      = ""; // Slot to hold generated file name
	    char full_ccdatname[CBI_MAX_STRING_LENGTH] = "";
	    ccdat_fp = cbi_managed_file_open( ccdatfile, full_ccdatname, iidx, TSCAN_DIR, CHAN_CAL, CBI_MULTI_FILE, FALSE ); //
	    //printf("ccdatfile: %s\n", ccdatfile); //
	    cbpm_write_chan_cal(iidx, ccdat_fp, ccdatfile, start_delays, 0); //

	    if (debug_level >= CBI_DEBUG_LEVEL_1) {

	      sprintf(tmptitle, "Peak Fit: Bunch #%d (%d-turn avg) -- %s (%s)-- %s ",
		      bunch+1, 
		      CBPM_SCAN_TURNS,
		      CTL_System.p_Module[iidx]->comm.ethernet.hostname,
		      CTL_System.p_Module[iidx]->comm.ethernet.IP_address,
		      CTL_System.p_Module[iidx]->det.location, 
		      GD );

	      cbpm_find_delay_plot_peak( full_ccdatname, 1, 5, pparams, tmptitle, titles, TRUE, dp->cbpm_find_delay_out.fcoeffs, 
					 dp->cbpm_diagnostic.pre_pf_offset_chan_delays); 
	    }
	  }

	  cbi_sleep(1500);

	  char coeff_names[CBPM_MAX_CARDS][2] = {"a\0", "b\0", "c\0", "d\0"};

	  int plotcc = TRUE;

	  //// Fitter diagnostic
	  //printf("\n\nFit coefficients generated on the instrument:\n");
	  //printf("----------------------------------------------\n");
	  // Set up to plot fitter data -------------------------------------------
	  for (i = 0; i < CBPM_MAX_ACQ_CONTROLLERS; i ++ ) {
	    //printf("2nd-order fit coefficients for card %d:\n", i);
	    //printf("%s0 = %f\n",   coeff_names[i], dp->cbpm_find_delay_out.fcoeffs[(i*3)    ] );
	    //printf("%s1 = %f\n",   coeff_names[i], dp->cbpm_find_delay_out.fcoeffs[(i*3) + 1] );
	    //printf("%s2 = %f\n\n", coeff_names[i], dp->cbpm_find_delay_out.fcoeffs[(i*3) + 2] );
	    if( isnan( dp->cbpm_find_delay_out.fcoeffs[(i*3)    ] ) ) {
	      plotcc = FALSE;
	    }
	    if( isnan( dp->cbpm_find_delay_out.fcoeffs[(i*3) + 1] ) ) {
	      plotcc = FALSE;
	    }
	    if( isnan( dp->cbpm_find_delay_out.fcoeffs[(i*3) + 2] ) ) {
	      plotcc = FALSE;
	    }
	  }


	  //// Fitter diagnostic
	  /* 	printf("\nFit coefficients generated locally:\n"); */
	  /* 	printf("----------------------------------------------\n"); */
	  /* 	fitter_test(iidx);/*
	     /* 	printf("\n"); */

	  /* 	sprintf(tmptitle, "Fitter Data for Bunch #%d (%d-turn avg) %d %d %d %d    --     %s", \ */
	  /* 		bunch+1, \ */
	  /* 		CBPM_FIT_TURNS, \ */
	  /* 		dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][0][chan], \ */
	  /* 		dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][1][chan], \ */
	  /* 		dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][2][chan], \ */
	  /* 		dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][3][chan], \ */
	  /* 		CTL_System.p_Module[iidx]->core->cbi_ident.ipaddr ); */



	  supp_plotvals[0] = (float)(p7-CBPM_CAL_RADIUS)/100;
	  supp_plotvals[1] = (float)(p8-CBPM_CAL_RADIUS)/100;
	  supp_plotvals[2] = (float)(p9-CBPM_CAL_RADIUS)/100;
	  supp_plotvals[3] = (float)(p10-CBPM_CAL_RADIUS)/100;

	  supp_plotvals[4] = (float)(p7+CBPM_CAL_RADIUS)/100;
	  supp_plotvals[5] = (float)(p8+CBPM_CAL_RADIUS)/100;
	  supp_plotvals[6] = (float)(p9+CBPM_CAL_RADIUS)/100;
	  supp_plotvals[7] = (float)(p10+CBPM_CAL_RADIUS)/100;


	  //if (plotcc == FALSE) {
	  //  printf("\nReceived bad coefficients, skipping fitter data plot\n");
	  //} else {
	  //printf("Chan cal data plot disabled, do it manually.\n");
	  ////gp_cc_plot( iidx, ccdatfile, TSCAN, 1, 5, tmptitle, titles );
	  //gp_cc_plot( iidx, ccdatfile, full_fname, 1, 5, pparams, tmptitle, titles ); 
	  //printf("DUMPING PLOTTER VALUE\n");
	  //}


	  //// Fitter diagnostic
	  /* 	// Set up to plot fitter error ------------------------------------------- */
	  /* 	for (i = 0; i < 8; i++) { */
	  /* 	  supp_plotvals[i] = 0; */
	  /* 	} */
	  /* 	sprintf(tmptitle, "Fitter Data Error for Bunch #%d     --     %s",  */
	  /* 		bunch+1,  */
	  /* 		CTL_System.p_Module[iidx]->core->cbi_ident.ipaddr ); */
	  ////gp_plot( ccerrfile, TSCAN, 1, 5, pparams, tmptitle, titles );

	} // endIF waveform_data
	
      } //endIF active

    } //endFOR iidx
  }
  
  cbpm_time_scan_summary(printf_level);
  
  cbpm_print_turn_count_summary();

  return CBI_F_SUCCESS;
}

void CheckValidity(int species, int *status, CBPM_DATA *dp, CBPM_CONTROL *cp, int rollback_changes)
{
  int i, j;
  int addr,card,chan;
  float fcoeffs[3*CBPM_MAX_ACQ_CONTROLLERS];
  int offsets[CBPM_MAX_ACQ_CONTROLLERS];
  int ChipValues[CBPM_MAX_ACQ_CONTROLLERS];
  float FitValues[CBPM_MAX_ACQ_CONTROLLERS];
  float maxAmparr[] = {.0, .0, .0, .0};
  float maxAmp;
  float minAmp;

  for(int card= 0; card < CBPM_MAX_CARDS; card++) {
    for(int test = 0; test < CBPM_NUM_TESTS; test++) {
      prevStatuses[test][card] = dp->cbpm_test_results.Statuses[test][card];
      
      StatusVals[test][card] = 0;
      StatusErrs[test][card] = 0;
      StatusWarns[test][card] = 0;
          
      dp->cbpm_test_results.Statuses[test][card] = CBPM_TEST_ENUM_UNINITIALIZED;
    }
  }

  int   prevChipValues[CBPM_MAX_ACQ_CONTROLLERS];

  int tsetup;
  tsetup = cbpm_measurement_tsetup();
  memcpy(fcoeffs, dp->cbpm_find_delay_out.fcoeffs, sizeof(dp->cbpm_find_delay_out.fcoeffs[0])*12);
  memcpy(offsets, dp->cbpm_diagnostic.pre_pf_offset_chan_delays, sizeof(dp->cbpm_diagnostic.pre_pf_offset_chan_delays[0])*4);

  memcpy(FitValues, dp->cbpm_find_delay_out.peaks, sizeof(dp->cbpm_find_delay_out.peaks[0])*4);
  
  int tblock;
  if (dp->cbpm_raw_data_header.num_bunches[0] != 0) {
    tblock = 0;
    } else if (dp->cbpm_raw_data_header.num_bunches[1] != 0) {
      tblock = 1;
    } else {
    if (debug_level >= CBI_DEBUG_LEVEL_4) {
      printf("Timing block could not be determined!\n");
    }//if
  }//if  

  for (i=0; i<4; i++) {
    prevChipValues[i] = 0;
  }
  
  prevChipValues[0] = cp->config_params.timing_config.setups[tsetup].chan_delays[tblock][0];
  prevChipValues[1] = cp->config_params.timing_config.setups[tsetup].chan_delays[tblock][1];
  prevChipValues[2] = cp->config_params.timing_config.setups[tsetup].chan_delays[tblock][2];
  prevChipValues[3] = cp->config_params.timing_config.setups[tsetup].chan_delays[tblock][3];
	 
  //memcpy(ChipValues, dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][0], 
  //	 sizeof(dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][0])*4);	 
  
  int *test = dp->cbpm_bfind_delay_out.chipshist[dp->cbpm_find_delay_out.num_retries+1];
  int size = sizeof(dp->cbpm_bfind_delay_out.chipshist[dp->cbpm_find_delay_out.num_retries+1][0]);

  memcpy(ChipValues, dp->cbpm_bfind_delay_out.chipshist[dp->cbpm_find_delay_out.num_retries+1], 
	 sizeof(dp->cbpm_bfind_delay_out.chipshist[dp->cbpm_find_delay_out.num_retries+1][0])*4);	 

  int min_start_delay = dp->cbpm_bfind_delay_out.chipshist[dp->cbpm_find_delay_out.num_retries+1][0];
  int max_start_delay = min_start_delay;

  for (int card = 0; card<4; card++) {
    for( chan = 0; chan < CBPM_MAX_CARD_ADCS_PER_BLOCK; chan++) {
      int addr = (card * CBPM_MAX_CARD_ADCS_PER_BLOCK) + chan;
      float *data  = &(dp->cbpm_chan_cal_work.chan_cal_data[card].data[0]);
      float *error  = &(dp->cbpm_chan_cal_work.chan_cal_data[card].error[0]);

      //FitValues[addr] = roundf(FitValues[addr]);

      	//dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][j][0]
      	
      //ChipValues[addr] = (int) roundf(FitValues[addr] + offsets[addr]);
      //}
      //else {
      //	ChipValues[addr] = dp->cbpm_find_delay_out.peaks[addr];
      //}	
		
      float y;
      minAmp = *(data + 0);
      maxAmp = *(data + 0);
      for(i=0; i<(2*CBPM_CAL_RADIUS+1); i++) {	 // added another point so the number is odd
		y = *(data + i);
		//printf("y = %f\n",y);

		if (y>maxAmp) {
		  maxAmp = y;
		}	
		if (y<minAmp) {
		  minAmp = y;
		}	
	    PerformTest(TEST_SIGMA_IS_ZERO, card, *(error + i), dp->cbpm_test_results.Statuses, dp);
      }
		
      int ISsaturated = maxAmp >= CBPM_ADC_SAT_THRESHOLD;
      //if (addr == 1) ISsaturated = 1;

      //Checks ADC Saturation
      PerformTest(TEST_COLLECT_RAW_ADC_SATURATED, addr, ISsaturated, dp->cbpm_test_results.Statuses, dp);		
		
      maxAmparr[addr] = maxAmp;

      //Check to see if the button value ever gets aboue High Threshold
      //printf("SIGNAL_TOO_HIGH, %i, %f, %f\n", addr, maxAmp, CBPM_ADC_HIGH_THRESHOLD);
      PerformTest(TEST_SIGNAL_TOO_HIGH, addr, maxAmp > CBPM_ADC_HIGH_THRESHOLD, dp->cbpm_test_results.Statuses, dp);
    
      //Check to see if the button value fails to ever get above a nominal value
      PerformTest(TEST_SIGNAL_TOO_LOW, addr, minAmp < CBPM_ADC_LOWER_THRESHOLD, dp->cbpm_test_results.Statuses, dp);
	  	
      //Check to see if minimum ever drops before a certain fraction of peak amplitude
      PerformTest2Input(TEST_OUTLIER_DETECTED, addr, fabs(maxAmp-minAmp),fabs(maxAmp), dp->cbpm_test_results.Statuses, dp);

      //if (FitValues[addr] != -999) {
	//Check to see if new chip value is too close to edges at 0 and 1023
	PerformTest(TEST_OFFSET_TOO_CLOSE_TO_HIGH_EDGE, addr, (float) offsets[addr], dp->cbpm_test_results.Statuses, dp);
	PerformTest(TEST_OFFSET_TOO_CLOSE_TO_LOW_EDGE,  addr, (float) offsets[addr], dp->cbpm_test_results.Statuses, dp);

	//Check to see if new chip fitter peak is too close to edges at 0 and 20
	PerformTest(TEST_FITVALUE_TOO_CLOSE_TO_HIGH_EDGE,  addr,  FitValues[addr], dp->cbpm_test_results.Statuses, dp);
	PerformTest(TEST_FITVALUE_TOO_CLOSE_TO_LOW_EDGE,   addr,  FitValues[addr], dp->cbpm_test_results.Statuses, dp);
      //}
    
      PerformTest(TEST_BEFORE_ADJUSTTIMING_ADC_RMS_TOO_BIG, addr, dp->cbpm_find_delay_out.prevADCRMSs[addr], dp->cbpm_test_results.Statuses, dp);	
      PerformTest(TEST_AFTER_ADJUSTTIMING_ADC_RMS_TOO_BIG,  addr, dp->cbpm_find_delay_out.postADCRMSs[addr], dp->cbpm_test_results.Statuses, dp);	    

      int delay = dp->cbpm_bfind_delay_out.chipshist[dp->cbpm_find_delay_out.num_retries+1][addr];
      if (delay<min_start_delay) {
		min_start_delay = delay; 
      }
      if (max_start_delay<delay) {
		max_start_delay = delay; 
      }

      //Check to see if 2nd order term in fit is within a reasonable range
      PerformTest2Input(TEST_FIT_CONCAVITY_TERM_TOO_HIGH,    addr, -fcoeffs[addr*3  ], fabs(maxAmparr[addr]), dp->cbpm_test_results.Statuses, dp);
      PerformTest2Input(TEST_FIT_CONCAVITY_TERM_TOO_LOW,     addr, -fcoeffs[addr*3  ], fabs(maxAmparr[addr]), dp->cbpm_test_results.Statuses, dp);

      //Check to see if Linear term in fit is within a reasonable range
      PerformTest2Input(TEST_FIT_LINEAR_TERM_TOO_HIGH,       addr, fcoeffs[addr*3+1], fabs(maxAmparr[addr]), dp->cbpm_test_results.Statuses, dp);
      PerformTest2Input(TEST_FIT_LINEAR_TERM_TOO_LOW,        addr, fcoeffs[addr*3+1], fabs(maxAmparr[addr]), dp->cbpm_test_results.Statuses, dp);

      //Check to see if Constant term in fit is within a reasonable range
      //PerformTest2Input(TEST_FIT_CONSTANT_TERM_TOO_HIGH,     addr, fcoeffs[addr*3+2], maxAmparr[addr], dp->cbpm_test_results.Statuses, dp);
      PerformTest2Input(TEST_FIT_CONSTANT_TERM_TOO_LOW,      addr, fcoeffs[addr*3+2], fabs(maxAmparr[addr]), dp->cbpm_test_results.Statuses, dp);
    
      //printf("Max amp for addr %i: %f\n", addr, maxAmparr[addr]);
		
      //Checks to see if chip value has changed too much since last time
      /*if (rollback_changes) {
        //Checks to see if chip value has changed too much since last time
        PerformTest(TEST_CHIP_CHANGED_TOO_MUCH, addr, 0, dp->cbpm_test_results.Statuses, dp);

        //Checks to see if chip value difference from Channel 0 has changed too much since last time - probably obsolete this?
        if (addr !=0 ){
	      PerformTest(TEST_CHIP_TO_CHIP_VALUE_CHANGED_TOO_MUCH, addr, 0, dp->cbpm_test_results.Statuses, dp);
        }
      }
      else
      {*/
        //Checks to see if chip value has changed too much since last time
        PerformTest(TEST_CHIP_CHANGED_TOO_MUCH, addr, abs( prevChipValues[addr] - ChipValues[addr] ), dp->cbpm_test_results.Statuses, dp);

        //Checks to see if chip value difference from Channel 0 has changed too much since last time - probably obsolete this?
        if (addr !=0 ){
	      PerformTest(TEST_CHIP_TO_CHIP_VALUE_CHANGED_TOO_MUCH, addr, 
		    abs( (prevChipValues[0]-prevChipValues[addr]) - (ChipValues[0]-ChipValues[addr]) ),
		    dp->cbpm_test_results.Statuses, dp);
        }
      //}

    }
  }
	
  /*
  int tblock;
	
  if (dp->cbpm_raw_data_header.num_bunches[0] != 0) {
    tblock = 0;
  } else if (dp->cbpm_raw_data_header.num_bunches[1] != 0) {
    tblock = 1;
  } else {
  }	*/
      
  //int block_delays_uninitialized = dp->cbpm_timing_config.setups[tsetup].block_delays[tblock] < 0;    
	
  //MGS TODO - move this test up to where the block delay gets set
  PerformTest(TEST_BLOCK_DELAY_TOO_CLOSE_TO_HIGH_EDGE, 0,  dp->cbpm_timing_config.setups[tsetup].block_delays[tblock], dp->cbpm_test_results.Statuses, dp);
  PerformTest(TEST_BLOCK_DELAY_TOO_CLOSE_TO_LOW_EDGE,  0,  dp->cbpm_timing_config.setups[tsetup].block_delays[tblock], dp->cbpm_test_results.Statuses, dp);   
  
  PerformTest(TEST_PEAK_FIT_SCAN_TOO_SLOW,  0, (float) dp->cbpm_find_delay_out.numTurnsToCollectPeakData, dp->cbpm_test_results.Statuses, dp);    

  //Check to see if all 4 cards choose a chip value somewhere near each other
  PerformTest(TEST_CHIPS_TOO_FAR_APART, 0, max_start_delay - min_start_delay, dp->cbpm_test_results.Statuses, dp);

  //Check to see if Diff of Crossed sum is too far from zero
  int DiffOfCrossSum = abs(maxAmparr[0] + maxAmparr[2] - ( maxAmparr[1] + maxAmparr[3] ));
  int AvgOfPeaks = abs(maxAmparr[0] + maxAmparr[1] + maxAmparr[2] + maxAmparr[3])/4;
	
  PerformTest2Input(TEST_DIFF_OF_CROSS_SUM_TOO_BIG, 0, DiffOfCrossSum, AvgOfPeaks, dp->cbpm_test_results.Statuses, dp);
  
  PerformTest(TEST_TOO_MANY_TRIES, 0, dp->cbpm_find_delay_out.num_retries, dp->cbpm_test_results.Statuses, dp);   

  *status = DetermineStatus(dp->cbpm_test_results.Statuses);
}

int DisplayStatus(int Array[CBPM_FIND_DELAYS_HIST_LENGTH][CBPM_MAX_ACQ_CONTROLLERS], CBPM_DATA *dp, int showFullArray, int showingInstr) {
			
  int i;
  char *test_numbers = "  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23\n";	
  int testsums[CBPM_NUM_TESTS];
  int testsumoffsets[CBPM_NUM_TESTS] = {0, 0, 0, 0, 3, 3, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3};
  int cardsums[CBPM_MAX_CARDS];
  int cardsumoffsets[CBPM_MAX_CARDS] = {1, 5, 5, 5};

  #include "cbpm_instrument_code_param.h"	

  if (showFullArray) {
	  printf("\n");
	  printf(test_numbers);
	  for(int test = 0; test < CBPM_NUM_TESTS; test++) {
		testsums[test] = 0;
	  }

	  for(int card= 0; card < CBPM_MAX_CARDS; card++) {
		cardsums[card] = 0;
	    for(int test = 0; test < CBPM_NUM_TESTS; test++) {
	      cardsums[card] += Array[test][card]-1;
	      testsums[test] += Array[test][card]-1;

	      printf("%3i ",Array[test][card]-1);
	    }//for

	    printf(" %3i",cardsums[card]+cardsumoffsets[card]);
	    printf("\n");
	  }//for

	  printf("\n");
	  for(int test = 0; test < CBPM_NUM_TESTS; test++) {
	  	printf("%3i ",testsums[test]+testsumoffsets[test]);
	  }
  	printf("\n\n");
  }
  //fflush(stdout);
  
  int found_one = CBI_FALSE;
  for(int test = 0; test < CBPM_NUM_TESTS; test++) {
  	for(int card= 0; card < CBPM_MAX_CARDS; card++) {
  		switch (Array[test][card]-1) {
  			case -1:
  			case 0:
  				continue;
  				break;
  			case 1:
	  			printf("\nTest %2i, Card %1i, Warning: %45s, ", test, card, tests[test].name);
	  			found_one = CBI_TRUE;
	  			break;
	  		case 2:
	  			printf("\nTest %2i, Card %1i, Error:   %45s, ", test, card, tests[test].name);
	  			found_one = CBI_TRUE;
	  			break;
	  		default:
	  			printf("\nTest %2i, Card %1i, Unknown Status value: %45s, ", test, card, tests[test].name);
	  			found_one = CBI_TRUE;
	  			continue;
  		}
      
      if (showingInstr) {
        printf("Val: %7.1f", dp->cbpm_btest_results.StatusVals[dp->cbpm_find_delay_out.num_retries+1][test][card]);
      } else {
        printf("W: %7.1f, E: %7.1f, Val: %7.1f",StatusWarns[test][card],StatusErrs[test][card], StatusVals[test][card]);
      }
  	}
  }
  if (found_one) {
  	printf("\n");
  }
}

int PerformTest(int test_index, int addr, float value, int StatusArray[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS], CBPM_DATA *dp)
{
	
  if (!dp->cbpm_code_params.tests[test_index].enable) {
    return CBPM_TEST_ENUM_UNINITIALIZED;
  }
  	
  float errorTest = dp->cbpm_code_params.tests[test_index].error_threshold;
  float warningTest = dp->cbpm_code_params.tests[test_index].warning_threshold;
  int typeTest = dp->cbpm_code_params.tests[test_index].type;
  int testResult = CBPM_TEST_ENUM_UNINITIALIZED;

  switch (typeTest) {
  case (CBPM_TEST_GREATERTHAN):
    if (value >= errorTest)		
      testResult = CBPM_TEST_ENUM_ERROR;
    else if (value >= warningTest)	
      testResult = CBPM_TEST_ENUM_WARNING;
    else						
      testResult = CBPM_TEST_ENUM_SUCCESS;

    break;
	
  case (CBPM_TEST_LESSTHAN):
    if (value <= errorTest)		
      testResult = CBPM_TEST_ENUM_ERROR;
    else if (value <= warningTest)	
      testResult = CBPM_TEST_ENUM_WARNING;
    else						
      testResult = CBPM_TEST_ENUM_SUCCESS;
	
    break;
  case (CBPM_TEST_EQUAL_ERROR):
    if (value == errorTest)
      testResult = CBPM_TEST_ENUM_ERROR;
    else
      testResult = CBPM_TEST_ENUM_SUCCESS;
    break;
  case (CBPM_TEST_EQUAL_WARNING):
    if (value == errorTest)
      testResult = CBPM_TEST_ENUM_WARNING;
    else
      testResult = CBPM_TEST_ENUM_SUCCESS;
    break;
		
  case (CBPM_TEST_TRUE_IS_WARNING):
    if (value == 1)
      testResult = CBPM_TEST_ENUM_WARNING;
    else
      testResult = CBPM_TEST_ENUM_SUCCESS;
    break;							
  case (CBPM_TEST_TRUE_IS_ERROR):
    if (value == 1)
      testResult = CBPM_TEST_ENUM_ERROR;
    else
      testResult = CBPM_TEST_ENUM_SUCCESS;
    break;							
  case (CBPM_TEST_FALSE_IS_ERROR):
    if (value == 0)
      testResult = CBPM_TEST_ENUM_ERROR;
    else
      testResult = CBPM_TEST_ENUM_SUCCESS;
    break;
									
  default:
    testResult = -999;
  }
  
  /*if (debug_level >= CBI_DEBUG_LEVEL_2) {  
    printf("Test i:%i: a:%i, v:%f, W:%f, E:%f, %i\n", test_index, addr, value, warningTest, dp->cbpm_code_params.tests[test_index].error_threshold, testResult);  
    }*/
	
  if (dp->cbpm_test_results.Statuses[test_index][addr] < testResult) {
    StatusArray[test_index][addr]  = testResult;
  }
  
  StatusVals[test_index][addr] = value;
  StatusErrs[test_index][addr] = errorTest;
  StatusWarns[test_index][addr] = warningTest;
	
  return testResult;	
}

int PerformTest2Input(int test_index, int addr, float value1, float value2, int StatusArray[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS], CBPM_DATA *dp)
// See if value1 is <inequality> value2*threshold
{

  float errorTest = dp->cbpm_code_params.tests[test_index].error_threshold;
  float warningTest = dp->cbpm_code_params.tests[test_index].warning_threshold;
  int typeTest = dp->cbpm_code_params.tests[test_index].type;
	
  if (!dp->cbpm_code_params.tests[test_index].enable) {
    return CBPM_TEST_ENUM_UNINITIALIZED;
  }	
  	
  int testResult = CBPM_TEST_ENUM_UNINITIALIZED;

  switch (typeTest) {
  case (CBPM_TEST_GREATERTHAN2INPUT):
    if (value1 >= value2*errorTest)		
      testResult = CBPM_TEST_ENUM_ERROR;
    else if (value1 >= value2*warningTest)	
      testResult = CBPM_TEST_ENUM_WARNING;
    else						
      testResult = CBPM_TEST_ENUM_SUCCESS;

    break;
	
  case (CBPM_TEST_LESSTHAN2INPUT):
    if (value1 <= value2*errorTest)		
      testResult = CBPM_TEST_ENUM_ERROR;
    else if (value1 <= value2*warningTest)	
      testResult = CBPM_TEST_ENUM_WARNING;
    else						
      testResult = CBPM_TEST_ENUM_SUCCESS;
	
    break;
  default:
    testResult = -999;
  }

  if (dp->cbpm_test_results.Statuses[test_index][addr] < testResult) {
    StatusArray[test_index][addr]  = testResult;
  }
  
  //float dispFactor = 1000.;
  if (debug_level >= CBI_DEBUG_LEVEL_2) {
    printf("Test i:%i: a:%i, v1/v2:%f, W:%f, E:%f, %i\n", test_index, addr, CBPM_TEST_DISPLAY_FACTOR*value1/value2, CBPM_TEST_DISPLAY_FACTOR*warningTest, CBPM_TEST_DISPLAY_FACTOR*errorTest, testResult);  
  }
  
  if (value2>0) {
    StatusVals[test_index][addr] = CBPM_TEST_DISPLAY_FACTOR*value1/value2;
  } else {
    StatusVals[test_index][addr] = -999;
  }
    
  StatusErrs[test_index][addr] = CBPM_TEST_DISPLAY_FACTOR*errorTest;
  StatusWarns[test_index][addr] = CBPM_TEST_DISPLAY_FACTOR*warningTest;  
	
  return testResult;
}

int DetermineStatus(int Status[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS]) {

  int maxStatus = 0;
  int card, test;
	
  for (int card = 0; card<CBPM_MAX_CARDS; card++) {
    for (int test = 0; test < CBPM_NUM_TESTS; test++) {
      if (maxStatus< Status[test][card])
	{
	  maxStatus = Status[test][card];
	}
    }
  }

  int returnStatus;

  switch (maxStatus) {
  case CBPM_TEST_ENUM_UNINITIALIZED :
  case CBPM_TEST_ENUM_SUCCESS :
    returnStatus = CBI_F_SUCCESS;
    break;
  case CBPM_TEST_ENUM_WARNING :
    returnStatus = CBI_F_WARNING;
    break;
  case CBPM_TEST_ENUM_ERROR :
    returnStatus = CBI_F_FAILURE;
    break;
  default:
    returnStatus = CBI_F_FAILURE;
    break;
  }

  return returnStatus;
}

//--------------------------------------
// Clipi
//
// clips a int between max and min
//--------------------------------------
int clipi(int val, int min, int max)
{
  if (val<min)
    return min;
  else if (val>max)
    return max;
  else
    return val;
      
}

//--------------------------------------
// Wrapi
//
// wraps a int between max and min
//--------------------------------------
int Wrapi(int val, int min, int max)
{
   if (val<min)
      return Wrapi(val+max, min, max);
   else if (val>max)
      return Wrapi(val-max, min, max);
   else
   	  return val;
      
}
