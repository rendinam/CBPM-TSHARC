//-----------------------------------------------------------------------
// File         :  interrupt_handlers.c
//
// Description  :  Contains functions that can be installed as interrupt
//                 handlers.
//
// Contributors :  M. Rendina
//-----------------------------------------------------------------------

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


//------------------------------------------------------------
// Timer interrupt for approximate wall-clock 
// time comparisons
//------------------------------------------------------------
// Compute how many turns markers in a given period
// of time have arrived to determine a timing 
// integrity status.
//
// If the difference in the number of turns markers received
// in that period of time is more than the upper threshold
// (determined by TURNS_HI_FRAC) or below the lower threshold
// (determined by TURNS_LOW_FRAC), then flag the timing signal
// as poor.
//
// 195312 Expected number of turn markers in 0.5 second
//------------------------------------------------------------
#define TURNS_EXPECTED           (CBPM_CLOCK_SAMPLE_DURATION / (2.56e-6))

#define TURNS_HI_FRAC            0.05
#define TURNS_LOW_FRAC           0.05

void timer0H_int_handler( int unused ) {
	
   int turns_diff; 
   
   // Handle turn counter wrapping.
   if (*p_turn_counter < turncount) { 
      turns_diff = (0xFFFFF - turncount) + *p_turn_counter + 1;
   }  else {
      turns_diff = *p_turn_counter - turncount;
   }
   	 
   
   if (	(turns_diff < (TURNS_EXPECTED - (TURNS_LOW_FRAC * TURNS_EXPECTED))) ||
        (turns_diff > (TURNS_EXPECTED + (TURNS_HI_FRAC * TURNS_EXPECTED))) ) {
      	
      INST_heartbeat.timing_integrity =  0;  // Poor integrity
   } else {
   	  INST_heartbeat.timing_integrity =  1;  // Good integrity
   }
   
   INST_heartbeat.turns_seen = turns_diff;
   
   
   turncount = *p_turn_counter;
   
   return;
}

