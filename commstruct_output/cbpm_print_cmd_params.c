///-----------------------------------------------------------------------+
/// File         :  cbpm_print_cmd_params_struct.c                        |
///                                                                       |
/// Description  :  Dumps the contents of the CMD_PARAMS struct to the    |
///                 console.                                              |
///                                                                       |
/// Arguments    :  Index of instrument's master struct.                  |
///                                                                       |
/// Author       :  M. Rendina                                            |
///-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_print_cmd_params_struct(int BPM_IDX, FILE *fp) {

  int i;

  CBPM_DATA  *dp;
  dp = CTL_System.p_Module[BPM_IDX]->dsp_data;

  fprintf(fp, "    CMD PARAMS Structure Contents\n");
  fprintf(fp, "species     :  %d\n", dp->cbpm_cmd_params.species);
  fprintf(fp, "num_turns   :  %d\n", dp->cbpm_cmd_params.num_turns);
  fprintf(fp, "trig_turns_delay:  %d\n", dp->cbpm_cmd_params.trig_turns_delay);
  fprintf(fp, "spacex_turn :  %d\n", dp->cbpm_cmd_params.spacex_turn);
  fprintf(fp, "delay_cal   :  %d\n", dp->cbpm_cmd_params.delay_cal);
  fprintf(fp, "gain_cal    :  %d\n", dp->cbpm_cmd_params.gain_cal);
  fprintf(fp, "gain_xcal   :  %d\n", dp->cbpm_cmd_params.gain_xcal);
  fprintf(fp, "avg_mode    :  %d\n", dp->cbpm_cmd_params.avg_mode);
  fprintf(fp, "scale_mode  :  %d\n", dp->cbpm_cmd_params.scale_mode);
  fprintf(fp, "update_mode :  %d\n", dp->cbpm_cmd_params.update_mode);

  // Print the bunch patterns before and after rotation
  cbpm_print_patterns( dp->cbpm_cmd_params.bunch_pat, dp->cbpm_cmd_params.rot_bunch_pat, fp );

  ////fprintf(fp, "tot_bunches:  %d\n", dp->cbpm_cmd_params.tot_bunches);
  ////fprintf(fp, "num_bun[0] :  %d\n", dp->cbpm_cmd_params.num_bunches[0]);
  ////fprintf(fp, "num_bun[1] :  %d\n", dp->cbpm_cmd_params.num_bunches[1]);
  ////fprintf(fp, "b_p_offset :  %d\n", dp->cbpm_cmd_params.bun_pat_offset);
  ////fprintf(fp, "bits wrap'd:  %3d   %3d\n", 
  ////  dp->cbpm_cmd_params.num_BP_bits_wrapped[0], 
  ////  dp->cbpm_cmd_params.num_BP_bits_wrapped[1] );
  fprintf(fp, "-------------\n");

  return F_SUCCESS;

}
