//-----------------------------------------------------------------------------
// File         :  get_betatron_phase_A.c
//
// Description  :  This file defines the get_betatron phase funcionality.  
//                 It calls collect_raw_data to collect data and then processes 
//                 the phase word for each turn of data collected.
//
// NOTE: This file gets its own, separate set of compilation flags and options
//       to force 'double' precision floating point variables to be 64-bits
//       wide.  This is done only for this file and not for the entire project
//       because the project wide option is too disruptive and may cause
//       constants to be made double precision as well, slowing computations.
//-----------------------------------------------------------------------------

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"

double accumulator[CBPM_MAX_PHASE_DIM][CBPM_NUM_PHASE_COEFFS][CBPM_MAX_CARDS];
double avg_accumulator[CBPM_MAX_CARDS]; //new


int get_betatron_phase( void ) {

  // Add function ID to the trace
  int FID = GET_BETATRON_PHASE_ID;   
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  int species;
  int bunch;
  int sample, ph_off, dim, coeff, card;
  int ph_race_wait = 0;
  int offset = 0;
  int istat = CBI_F_SUCCESS;
  
  int loop_mode = CBI_TRUE;
  int tsetup = INST_op_timing.active_timing_setup;
  
  volatile int *p_error = INST_cmd.error;
  
  CBPM_RAW_PARAMS   raw_params;
  CBPM_CAL_PARAMS   cal_params;
  CBPM_PROC_PARAMS  proc_params;
  
  
  // Set up CBPM_RAW_PARAMS struct and load CBPM_CAL_PARAMS struct 
  // with control system inputs
  raw_params.num_turns        = CBPM_MAX_RAW_SAMPLES; 
  raw_params.scale_mode       = INST_cmd_params.scale_mode;
  raw_params.trig_turns_delay = 0;
  raw_params.spacex_turn      = 0;
  raw_params.use_data_enable  = INST_cmd_params.use_data_enable;
  raw_params.trig_mask        = INST_cmd_params.trig_mask;
  
  cal_params.gain_cal     = INST_cmd_params.gain_cal;
  // (NOTE:  for the moment, set delay_init ONLY if delay_cal is true) (not needed?)
  //cal_params.delay_init   = INST_cmd_params.delay_cal;
  cal_params.delay_cal    = INST_cmd_params.delay_cal;
  cal_params.gain_xcal    = INST_cmd_params.gain_xcal;   
  
  proc_params.avg_mode        = INST_cmd_params.avg_mode;
  
  
  //---------------------------------
  // Clear float result outputs 
  //---------------------------------
  for (offset = 0; offset < CBPM_NUM_PHASE_COEFFS; offset++) {
    for (ph_race_wait = 0; ph_race_wait < CBPM_HALF_TURN_OPTIONS; ph_race_wait++) {
      for (dim = CBPM_HORZ_DIM; dim <= CBPM_VERT_DIM; dim++) {         
	for (coeff = CBPM_COS_COEFF; coeff <= CBPM_SIN_COEFF; coeff++) {
	  for (card = 0; card < CBPM_MAX_CARDS; card++) {
	    INST_beta_phase_out.phase_results[offset][ph_race_wait][dim][coeff][card] = 0.0;
	  }
	} //endFOR coeff
      } // endFOR dim
    } // endFOR ph_race_wait
  } // endFOR offset
  
  
  species = INST_cmd_params.species;
  
  
  //---------------------------------------------------------------------
  // Obtain the index of the bunch requested.
  //---------------------------------------------------------------------
  int bunches[CBPM_MAX_TIMING_BLOCKS] = {0,0};
  int tblock, bunch_idx;
  
  for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
    
    if ( INST_raw_data_header.num_bunches[tblock] == 0 ) {
      continue;
    }
    
    if (timing_mode_G == m4ns) {
      bunpat_reduce_pre_4ns( bunches );
    } else {
      bunpat_reduce_pre( tblock, bunches );
    }
    
    bunch_idx = bunches[tblock];
    
  }
  
  // Determine the turns-offset and wait values to use.
  int stored_race_wait, stored_turns_offset;
  
  if (bunch_idx < INST_phase_config.phase_jump_boundaries[species][0]) {
    stored_turns_offset = INST_phase_config.phase_turn_offsets[species][0];
    stored_race_wait    = INST_phase_config.phase_wait_values[species][0];
  } else {
    stored_turns_offset = INST_phase_config.phase_turn_offsets[species][1];
    stored_race_wait    = INST_phase_config.phase_wait_values[species][1];
  }
  
  
  //---------------------------------------------------------------------
  // If requested, sample all allowed combinations of phase_race_wait 
  // and turn offsets otherwise, just calculate the accumulator 
  // quantities for the (predetermined) race_wait and turn_offset
  // settings needed for an accurate measurement.
  //---------------------------------------------------------------------
  for (ph_race_wait = 0; ph_race_wait < CBPM_HALF_TURN_OPTIONS; ph_race_wait++) {
    
    // Only single out the race_wait setting that is required for reporting
    // accurate phase angles, not the entire table that is used for installation
    // and setup.
    if ((INST_cmd_params.generate_phase_tables == FALSE) &&
	(stored_race_wait != ph_race_wait)       ) { 
      continue;
    }
    
    raw_params.phase_race_wait = ph_race_wait;
    
    //-------------------------     
    // Loop over turn-offsets  
    //-------------------------     
    for (offset = 0; offset < CBPM_NUM_DP_TURNS_OFFSET; offset++) {  // Max investigatable number of turn offsets 
      
      // Only single out the turn offset setting that is required for reporting
      // accurate phase angles, not the entire table that is used for installation
      // and setup.
      if ( (INST_cmd_params.generate_phase_tables == FALSE) &&
	   (abs(CBPM_MIN_DP_TURNS_OFFSET)+stored_turns_offset != offset)   ) {
	continue;
      }
      
      
      ph_off = offset + CBPM_MIN_DP_TURNS_OFFSET;
      
      
      //------------------------------
      // Clear the accumulator array
      //------------------------------
      for (dim = CBPM_HORZ_DIM; dim <= CBPM_VERT_DIM; dim++) {
	for (coeff = CBPM_COS_COEFF; coeff <= CBPM_SIN_COEFF; coeff++) {
	  for (card = 0; card < CBPM_MAX_CARDS; card++) {
	    accumulator[dim][coeff][card] = 0.0;
	  }
	}
      }
      
      
      //-------------------------------------
      // Collect the raw data
      //-------------------------------------
      istat = collect_raw(raw_params); 
      
      if (istat == CBI_F_FAILURE) {
	cbpm_set_error(GET_BETA_PHASE_DATA_ERR, p_error);
	INST_stat.num_levels--;
	return CBI_F_FAILURE;
      }
      
      //-------------------------------------
      // Incrementally load the accumulator 
      // array for each sample
      //-------------------------------------
      istat = NormalBetatron(CBPM_MAX_RAW_SAMPLES, ph_off);
      if (istat == CBI_F_FAILURE) {
	cbpm_set_error(GET_BETA_PHASE_ERR, p_error);
	INST_stat.num_levels--;
	return CBI_F_FAILURE;
      }
      
      
      
      //------------------------------------------------------------
      // Copy double precision accumulator values to float outputs
      //------------------------------------------------------------
      for (dim = CBPM_HORZ_DIM; dim <= CBPM_VERT_DIM; dim++) {
	for (coeff = CBPM_COS_COEFF; coeff <= CBPM_SIN_COEFF; coeff++) {
	  for (card = 0; card < CBPM_MAX_CARDS; card++) {
	    INST_beta_phase_out.phase_results[offset][ph_race_wait][dim][coeff][card] =
	      (float) accumulator[dim][coeff][card];
	  }
	}
      }
      
      
    } // endFOR turn number offsets
    
  } // endFOR phase_race_wait
  
  
  
  // Restore phase race condition wait flag
  //INST_cmd_params.phase_race_wait = CBI_FALSE;
  raw_params.phase_race_wait = CBI_FALSE;
  
  // Decrement the trace and return status
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
   
}  




//----------------------------------------------------------------------------
// NormalBetatron will take the data acquired by collect_raw_data and 
// accumulate the phase data for each plane, placing the final results in 
// INST_beta_phase_out.phase_results[]
//
// Inputs:  num_turns    - number of turns in raw data sample
//          phase_offset - number of turns offset between data and valid 
//                         phase value
//----------------------------------------------------------------------------
int NormalBetatron(int num_turns, int phase_offset) {

  // Add function ID to the trace
  int FID = NORMALBETATRON_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  int tsetup = INST_op_timing.active_timing_setup;
  
  int shift[] = {19, 10}; // Bit shifts of the phase word encoded on the
  // timing signal for HORiZontal and VERTical phase 
  // values respectively.
  
  int   phase;
  float coeff[CBPM_MAX_PHASE_DIM][CBPM_NUM_PHASE_COEFFS];
  ////int   temp_adc;
  int   temp_adc[CBPM_MAX_CARDS];
  float corrected_sum;
  
  int card, turn, dim, icoeff, first, last;
  
  volatile int *phase_ptr;
  
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    avg_accumulator[card] = 0.0;
  }
  
  //----------------------------------------------------
  // We potentially do less than "num_turns" of data 
  // because of the delay between button and phase data.
  //----------------------------------------------------
  if (phase_offset >= 0) {
    first     = 0;
    last      = num_turns - phase_offset;
  } else {
    first     = -phase_offset;
    last      = num_turns;
  }
  
  phase_ptr = &INST_ph_word_buf.ph_words[0];
  
  int tblock;
  //---------------------------------------------------
  // Phase measurements use a SINGLE BUNCH only.
  // Note which timing block has acquired this bunch.
  //---------------------------------------------------
  if (INST_raw_data_header.num_bunches[0] == 1) {
    tblock = 0;
  } else {
    tblock = 1;
  }
  
  float raw_dat;
  int gain_setting;
  int scale_mode = INST_cmd_params.scale_mode;
  
  // Only a single channel exists per timing block per card for BPMs.
  int chan       = 0; 
  
  
  //-------------------------------------------------
  // For the data associated with each VALID turn 
  // (turns that match raw data with corresponding 
  //  phase data)
  //-------------------------------------------------
  for (turn = first; turn < last; turn++) {
    
    
    //-------------------------------------------------
    // Get the phase values for each dimension     
    //-------------------------------------------------
    for (dim = CBPM_HORZ_DIM; dim <= CBPM_VERT_DIM; dim++) { 
      phase  = (*(phase_ptr + turn + phase_offset) >> shift[dim]) & CBI_PHASE_WORD_MASK; 
      coeff[dim][CBPM_COS_COEFF] = cos_table[phase];
      coeff[dim][CBPM_SIN_COEFF] = sin_table[phase];
    }
    
    
    //-------------------------------------------------
    // Generate the total button signal for this turn
    //-------------------------------------------------
    corrected_sum = 0.0;
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
      temp_adc[card]      =  sign_extend( *(p_adc_bufs[card][tblock] + turn) );
      avg_accumulator[card] += temp_adc[card];
      gain_setting  =  INST_op_gain.active_gain_settings[card][tblock];
      corrected_sum += correct_value( temp_adc[card], scale_mode, tsetup, gain_setting, tblock, card, chan );
    }
    
    
    //-------------------------------------------------
    // Now generate the accumulator values
    //-------------------------------------------------
    // Loop over horziontal and vertical 
    for (dim = CBPM_HORZ_DIM; dim <= CBPM_VERT_DIM; dim++) {
      
      // Loop over cosine and sine
      for (icoeff = CBPM_COS_COEFF; icoeff <= CBPM_SIN_COEFF; icoeff++) {
	
	// Loop over cards
	for (card = 0; card < CBPM_MAX_CARDS; card++) {
	  gain_setting = INST_op_gain.active_gain_settings[card][tblock];
	  raw_dat = temp_adc[card];
	  raw_dat = correct_value( raw_dat, scale_mode, tsetup, gain_setting, tblock, card, chan );
	  raw_dat = raw_dat / corrected_sum;
	  accumulator[dim][icoeff][card] += coeff[dim][icoeff] * raw_dat;
	}
	
      }
    }
    
    
  } // endFOR first --> last turn
  
  
  //----------------------------------------------------------------------------
  // Compute the average signal from each card over the total number of turns.
  //----------------------------------------------------------------------------
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    INST_beta_phase_out.signal[card] = avg_accumulator[card] / num_turns;
  }
  
  
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
  
}









