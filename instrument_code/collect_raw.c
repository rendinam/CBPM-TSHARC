//-----------------------------------------------------------------------+
// File         :  collect_raw.c                                         |
//                                                                       |
// Description  :  Mid-level routine which calls collect_adc to take     |
//                 data and takes care of updating the raw_data_header.  |
//                                                                       |
// Arguments    :  RAW_PARAMS structure with acquisition paramters       |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                 INST_F_WARNING   - some channels out of tolerance     |
//                 CBI_F_FAILURE   - error                               |
//                                                                       |
// Author       :  M. Palmer                                             |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int collect_raw( CBPM_RAW_PARAMS params ) { 
	
  int  card, tblock, adc;
  int  f_stat;
  
  int chan = 0;

  volatile int *p_error = INST_cmd.error;
  
  // Set the function ID in the trace
  int FID = COLLECT_RAW_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  
  //-----------------------------------------------------
  // Initialize the raw_data_header prior to taking data
  //-----------------------------------------------------
  INST_raw_data_header.turn_counter =  0;
  
  for (card = 0; card < CBPM_MAX_CARDS; card++ ) {
    //-------------------------------------------------------------
    // Initialize the LOW THRESHOLD flag bitfields
    // to signify all low signal levels before begninning
    // any measurement.  
    // For automatic gain adjustment purposes
    // this is necessary for any measurements that only activate
    // bunches living on a SINGLE timing block since the threshold
    // information for the unused block is never populated and thus
    // automatic gain adjustments would otherwise employ faulty
    // threshold information.
    
    // This is here to deal with multi-bunch complications
    //-------------------------------------------------------------
    INST_raw_data_header.ADC_lower_thresh_bitfields[card] = 0;
    INST_raw_data_header.ADC_low_thresh_bitfields[card] = 0;
    INST_raw_data_header.ADC_zero_val_bitfields[card] = 0;
    // Initialize each channel's bit in low_thresh_bitfields to high.
    for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
      INST_raw_data_header.ADC_zero_val_bitfields[card] |= set_chan_stat(tblock, chan);
      INST_raw_data_header.ADC_lower_thresh_bitfields[card] |= set_chan_stat(tblock, chan);
      INST_raw_data_header.ADC_low_thresh_bitfields[card] |= set_chan_stat(tblock, chan);
    }
    INST_raw_data_header.ADC_high_thresh_bitfields[card] = 0;
    INST_raw_data_header.ADC_saturation_bitfields[card]  = 0;
    for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
      found_in_range[tblock][card]          = CBI_FALSE;
      INST_raw_data_header.gain[card][tblock] = 0;
    }
    
  }
  
  INST_raw_data_header.num_turns        = -1;
  INST_raw_data_header.scale_mode       = -1;
  INST_raw_data_header.trig_turns_delay = -1;
  INST_raw_data_header.spacex_turn      = -1;
  
  
  //-------------------------------------------------------------------
  // Fill ADC buffers
  //-------------------------------------------------------------------
  f_stat = collect_adc(params);
  
  
  if (f_stat == CBI_F_FAILURE) {
    cbpm_set_error(COLLECT_RAW_ERR,p_error);
    //INST_stat.num_levels--;
    //return CBI_F_FAILURE;
  }
  
  
  
  //-----------------------------------------------------
  // Update remaining raw_data_header members with values 
  // reflecting the examination of the data.
  //-----------------------------------------------------
  INST_raw_data_header.num_turns        = params.num_turns;
  INST_raw_data_header.trig_turns_delay = params.trig_turns_delay;
  INST_raw_data_header.spacex_turn      = params.spacex_turn;
  INST_raw_data_header.scale_mode       = params.scale_mode;
  
  
  //-----------------------------------------------------
  // Decrement the trace and return status value
  //-----------------------------------------------------
  INST_stat.num_levels--;
  return f_stat;
  
}

int check_raw_buf(CBPM_RAW_PARAMS params, int buffer_offset) {
	
  // Set the function ID in the trace
  int FID = CHECK_RAW_BUF_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;	
	
   int bunch, tblock, start, turn, card, chan, comparison;
   float det_signal;
   
   // Variables that need to be cleared on each entry into function
   int    incoming_values[CBPM_MAX_CARDS];
   int    raw_abs_maxes[CBPM_MAX_CARDS];   
   
   block_bunch_tally[0] = 0;
   block_bunch_tally[1] = 0;
   
   for (card = 0; card < CBPM_MAX_CARDS; card++) {
    INST_raw_data_header.ADC_high_thresh_bitfields[card] = 0;
    INST_raw_data_header.ADC_saturation_bitfields[card]  = 0;   
    INST_raw_data_header.ADC_lower_thresh_bitfields[card]  = 0;   
    INST_raw_data_header.ADC_low_thresh_bitfields[card]  = 0;   
   }
   
   for (card = 0; card < CBPM_MAX_CARDS; card++) {
   	  for (chan = 0; chan < CBPM_MAX_CARD_ADCS_PER_BLOCK; chan++) {
   	  	 raw_abs_maxes[card]   = -CBPM_ADC_LIMIT;
   	  }
   }   
	
	for (bunch = 0; bunch < INST_raw_data_header.tot_bunches; bunch++) {	
		
		
	   tblock = block_list[bunch];		   
	   
	   //-----------------------------------------------------------------
	   // Starting index depends on number of wrapped 
	   // bunch pattern bits for this timing block.
	   //-----------------------------------------------------------------
	   start = INST_raw_data_header.num_BP_bits_wrapped[tblock];	   	
	   	
	   //------------------------------------------------------------------
	   // Examine and process all turns of data for the bunch in question.
	   //------------------------------------------------------------------
	   for (turn = 0; turn < params.num_turns; turn++) {
   	
   	
	   	  det_signal = 0.0;
	   	  //-----------------------------------------
	   	  // Extract data from the front-end buffers
	   	  //-----------------------------------------
	   	  for (card = 0; card < CBPM_MAX_CARDS; card++) {

	            incoming_values[card]  = sign_extend( *(p_adc_bufs[card][tblock] + buffer_offset + start + block_bunch_tally[tblock] + (turn * INST_raw_data_header.num_bunches[tblock])) ); 
	            
	            comparison = abs(incoming_values[card]);
	            if (raw_abs_maxes[card] < comparison) {
	            	raw_abs_maxes[card] = comparison;
	            }
            
	   	  }
	   }
	   block_bunch_tally[tblock]++;
	   
	}
	
	for (bunch = 0; bunch < INST_raw_data_header.tot_bunches; bunch++) {
		tblock = block_list[bunch];	
	
		for (card = 0; card < CBPM_MAX_CARDS; card++) {
	
				if ( (raw_abs_maxes[card] <= CBPM_ADC_LOWER_THRESHOLD) ) {
					INST_raw_data_header.ADC_lower_thresh_bitfields[card] |= set_chan_stat(tblock, 0);
				}

				if ( (raw_abs_maxes[card] <= CBPM_ADC_LOW_THRESHOLD) ) {
					INST_raw_data_header.ADC_low_thresh_bitfields[card] |= set_chan_stat(tblock, 0);
				}

				if ( (raw_abs_maxes[card] >= CBPM_ADC_HIGH_THRESHOLD) ) {
					INST_raw_data_header.ADC_high_thresh_bitfields[card] |= set_chan_stat(tblock, 0);
				}

				if ( (raw_abs_maxes[card] >= CBPM_ADC_SAT_THRESHOLD) ) {
					INST_raw_data_header.ADC_saturation_bitfields[card] |= set_chan_stat(tblock, 0);
				}
		}
	}
            	
  //-----------------------------------------------------
  // Decrement the trace and return status value
  //-----------------------------------------------------
  INST_stat.num_levels--;	
  return CBI_F_SUCCESS;
}
