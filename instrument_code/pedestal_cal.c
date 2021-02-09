//------------------------------------------------------------------------
// File         :  pedestal_cal.c
//
// Description  :  Measure pedestal data with all ADCs for all
//                 available gain settings.
//------------------------------------------------------------------------

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int pedestal_cal( void ) {
	
  //-------------------------------------
  // Set routine ID in the trace
  //-------------------------------------
  int FID = PEDESTAL_CAL_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;	
  
  volatile int *p_error = INST_cmd.error;  
  
  int gain_setting, card;
  int original_gain_setting, start_gain_setting, end_gain_setting;
  int active_buf;
  int istat;
  int species      = INST_cmd_params.species;
  int num_turns    = INST_cmd_params.num_turns;
  
  int d[2]; // Array for obtaining number of bunches to digitize in each timing block.
  
  int tblock;
  int tsetup = INST_op_timing.active_timing_setup;
  int ped_table_set;
  
  int bunch;
  int orig_bbucket = 0;
  int nkept;

  CBPM_RAW_PARAMS params;


  if (tsetup >= CBPM_MAX_MULTIGAIN_SETUPS) {
    ped_table_set = tsetup - CBPM_MAX_MULTIGAIN_SETUPS;
  } else {
    ped_table_set = tsetup;
  }
  
  //-------------------------------------
  // Load the data collection parameters
  //-------------------------------------
  params.num_turns        = INST_cmd_params.num_turns;
  params.scale_mode       = RAW_DATA;
  params.trig_turns_delay = INST_cmd_params.trig_turns_delay;
  params.spacex_turn      = INST_cmd_params.spacex_turn;
  params.use_data_enable  = CBI_FALSE;
  
  
  INST_proc_buf_header.write_ptr[0] = 0;
  
  
  // Might need to put in something here to determine best bucket for performing a
  // pedestal calibration.  For now, it will only use bunch 0.
  
  // Save the gain setting used when entering this function.
  // NOTE: This only checks  CARD 0 -- BLOCK A, CHANNEL 0 due to gain settings being
  //       forced to be equal to one another across all cards/channels by convention.
  /* original_gain_setting = INST_op_gain.active_gain_settings[0][0][0]; */
  
  int fixed_gain;
  if ( (tsetup == s4ns_P_FIX_G)  ||
       (tsetup == s4ns_E_FIX_G)  ||
       (tsetup == s14ns_FIX_G) ) {
    fixed_gain = TRUE;
  } else {
    fixed_gain = FALSE;
  }
  
  
  // Determine which gain mode is active upon entering this routine.
  if (fixed_gain == TRUE) {
    start_gain_setting = 0;
    end_gain_setting   = 1;
  } else {
    start_gain_setting = 1;
    end_gain_setting   = CBPM_MAX_GAINS;
  }
  
  
  
  //------------------------------------------------------------
  // Loop over all gain settings appropriate to the gain mode
  // active when entering this routine.
  // Acquire under all available variable gain settings if a
  // variable gain setting is active.
  // Acquire a single time under the fixed gain setting if that
  // setting is active.
  //------------------------------------------------------------
  int new_tblock;
  for ( tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++ ) {
    
    for (gain_setting = start_gain_setting; gain_setting < end_gain_setting; gain_setting++ ) {
      
      // Set the gain on each card for this tblock's channel
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
        for (new_tblock = 0; new_tblock < CBPM_MAX_TIMING_BLOCKS; new_tblock++) {
	  INST_op_gain.active_gain_settings[card][new_tblock] = gain_setting;
	}
      }
      set_gains();
      
      //------------------------------------------------
      // Gather raw data (ignore return status for now)
      //------------------------------------------------
      istat = collect_raw(params);
      
      if (istat == CBI_F_FAILURE) {
	cbpm_set_error(PED_CAL_COLLECT_RAW_ERR, p_error);
	//INST_stat.num_levels--;
	//return CBI_F_FAILURE;
      }       
      
      
      //-------------------------------------------------
      // Process the data (ignore return status for now)
      //-------------------------------------------------
      istat = process_data(PROC_SIMPLE_AVG_MODE, params, -1);
      
      if (istat == CBI_F_FAILURE) {
	cbpm_set_error(PED_CAL_PROC_DATA_ERR, p_error);
	//INST_stat.num_levels--;
	//return CBI_F_FAILURE;
      }        
      
      
      // Copy the results to the private output structure.
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
	
	INST_ped_cal_out.ped_table[tblock][card][gain_setting]     = INST_signal_stat.signal_avg[card];
	INST_ped_cal_out.ped_rms_table[tblock][card][gain_setting] = INST_signal_stat.signal_rms[card];
	
	// If directed, copy the results to the global pedestal table
	if (INST_cmd_params.update_mode == CBI_UPDATE) {
	  INST_pedestal_config.tables[ped_table_set].ped_table[tblock][card][gain_setting]     = INST_signal_stat.signal_avg[card];
	  INST_pedestal_config.tables[ped_table_set].ped_rms_table[tblock][card][gain_setting] = INST_signal_stat.signal_rms[card];
	}
      }
      
    } // ENDfor gain_setting


    //------------------------------------------------------
    // First pass through timing blocks
    // Increment the bunch to examine.
    // This causes the tblock loop to use the other timing
    // block on each card, thus getting a pedestal value
    // for all ADCs present on the device.
    //------------------------------------------------------
    if (tblock == 0) { 
      for (bunch = 0; bunch < MAX_BUNCHES; bunch++) {
	if (INST_cmd_params.rot_bunch_pat[bunch] == 1) {
	  if ( bunch != (CBPM_MAX_BUNCHES-1) ) {
	    INST_cmd_params.rot_bunch_pat[bunch]   = 0;
	    INST_cmd_params.rot_bunch_pat[bunch+1] = 1;
	  } else { // If last bit is set (will usually not be the case), wrap.
	           // TODO: Is it necessary to keep wrapped bit from landing in the 26-bunch
	           // dead zone at the beginning of the pattern when the cTMD has been set
	           // correctly?
	    INST_cmd_params.rot_bunch_pat[bunch] = 0;
	    INST_cmd_params.rot_bunch_pat[0]     = 1;
	  }
	  orig_bbucket = bunch;
	  break;
	}
      }
      
      
      // Load the adjusted pattern into the bunch pattern registers.
      d[0] = d[1] = 0;
      nkept = load_bunch_pattern(d);
      INST_raw_data_header.num_bunches[0] = d[0];
      INST_raw_data_header.num_bunches[1] = d[1];
      
    } 
    
    
  } // ENDfor tblock
  
  
  //---------------------------------------
  // Decrement the trace and return status
  //---------------------------------------
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
  
}








