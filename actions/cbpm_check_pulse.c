//-----------------------------------------------------------------------+
// File         : cbpm_check_pulse.c                                     |
//                                                                       |
// Description  : Retrieves heartbeat and timing integrity information   |
//                from an instrument and displays a summary.             |
//                                                                       |
// Arguments    : None                                                   |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_check_pulse_post( int iidx ) {
  
  int stat;

  CBPM_DATA  *dp;

  dp = CTL_System.p_Module[iidx]->dsp_data;

  int  dsp_hb1, dsp_hb2;
  int  integ1, integ2;
  char integrity_names[2][5] = {"POOR", "GOOD"};
  int  integrity_flag = 1;

  stat = cbi_gp_struct(READ, CTL_System.comm_method, CBI_HEARTBEAT_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT); 

  if ( cbi_check_heartbeat(iidx) ) {
    printf("-----------------\n");
    printf("Instrument heartbeat:    OK!\n");
    printf("Timing integrity    : %s   Turns seen: %d\n", 
	   integrity_names[ cbi_check_timing_integrity(iidx) ],
	   CTL_System.p_Module[iidx]->core->cbi_heartbeat.turns_seen ); //   dp->cbi_CO_heartbeat.turns_seen );
  } else {
    printf("BAD Instrument Heartbeat!\n");
    printf("Timing integrity info is not available.\n");
  }
  printf("\n\n");

  return CBI_F_SUCCESS;
  
}
