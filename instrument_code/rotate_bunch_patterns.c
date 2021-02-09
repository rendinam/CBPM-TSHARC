//-------------------------------------------------------------------------
// File         :  rotate_bunch_patterns.c
//
// Description  :  Two (2) functions used to rotate the bunch pattern 
//                 provided by the control system in a timing 
//                 mode-dependent way based on the pre-determined bunch
//                 pattern offset values to allow for consistent indexing
//                 of collected data.
//
// Contributors :  M. Rendina
//-------------------------------------------------------------------------

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


//========================================================
// Function: rotate_bunch_pattern_4ns
//
// Accepts an offset index, originally obtained from the 
// find_bunch routine that indicates where in THIS BPM's
// bunch pattern specification "Train 1, Bunch 1" is 
// expected to arrive.
//
// Reads in the bunch pattern as provided by the control
// side, and streams it into a rotated array of integers
// to reflect the shift of the arrival of T1B1 with 
// respect to the turn marker at this BPM's location.
//
// Return codes: 
// CBI_F_SUCCESS        (0) If a normal rotation occurred
//
//========================================================
int rotate_bunch_pattern_4ns( int offset, int wrapped_bits[] ) {
  
  int bit, i, j = 0;
  int bunch_bit;
  int reg_width = BP_REG_WIDTH;
  
  for (i = 0; i < MAX_BP_REGS; i++) {
    bunch_bit = 0x0001;
    for (bit = 0; bit < reg_width ; bit++)  {
      if ( INST_cmd_params.bunch_pat[i] & bunch_bit ) {
	INST_cmd_params.rot_bunch_pat[(offset + j) % MAX_BUNCHES] = 1;
	// Detect and count bits that wrap
	if ( (offset+j) >= MAX_BUNCHES ) {
	  if ( (offset+j) % 2 == 0 ) {
	    wrapped_bits[0]++;
	  } else {
	    wrapped_bits[1]++;
	  }
	}
      } else {
	INST_cmd_params.rot_bunch_pat[(offset + j) % MAX_BUNCHES] = 0;
      }
      j++;
      bunch_bit = ( bunch_bit << 1 );
    } // endFOR bit
    if (j == MAX_BUNCHES ) break;
  } // endFOR i
  
  return CBI_F_SUCCESS;
}



//========================================================
// Function: rotate_bunch_pattern_14ns
//
// Accepts the timing block to operate upon and an offset 
// index, originally obtained from the find_bunch routine
// that indicates where in THIS BPM's bunch pattern 
// specification "Train 1, Bunch 1" is expected to arrive
// using either the EVEN or ODD bits only as required by
// the requested timing block argument.
//
// Reads in the bunch pattern as provided by the control
// side, and streams it into a rotated array of integers
// to reflect the shift of the arrival of T1B1 (the bunch
// pattern rotation offset) with respect to the turns 
// marker at this BPM's location.
//
// Return codes: 
// CBI_F_SUCCESS        (0) If a normal rotation occurred
//
// (# bunches that wrapped) If bunch pattern wrapped in 
//                          the process
//========================================================
int rotate_block_bunch_pattern( int tblock, int offset ) {
  
  int bit, reg, i;
  int j = 0;
  int bunch_bit;
  int wrapped_bits = 0;
  int r_index;
  int rot_temp_array[183] = {0};
  int REGS_FOR_366_BITS = 23;
  int reg_width = 16;
  
  // Rotate bunch pattern for the requested block out into
  // the temporary rotation array (rot_array).
  for (reg = 0; reg < REGS_FOR_366_BITS; reg++) {	
    
    bunch_bit = (0x0001 << tblock);
    
    //---------------------------------------------
    // Last register of 23 has fewer bits in 14ns 
    // mode.  
    // (22regs * 16bits) + (1reg * 14bits) = 366
    //---------------------------------------------
    if (reg == 22) {
      reg_width = 14;
    }
    
    //---------------------------------------------
    // Examine every other bit in each register,
    // starting with either an EVEN or ODD bit
    // depending on timing block.
    //---------------------------------------------
    for (bit = 0; bit < reg_width/2; bit++) {
      r_index = (offset/2 + j) % 183; // index to use in rotated temp array
      if ( INST_cmd_params.bunch_pat[reg] & bunch_bit ) {
	rot_temp_array[r_index] = 1;
	if ( ((offset/2) + j) > 182 ) {
	  wrapped_bits++;
	}
      } else {
	rot_temp_array[r_index] = 0;
      }
      j++;
      bunch_bit = ( bunch_bit << 2 );
    } // endFOR bit
    
  } // endFOR for i
  
  //---------------------------------------------
  // Load the rotated bunch sub-pattern into just 
  // the bits in the master rotated bunch pattern
  // corresponding to the timing block requested.
  //---------------------------------------------
  for ( i = 0; i < MAX_BUNCHES/2; i++) {
    INST_cmd_params.rot_bunch_pat[(i*2)+tblock] = rot_temp_array[i];
  }
  
  return wrapped_bits;
  
}

