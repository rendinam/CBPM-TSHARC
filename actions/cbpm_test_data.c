//-----------------------------------------------------------------------+
// File         :  cbpm_test_data.c                                      |
//                                                                       |
// Description  :  Functions for setting up and retrieving the payload   |
//                 of the test raw data command on each BPM.             |
//                                                                       |
// Arguments    :                                                        |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_test_data_prep(void) {
  
  int i, k, chan;

  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  printf("********* TEST RAW DATA ************\n");


  int val = 10;
  dp->cbpm_diagnostic. magic_wait_calibration_value = val;

  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  int retval;
  CBI_INIT_PROMPTS(1);

  prompts[0].supp_text_ptr    = NULL;
  prompts[0].prompt_text      = "Enter value to use for scrubbing  ";
  prompts[0].data_type        = CBI_INT;
  prompts[0].valid_inputs     = NULL;
  prompts[0].validator        = NULL;
  prompts[0].default_value    = 123;
  prompts[0].destination      = &(dp->cbpm_diagnostic.scrub_value);
  prompts[0].last_val_default = CBI_FALSE;


  retval = CBI_PROMPTER();


  dp->cbpm_find_delay_params.hist_index     = val;
  dp->cbpm_find_delay_params.use_data_enable     = val;

  dp->cbpm_diagnostic.pre_pf_peak_block_delays[0] = val;
  dp->cbpm_diagnostic.pre_pf_peak_block_delays[1] = val;
  dp->cbpm_diagnostic.pre_pf_peak_block_delays[2] = val;
  dp->cbpm_diagnostic.pre_pf_peak_block_delays[3] = val;
  dp->cbpm_diagnostic.pre_pf_offset_chan_delays[0] = val;
  dp->cbpm_diagnostic.pre_pf_offset_chan_delays[1] = val;
  dp->cbpm_diagnostic.pre_pf_offset_chan_delays[2] = val;
  dp->cbpm_diagnostic.pre_pf_offset_chan_delays[3] = val;
  

  return retval;

}





#define NUM_VALS_TO_RETRIEVE     100

int cbpm_test_data_post( int iidx ) {

  int i, k, chan;

  CBPM_DATA     *dp;

  // Assign pointers
  dp      = CTL_System.p_Module[iidx]->dsp_data;

  dp->cbpm_find_delay_params.hist_index    = -50;
  dp->cbpm_find_delay_params.use_data_enable     = -50;

  printf("            WIPE Find delay params check:\n");
  printf(" use_data_enable    = %d\n", dp->cbpm_find_delay_params.use_data_enable );
  printf(" hist_index    = %d\n", dp->cbpm_find_delay_params.hist_index );

  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER0_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER1_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER2_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER3_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER4_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER5_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER6_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER7_TAG, CTL_System.p_Module[iidx], NUM_VALS_TO_RETRIEVE );

  k=cbi_gp_struct(READ, ETHERNET, CBPM_FIND_DELAY_PARAMS_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  printf("             ETHERNET READBACK Find delay params check:\n");
  printf(" use_data_enable    = %d\n", dp->cbpm_find_delay_params.use_data_enable );
  printf(" hist_index    = %d\n", dp->cbpm_find_delay_params.hist_index );

  k=cbi_gp_struct(READ, XBUS, CBPM_FIND_DELAY_PARAMS_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  printf("             XBUS READBACK Find delay params check:\n");
  printf(" use_data_enable    = %d\n", dp->cbpm_find_delay_params.use_data_enable );
  printf(" hist_index    = %d\n", dp->cbpm_find_delay_params.hist_index );

  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_DIAGNOSTIC_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  int card;
  printf("\n\npre_pf_peak_block_delays: ");
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    printf("%d  ", dp->cbpm_diagnostic.pre_pf_peak_block_delays[card] );
  }
  printf("\n\npre_pf_offset_chan_delays: ");
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    printf("%d  ", dp->cbpm_diagnostic.pre_pf_offset_chan_delays[card] );
  }
  printf("\n\n\n");


  printf("ADC buffers\n");
  for (i = 0; i < NUM_VALS_TO_RETRIEVE; i++) {
    printf("%6d)  %3d %3d %3d %3d %3d %3d %3d %3d\n",
	   i,
	   cbpm_adc_buffer_get(dp, 0, 0, i),
	   cbpm_adc_buffer_get(dp, 0, 1, i),
	   cbpm_adc_buffer_get(dp, 1, 0, i),
	   cbpm_adc_buffer_get(dp, 1, 1, i),
	   cbpm_adc_buffer_get(dp, 2, 0, i),
	   cbpm_adc_buffer_get(dp, 2, 1, i),
	   cbpm_adc_buffer_get(dp, 3, 0, i),
	   cbpm_adc_buffer_get(dp, 3, 1, i) );
  }
  printf("\n");

  return CBI_F_SUCCESS;

}

