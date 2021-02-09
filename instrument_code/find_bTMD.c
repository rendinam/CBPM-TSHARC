//-----------------------------------------------------------------------+
// File         :  find_bTMD.c                                           |
//                                                                       |
// Description  :  Determines the optimal "block turns marker delay"     |
//                 (bTMD) setting for the timing block in question by    |
//                 scanning through all possible bTMDs and performing a  |
//                 find_bunch operation at each.                         |
//                                                                       |
//                 Typically, two (occasionally three) values            |
//                 of bunch bucket index are found in as many            |
//                 contiguous groups during this process.  The bTMD      |
//                 setting that corresponds to the center of the largest |
//                 of the groups with the lowest of the two bunch bucket |
//                 index values will be loaded into the appropriate      |
//                 register and saved for future use.                    |
//                                                                       |
// Author       :  M.Rendina                                             |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int find_bTMD( ) {

   // Add the function ID to the trace
   int FID = FIND_BTMD_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;
   
   volatile int *p_error = INST_cmd.error;         
   
   int i, j, bucket, card, btmd;
   
   int istat = CBI_F_SUCCESS;
   
   int species = INST_cmd_params.species;
   
   int bTMD_ADDRESS;
   
   int *p_pat_offset;
   
   int btmd_lower = 0;
   //---------------------------------------
   // Explicit parameter determination for 
   // each combination of 
   // timing setup / timing block.
   //     FIXME:  Expand to accommodate
   //             other timing modes too.
   //---------------------------------------
   int tsetup = INST_op_timing.active_timing_setup;
   int tblock = INST_cmd_params.tblock;
   
   if (timing_mode_G == m4ns) {
   	  if (tblock == 0) {
   	    bTMD_ADDRESS = BLOCK_TMD_A;
   	    p_pat_offset = &(bun_pat_offset[0]);
   	  } else {
   	  	btmd_lower = INST_timing_config.setups[tsetup].bTMDs[0];
   	  	bTMD_ADDRESS = BLOCK_TMD_B;
   	    p_pat_offset = &(bun_pat_offset[1]);
   	  }
   }   	
   if (timing_mode_G == m14ns && species == POSITRONS) {
   	  bTMD_ADDRESS = BLOCK_TMD_A;
   	  p_pat_offset = &(bun_pat_offset[0]);
   }
   if (timing_mode_G == m14ns && species == ELECTRONS) {
      bTMD_ADDRESS = BLOCK_TMD_B;
   	  p_pat_offset = &(bun_pat_offset[1]);
   }   		
   //MGS TODO needs work. are there any cases where it falls all the way through? is it ok to NOT set the p_pat_offset?

   
   int offset_array[10];

   
   int trans[4] = {0,0,0,0}; // Space to save transition points
   int tcount   = 0;         // Number of transitions
   
   //-----------------------------------
   // Scan through all the block TMDs
   //-----------------------------------
   for (btmd = btmd_lower; btmd <= CBPM_MAX_BTMD_VALUE; btmd++) {
   	
      VAL_AT( bTMD_ADDRESS ) = btmd;
   	
      istat = find_bunch();
      
      if (istat == CBI_F_FAILURE) {
          cbpm_set_error(FIND_BTMD_FIND_BUNCH_ERR1, p_error);
          //INST_stat.num_levels--;
          //return CBI_F_FAILURE;
      }  
           
      offset_array[btmd] = *p_pat_offset;


      // Note transition points of offset value regimes. Transition 
      // point stored is the index of the first entry in the next 
      // collection of homogeneous bunch pattern offset values.
      if ( (btmd != 0)  &&  (offset_array[btmd] != offset_array[btmd-1]) ) {
      	 trans[tcount] = btmd;
      	 tcount++;
      }
      if ( btmd == CBPM_MAX_BTMD_VALUE ) {
      	 trans[tcount] = CBPM_MAX_BTMD_VALUE + 1;
      }
      
   }
   
   
   //---------------------------------------------
   // Calculate the center of the _2nd_ region of 
   // homegeneous offset values, add to the first 
   // transition point index.
   //---------------------------------------------
   int target = ceilf( (trans[1] - trans[0]) / 2 );
   target     = target + trans[0];
   
   
   //---------------------------------------------
   // Set block turns marker delay to the target 
   // found above and make the value available to
   // the control system.
   //
   // Re-run the find_bunch routine using the 
   // optimal bTMD.
   //---------------------------------------------
   VAL_AT( bTMD_ADDRESS )         = target;
   

   INST_timing_config.setups[tsetup].bTMDs[tblock] = target;
   
   istat = find_bunch();
   
   if (istat == CBI_F_FAILURE) {
      cbpm_set_error(FIND_BTMD_FIND_BUNCH_ERR2, p_error);
      //INST_stat.num_levels--;
      //return CBI_F_FAILURE;
   }  

   
   // Decrement the trace
   INST_stat.num_levels--;
   return CBI_F_SUCCESS;
   
}

