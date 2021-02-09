//-----------------------------------------------------------------------+
// File         :  find_bunch.c                                          |
//                                                                       |
// Description  :  Support routines for performing a timing scan to find |
//                 which timing bunch contains "train 1, bunch 1"        |
//                 The machine must be running with only one bunch for   |
//                 this to work correctly.                               |
//                                                                       |
// Author       :  M.Rendina                                             |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int find_bunch( void ) {

  // Add function ID to the trace
  int FID = FIND_BUNCH_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  volatile int *p_error = INST_cmd.error;      
  
  int reg, bun, bunch, delay, card, acq_con;    // Counters  
  int istat = CBI_F_SUCCESS;
  
  int tsetup = INST_op_timing.active_timing_setup;
  int species = INST_cmd_params.species;
  
  //----------------------------------------------
  // Accomodate different numbers of samples
  // per bunch for different bunch spacing modes.
  //----------------------------------------------
  int MAX_DELAY, STEP_SIZE, DELAY_STEPS;
  int num_bunches = 0;
  
  int tblock;
  tblock = INST_cmd_params.tblock;
  
  
  if (timing_mode_G == m4ns) {
    DELAY_STEPS = CBPM_4NS_FIND_BUNCH_STEPS;
    MAX_DELAY   = CBPM_4NS_BUCKET_LEN;
  } else if (timing_mode_G == m14ns) {
    DELAY_STEPS = CBPM_14NS_FIND_BUNCH_STEPS;
    MAX_DELAY   = CBPM_14NS_BUCKET_LEN;
  } else {
    cbpm_set_error(FB_UNKNOWN_TIMING_MODE_ERR, p_error);
    // Decrement the trace level
    INST_stat.num_levels--;  
    return CBI_F_FAILURE;
  }   		
  
  num_bunches = MAX_BUNCHES/2;
  STEP_SIZE   = (int)(MAX_DELAY/DELAY_STEPS);
  
  INST_raw_data_header.num_bunches[tblock] = num_bunches;
  
  //-------------------------
  // Acquisition parameters
  //-------------------------
  CBPM_RAW_PARAMS local_raw_params= {INST_cmd_params.num_turns,
				     RAW_DATA,  // scale_mode
				     0,         // trig_turns_delay
				     0,         // spacex_turn
				     CBI_FALSE, // use_data_enable
				     0};        // trig_mask
  
  
  int num_turns = local_raw_params.num_turns;
  
  
  if (timing_mode_G == m4ns) {
    //----------------------------------------------
    // Set bunch pattern registers to ALL ENABLED
    // since this is a simple way of enabling the
    // EVEN bits for the scan performed below.
    //  Should be possible to simplify this to just
    // enabling all bits for all timing modes, the
    // hardware itself will take care of only 
    // sampling the correct number of bunches for 
    // each mode.
    //----------------------------------------------
    ////REORG-new////
    for (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      for(reg = 0; reg < MAX_BP_REGS-1; reg++) {
	*(p_acq_bunch_patterns[acq_con]+reg) = MAX_BP_REG_VAL;
      }
    }
    // Last register MAY be shorter than the rest
    for (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      *(p_acq_bunch_patterns[acq_con]+(MAX_BP_REGS-1)) = MAX_SHORT_BP_REG_VAL;
    }
    ////\REORG-new////
    
  }
  
  if (timing_mode_G == m14ns) {
    //--------------------------------------------------------------
    // Enable 366 (183 x 2species) bits in pattern registers 
    // (16-bit registers)
    //-------------------------------------------------------------- 
    ////REORG-new////
    for (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      for(reg = 0; reg < CBPM_14NS_BUNCH_PAT_REGS; reg++) {
	*(p_acq_bunch_patterns[acq_con]+reg) = MAX_BP_REG_VAL;
      }
    }
    // Last register in this timing mode has fewer bits.
    for (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      *(p_acq_bunch_patterns[acq_con]+(CBPM_14NS_BUNCH_PAT_REGS)) = CBPM_14NS_SH_BP_REG_VAL;
    }
    ////\REORG-new////      
    
  }

  
  //---------------------------------------------------------
  // Set the gain for each channel in a single timing block.
  //---------------------------------------------------------
  int starting_CD;
  
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    *p_chan_delays[card][tblock] = starting_CD = CBPM_DELAY_MID;
  }
  
  
  //---------------------------------------------------------------------
  // Request an acquisition for each delay setting in the range of the
  // global delay scan.  ADC buffers get filled in the following fashion:
  //
  //  Dx  = delay step
  //  Bx  = bunch
  //  Tux = turn
  //
  //  |             |                         |           |            |
  //  +---+---+---+ +---+---+---+  ---+---+---+---+---+---+ ---+---+---+
  //  | D0| D0| D0| | D0| D0| D0|   D1| D1| D1| D1| D1| D1|  D2| D2| D2| 
  //  |B0 |B..|BN | |B0 |B..|BN |  B0 |B..|B0 |B1 |B..|B1 | B0 |B1.|B0 | o o o 
  //  |Tu0|Tu0|Tu0| |Tu1|Tu1|Tu1|  Tu0|Tu0|Tu0|Tu1|Tu1|Tu1| Tu0|Tu0|Tu1|
  //  +---+---+---+ +---+---+---+  ---+---+---+---+---+---+ ---+---+---+
  //
  //---------------------------------------------------------------------
  int dstep, turn, buffer_offset;
  int lblock         = 0;
  
  for(dstep = 0;  dstep < DELAY_STEPS;  dstep++) {
    
    lblock += STEP_SIZE;
    set_block_delay(tblock, lblock);
    
    buffer_offset = dstep * num_bunches * num_turns;
    memfirsts( buffer_offset );
    
    //------------------------------
    // *** FILL THE ADC BUFFERS ***
    //------------------------------
    istat = collect_adc( local_raw_params );
    
    if (istat == CBI_F_FAILURE) {
      cbpm_set_error(FB_COLLECT_ADC_ERR,p_error);
      //INST_stat.num_levels--;
      //return CBI_F_FAILURE;
    }
    
  }
  
  // Reset mem_first
  memfirsts( 0 );
  
  
  //----------------------------------------
  // For each delay step, accumulate all 
  // turns worth of data for each bunch. 
  //----------------------------------------
  // Holds average of each bunch over turns for a given delay setting   
  float bunch_accum[CBPM_MAX_CARDS][320];
  float max_signal;
  
  int bunch_of_max[CBPM_MAX_CARDS];
  int gdelay_of_max[CBPM_MAX_CARDS];
  
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
          
    if (species == POSITRONS) {
      max_signal = -500000.0;
    } else {
      max_signal =  500000.0;
    }
    
    bunch_of_max[card] = -1;
    gdelay_of_max[card] = 0;
    
    for(dstep = 0; dstep < DELAY_STEPS; dstep++) { 
      
      // Initialize accumulator array
      for (bun = 0; bun < 320; bun++) {
	bunch_accum[card][bun] = 0.0;
      }
      
      // Generalize to at least get an initial CD guess for EACH signal input.
      
      // Accumulate all turns for each bunch (on a single card)
      for(bunch = 0;  bunch < num_bunches;  bunch++) {
	for(turn = 0; turn < num_turns; turn++) {
	  if (tblock == 0) {
	    bunch_accum[card][bunch] += *(p_adc_bufs[card][0] + (dstep*num_bunches*num_turns) + (turn*num_bunches) + bunch);
	  } else {
	    bunch_accum[card][bunch] += *(p_adc_bufs[card][1] + (dstep*num_bunches*num_turns) + (turn*num_bunches) + bunch);
	  }
	}
      }
      // Compute average of each bunch's data over turns.
      for(bunch = 0;  bunch < num_bunches;  bunch++) {
	bunch_accum[card][bunch] /= num_turns;
	// determine the bunch with largest signal at this delay step
	if ( (species == POSITRONS) && (bunch_accum[card][bunch] > max_signal) ||
	     (species == ELECTRONS) && (bunch_accum[card][bunch] < max_signal) )  {
	  max_signal    = bunch_accum[card][bunch];
	  bunch_of_max[card]  = bunch;
	  gdelay_of_max[card] = dstep * STEP_SIZE;
	}
      }
      
    } //endFOR dstep
    
  } //endFOR card

  
  // max_signal now holds the largest signal found out of all
  // averages.
  
  // bunch_of_max[CARDS][CHANNELS] holds the bunch where the largest average
  // signal was found for each card
  
  //----------------------------------------------------------------
  // Determine which bunch value occurs most over all signal inputs
  //----------------------------------------------------------------
  int bunch_candidates[CBPM_MAX_CARDS];
  // Initialize bunch_candidates array
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    bunch_candidates[card] = 0;
  }
  // Get a tally of how many times each potential bunch value appears 
  // over all signal inputs ('channels').
  int cardslot;
  for (cardslot = 0; cardslot < CBPM_MAX_CARDS; cardslot++) {
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
      if (bunch_of_max[card] == bunch_of_max[cardslot]) {
	bunch_candidates[cardslot]++;
      }
    }
  }
  
  // Note the channel that belongs to the group that provided the 'most popular'
  // bunch value determination.  Use that bunch value as the solution below.
  int max_hits  = 0;
  int best_card = 0;
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    if (bunch_candidates[card] > max_hits) {
      max_hits  = bunch_candidates[card];
      best_card = card;
    }
  }
  
  //------------------------------------------------------------
  // Save bunch pattern offset to DSP global variable and make 
  // it available to the control system through the cmd_params
  // structure.
  //------------------------------------------------------------
  if (timing_mode_G == m4ns) {
    if (tblock == 0) {  
      bun_pat_offset[0]               = bunch_of_max[best_card]*2;
      INST_timing_config.setups[tsetup].BP_offsets[tblock] = bunch_of_max[best_card]*2;
    }
    if (tblock == 1) {
      bun_pat_offset[1]               = bunch_of_max[best_card]*2 + 1;
      INST_timing_config.setups[tsetup].BP_offsets[tblock] = bunch_of_max[best_card]*2 + 1;
    }   	  		
  }
  if (timing_mode_G == m14ns) {
    if (species == POSITRONS) {
      bun_pat_offset[0]               = (bunch_of_max[best_card]*2);
      INST_timing_config.setups[tsetup].BP_offsets[tblock] = (bunch_of_max[best_card]*2);
    }
    if (species == ELECTRONS) {
      bun_pat_offset[1]               = (bunch_of_max[best_card]*2) + 1;
      INST_timing_config.setups[tsetup].BP_offsets[tblock] = (bunch_of_max[best_card]*2) + 1;
    }
  }
  
  
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    INST_timing_config.setups[tsetup].chan_delays[tblock][card] = starting_CD;
  }    
  
  
  // Decrement the trace level
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
  
}
