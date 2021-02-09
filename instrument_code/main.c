//-----------------------------------------------------------------------
// File         :  main.c
//
// Description  :  First routine that is executed when the DSP starts up
//-----------------------------------------------------------------------

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


void main ( void )
{
	
   // Allow some time for a boot/reset to complete cleanly
   magic_wait( 100000 );	// about 5ms
	
   // Initialize trace information
   INST_stat.num_levels = 0;

   // Store main routine trace info
   INST_stat.trace[INST_stat.num_levels++] = MAIN_ID;
   
   // Initialize the DSP
   dsp_init();

   // Look for commands being sent to the DSP
   dispatch();

}
