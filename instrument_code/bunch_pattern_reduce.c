//-----------------------------------------------------------------------
// File         :  bunch_pattern_reduce.c
//
// Description  :  Utility functions for reducing a received bunch
//                 pattern to the first selected bunch and another
//                 function for restoring the original, rotated bunch
//                 pattern.
//
// Contributors :  M. Rendina
//-----------------------------------------------------------------------

// Instrumentation Coref
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"

//------------------------------------------------------------------------
// PRIOR to calibration-- timing, gain, or both, the bunch pattern 
// register values must be saved and replaced with a single bunch bit AT 
// THE POSITION IN THE REGISTERS OF THE FIRST HIGH BIT as seen in the 
// incoming bit patterns.
//
// This is because calibration procedures are designed and only needed
// for a single bunch.  The internal 72/125MHz timing will take care of
// handling any additional bunches for the real data collection that
// follows.
// The requested and rotated bunch pattern must be restored immediately
// after the timing and/or gain adjustment steps.
//
// Accepts return argument by reference of the bunch to be used for
// calibration.  Passes this back up to the calling function.
//------------------------------------------------------------------------
int bunpat_reduce_pre(int tblock, int calib_bunches[]) {
	
  // Add function ID to the trace
  int FID = BUNCH_PATTERN_REDUCE_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;		
  
  int reg, j, bunch;
  int nkept; // Number of bunches to examine
  int calib_reg = 0;
  int calib_bit = 0;
  int first_block_bit_found = CBI_FALSE;
  
  
   // Save a copy of the rotated bunch pattern
  for (bunch = 0; bunch < MAX_BUNCHES; bunch++) {
    saved_rot_bun_pat[bunch] = INST_cmd_params.rot_bunch_pat[bunch];	
  }
  
  
  for (reg = 0; reg < MAX_BP_REGS; reg++) {
    
    //---------------------------------------------------
    // Save pre-rotation pattern values from cmd params
    // register by register as visited in this loop.
    //---------------------------------------------------
    saved_bun_pat[reg] = INST_cmd_params.bunch_pat[reg]; 
    
    // First high bit has not been found yet, examine this register.
    if ( (first_block_bit_found == CBI_FALSE) && (INST_cmd_params.bunch_pat[reg] != 0) ) {
      
      //-------------------------------------------------
      // Examine each bit in turn.  The first high one 
      // encountered is the target.  Keep it, make note 
      // of where it lives, and set all the others low.
      //-------------------------------------------------
      for( j = 0; j < BP_REG_WIDTH; j++ ) {
	if(  bitval(INST_cmd_params.bunch_pat[reg], j)  &&  (parity(j) == tblock)  &&  (!first_block_bit_found) ) {
	  calib_reg = reg;
	  calib_bit = j;
	  first_block_bit_found = CBI_TRUE;
	} else {
	  unsetbit( INST_cmd_params.bunch_pat[reg], j );
	}
	
      }
    } else {
      INST_cmd_params.bunch_pat[reg] = 0;
    }
    
  } //endFOR reg
  
  rotate_block_bunch_pattern( tblock, bun_pat_offset[tblock] );
  
  calib_bunches[tblock] = (calib_reg * BP_REG_WIDTH) + calib_bit;
  
  int d[2];
  nkept = load_bunch_pattern( d );  // CMD_PARAMS.rot_bunch_pat --> REGISTERS
  
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
}




int bunpat_reduce_pre_4ns(int bunches[]) {
  
  // Add function ID to the trace
  int FID = BUNCH_PATTERN_REDUCE_4NS_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;	
  
  int i, j, bunch;
  int nkept; // Number of bunches to examine
  int calib_reg = 0;
  int calib_bit = 0;
  int first_bit_found = CBI_FALSE;
  
  // Save post-rotation pattern to eliminate some rotation steps.
  for (bunch = 0; bunch < MAX_BUNCHES; bunch++) {
    saved_rot_bun_pat[bunch] = INST_cmd_params.rot_bunch_pat[bunch];
  }
  
  
  for (i = 0; i < MAX_BP_REGS; i++) {    
    // Save pre-rotation pattern values from cmd params
    saved_bun_pat[i] = INST_cmd_params.bunch_pat[i]; // just in case regs are not R/W for some reason.
    
    // First high bit has not been found yet, examine this register.
    if ( (first_bit_found == CBI_FALSE) && (INST_cmd_params.bunch_pat[i] != 0) ) {
      for( j = 0; j < BP_REG_WIDTH; j++ ) {
	if(  bitval(INST_cmd_params.bunch_pat[i], j) ==  1 ) {
	  calib_reg = i;
	  calib_bit = j;
	  first_bit_found = CBI_TRUE;
	}
      }
    }
    INST_cmd_params.bunch_pat[i] = 0;
  }               
  
  bunches[0] = (calib_reg * BP_REG_WIDTH) + calib_bit;
  
  int d[2];
  nkept = load_bunch_pattern( d );           // CMD_PARAMS.rot_bunch_pat --> REGISTERS    
  
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
}





//------------------------------------------------------------------------
// Put the bunch patterns and num_bunches specifiers back the way they 
// were before calibrating i.e. the way the control application orginally
// requested.
//------------------------------------------------------------------------
int bunpat_reduce_post() {
  
  int i;
  int nkept; // Number of bunches to examine
  
  // Restore bunch pattern from saved copy
  for (i = 0; i < MAX_BP_REGS; i++) {                  
    INST_cmd_params.bunch_pat[i] = saved_bun_pat[i]; 
  }   
  
  // Restore rotated bunch pattern from saved copy
  for (i = 0; i < MAX_BUNCHES; i++) {
    INST_cmd_params.rot_bunch_pat[i] = saved_rot_bun_pat[i];
  }
  
  int d[2];
  nkept = load_bunch_pattern( d );  // CMD_PARAMS.rot_bunch_pat --> REGISTERS
  
  return CBI_F_SUCCESS;
}


