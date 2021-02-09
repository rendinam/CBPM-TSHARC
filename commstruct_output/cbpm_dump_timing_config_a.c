//------------------------------------------------------------------------
// File         :  cbpm_dump_timing_config_a.c
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

int cbpm_dump_timing_config( FILE *fp, int iidx ) {

  CBPM_DATA *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  fprintf(fp, " Communication Structure Dump for: cbpm_timing_config\n");
  int  v0,  v1,  v2,  v3,  v4,  v5,  v6;
  int sv0, sv1, sv2, sv3, sv4, sv5, sv6;

  fprintf(fp, "setups:\n");
  for (v0 = 0; v0 < CBPM_MAX_TIMING_SETUPS; v0++) {


    fprintf(fp, "timing_mode: %d  \n", dp->cbpm_timing_config.setups[v0].timing_mode);
    fprintf(fp, "cTMD: %d  \n", dp->cbpm_timing_config.setups[v0].cTMD);
    fprintf(fp, "BP_offsets:\n");
    for (sv0 = 0; sv0 < CBPM_MAX_TIMING_BLOCKS; sv0++) {
      fprintf(fp, "%d  ", dp->cbpm_timing_config.setups[v0].BP_offsets[sv0] );
    }
    fprintf(fp, "\n");
    fprintf(fp, "bTMDs:\n");
    for (sv0 = 0; sv0 < CBPM_MAX_TIMING_BLOCKS; sv0++) {
      fprintf(fp, "%d  ", dp->cbpm_timing_config.setups[v0].bTMDs[sv0] );
    }
    fprintf(fp, "\n");
    fprintf(fp, "block_delays:\n");
    for (sv0 = 0; sv0 < CBPM_MAX_TIMING_BLOCKS; sv0++) {
      fprintf(fp, "%d  ", dp->cbpm_timing_config.setups[v0].block_delays[sv0] );
    }
    fprintf(fp, "\n");
    fprintf(fp, "chan_delays:\n");
    for (sv0 = 0; sv0 < CBPM_MAX_TIMING_BLOCKS; sv0++) {
      for (sv1 = 0; sv1 < CBPM_MAX_CARDS; sv1++) {
        fprintf(fp, "%d  ", dp->cbpm_timing_config.setups[v0].chan_delays[sv0][sv1] );
      }
      fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");

  return CBI_F_SUCCESS;

}


