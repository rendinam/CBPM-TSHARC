///-----------------------------------------------------------------------+
/// File         :  cbpm_print_proc_buf_entries.c                         |
///                                                                       |
/// Description  :  Dumps the contents of the PROC_BUF struct to the      |
///                 console.                                              |
///                                                                       |
/// Arguments    :  Index of instrument's master struct.                  |
///                 Write verbosity mode (0 = condensed, 1 = full)        |
///                   Condensed contains bunch index, button avgs and RMS |
///                   Full contains a dump of all memebers of each struct |
///                 First entry in the processed data buffer (starts at 0)|
///                 Last entry to print in the processed data buffer      |
///                 Pointer to FILE stream (can be disk file or STDOUT)   |
///                                                                       |
/// Author       :  M. Rendina                                            |
///-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_print_proc_buf_entries(int mode, int iidx, int first, int last, FILE *fp) {

  int i, j;

  CBPM_DATA  *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;
  
  char cesr_types[2][3] = {"", "EXT"};
  int cesr_type = 0;
  int cesr_idx;
  if ( strstr(CTL_System.p_Module[iidx]->comm.xbus.adr_node, "STD") == NULL ) {
    cesr_type = 1;
  }

  printf("Processed data acquisition results for BPM %s (%s)   CESR index: %s %d\n", 
	 CTL_System.p_Module[iidx]->comm.ethernet.hostname,
	 CTL_System.p_Module[iidx]->comm.ethernet.IP_address,
	 cesr_types[cesr_type],
	 CTL_System.p_Module[iidx]->comm.xbus.element  );

  if (mode == 0) {
    fprintf(fp, "# ");
    fprintf(fp, "%s\n", CTL_System.p_Module[iidx]->det.name );

    fprintf(fp, "#                     Button Averages                         |                       RMSs \n");
    fprintf(fp, "#          0             1             2             3        |      0             1             2             3      \n");
    fprintf(fp, "#          TI            BI            BO            TO       |\n");
    fprintf(fp, "#    -----------------------------------------------------------------------------------------------------------------\n");
  }

  for (i = first; i <= last; i++ ) {

    if (mode == 1) {
      fprintf(fp, "   PROC_BUF Structure Contents  [ %d ] \n", i);

      fprintf(fp, "bunch_id     : %d \n", dp->cbpm_proc_buf.proc_data[i].msmt_and_bun_id);
      fprintf(fp, "turn_counter : %d \n", dp->cbpm_proc_buf.proc_data[i].turn_counter);
      fprintf(fp, "button       : ");
      for (j = 0; j < CBPM_MAX_CARDS; j++) {
	fprintf(fp, "%f   ", dp->cbpm_proc_buf.proc_data[i].signal[j]);
      }
      fprintf(fp, "\n");
      fprintf(fp, "button_rms   : ");
      for (j = 0; j < CBPM_MAX_CARDS; j++) {
	fprintf(fp, "%f   ", dp->cbpm_proc_buf.proc_data[i].signal_rms[j]);
      }
      fprintf(fp, "\n\n");

    } else {

      fprintf(fp, "%3d) ", i);
      for (j = 0; j < CBPM_MAX_CARDS; j++) {
	fprintf(fp, "%12f  ", dp->cbpm_proc_buf.proc_data[i].signal[j]);
      }
      fprintf(fp, "  ");
      for (j = 0; j < CBPM_MAX_CARDS; j++) {
	fprintf(fp, "%12f  ", dp->cbpm_proc_buf.proc_data[i].signal_rms[j]);
      }
      fprintf(fp, "\n");

    }

  }

  fprintf(fp, "\n\n");
      
  return F_SUCCESS;

}
