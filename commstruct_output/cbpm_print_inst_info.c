///-----------------------------------------------------------------------+
/// File         :  cbpm_print_inst_info.c                                |
///                                                                       |
/// Description  :  Prints out formatted instrument information as can be |
///                 used to supplement the main instrument list output.   |
///                                                                       |
/// Arguments    :  Index of instrument's master struct.                  |
///                 Pointer to file stream that will receive data         |
///                 Flag for printing just the column labels (TRUE), or   |
///                          printing the info fields from a single       |
///                          instrument.                                  |
///                                                                       |
///                By convention, the output should not include trailing  |
///                a newline character.  The calling function handles     |
///                this formatting.                                       |
///                                                                       |
/// Author       :  M. Rendina                                            |
///-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_print_inst_info(int iidx, FILE *fp, int mode) {

  int i;

  CBPM_DATA  *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;


  if (mode == CBI_LABELS) {
    // Print column labels
    fprintf(fp, " Timing Setup" );

  } else if (mode == CBI_DIVIDER) {
    // Print a dividing line between labels and data fields
    fprintf(fp, "--------------" );

  } else {
    // Print informational fields
    fprintf(fp, " %s ", tsetup_names[dp->cbpm_op_timing.active_timing_setup] );
  }


  return F_SUCCESS;

}
