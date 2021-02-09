//------------------------------------------------------------------------
// File         :  cbpm_dump_phase_config_a.c
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

int cbpm_dump_phase_config( FILE *fp, int iidx ) {

  CBPM_DATA *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  fprintf(fp, " Communication Structure Dump for: cbpm_phase_config\n");
  int  v0,  v1,  v2,  v3,  v4,  v5,  v6;
  int sv0, sv1, sv2, sv3, sv4, sv5, sv6;

  fprintf(fp, "phase_jump_boundaries:\n");
  for (v0 = 0; v0 < CBI_NUM_SPECIES; v0++) {
    for (v1 = 0; v1 < CBPM_MAX_PHASE_JUMPS; v1++) {
      fprintf(fp, "%d  ", dp->cbpm_phase_config.phase_jump_boundaries[v0][v1] );
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
  fprintf(fp, "phase_turn_offsets:\n");
  for (v0 = 0; v0 < (CBI_NUM_SPECIES); v0++) {
    for (v1 = 0; v1 < (CBPM_MAX_PHASE_JUMPS+1); v1++) {
      fprintf(fp, "%d  ", dp->cbpm_phase_config.phase_turn_offsets[v0][v1] );
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
  fprintf(fp, "phase_wait_values:\n");
  for (v0 = 0; v0 < (CBI_NUM_SPECIES); v0++) {
    for (v1 = 0; v1 < (CBPM_MAX_PHASE_JUMPS+1); v1++) {
      fprintf(fp, "%d  ", dp->cbpm_phase_config.phase_wait_values[v0][v1] );
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");

  return CBI_F_SUCCESS;

}


