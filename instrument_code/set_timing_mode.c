//-----------------------------------------------------------------------+
// File         :  set_bunch_spacing_mode.c                              |
//                                                                       |
// Description  :  Sets the operational timing mode of the instrument by |
//                 configuring the timing board.                         |
//                                                                       |
//                 The available bunch spacing modes are:                |
//                                                                       |
//                    72MHz       50MHz    83MHz      125MHz             |
// ___________________________________________________________           |
// Bunch spacing       14ns      10ns       6ns         4ns              |
// Block A N           57        54        58          60                |
// Pulsewidth           2         1         1           1                |
// Total buckets      183       256       426         640                |
//                                                                       |
// Arguments    :  integer - m4ns (0), m14ns (1)                         |
//                                                                       |
// Contributors : M. Rendina                                             |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int set_timing_mode( int tmode ) {

   // Add function ID to the trace
   INST_stat.trace[INST_stat.num_levels++] = SET_TIMING_MODE_ID;
	
   int frequency_param;
   int pulsewidth_param;
	
   
   switch (tmode) {
      case m4ns:
         frequency_param   = CBPM_4NS_FREQ_CODE;
	     pulsewidth_param  = CBPM_4NS_PULSEWIDTH_CODE;
         INST_stat.state   = m4ns;
         timing_mode_G     = m4ns;
         MAX_BUNCHES       = CBPM_4NS_MAX_BUNCHES;
         break;  
      case m14ns:
   	     frequency_param   = CBPM_14NS_FREQ_CODE;
         pulsewidth_param  = CBPM_14NS_PULSEWIDTH_CODE;
         INST_stat.state   = m14ns;
         timing_mode_G     = m14ns;
         MAX_BUNCHES       = CBPM_14NS_MAX_BUNCHES;
         break;  
      default:
         break;
   }         


   // Write the parameters to the timing board
   VAL_AT(A_FREQUENCY)  = frequency_param;
   magic_wait( 10000 );
   VAL_AT(A_PULSEWIDTH) = pulsewidth_param;
   magic_wait( 10000 );
   VAL_AT(B_FREQUENCY)  = frequency_param;
   magic_wait( 10000 );
   VAL_AT(B_PULSEWIDTH) = pulsewidth_param;
   magic_wait( 10000 );
  
   
   // Decrement the trace
   INST_stat.num_levels--;
   return CBI_F_SUCCESS;
	
}

