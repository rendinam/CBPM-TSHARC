///-----------------------------------------------------------------------+
/// File         :  cbpm_print_turn_count_summary.c                       |
///                                                                       |
/// Description  :  Prints a formatted table of the turns counter value   |
///                 for each active instrument.  This will only print out |
///                 valid values if the raw_buffer_header contents have   |
///                 been obtained prior to this call.                     |
///                                                                       |
/// Arguments    :  none                                                  |
///                                                                       |
/// Author       :  M. Rendina                                            |
///-----------------------------------------------------------------------+

#include "cbpm_includes.h"

#define MAX_COLUMNS                   4

int cbpm_print_turn_count_summary( void ) {

  // Provide a turn-synchronization summary.
  // Loop over all instruments, and show the value of the main turn counter
  // that was stored into the raw data header at the beginning of the acquisition.
  int iidx, inst_cnt = 0, num_per_row = 8;
  int col, row;
  int entries_per_column;
  int entry, max_rows, row_cnt = 1;
  int entries_in_row;
  CBPM_DATA  *dp;

  int n_online = num_online();

  printf("\n\n                                  - Turn counter synchronization summary -\n\n");

  max_rows = ceilf((float)(n_online / (float)MAX_COLUMNS));

  if (max_rows < 1) {
    max_rows = n_online;
  }


  iidx = 0;
    
  for (row = 0; row < max_rows; row++) {

    entries_in_row = MAX_COLUMNS;

    for (entry = 0; entry < entries_in_row; entry++) {
      if (inst_cnt < n_online) {
	if( CTL_System.p_Module[iidx+(entry*max_rows)]->active == CBI_ACTIVE) {
	  dp = CTL_System.p_Module[iidx+(entry*max_rows)]->dsp_data;      
	  printf("  %-7s %-9s %-8d |", 
		 CTL_System.p_Module[iidx+(entry*max_rows)]->det.location,
		 CTL_System.p_Module[iidx+(entry*max_rows)]->comm.ethernet.hostname,
		 dp->cbpm_raw_data_header.turn_counter  );
	} else {
	  printf("  %-7s %-9s          |", 
		 CTL_System.p_Module[iidx+(entry*max_rows)]->det.location,
		 CTL_System.p_Module[iidx+(entry*max_rows)]->comm.ethernet.hostname );
	}
      }
      inst_cnt++;
    }
    iidx++;
    printf("\n");
    
    
    if (row_cnt == max_rows) {
      break;
    } else {
      row_cnt++;
    }
    
    
  } // endFOR row
  

  return CBI_F_SUCCESS;

}
