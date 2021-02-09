//-----------------------------------------------------------------------+
// File         :  cbpm_reset_and_init.c                                 |
//                                                                       |
// Description  :  A SEQUENCE-type command.                              |
//                 Performs an XBUS-requested low-level reset of the DSP |
//                 and in turn the Coldfire (ethernet) module.  Then     |
//                 re-sends all control-system-resident timing and gain  |
//                 values, before executing a SET_GAINS command and then |
//                 a SET_TIMING_SETUP command, leaving the instrument in |
//                 the configured state specified by the                 |
//                 control-system-resident timing and gain parameters    |
//                 that were present immediately prior to the reset      |
//                 sequence.                                             |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


#define   NUM_STEPS                5
#define   BOOT_WAIT_SECS           22


int cbpm_reset_and_init( void ) {
  
  CBPM_DATA *dpt, *dp;
  dpt = CTL_System.p_Module_temp->dsp_data;
  
  int card, tblock;
  int iidx;
  int status;

  //----------------------------------------------------------
  // Variables whose values need to be preserved across
  // multiple iterations (must be static)
  //----------------------------------------------------------
  static int recovery_mode_detected = CBI_FALSE;
  static int active_states[CBI_MAX_INSTRUMENTS];


  CBI_COMMAND_SEQUENCE_START(NUM_STEPS)

    // If any instruments are in recovery mode, temporarily disable 
    // all instruments that do not exhibit a failure status to save
    // time in reinitializing.
    CBI_COMMAND_SEQUENCE_STEP( 1 )
      CTL_System.skip_comm_connect = TRUE;
      for (iidx = 0; iidx < CBI_MAX_INSTRUMENTS; iidx++) {
        active_states[iidx] = CBI_UNAVAILABLE; 
      }
      for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
        if (CTL_System.p_Module[iidx]->in_recovery == CBI_TRUE) {
	  recovery_mode_detected = CBI_TRUE;
	  break;
        }
      }
      if (recovery_mode_detected) {
	printf("\n\nSaving active state summary...\n");
        printf("\nDeactivating all instruments...\n");
        printf("Activating only instruments with problem codes in preparation for a reset...\n");
        for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
	  active_states[iidx] = CTL_System.p_Module[iidx]->active;
	  CTL_System.p_Module[iidx]->active = CBI_INACTIVE;
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



    CBI_COMMAND_SEQUENCE_STEP( 2 )
      printf("Waiting %ds for instruments to boot...\n", BOOT_WAIT_SECS);
      fflush(stdout);
      sleep(BOOT_WAIT_SECS);
      printf("Pushing gain configuration values...\n");
      fflush(stdout);
      cbi_load_command(CTL_System.command_list, 
		       CTL_System.itype.num_platform_commands,
		       "PUSH FULL CONFIG",
		       CBI_FALSE);



    CBI_COMMAND_SEQUENCE_STEP( 3 )
      printf("Setting gains\n");
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
	for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
	  dpt->cbpm_op_gain.active_gain_settings[card][tblock] = appconfig.default_gain_setting;
	}
      }
      fflush(stdout);
      cbi_load_command(CTL_System.command_list,
		       CTL_System.itype.num_platform_commands,
		       "SET GAINS",
		       CBI_FALSE);

  

    CBI_COMMAND_SEQUENCE_STEP( 4 )
      printf("Setting timing setup\n");
      fflush(stdout);
      cbi_load_command(CTL_System.command_list, 
		       CTL_System.itype.num_platform_commands,
		       "SET TIMING SETUP",
		       CBI_FALSE);
    
    
    CBI_COMMAND_SEQUENCE_STEP( 5 )
      CTL_System.skip_comm_connect = TRUE;
      cbi_open_sockets_active();
      if (recovery_mode_detected) {
	printf("Activating all previously-active instruments in good standing...  \n");
	for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
	  if ( CTL_System.p_Module[iidx]->in_recovery == CBI_TRUE ) { 
	    CTL_System.p_Module[iidx]->in_recovery = CBI_FALSE; // If it was in recovery,
                                                                // it is already active.
	    status = cbi_check_heartbeat(iidx);
	    if ( status == -1 ) {
	      printf("Heartbeat check error!  DISABLING INSTRUMENT %s @ %s.\n", 
 		     CTL_System.p_Module[iidx]->comm.ethernet.hostname, 
 		     CTL_System.p_Module[iidx]->det.location ); 
	      CTL_System.p_Module[iidx]->active = CBI_INACTIVE;
	    }
	    if (status == FALSE) {
	      printf("Heartbeat NOT PRESENT!  DISABLING INSTRUMENT %s @ %s.\n", 
		     CTL_System.p_Module[iidx]->comm.ethernet.hostname, 
		     CTL_System.p_Module[iidx]->det.location ); 
	      CTL_System.p_Module[iidx]->active = CBI_INACTIVE;
	    }
	    
	    cbi_sleep(250);
	    status = cbi_check_timing_integrity(iidx);
	    if (status == -1) {
	      printf("Timing integrity check error!  DISABLING INSTRUMENT %s @ %s.\n", 
 		     CTL_System.p_Module[iidx]->comm.ethernet.hostname, 
 		     CTL_System.p_Module[iidx]->det.location ); 
 	      CTL_System.p_Module[iidx]->active = CBI_INACTIVE;
	    }
	    if(status == FALSE) {
	      printf("Timing integrity NOT PRESENT!  DISABLING INSTRUMENT %s @ %s.\n", 
 		     CTL_System.p_Module[iidx]->comm.ethernet.hostname, 
 		     CTL_System.p_Module[iidx]->det.location ); 
 	      CTL_System.p_Module[iidx]->active = CBI_INACTIVE;
	    }
		 
	  } else {
	    CTL_System.p_Module[iidx]->active = active_states[iidx];
	  }

	} //endFOR iidx
      }
      fflush(stdout);
      // Reinitialize static state-preservation array for next time.
      for (iidx = 0; iidx < CBI_MAX_INSTRUMENTS; iidx++) {
        active_states[iidx] = CBI_UNAVAILABLE; 
      }
      recovery_mode_detected = CBI_FALSE;
      cbi_close_sockets();
      CBI_NEXT_SEQUENCE_STEP
      

  CBI_COMMAND_SEQUENCE_END

}



