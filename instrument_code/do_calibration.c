//-----------------------------------------------------------------------+
// File         :  do_calibration.c                                      |
//                                                                       |
// Description  :  Controls the calibration sequence (gain and timing)   |
//                 for a bunch                                           |
//                                                                       |
// Param Inputs :  raw_params                                            |
//                 cal_params                                            |
//                 bunches - Integer array to receive the bunch          |
//                                 bucket(s) that were used to perform   |
//                                 the calibration procedure.            |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                 CBI_F_FAILURE   - error                               |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int do_calibration( CBPM_RAW_PARAMS raw_params, 
                    CBPM_CAL_PARAMS cal_params, int bunches[] ) {

   // Add function ID to the trace
   int FID = DO_CALIBRATION_ID; 
   INST_stat.trace[INST_stat.num_levels++] = FID;
                    	           	
   int stat = CBI_F_SUCCESS;
   int tblock, chan;

   volatile int *p_error = INST_cmd.error;
   
   
   //-----------------------------------------------
   // Each timing block gets examined in turn for
   // bunch assignments.
   //-----------------------------------------------
   for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
   	
   	  //--------------------------------------------
      // If a timing block has one or more bunches  
      // enabled, allow for timing and/or gain 
      // calibrations for that block. 
      //--------------------------------------------
   	  if ( INST_raw_data_header.num_bunches[tblock] == 0 ) {
   	    continue;
   	  }
   	  
      //------------------------------------------------------------------------
      // Calibration options:
      // 1) If BOTH gain_adj and delay_cal flags set, perform both actions.
      //    In addition, if the real-time gain cross-cal flag is set, activate 
      //    that functionality.
      // 2) If ONLY the gain_adj flag is set, then use the auto_gain function 
      //    to set all channels to the gain with best S/N such that we are not 
      //    too close to saturation on any channel.
      // 3) If ONLY the delay_cal flag is set, we assume that the present gain 
      //    settings are valid and simply apply channel delay corrections such 
      //    that the instrument will digitize at the peak of the waveform.  If 
      //    saturation is encountered during this process, the find_delays 
      //    routine will return an error.
      //------------------------------------------------------------------------
      if ((cal_params.gain_cal  == CBI_TRUE) && (cal_params.delay_cal == CBI_TRUE) ) {
      	
         // TODO: Allow for 'cross-calibration', if specified
         //              Multi-pass  Collect data
         stat = auto_gain(CBI_FALSE, CBI_TRUE, CBPM_MAX_GAIN_SETTING);
         
         if (stat == CBI_F_FAILURE) {
            cbpm_set_error(DO_CAL_AUTO_GAIN_ERR1, p_error);
            //INST_stat.num_levels--;
            //return CBI_F_FAILURE;
         } 
                  
         if (timing_mode_G == m4ns) {
         	bunpat_reduce_pre_4ns( bunches );
         } else {
   	        bunpat_reduce_pre( tblock, bunches );
         }                       
         stat = iterate_find_delays( raw_params );
         
         if (stat == CBI_F_FAILURE) {
            cbpm_set_error(DO_CAL_AUTO_FIND_DELAY1, p_error);
            //INST_stat.num_levels--;
            //return CBI_F_FAILURE;
         }          
                 
      } else if (cal_params.gain_cal  == CBI_TRUE) { 
   	
        //              Multi-pass  Collect data
        stat = auto_gain(CBI_FALSE, CBI_TRUE, CBPM_MAX_GAIN_SETTING);
        
         if (stat == CBI_F_FAILURE) {
            cbpm_set_error(DO_CAL_AUTO_GAIN_ERR2, p_error);
            //INST_stat.num_levels--;
            //return CBI_F_FAILURE;
         }         
     
      } else if (cal_params.delay_cal == CBI_TRUE) { 
     	
      	 if (timing_mode_G == m4ns) {
            bunpat_reduce_pre_4ns( bunches );
         } else {
   	        bunpat_reduce_pre( tblock, bunches );
         }
         stat = iterate_find_delays( raw_params );
         
         if (stat == CBI_F_FAILURE) {
            cbpm_set_error(DO_CAL_AUTO_FIND_DELAY2, p_error);
            //INST_stat.num_levels--;
            //return CBI_F_FAILURE;
         }          
      
      } else {
   	
         stat = CBI_F_SUCCESS;
      
      }
   
   
      //------------------------------------------------
      // Undo the bunch pattern reduction if it 
      // was performed above.
      //------------------------------------------------
      if (cal_params.delay_cal == CBI_TRUE) {
         bunpat_reduce_post();
      }
      
      
   } //endFOR tblock
      
      
   //----------------------------------------
   // Decrement the trace and return status
   //----------------------------------------
   INST_stat.num_levels--;
   
   if (stat == CBI_F_FAILURE) {
     cbpm_set_error(DO_CAL_ERR, p_error);
   }
   	
   return stat;
}

