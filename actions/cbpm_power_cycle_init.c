//-----------------------------------------------------------------------+
// File         :  cbpm_power_cycle_init.c                               |
//                                                                       |
// Description  :  A SEQUENCE-type command.                              |
//                 Cycles the power on the active instruments, then      |
//                 calls reset_and_init to reboot and reload all         |
//                 configuration data and reset the state of the         |
//                 instrument to what it was right before the power      |
//                 cycle.                                                |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


#define   NUM_STEPS                6
#define   BOOT_WAIT_SECS           22


int cbpm_power_cycle_init( void ) {

  CBPM_DATA *dpt, *dp;
  dpt = CTL_System.p_Module_temp->dsp_data;

  int card, tblock;
  int iidx, status, one = 1;

  //----------------------------------------------------------
  // Variables whose values need to be preserved across
  // multiple iterations (must be static)
  //----------------------------------------------------------
  static int recovery_mode_detected = CBI_FALSE;


  CBI_COMMAND_SEQUENCE_START(NUM_STEPS)


    //---------------------------------------------------------------
    // Cycle the power on all active instruments
    //---------------------------------------------------------------
    CBI_COMMAND_SEQUENCE_STEP( 1 )
    
	  //-------------------------------------------------
	  // Define the list of prompts and their parameters
	  //-------------------------------------------------
	  int retval, input = FALSE;
	  CBI_INIT_PROMPTS(1);


	  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
	  int map_out[4]      = {    TRUE,     TRUE,     FALSE,    FALSE};
	  prompts[0].supp_text_ptr    = NULL;
	  prompts[0].prompt_text      = "\n\nWould you like to cycle power for ALL active instruments? (y/n): ";
	  prompts[0].data_type        = CBI_CHAR;
	  prompts[0].valid_inputs     = valid_inputs;
	  prompts[0].map_out          = map_out;
	  prompts[0].default_value    = 'n';
	  prompts[0].destination      = &(input);
	  prompts[0].last_val_default = FALSE;

	  retval = CBI_PROMPTER();
	  
	  if (input == FALSE ) {
	    printf("Power cycle operation aborted.\n");
	    CBI_COMMAND_SEQUENCE_BREAK
	  }      
    
      CTL_System.skip_comm_connect = TRUE;
      cbi_power_cycle_prep();
      fflush(stdout);
      cbi_load_command(CTL_System.command_list,
		       CTL_System.itype.num_platform_commands,
		       "POWER CYCLE",
		       CBI_FALSE);


    //---------------------------------------------------------------
    // If any instruments are in recovery mode, temporarily disable 
    // all instruments that do not exhibit a failure status.
    //---------------------------------------------------------------
    CBI_COMMAND_SEQUENCE_STEP( 2 )
      CTL_System.skip_comm_connect = TRUE;
      for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
        if (CTL_System.p_Module[iidx]->in_recovery == CBI_TRUE) {
	  recovery_mode_detected = CBI_TRUE;
	  break;
        }
      }
      if (recovery_mode_detected) {
        printf("\n\nDeactivating all instruments...\n");
        printf("Activating only instruments with problem codes in preparation for a reset...\n");
        for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
	  CTL_System.p_Module[iidx]->active = CBI_INACTIVE; // disabled for testing
  	  if (CTL_System.p_Module[iidx]->in_recovery == CBI_TRUE) {
	    printf(" Activating instrument %s\n", CTL_System.p_Module[iidx]->comm.ethernet.hostname );
	    CTL_System.p_Module[iidx]->active = CBI_ACTIVE;
	  }
        } 
      }
      fflush(stdout);
      cbi_load_command(CTL_System.command_list, 
	  	       CTL_System.itype.num_platform_commands,
		       "RESET INSTRUMENT",
		       CBI_FALSE);


    //--------------------------------------------------
    // Wait for instruments to complete initial boot-up
    // and then push all configuration info back into
    // them.
    //--------------------------------------------------
    CBI_COMMAND_SEQUENCE_STEP( 3 )
      printf("Waiting %ds for instruments to boot...\n", BOOT_WAIT_SECS);
      fflush(stdout);
      sleep(BOOT_WAIT_SECS);
      printf("Pushing all cached timng and gain configuration values...\n");
      fflush(stdout);
      cbi_load_command(CTL_System.command_list, 
		       CTL_System.itype.num_platform_commands,
		       "PUSH FULL CONFIG",
		       CBI_FALSE);


    //--------------------------------------------------
    // Set the gains back to the cached value(s)
    //--------------------------------------------------
    CBI_COMMAND_SEQUENCE_STEP( 4 )
      printf("Setting gains\n");
      fflush(stdout);
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
	for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
	  dpt->cbpm_op_gain.active_gain_settings[card][tblock] = appconfig.default_gain_setting;
	}
      }
      cbi_load_command(CTL_System.command_list,
		       CTL_System.itype.num_platform_commands,
		       "SET GAINS",
		       CBI_FALSE);

  
    //--------------------------------------------------
    // Set the timing setup back to the cached value
    //--------------------------------------------------
    CBI_COMMAND_SEQUENCE_STEP( 5 )
      printf("Setting timing setup\n");
      fflush(stdout);
      cbi_load_command(CTL_System.command_list, 
		       CTL_System.itype.num_platform_commands,
		       "SET TIMING SETUP",
		       CBI_FALSE);
    
    
    //----------------------------------------------------
    // Re-activate all instruments that did not encounter
    // a problem during the previous sequence of events.
    //----------------------------------------------------
    CBI_COMMAND_SEQUENCE_STEP( 6 )
      if (recovery_mode_detected) {
	// Activate all online instruments.
	// FIXME: Just activate the ones that were active at the time of the
	//        deactivation pass in step 1.  Save state info.
	printf("Activating all instruments in good standing...  \n"); // FIXME: Make this better targeted
	for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
	  if (CTL_System.p_Module[iidx]->in_recovery == CBI_TRUE &&
	      CTL_System.p_Module[iidx]->active      == CBI_INACTIVE) {
	    // Reset recovery flag but leave disabled.
	    CTL_System.p_Module[iidx]->in_recovery == CBI_FALSE;
	  } else {
	    CTL_System.p_Module[iidx]->active = CBI_ACTIVE;
	  }
	}
      }
      recovery_mode_detected = CBI_FALSE;
      // As part of the last step in this sequence, reset all the turn counters
      // in the instruments via the "CBPM COMMAND" node.
      printf("Synchronizing all instrument turn counters...  ");
      status = vxputn_c(CBPM_COMMAND_NODE, 1, 1, &one);
      if (status == MPM_SUCCESS) {	
	printf("done.\n");
      } else {
	printf("ERROR writing %d to %s element %d!\n", one, CBPM_COMMAND_NODE, 1);
      }
      fflush(stdout);
      CBI_NEXT_SEQUENCE_STEP
      

  CBI_COMMAND_SEQUENCE_END

}

