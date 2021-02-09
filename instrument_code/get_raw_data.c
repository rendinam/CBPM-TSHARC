//-----------------------------------------------------------------------+
// File         :  get_raw_data.c                                        |
//                                                                       |
// Description  :  Handles the acqusition of raw turn-by-turn data.      |
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


int get_raw_data ( void ) {
	
   // Add function ID to the trace
   int FID = GET_RAW_DATA_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;	

   int status = CBI_F_SUCCESS;
   
   int i, j;

   int bunch, card;
   
   volatile int *p_error = INST_cmd.error;

   CBPM_RAW_PARAMS  raw_params;
   CBPM_CAL_PARAMS  cal_params;
   CBPM_PROC_PARAMS proc_params;

   int species = INST_cmd_params.species;
   
   // Load the CBI_RAW_PARAMS and CBI_CAL_PARAMS structs with
   // control system inputs
   raw_params.num_turns        = INST_cmd_params.num_turns; 
   raw_params.scale_mode       = INST_cmd_params.scale_mode;
   raw_params.trig_turns_delay = INST_cmd_params.trig_turns_delay;  
   raw_params.spacex_turn      = INST_cmd_params.spacex_turn;
   raw_params.use_data_enable  = INST_cmd_params.use_data_enable;
   raw_params.trig_mask        = INST_cmd_params.trig_mask;
   
   cal_params.gain_cal        = INST_cmd_params.gain_cal;
   cal_params.delay_init      = CBI_TRUE;  // needed for injection studies 
   cal_params.delay_cal       = INST_cmd_params.delay_cal;
   cal_params.gain_xcal       = INST_cmd_params.gain_xcal;

   proc_params.avg_mode        = INST_cmd_params.avg_mode; 
   
   // Check if pre-acquisition timing and/or gain calibrations were requested
   if (INST_cmd_params.delay_cal || INST_cmd_params.gain_cal) { 
   	
     int bunch_bucket[CBPM_MAX_CHANS_PER_CARD];
     status = do_calibration(raw_params, cal_params, bunch_bucket);
     
     if (status == CBI_F_FAILURE) {
       cbpm_set_error(GET_RAW_DATA_CAL_ERR, p_error);
       //INST_stat.num_levels--;
       //return CBI_F_FAILURE;
     }
     
   }

   
   //----------------------
   // Collect the raw data
   //----------------------
   status = collect_raw(raw_params);
   if (status == CBI_F_FAILURE) {
      cbpm_set_error(GET_RAW_DATA_DAT_ERR, p_error);
      INST_stat.num_levels--;
      return CBI_F_FAILURE;
   }

   int summ_idx = 0;
   status = process_data(PROC_THRESHOLDS_ONLY, raw_params, summ_idx);
   if (status == CBI_F_FAILURE) {
      cbpm_set_error(GET_RAW_DATA_PROC_ERR, p_error);
      //INST_stat.num_levels--;
      //return CBI_F_FAILURE;
   }   
   
   // Decrement the trace and return status
   INST_stat.num_levels--;
   return CBI_F_SUCCESS;
}

