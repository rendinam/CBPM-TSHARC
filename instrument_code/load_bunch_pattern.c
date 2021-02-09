//-------------------------------------------------------------------------
// File         :  load_bunch_pattern.c
//
// Description  :  Two (2) functions used to load a rotated bunch pattern 
//                 into the hardware registers on all four front-end cards
//                 and to record the sequence of ADC blocks needed to 
//                 access data captured on each channel.
//
// Contributors :  M. Rendina
//-------------------------------------------------------------------------

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"



//============================================================================
// Function: load_bunch_pattern()
//
// Loads the bunch pattern provided in the rot_bun_pat array member of the 
// command parameters data struct into the BPMs bunch pattern registers.
// 
// Returns: Total number of bunches set to be sampled on each timing block.
//============================================================================
int load_bunch_pattern(int *nbunches) {	
	
  int nkept=0;
  int patword;
  int chkbit_pat;
  int i, j, k, acq_con;
  int reg_width = BP_REG_WIDTH;
  int idx_in_rotated;
  
  // For each register
  for (i = 0; i < MAX_BP_REGS; i++) {
    
    patword = 0;
    if ( i == MAX_BP_REGS-1 ) { // Special case for last register, if needed.
      reg_width = LAST_BP_REG_WIDTH;
    }
    
    // Create a single word (bit pattern) out of reg_width array values at a time.
    for (j = 0; j < reg_width; j++) {  
      chkbit_pat = 0x01;
      // if jth element is '1', make jth bit of patword a 1
      idx_in_rotated = (i*reg_width) + j;
      if (INST_cmd_params.rot_bunch_pat[(i*reg_width) + j] == 1) {
	
	if (idx_in_rotated % 2 == 0) {
	  nbunches[0]++; // Bump timing block A bunch total
	} else {
	  nbunches[1]++; // Bump timing block B bunch total
	}
	nkept++;   // We'll examine this bunch bucket, bump the overall bunch total
	chkbit_pat = (chkbit_pat << j);
	patword = (patword | chkbit_pat); // set the jth bit of patword to 1 using OR operation
      }
    } // end for j	       
    
    
    for (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      *(p_acq_bunch_patterns[acq_con]+i) = patword;
    }
    
  }
  
  // Compose and make available the parity list for this loaded bunch pattern.
  // Fills an array with an EVEN or ODD marker for each bunch pattern bit that is set.
  compose_block_list();
  
  return nkept;
}




//============================================================================
// Function: compose_block_list()
//
// Populates the global block_list[] array with a flag corresponding to the 
// parity (EVEN/ODD-ness) i.e. the TIMING BLOCK of the bunch bit index of each
// active bit in the bunch pattern register.  This is used to obtain data from
// the correct ADC buffer when reading out values from an acquistion.
//============================================================================
int compose_block_list(void) {
  int bit; 
  int pslot = 0;
  
  for (bit = 0; bit < CBPM_MAX_BUNCHES; bit++) {
    if(INST_cmd_params.rot_bunch_pat[bit] == 1) {
      if (bit % 2 == 0) {
	block_list[pslot] = 0;
      } else {
	block_list[pslot] = 1;
      }
      pslot++;
    }
  }
  
  return CBI_F_SUCCESS;
}


