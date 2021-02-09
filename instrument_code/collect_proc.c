//-----------------------------------------------------------------------+
// File         :  collect_proc.c  (DSP routine)                         |
//                                                                       |
// Description  :  Mid-level routine which calls collect_raw to take raw |
//                 data, process_adc to process the raw data, and takes  |
//                 care of updating the processed data summary info      |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                 CBI_F_FAILURE   - error                               |
//                                                                       |
// Author       :  M. Palmer                                             |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int collect_proc (int summ_idx,
                  CBPM_RAW_PARAMS   raw_params,
                  CBPM_PROC_PARAMS  proc_params,
                  CBPM_CAL_PARAMS   cal_params)  {
                  	
   int sample;
   int f_stat;
   int return_val = CBI_F_SUCCESS;
   int write_ptr;
   int active_buf;
   volatile int *p_error = INST_cmd.error;

   int species, train, bunch;

   // Add function ID to the trace
   int FID = COLLECT_PROC_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;

   
   	  //------------------------
      // Collect the raw data
      //------------------------
      f_stat = collect_raw(raw_params);
      if (f_stat == CBI_F_FAILURE) {
         cbpm_set_error(COLLECT_PROC_CR_ERR,p_error);
         INST_stat.num_levels--;
         return CBI_F_FAILURE;
      }

      //--------------------------------------------------------------
      // Process raw data and store in the currently active PROC_BUF
      //--------------------------------------------------------------
      f_stat = process_data(proc_params.avg_mode, 
                            raw_params, summ_idx);
                            
                            
      if (f_stat == CBI_F_FAILURE) {
         cbpm_set_error(COLLECT_PROC_PD_ERR,p_error);
         INST_stat.num_levels--;   
         return CBI_F_FAILURE;
      }


      
   INST_stat.num_levels--;   
   return CBI_F_SUCCESS; 
}

