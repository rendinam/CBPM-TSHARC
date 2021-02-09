//-----------------------------------------------------------------------+
// File         :  set_delays.c                                          |
//                                                                       |
// Description  :  Sets all block and channel delays on all cards and    |
//                 all channels.                                         |
//                                                                       |
// Param Inputs :  None                                                  |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                 CBI_F_FAILURE   - error                               |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int set_delays( int tsetup ) {
	
  // Add function ID to trace
  int FID = SET_DELAYS_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  int tblock, card;
  
  //-----------------------------------------------
  // Apply all valid channel delays to registers
  //-----------------------------------------------
  for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
      
      if (INST_timing_config.setups[tsetup].chan_delays[tblock][card] == CBI_UNINITIALIZED_PARAM) {
	*p_chan_delays[card][tblock] = CBPM_DELAY_MID;
      } else if (INST_timing_config.setups[tsetup].chan_delays[tblock][card] < CBI_UNINITIALIZED_PARAM ||
		 INST_timing_config.setups[tsetup].chan_delays[tblock][card] > CBPM_MAX_CHAN_DELAY_COUNTS ) {
	*p_chan_delays[card][tblock]                                      = CBPM_DELAY_MID;
	INST_timing_config.setups[tsetup].chan_delays[tblock][card] = CBPM_DELAY_MID;
      } else {
	*p_chan_delays[card][tblock] = INST_timing_config.setups[tsetup].chan_delays[tblock][card];
      }      	
      
    } //endFOR card
  } //endFOR tblock
  
  
  //-----------------------------------------------------
  // Apply all valid global (block) delays to registers
  //-----------------------------------------------------
  for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
    
    if (INST_timing_config.setups[tsetup].block_delays[tblock] == CBI_UNINITIALIZED_PARAM) {
      set_block_delay(tblock, 0);
    } else if (INST_timing_config.setups[tsetup].block_delays[tblock] < CBI_UNINITIALIZED_PARAM ||
	       INST_timing_config.setups[tsetup].block_delays[tblock] > CBPM_MAX_BLOCK_DELAY_COUNTS ) {
      set_block_delay(tblock, 0);
      INST_timing_config.setups[tsetup].block_delays[tblock] = 0;
    } else {
      set_block_delay(tblock, INST_timing_config.setups[tsetup].block_delays[tblock] );
    }      	
    
  }
  
  // Decrement the trace and return status
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
  
}


      
      
