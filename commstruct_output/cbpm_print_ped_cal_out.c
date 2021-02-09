///-----------------------------------------------------------------------+
/// File         :  cbpm_print_ped_cal_out.c                              |
///                                                                       |
/// Description  :  Dumps the contents of the PED_CAL_OUT struct to the   |
///                 console.                                              |
///                                                                       |
/// Arguments    :  Index of instrument's master struct.                  |
///                                                                       |
/// Author       :  M. Rendina                                            |
///-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_print_ped_cal_out(int iidx, FILE *fp) {

  int i, j, setting;
  int gain_value;

  CBPM_DATA  *dp;
  dp   = CTL_System.p_Module[iidx]->dsp_data;

  fprintf(fp, "\n\n");

  int step = 0;
  for ( setting = 0; setting < CBPM_MAX_GAINS; setting++) {
	gain_value = dp->cbpm_gain_config.gain_codes[setting];
    if (gain_value == 0) {
      fprintf(fp, "Gain setting %d            FIXED GAIN      \n", step );
    } else {
      fprintf(fp, "Gain setting %d       Decimal gain value %d      Decimal gain code %d\n", step, gain_value, gain_value+256  );
    }
    fprintf(fp, "-----------------------------------------------------------------------------------------------------------\n");
    fprintf(fp, "                   Chan-0                                                Chan-1    \n");
    for (i = 0; i < CBPM_MAX_CARDS; i++ ) {
      fprintf(fp, "ped_%d = %12f   rms_%d = %12f       |       ped_%d = %12f   rms_%d =  %12f\n", \
             i, \
	     dp->cbpm_ped_cal_out.ped_table    [0][i][step], \
	     i, \
	     dp->cbpm_ped_cal_out.ped_rms_table[0][i][step], \
	     i, \
	     dp->cbpm_ped_cal_out.ped_table    [1][i][step], \
	     i, \
	     dp->cbpm_ped_cal_out.ped_rms_table[1][i][step] );
    }
    fprintf(fp, "\n");
    step++;
  }

  return F_SUCCESS;

}
