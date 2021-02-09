//-----------------------------------------------------------------------+
// File         :  auto_gain.c                                           |
//                                                                       |
// Description  :  Cycles through the available VARIABLE gain settings   |
//                 (settings > 0) and picks the one with best            |
//                 signal-to-noise characteristics.                      |
//                                                                       |
// Arguments    :                                                        |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                 CBI_F_FAILURE   - error                               |
//                                                                       |
// Author       :  M. Palmer / M. Rendina                                |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


#define   GAIN_CHECK_TURNS   256
#define   MAX_CAL_PASSES     10


int auto_gain(int single_pass, int collect_data, int max_gain_setting) {

   // Add function ID to the trace
   int FID = AUTO_GAIN_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;
   
   volatile int *p_error = INST_cmd.error;   
		
   int stat;
   int tblock, card;
   int status_word, chan_stat_low, chan_stat_high;
   int num_chans_OK;
   int blockchan    = 0;
   int cal_cnt      = 0;
   int changed_gain = CBI_FALSE;

   CBPM_RAW_PARAMS   raw_params;
   CBPM_PROC_PARAMS  proc_params;
   CBPM_CAL_PARAMS   cal_params;
   
   int gain_adj  = CBI_TRUE;
        
         
   // Skip any gain adjustment if the active gain setting is 0 (FIXED gain)
   if ( INST_op_gain.active_gain_settings[0][0] == 0 ) {
   	 INST_stat.num_levels--;
     return CBI_F_SUCCESS;
   }
   
   
   int summ_index = 0;
   
   //--------------------------------------
   // Loop until good gain settings found
   //--------------------------------------
   while (gain_adj == CBI_TRUE) {

   	  changed_gain = CBI_FALSE;
   	
   	  //--------------------------------------
      // Get a fresh sampling of data, if 
      // requested, and check it for 
      // threshold crossings.
      //--------------------------------------
      if (collect_data) {
      	
         raw_params.num_turns        = GAIN_CHECK_TURNS;
         raw_params.scale_mode       = RAW_DATA;
         raw_params.trig_turns_delay = 0;
         raw_params.spacex_turn      = 0;
         raw_params.use_data_enable  = CBI_FALSE;

         cal_params.gain_cal         = INST_cmd_params.gain_cal;
         cal_params.delay_init       = INST_cmd_params.delay_cal;
         cal_params.delay_cal        = INST_cmd_params.delay_cal;
         cal_params.gain_xcal        = INST_cmd_params.gain_xcal;

         proc_params.avg_mode        = INST_cmd_params.avg_mode;
         
         // Reset processed buffer index before every collection needed
         // for gain setting assessments.
         INST_proc_buf_header.write_ptr[0] = 0;
         
         stat = collect_proc(summ_index, raw_params, proc_params, cal_params);
         if (stat == CBI_F_FAILURE) {
         	cbpm_set_error(AUTO_GAIN_COLLECT_PROC_ERR, p_error);
         	   INST_stat.num_levels--;
         	   return CBI_F_FAILURE;
         }
      }



      //----------------------------------------------------------------
      // This adjustment method keeps all gains the same across all 
      // cards in a given timing block.
      //----------------------------------------------------------------
      int chan_stat_high[CBPM_MAX_TIMING_BLOCKS][CBPM_MAX_CARDS];
      int chan_stat_low[CBPM_MAX_TIMING_BLOCKS][CBPM_MAX_CARDS];
      
      // New method
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
      	 for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
      		
           status_word         = INST_raw_data_header.ADC_high_thresh_bitfields[card];
           chan_stat_high[tblock][card] = get_chan_stat(status_word, tblock, blockchan);
               
           status_word         = INST_raw_data_header.ADC_low_thresh_bitfields[card];
           chan_stat_low[tblock][card] = get_chan_stat(status_word, tblock, blockchan);
      	}
      }

      
      // Adjust the active gain settings as necessary.
      // Adjustments are ganged together across all cards and both timing blocks.
      if ( chan_stat_high[0][0] || chan_stat_high[0][1] || chan_stat_high[0][2] || chan_stat_high[0][3] ||
           chan_stat_high[1][0] || chan_stat_high[1][1] || chan_stat_high[1][2] || chan_stat_high[1][3] ) {
      	 if (INST_op_gain.active_gain_settings[0][0] > CBPM_MIN_GAIN_SETTING) {
      	 	for (card = 0; card < CBPM_MAX_CARDS; card++) {
               INST_op_gain.active_gain_settings[card][0]--;
               INST_op_gain.active_gain_settings[card][1]--;
      	 	}
            changed_gain = CBI_TRUE;
      	 }
      } else if ( (chan_stat_low[0][0] && chan_stat_low[0][1] && chan_stat_low[0][2] && chan_stat_low[0][3]) &&
                  (chan_stat_low[1][0] && chan_stat_low[1][1] && chan_stat_low[1][2] && chan_stat_low[1][3]) ) {
      	 if (INST_op_gain.active_gain_settings[0][0] < max_gain_setting) {
      	 	for (card = 0; card < CBPM_MAX_CARDS; card++) {
               INST_op_gain.active_gain_settings[card][0]++;
               INST_op_gain.active_gain_settings[card][1]++;
      	 	}
            changed_gain = CBI_TRUE;
      	 }
      }

      
      
      //----------------------------------------------------------
      // If all channels are OK (ie, either in tolerance or at 
      // their highest gain setting), the looping is done
      // Otherwise, we need to move to a new gain setting
      //----------------------------------------------------------
      if ( changed_gain ) {
      	
         set_gains();
  
      } else {
  
      	gain_adj = CBI_FALSE;

      }

      
      // Do NOT do multiple passes if in single-pass mode
      if (single_pass == CBI_TRUE) gain_adj = CBI_FALSE;
      
      
      // Declare an error if unable to converge
      /*if (cal_cnt++ == MAX_CAL_PASSES) {
         cbpm_set_error(AUTO_GAIN_CONVERGE_ERR, p_error);
         INST_stat.num_levels--;
         return CBI_F_FAILURE;
      }   */
       
      
   } // EndWHILE (gain_adj == CBI_TRUE)

   
   
   // Decrement the trace and return
   INST_stat.num_levels--;
   return CBI_F_SUCCESS;
    
}




