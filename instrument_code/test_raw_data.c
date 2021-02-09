//-----------------------------------------------------------------------
// File         :  test_raw_data.c
//
// Description  :  Loads up several structures with testing values to
//                 verify loading and communications behavior as well as
//                 other assorted testing snippets that need to be run
//                 on demand but do not warrant their own command 
//                 definition.
//-----------------------------------------------------------------------

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int test_raw_data ( void ) 
{
   
   // Add function ID to trace
   int FID = TEST_RAW_DATA_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;

   int i, acq_con;
   

   //-----------------------------
   //For calibrating magic wait
   //-----------------------------
   for (i = 0; i < 100; i++) {
     set_flag0_high();
     magic_wait( INST_diagnostic.magic_wait_calibration_value );
     set_flag0_low();
     magic_wait( INST_diagnostic.magic_wait_calibration_value );
   }
   
   
   //-----------------------------------------------------
   // Scrub front-end card ADC buffers (Timing Block 0/A)
   //-----------------------------------------------------
   int val = INST_diagnostic.scrub_value;
   
   for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      *p_acq_sram_ctrl[acq_con] = 0x0001;
   }
   
   for (i = 0; i < sizeof(INST_adc_buffer0); i++) {
   	  for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
   	  	 *p_acq_sram_data[acq_con] = val+i+acq_con;
   	  }
   }
   
   for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      *p_acq_sram_ctrl[acq_con] = 0;
   }
   
   
   //-----------------------------------------------------
   // Scrub front-end card ADC buffers (Timing Block 1/B)
   //-----------------------------------------------------
   for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      *p_acq_sram_ctrl[acq_con] = 0x0002;
   }
   
   
   for (i = 0; i < sizeof(INST_adc_buffer0); i++) {
   	  for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
   	  	 *p_acq_sram_data[acq_con] = val+i+CBPM_MAX_CARDS+acq_con;
   	  }
   }
   
   for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      *p_acq_sram_ctrl[acq_con] = 0;
   }
   
   
   INST_proc_sum_dat_buf.signal_sum[val] = val;
   
   add_debug( FID, 111, CBI_DEBUG_LEVEL_0 );
   add_debug( FID, INST_find_delay_params.hist_index, CBI_DEBUG_LEVEL_0 );
   add_debug( FID, INST_find_delay_params.use_data_enable, CBI_DEBUG_LEVEL_0 );

   add_debug( FID, INST_diagnostic.pre_pf_peak_block_delays[0], CBI_DEBUG_LEVEL_0 );
   add_debug( FID, INST_diagnostic.pre_pf_peak_block_delays[1], CBI_DEBUG_LEVEL_0 );
   add_debug( FID, INST_diagnostic.pre_pf_peak_block_delays[2], CBI_DEBUG_LEVEL_0 );
   add_debug( FID, INST_diagnostic.pre_pf_peak_block_delays[3], CBI_DEBUG_LEVEL_0 );
   add_debug( FID, INST_diagnostic.pre_pf_offset_chan_delays[0], CBI_DEBUG_LEVEL_0 );
   add_debug( FID, INST_diagnostic.pre_pf_offset_chan_delays[1], CBI_DEBUG_LEVEL_0 );
   add_debug( FID, INST_diagnostic.pre_pf_offset_chan_delays[2], CBI_DEBUG_LEVEL_0 );
   add_debug( FID, INST_diagnostic.pre_pf_offset_chan_delays[3], CBI_DEBUG_LEVEL_0 );
   add_debug( FID, 111, CBI_DEBUG_LEVEL_0 );
   
   magic_wait(100);
   
   INST_find_delay_params.hist_index     = INST_find_delay_params.hist_index + 1;
   INST_find_delay_params.use_data_enable    = INST_find_delay_params.use_data_enable + 1;
   
   
   INST_diagnostic.pre_pf_peak_block_delays[0] = INST_diagnostic.pre_pf_peak_block_delays[0] + 1;
   INST_diagnostic.pre_pf_peak_block_delays[1] = INST_diagnostic.pre_pf_peak_block_delays[1] + 1;
   INST_diagnostic.pre_pf_peak_block_delays[2] = INST_diagnostic.pre_pf_peak_block_delays[2] + 1;
   INST_diagnostic.pre_pf_peak_block_delays[3] = INST_diagnostic.pre_pf_peak_block_delays[3] + 1;
   INST_diagnostic.pre_pf_offset_chan_delays[0] = INST_diagnostic.pre_pf_offset_chan_delays[0] + 1;
   INST_diagnostic.pre_pf_offset_chan_delays[1] = INST_diagnostic.pre_pf_offset_chan_delays[1] + 1;
   INST_diagnostic.pre_pf_offset_chan_delays[2] = INST_diagnostic.pre_pf_offset_chan_delays[2] + 1;
   INST_diagnostic.pre_pf_offset_chan_delays[3] = INST_diagnostic.pre_pf_offset_chan_delays[3] + 1;
   
   
   int sample;

   
   magic_wait(100);
   
   // Decrement trace
   INST_stat.num_levels--;
   return CBI_F_SUCCESS;
   
}



