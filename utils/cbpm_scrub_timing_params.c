/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_scrub_timing_params.c                            |
*                                                                       |
* Description  :  Sets all block-dependent control system               |
*                 memory-resident timing parameters of a single         |
*                 instrument to the unitialized value of                |
*                 CBI_UNINITIALIZED_PARAM ('-1').                       |
*                                                                       |
* Arguments    :  Index of instrument in master structure array.        |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_scrub_timing_params_prep(void) {

  CBPM_DATA *dpt;
  dpt = CTL_System.p_Module_temp->dsp_data;

  int input;
  printf("Enter timing block to scrub [block A = 0,  block B = 1] : ");
  input = cbi_get_int();
  if (input == 0) {
    dpt->cbpm_cmd_params.tblock = 0;
  } else {
    dpt->cbpm_cmd_params.tblock = 1;
  }

  return F_SUCCESS;
}




int cbpm_scrub_timing_params_post( int iidx ) {
  
  int  tblock, card;
  int status;

  CBPM_DATA *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;
  
  int tsetup = dp->cbpm_op_timing.active_timing_setup;
  printf("TIMING setup = %d\n", tsetup);

  tblock = dp->cbpm_cmd_params.tblock;
  printf("SCRUB TIMING PARAMS for BLOCK %d\n", tblock);

  dp->cbpm_timing_config.setups[tsetup].BP_offsets[tblock]   = CBI_UNINITIALIZED_PARAM;
  dp->cbpm_timing_config.setups[tsetup].bTMDs[tblock]        = CBI_UNINITIALIZED_PARAM;
  dp->cbpm_timing_config.setups[tsetup].block_delays[tblock] = CBI_UNINITIALIZED_PARAM;
  
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    dp->cbpm_timing_config.setups[tsetup].chan_delays[tblock][card] = CBI_UNINITIALIZED_PARAM;
  }
  
  printf("Block delays = %d   %d\n", 
	 dp->cbpm_timing_config.setups[tsetup].block_delays[0],
	 dp->cbpm_timing_config.setups[tsetup].block_delays[1]   );

  // Send updated structure to instrument
  status = cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_TIMING_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);


  return CBI_F_SUCCESS;
}

