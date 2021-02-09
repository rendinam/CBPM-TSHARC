//------------------------------------------------------------------------
// File         :  cbpm_dump_cmd_params_a.c
// Date Created :  Fri Sep 14 14:08:01 2012
//
// Description  : This file was automatically genrated by the BIParser.
//                It is not a critical component of the communication data
//                structure imaging mechanism, and as such may be edited to
//                adjust the specific formatting employed when dumping the
//                contents of a given data structure without having to 
//                re-run the BIParser.
//
//                Note: If this file is edited and the BIParser is re-run
//                      and its output installed, the changes made to this
//                      file may be lost.  Make a backup copy of this file
//                      if you edit it.
//------------------------------------------------------------------------

#include "cbpm_includes.h"

int cbpm_dump_cmd_params( FILE *fp, int iidx ) {

  CBPM_DATA *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  fprintf(fp, " Communication Structure Dump for: cbpm_cmd_params\n");
  int  v0,  v1,  v2,  v3,  v4,  v5,  v6;
  int sv0, sv1, sv2, sv3, sv4, sv5, sv6;

  fprintf(fp, "species: %d  \n", dp->cbpm_cmd_params.species);
  fprintf(fp, "num_turns: %d  \n", dp->cbpm_cmd_params.num_turns);
  fprintf(fp, "bunch_pat:\n");
  for (v0 = 0; v0 < 40; v0++) {
    fprintf(fp, "0x%x  ", dp->cbpm_cmd_params.bunch_pat[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "rot_bunch_pat:\n");
  for (v0 = 0; v0 < 640; v0++) {
    fprintf(fp, "%d  ", dp->cbpm_cmd_params.rot_bunch_pat[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "trig_turns_delay: %d  \n", dp->cbpm_cmd_params.trig_turns_delay);
  fprintf(fp, "spacex_turn: %d  \n", dp->cbpm_cmd_params.spacex_turn);
  fprintf(fp, "delay_cal: %d  \n", dp->cbpm_cmd_params.delay_cal);
  fprintf(fp, "generate_delay_corrections: %d  \n", dp->cbpm_cmd_params.generate_delay_corrections);
  fprintf(fp, "gain_cal: %d  \n", dp->cbpm_cmd_params.gain_cal);
  fprintf(fp, "gain_xcal: %d  \n", dp->cbpm_cmd_params.gain_xcal);
  fprintf(fp, "avg_mode: %d  \n", dp->cbpm_cmd_params.avg_mode);
  fprintf(fp, "scale_mode: %d  \n", dp->cbpm_cmd_params.scale_mode);
  fprintf(fp, "update_mode: %d  \n", dp->cbpm_cmd_params.update_mode);
  fprintf(fp, "use_data_enable: %d  \n", dp->cbpm_cmd_params.use_data_enable);
  fprintf(fp, "trig_mask: %d  \n", dp->cbpm_cmd_params.trig_mask);
  fprintf(fp, "reset_proc_buf_idx: %d  \n", dp->cbpm_cmd_params.reset_proc_buf_idx);
  fprintf(fp, "generate_phase_tables: %d  \n", dp->cbpm_cmd_params.generate_phase_tables);
  fprintf(fp, "tblock: %d  \n", dp->cbpm_cmd_params.tblock);
  fprintf(fp, "bun_pat_offset: %d  \n", dp->cbpm_cmd_params.bun_pat_offset);
  fprintf(fp, "checksum: %d  \n", dp->cbpm_cmd_params.checksum);

  return CBI_F_SUCCESS;

}


