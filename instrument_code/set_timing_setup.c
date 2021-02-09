//-----------------------------------------------------------------------+
// File         :  set_timing_setup.c                                    |
//                                                                       |
// Description  :  Sets the operational timing setup of the instrument   |
//                 by configuring the timing board for a particular      |
//                 mode (clock frequency) and by loading all the relevant|
//                 timing parameters that define the particular setup    |
//                 into the hardware registers.                          |
//                                                                       |
// Contributors : M. Rendina                                             |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int set_timing_setup( void ) {

   // Add function ID to the trace
   int FID = SET_TIMING_SETUP_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;
   
   volatile int *p_error = INST_cmd.error;   
   
   int timing_mode;
   
   int tsetup = INST_op_timing.active_timing_setup;
   
   switch (tsetup) {
   	
   	 case s4ns_P_VAR_G:
   	 case s4ns_P_FIX_G:
   	 case s4ns_E_VAR_G:
   	 case s4ns_E_FIX_G:
   	   timing_mode = m4ns;
   	   break;
   	   
   	 case s14ns_VAR_G:
   	 case s14ns_FIX_G:
   	   timing_mode = m14ns;
   	   break;
   	   
     default:
       cbpm_set_error(STS_UNKNOWN_TIMING_MODE_ERR, p_error);
       INST_stat.num_levels--;
       return CBI_F_FAILURE;
   }   	     	   
   
   
   // Set the timing board hardware registers for
   // proper clock frequency and pulsewidth parameters.
   set_timing_mode( timing_mode );
   
   //   Set COMMON (cTMD) and BLOCK turns marker delays (bTMDs)
   //-------------------------------------------------------------------
   // Load a default value of 0 into the hardware if an uninitialized
   // parameter is encountered, but leave the uninitialized flag in
   // place for later acknowledgement.  
   // If the value is out of range and is not CBI_UNINITIALIZED_PARAM, 
   // this safeguards against out-of-range values being loaded into the 
   // hardware, which can cause lockups during acquisition, by loading 
   // a default value of 0 AND replacing the input value with a zero as 
   // well so that the control system has acknowledgement that the value
   // sent was not the one written.
   //-------------------------------------------------------------------
   if (INST_timing_config.setups[tsetup].cTMD == CBI_UNINITIALIZED_PARAM) {
       VAL_AT( COMMON_TMD )  = 0;
   } else if (INST_timing_config.setups[tsetup].cTMD < CBI_UNINITIALIZED_PARAM ||
              INST_timing_config.setups[tsetup].cTMD > CBPM_MAX_CTMD_VALUE ) {
       VAL_AT( COMMON_TMD )                   = 0;
       INST_timing_config.setups[tsetup].cTMD = 0;
   } else {
   	   VAL_AT( COMMON_TMD )  = INST_timing_config.setups[tsetup].cTMD;
   }
   
   magic_wait( 10000 );
   
   if (INST_timing_config.setups[tsetup].bTMDs[0] == CBI_UNINITIALIZED_PARAM) {
       VAL_AT( BLOCK_TMD_A ) = 0;
   } else if (INST_timing_config.setups[tsetup].bTMDs[0] < CBI_UNINITIALIZED_PARAM ||
              INST_timing_config.setups[tsetup].bTMDs[0] > CBPM_MAX_BTMD_VALUE ) {
       VAL_AT( BLOCK_TMD_A )                      = 0;
       INST_timing_config.setups[tsetup].bTMDs[0] = 0;
   } else {
   	   VAL_AT( BLOCK_TMD_A )  = INST_timing_config.setups[tsetup].bTMDs[0];
   }      
   
   magic_wait( 10000 );
   
   if (INST_timing_config.setups[tsetup].bTMDs[1] == CBI_UNINITIALIZED_PARAM) {
       VAL_AT( BLOCK_TMD_B ) = 0;
   } else if (INST_timing_config.setups[tsetup].bTMDs[1] < CBI_UNINITIALIZED_PARAM ||
              INST_timing_config.setups[tsetup].bTMDs[1] > CBPM_MAX_BTMD_VALUE ) {
       VAL_AT( BLOCK_TMD_B )                      = 0;
       INST_timing_config.setups[tsetup].bTMDs[1] = 0;
   } else {
   	   VAL_AT( BLOCK_TMD_B )  = INST_timing_config.setups[tsetup].bTMDs[1];
   }         
   
   magic_wait( 10000 );
   	
   
   // Set bunch pattern offsets
   bun_pat_offset[0] = INST_timing_config.setups[tsetup].BP_offsets[0];
   bun_pat_offset[1] = INST_timing_config.setups[tsetup].BP_offsets[1];
   
   // Set all block and channel delays
   set_delays(tsetup);
 
   // Decrement the trace
   INST_stat.num_levels--;
   return CBI_F_SUCCESS;
	
}

