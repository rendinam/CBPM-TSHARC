//-----------------------------------------------------------------------+
// File         :  get_proc_data.c  (DSP routine)                        |
//                                                                       |
// Description  :  Takes raw data, processes it, and loads the processed |
//                 data buffer with reduced data.                        |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                 CBI_F_FAILURE   - error                               |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int get_proc_data ( void ) {
	
   // Add function ID to the trace
   int FID = GET_PROC_DATA_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;	
	
   int istat = CBI_F_SUCCESS;

   volatile int *p_error = INST_cmd.error;

   CBPM_RAW_PARAMS   raw_params;
   CBPM_PROC_PARAMS  proc_params;
   CBPM_CAL_PARAMS   cal_params;


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

   proc_params.avg_mode        = INST_cmd_params.avg_mode;

   //----------------------------------------------------------------
   // Save the data enable flag state and trigger mask state provided 
   // by the control system before doing a delay adjustment for 
   // peak alignment.  This speeds up the process greatly as the 
   // time scan does not need to be triggered or honor the 
   // data-enable.
   // Set the data enable flag for the time scan portion to FALSE.
   //----------------------------------------------------------------
   raw_params.use_data_enable = CBI_FALSE;
   raw_params.trig_mask       = TRIG_MASK_NONE;		
   

   //--------------------------------
   // Timing and/or gain adjustment
   //--------------------------------
   int dummy_bunch_bucket_return[2];
   istat = do_calibration(raw_params, cal_params, dummy_bunch_bucket_return);
   
   if (istat == CBI_F_FAILURE) {
      cbpm_set_error(GET_PROC_DATA_CAL_ERR, p_error);
      INST_stat.num_levels--;
      return CBI_F_FAILURE;
   }
   
   //---------------------------------------------------------------
   // Restore triggering and data-enable flags from control system.
   //---------------------------------------------------------------
   raw_params.use_data_enable  = INST_cmd_params.use_data_enable;
   raw_params.trig_mask        = INST_cmd_params.trig_mask;
   
   
   //---------------------------------------------------------------
   // Collect processed data (always fill the first summ_buf 
   // location with data summary).
   //---------------------------------------------------------------
   int summ_index = 0;
   // If requested from control system, reset the buffer index
   // for processed data acquisitions back to the beginning.
   if (INST_cmd_params.reset_proc_buf_idx == CBI_TRUE) {
   	 INST_proc_buf_header.write_ptr[0] = 0;
   }
   istat = collect_proc(summ_index, raw_params, proc_params, cal_params);
   if (istat == CBI_F_FAILURE) {
      cbpm_set_error(GET_PROC_DATA_ERR,p_error);
      INST_stat.num_levels--;
      return CBI_F_FAILURE;
   }

   
   // Decrement the trace and return status
   INST_stat.num_levels--;
   return CBI_F_SUCCESS;
   
}




