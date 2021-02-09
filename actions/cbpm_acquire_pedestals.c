/*!
*-------------------------------------------------------------------------+
* File         :  cbpm_acquire_pedestals.c                                |
*                                                                         |
* Description  :  prep) Set up all necessary structures for performing a  |
*                 pedestal value determination on each channel in the     |
*                 device.                                                 |
*                                                                         |
*                 post) Read back status    and payload.                     |
*                                                                         |
* Arguments    :  None                                                    |
*                                                                         |
* Author       :  M. Rendina                                              |
*--------------------------------  -----------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_acquire_pedestals_prep( void ) {

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  int i;

  printf("***** PEDESTAL ACQUISITION *****\n");


  // Set bunch pattern ------------------------------------------------------
  cbi_clear_bp( dp->cbpm_cmd_params.bunch_pat );


  // CHANGE ME BACK TO BUNCH 1!
  //cbi_set_bunch( dp->cbpm_cmd_params.bunch_pat, 1 );
  cbi_set_bunch( dp->cbpm_cmd_params.bunch_pat, 51 );


  // Set the trigger mask
  dp->cbpm_cmd_params.trig_mask          = 0;

  // Get data fron consecutive turns.  (do not skip any)
  dp->cbpm_cmd_params.spacex_turn        = 0;
  dp->cbpm_cmd_params.delay_cal          = CBI_FALSE;       // NO timing calibration
  dp->cbpm_cmd_params.gain_cal           = CBI_FALSE;       // NO gain   calibration
  dp->cbpm_cmd_params.update_mode        = CBI_UPDATE;      // Cause the DSP to copy ped_cal_out to pedestal_config struct entries.
  dp->cbpm_cmd_params.avg_mode           = PROC_SIMPLE_AVG_MODE;
  dp->cbpm_cmd_params.scale_mode         = RAW_DATA;
  dp->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

  dp->cbpm_cmd_params.use_data_enable    = CBI_FALSE;
  dp->cbpm_cmd_params.trig_mask          = TRIG_MASK_A;

  // Set the number of turns to acquire
  dp->cbpm_cmd_params.num_turns = 1024;

  return CBI_F_SUCCESS;

}



int cbpm_acquire_pedestals_post(int iidx) {

  int stat;
  int debug_level = CTL_System.debug_verbosity;

  CBPM_DATA  *dp;

  dp = CTL_System.p_Module[iidx]->dsp_data;

  // Read back status and payload
  stat = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CMD_PARAMS_TAG,      CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  stat = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_BUF_HEADER_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  stat = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_BUF_TAG,        CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  stat = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_GAIN_CONFIG_TAG,     CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  stat = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_TIMING_TAG,       CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  stat = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_GAIN_TAG,         CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  stat = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PEDESTAL_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  if (debug_level > CBI_DEBUG_LEVEL_4) {
    cbpm_print_cmd_params_struct( iidx, stdout );
  }

  int active_buf = dp->cbpm_proc_buf_header.active_buf;
  int entry      = dp->cbpm_proc_buf_header.write_ptr[active_buf] - 1;

  int tsetup = dp->cbpm_op_timing.active_timing_setup;
  int ped_table_set;
  if (tsetup >= CBPM_MAX_MULTIGAIN_SETUPS) {
    ped_table_set = tsetup - CBPM_MAX_MULTIGAIN_SETUPS;
  } else {
    ped_table_set = tsetup;
  }
  printf("PEDESTAL ACQUISITION TIMING SETUP = %d [%s] \n", tsetup, tsetup_names[tsetup] );
  printf("PEDESTAL ACQUISITION COMBO TABLE  = %d \n", ped_table_set );

  printf("\n");

  int tblock, card, gsetting, chan = 0;


  int mg_setup;
  for ( mg_setup = 0; mg_setup < CBPM_MAX_MULTIGAIN_SETUPS; mg_setup++) {

    printf(" Timing setup group: %s\n", tsetup_names[ mg_setup ] );
    printf(" mg_setup = %d\n", mg_setup);

    printf("  PEDESTAL TABLE FOR %s (%s)\n", CTL_System.p_Module[iidx]->comm.ethernet.hostname, CTL_System.p_Module[iidx]->comm.ethernet.IP_address);
    for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
	for (gsetting = 0; gsetting < CBPM_MAX_GAINS; gsetting++) {
	  printf("% 7.3f  ", dp->cbpm_pedestal_config.tables[mg_setup].ped_table[tblock][card][gsetting] );
	}
	printf("\n");
      }
      printf("\n");
    }
    
    printf("  PED RMS TABLE FOR %s (%s)\n", CTL_System.p_Module[iidx]->comm.ethernet.hostname, CTL_System.p_Module[iidx]->comm.ethernet.IP_address);
    for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
	for (gsetting = 0; gsetting < CBPM_MAX_GAINS; gsetting++) {
	  printf("% 7.3f  ", dp->cbpm_pedestal_config.tables[mg_setup].ped_rms_table[tblock][card][gsetting] );
	}
	printf("\n");
      }
      printf("\n");
    }
    
    printf("\n\n");

  } // endFOR mg_setup


  return CBI_F_SUCCESS;

}
