//-----------------------------------------------------------------------+
// File         :  add_debug.c                                           |
//                                                                       |
// Description  :  Function to put debug words into the dsp_debug struct |
//                    and to clear the dsp_debug struct					 |
//                                                                       |
// Prototype    :  int add_debug(int, int, int}                          |
//				   int clear_debug()									 |
//				   int finish_handshake(int)							 |
//				   int init_debug_registers()						     |
//                                                                       |
// Arguments    :  routine ID                                            |
//                 debug word (integer)                                  |
//				   minimum debug level necessary to add this debug val   |
//																		 |
//				   None													 |
//																		 |
//				   Function status										 |
//																		 |
//				   None													 |
//																		 |
//                                                                       |
// Return Values:  CBI_F_SUCCESS   - normal return                       |
//                                                                       |
// Author       :  M. Palmer, M. Stedinger                               |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int add_debug(int routine, int debug, int min_debug_level_required) {

   // Add function ID to the trace
   INST_stat.trace[INST_stat.num_levels++] = ADD_DEBUG_ID;

   int write_ptr;

  if (INST_code_params.execution_flags[DSP_DEBUG_LEVEL] >= min_debug_level_required) {
   
	   // Check the write_ptr value and reset if necessary
	  if (INST_debug.write_ptr >= CBI_MAX_DEBUG_WORDS) {
      INST_debug.write_ptr = 0;
    }

    // Fill the debug information
    write_ptr = INST_debug.write_ptr;
    INST_debug.debug[write_ptr] = debug;
    INST_debug.routine[write_ptr++] = routine;
    INST_debug.write_ptr = write_ptr;

     int i, ind, start;
    ind = INST_debug.write_ptr;
    for (i= 0; (ind<CBI_MAX_DEBUG_WORDS) && (i<5); i++)
    {
      INST_debug.debug[ind] = 0;
      INST_debug.routine[ind] = 0;
      ind++;
    }

    ind = 0;
    start = i;
    for (i=start; i<5; i++)
    {
      INST_debug.debug[ind] = 0;
      INST_debug.routine[ind] = 0;
      ind++;	   	
    }
   
  }
 
   // Decrement the trace level and return SUCCESS
   INST_stat.num_levels--;
   return CBI_F_SUCCESS; 
}



int clear_debug() {
	
	int i;

   // Add function ID to the trace
   INST_stat.trace[INST_stat.num_levels++] = CLEAR_DEBUG_ID;
   	
	// Clear the whole array
	for (i=0; i<=CBI_MAX_DEBUG_WORDS; i++) {
		INST_debug.debug[i] = 0;
		INST_debug.routine[i] = 0;
	}
	
	// Set the Pointer back to zero
	INST_debug.write_ptr = 0;
  
  for (i=2; i<15; i++) {
		INST_stat.trace[i] = 0;
	}	
	INST_stat.trace[INST_stat.num_levels-1] = CLEAR_DEBUG_ID;

   // Decrement the trace level and return SUCCESS
   INST_stat.num_levels--;	
	return CBI_F_SUCCESS; 
}


int debug_point(int location) {
	int heartbeat;
	*debug_location_reg = location;
	
	//go = always run and no breakpoint, and  no breakpoint and not at location
	
	if ((*debug_run_reg == 1) && (*breakpoint == 0)) {
		return CBI_F_SUCCESS;
	}

	if ((*breakpoint == 0) || (*breakpoint == location) || (*debug_step_reg == 1)) {
	
		heartbeat = *debug_heartbeat_reg;
		*debug_run_reg = 0;
		*debug_step_reg = 0;		
		*debug_location_reg = location+1;
		while ((*debug_step_reg == 0) && (*debug_run_reg == 0)) {
			magic_wait(1);
	
	       *debug_heartbeat_reg = heartbeat++;
	       if (heartbeat == 0x7FFFFFFF) {
	           heartbeat = 0;
	           *debug_heartbeat_reg = 0;
	       }
	       
	       *output_reg = *input_reg;		
		}
		
		*debug_heartbeat_reg = 0;
		*debug_location_reg = location+2;
		
	}

	return CBI_F_SUCCESS;
	
   //Sample of Code using debug registers:

   /*finish_handshake(func_status); //Release CBIC
   
   *value1_reg = 5;
   *value2_reg = 4;
   *value3_reg = 3;
   *value4_reg = 2;
   *value5_reg = 1;
   debug_point(0x100);
   
   *value1_reg = 1;
   *value2_reg = 2;
   *value3_reg = 3;
   *value4_reg = 4;
   *value5_reg = 5;
   debug_point(0x200);

   *value1_reg = 5;
   *value2_reg = 4;
   *value3_reg = 3;
   *value4_reg = 2;
   *value5_reg = 1;
   debug_point(0x300);*/	
}

int init_debug_registers() {
	
  *debug_heartbeat_reg = 0;   
  *debug_run_reg = 1;
  *debug_step_reg = 0;
  *debug_location_reg = 0;
  *breakpoint = 0;   
    
  *input_reg = 0;
  *output_reg = 0;
  *value1_reg = 0;
  *value2_reg = 0;
  *value3_reg = 0;
  *value4_reg = 0;
  *value5_reg = 0;
  *status_reg = 0;	
  
  return CBI_F_SUCCESS;
}


int finish_handshake(int status) {
	
	*status_reg = status;
	
	int i;
	volatile int *p_error                    = INST_cmd.error;	
	
    // See if there were any math errors.  // FIXME: enable
    if (float_err_check() != CBI_F_SUCCESS) {
    //   stat = CBI_F_FAILURE;
    }

    // Put the status flag in the cmd struct
    INST_cmd.cmd_status = status;

    int found_errors = CBI_FALSE;
    for (i = 0; i < CBI_MAX_ERROR_WORDS; i++) {
       if (*(INST_cmd.error + i) != 0) found_errors = CBI_TRUE;
    }
    if (found_errors == CBI_FALSE) {
       cbpm_set_error(CBI_NO_ERR,p_error);
    } 
    
    
    //--------------------------------------------------------------
    // Need to pause before setting the handshake in case the DSP
    // completes during the interval between when the xbus has
    // picked up the old error information and the xbus picks up
    // the handshake. Delay needs to be on the order of 250 usec?
    //--------------------------------------------------------------
    int kill_time_count = 0;
    while (kill_time_count < 12500) {
       kill_time_count++;
    }

    
    //---------------------------------------------------
    // now set the handshake 
    //---------------------------------------------------
    INST_cmd.handshake = CBI_CMD_DONE_HANDSHAKE;
    
    return CBI_F_SUCCESS;
}
