/*!-----------------------------------------------------------------------+
* File         :  cbpm_sig_handlers.c                                     |
*                                                                         |
* Description  :  Signal handling routines                                |
*                                                                         |
* Arguments    :  POSIX Signal number to handle (integer)                 |
*                                                                         |
* Author       :  M. Rendina                                              |
*-------------------------------------------------------------------------+*/

#include "cbpm_includes.h"


/* //-------------------------------------------------------- */
/* // Checks every online instrument for open sockets and */
/* // closes them. */
/* //-------------------------------------------------------- */
/* int cleanup_sockets(void) { */

/*   int iidx; */
/*   for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) { */
/*     if ( CTL_System.p_Module[iidx]->active == CBI_ACTIVE ) { */
    
/*       if (CTL_System.p_Module[iidx]->comm.ethernet.socket_ID != CBI_SOCKET_CLOSED) { */
	
/* 	printf("Cleaning up <%s>... ", CTL_System.p_Module[iidx]->comm.ethernet.hostname ); */
/* 	// Tell server to close its end of socket */
/* 	cbi_net_close_socket(CTL_System.p_Module[iidx]->comm.ethernet.socket_ID); */
	
/* 	// Close local end of socket */
/* 	cbi_net_net_close(CTL_System.p_Module[iidx]->comm.ethernet.socket_ID); */
/* 	CTL_System.p_Module[iidx]->comm.ethernet.socket_ID = CBI_SOCKET_CLOSED; */
	
/* 	printf("Done.\n"); */
/*       } */

/*     } //endIF active */
/*   } //endFOR */
  
/*   return F_SUCCESS; */
/* } */

/* //-------------------------------------------------------- */
/* // For handling a CTRL-C event during normal operation */
/* //-------------------------------------------------------- */
/* void std_sigint_handler( int signum ) { */

/*   printf("\n\nCaught CTRL-C\n\n"); */

/*   cleanup_sockets(); */

/*   exit(0); */

/* } */



//--------------------------------------------------------
// For handling a CTRL-C event during external interface 
// (server) mode
//--------------------------------------------------------
void server_mode_sigint_handler( int signum ) {

  // Instruct the server to terminate
  CTL_CBPM_Intf_Monitor.state = CBI_CTL_INTF_OFF;

}



//--------------------------------------------------------
// For handling a CTRL-C event during continous 
// bunch-current monitoring mode
//--------------------------------------------------------
void continuous_mode_sigint_handler( int signum ) {

  // Instruct the control application's continuous 
  // monitoring loop to terminate.
  continuous_mode = FALSE;

}



//-------------------------------------------------------
// Just note that a SIGPIPE was received and exit.
//-------------------------------------------------------
void std_sigpipe_handler( int signum ) {

  printf("\n\nTerminating on signal SIGPIPE (%d)\n", SIGPIPE);
  cleanup_sockets();
  exit(0);

}


//--------------------------------------------------------
// To aid in recovering from broken pipe signals (not 
// yet fully understood) during continuous current
// measurement.
//--------------------------------------------------------
void continuous_mode_sigpipe_handler( int signum ) {

  int iidx, status;

  // Re-enable cursor display in case this happened in 
  // current monitor mode.
  printf("\033[?25h");

  printf("\n\nEncountered broken pipe during current monitoring!\n");
  printf("Backing out gracefully... ");

  for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
    if ( CTL_System.p_Module[iidx]->active == CBI_ACTIVE ) {

      printf("Instrument: %d\n", iidx);
      // Send the CBI_IDLE_CMD to the instrument to terminate its measurement loop.
      CTL_System.p_Module[iidx]->core->cbi_cmd.cmd       = CBI_IDLE_CMD;
      CTL_System.p_Module[iidx]->core->cbi_cmd.handshake = CBI_CMD_NEW_HANDSHAKE;
      
      status = cbi_gp_struct(WRITE, CTL_System.comm_method, CBI_CMD_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

    }
  }
  printf("DONE.\n\n");

  time_t      curtime;
  struct tm  *loctime;
  curtime = time(NULL);
  loctime = localtime(&curtime);

  FILE *fp;
  fp = fopen("Error.log", "a");
  fprintf(fp, "Caught SIGPIPE -- %s", asctime(loctime) );
  fclose(fp);

  cleanup_sockets();

  printf("This event has been logged for diagnostic purposes.\n\n");
  printf("You will need to restart the application in order to \n");
  printf("resume continuous monitoring.\n\n");
  exit(0);
}



//--------------------------------------------------------
// Test handler for segmentation violation trapping for 
// eventual debug trace on exit.
//--------------------------------------------------------
void segfault_handler( int signum ) {
  
  printf("SIGSEGV -- Segmentation Violation \n");
  cleanup_sockets();
  // Ultimately perform a debugging dump here
  //printf("TEST GETTING VALUES FROM ELSEWHERE:  cmd_names[0] = %s\n", cmd_names[0]);
  
  // Clean up communications so that the exit is graceful
  
  exit(1);

}

