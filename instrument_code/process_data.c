//-----------------------------------------------------------------------+
// File         :  process_data.c                                        |
//                                                                       |
// Description  :  Processes raw data and places the results in the      |
//                 processed data buffer.  Calls process_adc to process  |
//                 the raw data and then takes care of updating the      |
//                 processed data summary info.                          |
//                                                                       |
// Arguments    :  calc_avg_mode-Indicator of type of data treatment     |
//                               that process_adc() will perform.        |
//                 raw_params  - Pointer to RAW_PARAMS structure holding |
//                               data acquisition parameters.            |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                 CBI_F_FAILURE   - error                               |
//                                                                       |
// Authors      :  M. Palmer / M. Rendina                                |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int process_data(int calc_avg_mode, CBPM_RAW_PARAMS raw_params, int summ_idx) {	

   // Add function ID to trace
   int FID = PROCESS_DATA_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;
	
   int f_stat;
   int write_ptr;
   int active_buf;
   
   int i;
   int species, train, bunch;
   
   int tblock, card;
   int chan;
   
   volatile int *p_error = INST_cmd.error;

   int num_turns = raw_params.num_turns;
   
   
   //----------------------------------------------------
   // Prior to looping over bunches, always
   // re-initialize the block_bunch_tally array entries.
   //----------------------------------------------------
   block_bunch_tally[0] = 0;
   block_bunch_tally[1] = 0;
   
   
    
  
   //--------------------------------------------------------
   // For every bunch's worth of raw data that was acquired,
   // process the data and add the results to the processed 
   // data buffer as a new entry.
   //--------------------------------------------------------
   for (bunch = 0; bunch < INST_raw_data_header.tot_bunches; bunch++) {
   	
      // Get the active buffer index and the write_ptr
      active_buf = INST_proc_buf_header.active_buf;
      write_ptr  = INST_proc_buf_header.write_ptr[active_buf];

      // SINGLE PROCESSED DATA BUFFER
      INST_proc_buf.proc_data[write_ptr].turn_counter  = -1;

  
      //------------------
      // Process raw data
      //------------------
      f_stat = process_adc(calc_avg_mode,
                           bunch,
                           num_turns,
                           &INST_proc_buf.proc_data[write_ptr],
                           &INST_signal_stat);
                  
              
      if (f_stat == CBI_F_FAILURE) {
         cbpm_set_error(PROCESS_DATA_ERR, p_error);
         INST_stat.num_levels--;
         return CBI_F_FAILURE;
      }


      INST_proc_buf.proc_data[write_ptr].turn_counter  = INST_raw_data_header.turn_counter;

      //---------------------------------
      // Save summary data upon request  FIXME: enable
      //---------------------------------
      /*
      if (summ_idx >=0) {
      	 for (i = 0; i < CBPM_MAX_CARDS; i++ ) {
           INST_summ_buf.data[summ_idx].gain[i] = DSP_raw_data_header.gain[0][i];
           INST_summ_buf.data[summ_idx].gain[i] = DSP_raw_data_header.gain[1][i];
      	 }
         INST_summ_buf.data[summ_idx].delay         = DSP_raw_data_header.delay;
         INST_summ_buf.data[summ_idx].chan_delay    = DSP_raw_data_header.chan_delay;
               INST_summ_buf.data[summ_idx].tune_word = 
                  INST_proc_buf[active_buf].proc_data[write_ptr].tune_word;
   
         for (chan = 0; chan < CBPM_MAX_CARDS; chan++) {
            INST_summ_buf.data[summ_idx].signal[chan] = DSP_proc_buf[active_buf].proc_data[write_ptr].signal[chan];
         }
         for (chan = 0; chan < CBPM_MAX_CARDS; chan++) {
            INST_summ_buf.data[summ_idx].signal[chan] = DSP_proc_buf.proc_data[write_ptr].signal[chan];
         }
      
      }
      */
      
      //----------------------
      // Increment write_ptr 
      //----------------------
      if ( ++(INST_proc_buf_header.write_ptr[active_buf]) == CBPM_MAX_BUNCHES ) {
         INST_proc_buf_header.write_ptr[active_buf] = 0;
      }

            
   } // endFOR bunch
     
   
   
   // Decrement trace and return SUCCESS
   INST_stat.num_levels--;
   return CBI_F_SUCCESS;
}

