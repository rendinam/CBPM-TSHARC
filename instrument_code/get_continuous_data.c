//-----------------------------------------------------------------------+
// File         :  get_current_data.c                                    |
//                                                                       |
// Description  :  Continuously acquires data in all available buckets,  |
//                 and computes the sum of all buttons to reflect a value|
//                 proportional to the current for each bunch.           |
//                                                                       |
// Arguments    :  collection_mode - Allows selection of multibunch      |
//                  current monitoring, or single bunch continuous orbit |
//                  data collection.                                     |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                 CBI_F_FAILURE   - error                               |
// TODO: Rename file
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int get_continuous_data( int collection_mode ) {
	
  // Add function ID to the trace
  int FID = GET_CONTINUOUS_DATA_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  int istat = CBI_F_SUCCESS;
  
  int max_gain_setting;
  
  // Set parameters for the type of continuous 
  // acquisition mode specified.
  switch (collection_mode) {
  case CBPM_CONT_PROC:
    max_gain_setting = CBPM_MAX_GAIN_SETTING;
    break;
  case CBPM_CONT_CURR:
    max_gain_setting = CBPM_MAX_CURRMON_GAIN_SETTING; 
    break;
    
  default:
    return CBI_F_FAILURE;
  }
  
  volatile int *p_error = INST_cmd.error;
  
  CBPM_RAW_PARAMS   raw_params;
  CBPM_PROC_PARAMS  proc_params;
  CBPM_CAL_PARAMS   cal_params;
  
  int card, tblock;
  
  //---------------------------------------
  // Distribute the control system inputs
  //---------------------------------------
  raw_params.num_turns        = INST_cmd_params.num_turns;
  
  raw_params.scale_mode       = INST_cmd_params.scale_mode;
  raw_params.trig_turns_delay = INST_cmd_params.trig_turns_delay;
  raw_params.spacex_turn      = INST_cmd_params.spacex_turn;
  raw_params.use_data_enable  = INST_cmd_params.use_data_enable;
  raw_params.trig_mask        = INST_cmd_params.trig_mask;
  
  cal_params.gain_cal         = INST_cmd_params.gain_cal;
  cal_params.delay_init       = INST_cmd_params.delay_cal;
  cal_params.delay_cal        = INST_cmd_params.delay_cal;
  cal_params.gain_xcal        = INST_cmd_params.gain_xcal;
  
  
  //proc_params.num_samples     = INST_cmd_params.num_samples;
  proc_params.avg_mode        = INST_cmd_params.avg_mode;
  
  
  
  INST_proc_buf_header.active_buf = 0;
  // If requested from control system, reset the buffer index
  // for processed data acquisitions back to the beginning.
  if (INST_cmd_params.reset_proc_buf_idx == 1) {
    INST_proc_buf_header.write_ptr[0] = 0;
  }
  
  
  //---------------------------------------------------------------
  // Run the bunch calibration sequence (NOTE:  for the moment, 
  // set the delay initialization flag ONLY if delay_cal is true)
  //---------------------------------------------------------------
  //int dummy_bunch_bucket_return[2];
  //istat = do_calibration(raw_params, cal_params, dummy_bunch_bucket_return);
  //if (istat == CBI_F_FAILURE) {
  //     cbpm_set_error(GET_PROC_DATA_CAL_ERR, p_error);
  //  INST_stat.num_levels--;
  //  return CBI_F_FAILURE;
  //}
  
  
  
  //-------------------------------------------
  // Reset processed data buffer pass counter
  // before entering the filling loop.
  //-------------------------------------------
  INST_proc_sum_dat_buf.fill_count = 0;
  
  
  //---------------------------------------------------------------
  // Collect processed data (always fill the first summ_buf 
  // location with data summary).
  //---------------------------------------------------------------
  int summ_index = 0;
  
  while (CBI_TRUE) {
    
    istat = collect_proc(summ_index, raw_params, proc_params, cal_params);
    if (istat == CBI_F_FAILURE) {
      cbpm_set_error(GET_PROC_DATA_ERR,p_error);
      INST_stat.num_levels--;
      return CBI_F_FAILURE;
    }
    
    
    //-------------------------------------------------------
    // Adjust the gains as necessary based on the 
    // threshold checks performed in 
    // collect_proc()-->process_data()-->process_adc() above.
    //-------------------------------------------------------
    istat = auto_gain( CBI_TRUE, CBI_FALSE, max_gain_setting );
    if (istat == CBI_F_FAILURE) {
      cbpm_set_error(GET_CONT_AUTO_GAIN_ERR,p_error);
      INST_stat.num_levels--;
      return CBI_F_FAILURE;
    }    
    
    
    //-------------------------------------------------------
    // Check for an abort request from the control system.
    // This must take the form of a request for the IDLE 
    // command (IDLE_CMD).  This checks for a NEW handshake
    // as well as the IDLE_CMD value in the same way 
    // dispatch() examines the cbi_cmd struct prior to 
    // dispatching any new command.
    //-------------------------------------------------------
    if (INST_cmd.handshake == CBI_CMD_NEW_HANDSHAKE &&    
      INST_cmd.cmd  	== CBI_IDLE_CMD ) {            
      add_debug( FID, 314159, CBI_DEBUG_LEVEL_0 );					   
      INST_cmd.handshake = CBI_CMD_WORKING_HANDSHAKE; 
      cbpm_set_error(IDLE_COMMAND_LOOP_BREAK, p_error);
      break;                               
    }     
    
    
    //-------------------------------------------------------
    // Increment the fill counter each time a pass is made
    // through the necessary portion of the processed data
    // buffer of entries.  Also, update the gain values
    // being reported.
    //-------------------------------------------------------
    if (collection_mode == CBPM_CONT_CURR) {
      
      INST_proc_sum_dat_buf.fill_count++;
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
	for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
	  INST_proc_sum_dat_buf.gain_values[card][tblock] =
	    INST_op_gain.active_gain_settings[card][tblock];
	}
      }
      
    } else if (collection_mode == CBPM_CONT_PROC) {
      
      // Reset processed data buffer slot to first.
      INST_proc_buf_header.write_ptr[0] = 0;
      
    }	
    
  } //endWHILE infinite loop
  
  
  
  // Decrement the trace and return status
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
  
}

