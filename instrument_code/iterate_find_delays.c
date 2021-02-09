//-----------------------------------------------------------------------+
// File         :  iterate_find_delays.c                                 |
//                                                                       |
// Description  :  Algorithms to interate over find_delays               |
//                                                                       |
// Author       :  M. Stedinger                                          |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"

int iterate_find_delays( CBPM_RAW_PARAMS raw_params) {

  // Add function ID to the trace
  int FID = ITERATE_FIND_DELAYS_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;  
  
  if (INST_cmd_params.generate_delay_corrections == 0) {
  	  INST_stat.num_levels--;
  	  return find_delays(raw_params);
  }
  
   int istat = CBI_F_SUCCESS;
   volatile int *p_error = INST_cmd.error;
  
   int i, j;

   int bunch, card, chan, addr, test;
   int hist_ind;
   int try_num = 0;   
   
     int tsetup = INST_op_timing.active_timing_setup;
     int bunch_bit_parity, bit;   
     
	   //------------------------------------------------------------
	   // Determine the time scan bunch bit's parity.
	   // This is the channel on which the data will appear.
	   // Remember, there is only a single bit enabled for timing 
	   // scans and timing calibrations in general.
	   //------------------------------------------------------------
	   for (bit = 0; bit < MAX_BUNCHES; bit++) {
	     if(INST_cmd_params.rot_bunch_pat[bit] == 1) {
	       if (bit % 2 == 0) {
		 bunch_bit_parity = 0;
		 break;
	       } else {
		 bunch_bit_parity = 1;
		 break;
	       }
	     }
	   }        
   
  
	   for (card = 0; card < CBPM_MAX_CARDS; card++) {
	     for (chan = 0; chan < CBPM_MAX_CARD_ADCS_PER_BLOCK; chan++) {
	     	addr = (card * CBPM_MAX_CARD_ADCS_PER_BLOCK) + chan;
	   		for (hist_ind = 0; hist_ind< CBPM_FIND_DELAYS_HIST_LENGTH; hist_ind++)  {
	   			INST_bfind_delay_out.peakshist[hist_ind][addr]   = 0.;
	   			INST_bfind_delay_out.offsetshist[hist_ind][addr] = 0;
	   			INST_bfind_delay_out.chipshist[hist_ind][addr]   = 0;
	   			INST_bfind_delay_out.blockhist[hist_ind]         = 0;
	   			INST_bfind_delay_out.rollbackhist[hist_ind] = 0;
	   		}//endFOR hist_ind
	     }//endFOR chan
	   }//endFOR card 
  
	   int k;
	   INST_find_delay_params.hist_index = 0;
	   for (card = 0; card < CBPM_MAX_ACQ_CONTROLLERS; card++) {
	   	for (chan = 0; chan < CBPM_MAX_CARD_ADCS_PER_BLOCK; chan++) {
	   		
	   		addr = (card * CBPM_MAX_CARD_ADCS_PER_BLOCK) + chan;
		   	INST_find_delay_out.peaks[addr] = 0.0;

		   	INST_bfind_delay_out.peakshist[INST_find_delay_params.hist_index][addr]   = 0.;
		   	INST_bfind_delay_out.chipshist[INST_find_delay_params.hist_index][addr]   = INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][card];
		   	INST_bfind_delay_out.offsetshist[INST_find_delay_params.hist_index][addr] = INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][card];
		   	INST_bfind_delay_out.blockhist[INST_find_delay_params.hist_index]         = INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity];;

			INST_find_delay_out.offsets[addr] 										  = INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][card];	
	     }//endFOR chan
	   }//endFOR card 
	   
	   for (card = 0; card < CBPM_MAX_ACQ_CONTROLLERS; card++) {
		for(test = 0; test < CBPM_NUM_TESTS; test++) {
			INST_test_results.Statuses[test][card] = CBPM_TEST_ENUM_UNINITIALIZED;
			
			for(k = 0; k<CBPM_FIND_DELAYS_HIST_LENGTH; k++) {
				INST_btest_results.Statuseshist[k][test][card] = CBPM_TEST_ENUM_UNINITIALIZED;
			}			
		}
	   }
      	
   	   INST_find_delay_params.hist_index++;   
   	   
	istat = find_delays( raw_params );
     
     if (istat == CBI_F_FAILURE) {
       cbpm_set_error(TIME_SCAN_DO_CAL_ERR1, p_error);
       //INST_stat.num_levels--;
       //return CBI_F_FAILURE;
     }    	   
  
	   
		int timing_status = INST_btest_results.statushist[INST_find_delay_params.hist_index];
	            
    while (INST_code_params.execution_flags[USE_TIME_SCAN_TESTS] && (timing_status >= CBI_F_FAILURE) && (try_num < INST_code_params.execution_flags[FIND_DELAYS_NUM_RETRIES])) {	 
			INST_find_delay_params.hist_index++;
			
		   for (card = 0; card < CBPM_MAX_ACQ_CONTROLLERS; card++) {
			for(test = 0; test < CBPM_NUM_TESTS; test++) {
				INST_test_results.Statuses[test][card] = CBPM_TEST_ENUM_UNINITIALIZED;
			}
		   }			

	        istat = find_delays( raw_params );
        
		     if (istat == CBI_F_FAILURE) {
		       cbpm_set_error(TIME_SCAN_DO_CAL_ERR2, p_error);
		       //INST_stat.num_levels--;
		       //return CBI_F_FAILURE;
		     }  
   			timing_status = INST_btest_results.statushist[INST_find_delay_params.hist_index];
			try_num++;
			
			PerformTest(TEST_TOO_MANY_TRIES, 0, try_num, INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
		}//while
    
	INST_find_delay_out.num_retries = try_num;
	INST_find_delay_params.hist_index++;     
		   
   PerformTest(TEST_TOO_MANY_TRIES, 0, try_num, INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);     
   
   INST_find_delay_params.hist_index = 0;
   INST_raw_data_header.turn_counter = INST_code_params.execution_flags[OUTPUT_TURN_COUNTER];
   	   
  INST_stat.num_levels--;
  return istat;
}
