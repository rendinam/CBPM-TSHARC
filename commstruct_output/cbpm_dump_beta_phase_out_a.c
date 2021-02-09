//------------------------------------------------------------------------
// File         :  cbpm_dump_beta_phase_out_a.c
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

int cbpm_dump_beta_phase_out( FILE *fp, int iidx ) {

  CBPM_DATA *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  fprintf(fp, " Communication Structure Dump for: cbpm_beta_phase_out\n");
  int  v0,  v1,  v2,  v3,  v4,  v5,  v6;
  int sv0, sv1, sv2, sv3, sv4, sv5, sv6;

  fprintf(fp, "bunch_id: %d  \n", dp->cbpm_beta_phase_out.bunch_id);
  fprintf(fp, "tot_turns: %d  \n", dp->cbpm_beta_phase_out.tot_turns);
  fprintf(fp, "turn_ctr_offset: %d  \n", dp->cbpm_beta_phase_out.turn_ctr_offset);
  fprintf(fp, "phase_results:\n");
  for (v0 = 0; v0 < CBPM_NUM_DP_TURNS_OFFSET; v0++) {
    for (v1 = 0; v1 < CBPM_HALF_TURN_OPTIONS; v1++) {
      for (v2 = 0; v2 < CBPM_MAX_PHASE_DIM; v2++) {
        for (v3 = 0; v3 < CBPM_NUM_PHASE_COEFFS; v3++) {
          for (v4 = 0; v4 < CBPM_MAX_CARDS; v4++) {
            fprintf(fp, "%f  ", dp->cbpm_beta_phase_out.phase_results[v0][v1][v2][v3][v4] );
          }
          fprintf(fp, "\n");
        }
        fprintf(fp, "\n");
      }
      fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
  fprintf(fp, "signal:\n");
  for (v0 = 0; v0 < CBPM_MAX_CARDS; v0++) {
    fprintf(fp, "%f  ", dp->cbpm_beta_phase_out.signal[v0] );
  }
  fprintf(fp, "\n");

  return CBI_F_SUCCESS;

}


