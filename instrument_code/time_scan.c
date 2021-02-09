//-----------------------------------------------------------------------+
// File         :  time_scan.c                                           |
//                                                                       |
// Description  :  Function to allow generation of updates to the block  |
//                 (timing block-level) and/or channel (ADC-level)       |
//                 delays by calling the calibration routine with        |
//                 parameters appropriate for a delay                    |
//                 determinatino/adjustment.  This process also provides |
//                 a buffer containing waveform reproductions for the    |
//                 global delay scan window that the control program may |
//                 read back if desired.                                 | 
//                                                                       |
// Param Inputs :  None                                                  |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                 CBI_F_FAILURE   - error                               |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int time_scan( void ) {
	
  // Add function ID to the trace
  int FID = TIME_SCAN_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;	
  
  int istat = CBI_F_SUCCESS;
  
  int i, j;
  
  int bunch, card, addr;
  
  volatile int *p_error = INST_cmd.error;
  
  CBPM_RAW_PARAMS raw_params;
  CBPM_CAL_PARAMS cal_params;
  
  int species = INST_cmd_params.species;
  
  // Load the CBI_RAW_PARAMS and CBI_CAL_PARAMS structs with
  // control system parameter inputs
  raw_params.num_turns        = INST_cmd_params.num_turns; 
  raw_params.scale_mode       = INST_cmd_params.scale_mode;
  raw_params.trig_turns_delay = INST_cmd_params.trig_turns_delay;  
  raw_params.spacex_turn      = INST_cmd_params.spacex_turn;
  //raw_params.use_data_enable  = INST_cmd_params.use_data_enable;
  raw_params.trig_mask        = INST_cmd_params.trig_mask;
  
  cal_params.gain_cal         = INST_cmd_params.gain_cal;
  cal_params.delay_init       = CBI_TRUE;  // needed for injection studies 
  cal_params.delay_cal        = INST_cmd_params.delay_cal;
  cal_params.gain_xcal        = INST_cmd_params.gain_xcal;
  
  int bunch_buckets[CBPM_MAX_TIMING_BLOCKS] = {0,0};
  
  
  // Check if pre-acquisition timing and/or gain calibrations were requested
  if (INST_cmd_params.delay_cal || INST_cmd_params.gain_cal) { 
    
    istat = do_calibration(raw_params, cal_params, bunch_buckets);
    
    if (istat == CBI_F_FAILURE) {
      cbpm_set_error(TIME_SCAN_DO_CAL_ERR1, p_error);
      INST_stat.num_levels--;
      return CBI_F_FAILURE;
    }
    
    //-----------------------------------------------------------
    // If delay corrections were requested, run another timing
    // scan WITHOUT delay corrections applied to provide the 
    // control application with a plot of the waveforms showing 
    // the results of the corrections just performed.
    //-----------------------------------------------------------
    if (INST_cmd_params.generate_delay_corrections == 1) {
      INST_cmd_params.generate_delay_corrections = 0;
      istat = do_calibration(raw_params, cal_params, bunch_buckets);
      INST_cmd_params.generate_delay_corrections = 1;
    }
    
    if (istat == CBI_F_FAILURE) {
      cbpm_set_error(TIME_SCAN_DO_CAL_ERR2, p_error);
      INST_stat.num_levels--;
      return CBI_F_FAILURE;
    }
    
  }
  
  // Decrement the trace
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
  
}

