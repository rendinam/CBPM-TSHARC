//-----------------------------------------------------------------------+
// File         :  cbpm_get_temps.c                                      |
//                                                                       |
// Description  :  Reads out all temperature sensor data from instrument |
//                                                                       |
// Arguments    :  Instrument index                                      |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_get_temps_post(int iidx) {

  int card, status = 0;

  CBPM_DATA  *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TEMPERATURES_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);

  
  printf("   --  %s  :  %s    (%s %d)  --\n",
	 CTL_System.p_Module[iidx]->det.location,
	 CTL_System.p_Module[iidx]->comm.ethernet.hostname,
	 CTL_System.p_Module[iidx]->comm.xbus.pkt_node,
	 CTL_System.p_Module[iidx]->comm.xbus.element
	 );
  printf("Dig temp: %3.0f C\n", dp->cbpm_temperatures.dig_temp );
  printf("Tim temp: %3.0f C\n", dp->cbpm_temperatures.tim_temp );
  for (card = 0; card< CBPM_MAX_CARDS; card++) {
    printf("FE%d temp: %3.0f C\n",card, dp->cbpm_temperatures.fe_temps[card] );
  }

  return F_SUCCESS;
  
}
