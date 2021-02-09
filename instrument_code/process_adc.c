//-----------------------------------------------------------------------+
// File         :  process_adc.c (DSP routine)                           |
//                                                                       |
// Description  :  Generates average and RMS quantities from a single    |
//                 bunch's worth of data from the appropriate bank of    |
//                 ADC buffers over the requested number of turns.       |
//                 It then inserts this statistical data into the        |
//                 selected processed data buffer 'proc_data' struct     |
//                 entry.                                                |
//                                                                       |
// Arguments    :  bunch     - The entry in the global arary 'block_list'|
//                             corresponding to the bunch to be examined.|
//                             I.e. the index                            |
//                             of the bunch to examine when traversing   |
//                             all high bits in the bunch patterns from  |
//                             beginning to end.                         |
//                 avg_mode  -                                           |
//                 num_turns - number of turns to process from the raw   |
//                             buffer                                    |
//                 *proc_data   - Pointer to PROC_DATA... struct         |
//                 *signal_stat - Pointer to SIGNAL_STAT... struct       |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                 CBI_F_FAILURE   - error                               |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int process_adc(int calc_avg_mode, int bunch, int num_turns, 
                CBPM_PROC_DATA   *proc_data, 
                CBPM_SIGNAL_STAT *signal_stat) {
  
  
  // Add function ID to trace
  int FID = PROCESS_ADC_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  
  // Local loop and accumulation variables
  int   turn;
  int   card;
  float dat, square, frac;
  float corrected_dat[CBPM_MAX_CARDS];
  
  // Variables that need to be cleared on each entry into function
  int    incoming_values[CBPM_MAX_CARDS];
  int    raw_abs_maxes[CBPM_MAX_CARDS];
  
  float  signal_sum[CBPM_MAX_CARDS];
  double signal_sq[CBPM_MAX_CARDS];
  float  frac_sum[CBPM_MAX_CARDS];
  float  frac_sq[CBPM_MAX_CARDS];
  
  int chan = 0;
   
  //------------------------------------
  // Determine the timing block to 
  // use for the bunch being examined.
  //------------------------------------
  int tblock;
  tblock = block_list[bunch];
  
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    raw_abs_maxes[card]   = -CBPM_ADC_LIMIT;
  }
  
  
  // Initialize raw abs max value array for all available channels.
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    incoming_values[card] = 0;
    signal_sum[card]      = 0.0;
    signal_sq[card]       = 0.0;
    frac_sum[card]        = 0.0;
    frac_sq[card]         = 0.0;
  }
  
  
  float  det_signal                 = 0.0;
  float  det_sum                    = 0.0;
  
  int tsetup = INST_op_timing.active_timing_setup;
  
  // DSP error array
  volatile int *p_error = INST_cmd.error;
  
  int gain_setting;
  int scale_mode   = INST_cmd_params.scale_mode;
  
  
  int comparison;
  
  
  
  //-----------------------------------------------------------------
  // Starting index depends on number of wrapped 
  // bunch pattern bits for this timing block.
  //-----------------------------------------------------------------
  int start = INST_raw_data_header.num_BP_bits_wrapped[tblock];
  
  
  //------------------------------------------------------------------
  // Examine and process all turns of data for the bunch in question.
  //------------------------------------------------------------------
  for (turn = 0; turn < num_turns; turn++) {
    
    
    det_signal = 0.0;
    //-----------------------------------------
    // Extract data from the front-end buffers
    //-----------------------------------------
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
	
      incoming_values[card]  = sign_extend( *(p_adc_bufs[card][tblock] + start + block_bunch_tally[tblock] + (turn * INST_raw_data_header.num_bunches[tblock])) ); 
      
      //-------------------------------------------------------------------------
      // Apply correction(s) to the value extracted from the ADC buffers.
      //-------------------------------------------------------------------------
      dat                          = incoming_values[card];
      gain_setting = INST_op_gain.active_gain_settings[card][tblock];
      corrected_dat[card]    = correct_value( dat, scale_mode, tsetup, gain_setting, tblock, card, chan );
      det_signal                  += corrected_dat[card];
      
      // [abs() broken in TigerSHARC system libraries!]
      if (incoming_values[card] < 0) {
	comparison = -(incoming_values[card]);
      } else {
	comparison = incoming_values[card];
      }
      
      // Keep track of the maximum absolute RAW value encountered on each card.
      if ( comparison > raw_abs_maxes[card]) {
	raw_abs_maxes[card] = comparison;
      }
      
    } //endFOR card
    
    det_sum += det_signal;
    
    if (calc_avg_mode != PROC_THRESHOLDS_ONLY) {
      //-------------------------------------------
      // Now calculate incoming signal quantities
      //-------------------------------------------
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
	  
	gain_setting = INST_op_gain.active_gain_settings[card][tblock];
   	
	//-------------------------------------
	// Get value from the next turn
	//-------------------------------------
	dat     = corrected_dat[card];
	square  = dat * dat;
	
        
	// Accumulate data
	signal_sum[card] += dat;
	signal_sq[card]  += square;
	
	// Fractional sum
	if (calc_avg_mode == PROC_JITTER_SUPPRESSION_MODE) {
	  if (det_signal == 0) {
	    det_signal = 1;
	  }
	  frac                  = dat/det_signal;
	  frac_sum[card] += frac;
	  frac_sq[card]  += frac*frac;
	}
   	
      } //endFOR card
      
    } //endIF proc_calc_mode == PROC_FULL_CALC
    
  } //endFOR turn
  
  block_bunch_tally[tblock]++;
  
  
  
  //---------------------------------------------------
  // Set the signal amplitude flags based on the RAW 
  // card maximum absolute values for this bunch.
  //---------------------------------------------------
  int statword;
  
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    
    // Set a flag external to the loop over bunches if a bunch is encountered
    // on a card that has a peak amplitude within the target range.
    
    if (raw_abs_maxes[card] != 0) {
      statword = INST_raw_data_header.ADC_zero_val_bitfields[card];
      INST_raw_data_header.ADC_zero_val_bitfields[card]  = unset_chan_stat(statword, tblock, chan);
    }
    
    if ( (raw_abs_maxes[card] > CBPM_ADC_LOWER_THRESHOLD)) {
      statword = INST_raw_data_header.ADC_lower_thresh_bitfields[card];
      INST_raw_data_header.ADC_lower_thresh_bitfields[card] = unset_chan_stat(statword, tblock, chan);
    }

    if ( (raw_abs_maxes[card] > CBPM_ADC_LOW_THRESHOLD) && (raw_abs_maxes[card] < CBPM_ADC_HIGH_THRESHOLD) ) {
      
      found_in_range[tblock][card] = CBI_TRUE;
      statword = INST_raw_data_header.ADC_low_thresh_bitfields[card];
      INST_raw_data_header.ADC_low_thresh_bitfields[card] = unset_chan_stat(statword, tblock, chan);
    }
    
    if ( raw_abs_maxes[card] >= CBPM_ADC_HIGH_THRESHOLD ) {
      
      INST_raw_data_header.ADC_high_thresh_bitfields[card] |= set_chan_stat(tblock, 0);
      // UN-set any low-threshold bit also. 
      statword = INST_raw_data_header.ADC_low_thresh_bitfields[card];
      INST_raw_data_header.ADC_low_thresh_bitfields[card] = unset_chan_stat(statword, tblock, chan);
      
    } else if ( raw_abs_maxes[card] <= CBPM_ADC_LOW_THRESHOLD && (found_in_range[tblock][card] == CBI_FALSE) ) {
      
      statword = INST_raw_data_header.ADC_low_thresh_bitfields[card];
      INST_raw_data_header.ADC_low_thresh_bitfields[card] |= set_chan_stat(tblock, chan);
    } 
    
    // Check for ADC saturation separately
    if ( raw_abs_maxes[card] >= CBPM_ADC_SAT_THRESHOLD) {
      INST_raw_data_header.ADC_saturation_bitfields[card]  |= set_chan_stat(tblock, chan);
      
    } 
  
      
  } //endFOR card
  
  
 
  if (calc_avg_mode != PROC_THRESHOLDS_ONLY) {
    //----------------------------------------------------------------------
    // Now calculate the final statistical quantities for each input signal
    //----------------------------------------------------------------------
    det_sum /= num_turns;
    for(card = 0; card < CBPM_MAX_CARDS; card++) {
      	
      // Get button average and RMS
      signal_sum[card] /= num_turns; 
      signal_sq[card]  /= num_turns;
      signal_sq[card]   = sqrtf(signal_sq[card] - signal_sum[card]*signal_sum[card]);
      
      // Store results in signal_stat struct
      signal_stat->signal_avg[card] = signal_sum[card];
      signal_stat->signal_rms[card] = signal_sq[card];
      
      // Store (and calculate, as necessary) results for proc_data struct
      if (calc_avg_mode == PROC_SIMPLE_AVG_MODE) {
	proc_data->signal[card]     = signal_sum[card];
	proc_data->signal_rms[card] = signal_sq[card];
	
      } else if (calc_avg_mode == PROC_JITTER_SUPPRESSION_MODE) {
	
	frac_sum[card] /= num_turns;
	frac_sq[card]  /= num_turns;
	proc_data->signal[card]     = det_sum * frac_sum[card];
	proc_data->signal_rms[card] = det_sum * sqrtf(frac_sq[card] - frac_sum[card]*frac_sum[card]);
      }  
      else {
	
	cbpm_set_error(PROCESS_ADC_AVG_ERR, p_error);
	INST_stat.num_levels--;
	return CBI_F_FAILURE;
	
      }
      
    } //endFOR card
    
    
    //--------------------------------------------------------------
    // Store button sum in the condensed processed data ring buffer
    // communications structure.
    //--------------------------------------------------------------
    INST_proc_sum_dat_buf.signal_sum[bunch] = 0.0;
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
      INST_proc_sum_dat_buf.signal_sum[bunch] += signal_sum[card]; 
    }
    
    
  } //endIF proc_calc_mode == PROC_FULL_CALC
  
  
  // Decrement the trace and return success
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
  
}









