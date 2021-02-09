//-----------------------------------------------------------------------+
// File         : cbpm_reset_hardware.c                                  |
//                                                                       |
// Description  : Command support for direct, register level, resetting  |
//                of instrument hardware.                                |
//                Routines provided allow for low-level resets of:       |
//                 Coldfire (ethernet) communications board              |
//                 Digital board processor (DSP)   This, based on code   |
//                  running in the instrument, also causes a subsequent  |
//                  Coldfire board reset.                                |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int reset_coldfire( int iidx ) {

  int k;

  // Close any open connections on the remote ends gracefully, since this will 
  // forcibly reset the remote hardware.
  printf("Closing all open sockets to instruments.\n");
  cbi_close_sockets();

  printf("Resetting the Coldfire/CBInet server via XBUS for instrument index = %d...\n", iidx);
  k = cbi_xbus_poke(iidx, DIMM_RESET, 0);
  if (k != CBI_F_SUCCESS) {
    printf("\nMPMnet write failed!\n\n");
  }
  k = cbi_xbus_poke(iidx, DIMM_RESET, 1);
  if (k != CBI_F_SUCCESS) {
    printf("\nMPMnet write failed!\n\n");
  }

  //sleep(11); // Wait for reboot to complete.  Depends on Coldfire boot timeout ENV var value.

  if (k == CBI_F_SUCCESS) {
    printf("Done\n");
  }

  return CBI_F_SUCCESS;

}



int reset_coldfire_final( void ) {

  printf("Waiting 22s for CBInet servers to complete boot process...");
  fflush(stdout);
  sleep(22); // Wait for reboot to complete.  Depends on Coldfire boot timeout ENV var value.
  printf(" Done.\n");
  return CBI_F_SUCCESS;

}



// NOTE: Causes a reset of the Coldfire ethernet communications board as well.
//       This board has a several second boot-up time, due to initializations
//       and communication tries with the NTP and/or DNS server.
int reset_instrument( int iidx ) {

  char *func_name = (char *)__FUNCTION__;
  int k;

  k = cbi_xbus_poke( iidx, DSP_RESET, 0);
  if (k != CBI_F_SUCCESS) {

    sprintf(message, "MPMnet xbus reset write of 0 to address 0x%x for %s FAILED!",
	    DSP_RESET,
	    CTL_System.p_Module[iidx]->comm.ethernet.hostname );
    log_message(S_ERROR, func_name, message);
    return CBI_F_FAILURE;
  }
  
  k = cbi_xbus_poke( iidx, DSP_RESET, 1);
  if (k != CBI_F_SUCCESS) {
    
    sprintf(message, "MPMnet xbus reset write of 1 to address 0x%x for %s FAILED!",
	    DSP_RESET,
	    CTL_System.p_Module[iidx]->comm.ethernet.hostname );
    log_message(S_ERROR, func_name, message);
    return CBI_F_FAILURE;
  }

  return CBI_F_SUCCESS;

}
