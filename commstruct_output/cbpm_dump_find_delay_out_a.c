//------------------------------------------------------------------------
// File         :  cbpm_dump_find_delay_out_a.c
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

int cbpm_dump_find_delay_out( FILE *fp, int iidx ) {

  CBPM_DATA *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  fprintf(fp, " Communication Structure Dump for: cbpm_find_delay_out\n");
  int  v0,  v1,  v2,  v3,  v4,  v5,  v6;
  int sv0, sv1, sv2, sv3, sv4, sv5, sv6;

  fprintf(fp, "max_block_delay: %d  \n", dp->cbpm_find_delay_out.max_block_delay);
  fprintf(fp, "fcoeffs:\n");
  for (v0 = 0; v0 < CBPM_MAX_FIT_COEFFS; v0++) {
    fprintf(fp, "%f  ", dp->cbpm_find_delay_out.fcoeffs[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "peaks:\n");
  for (v0 = 0; v0 < CBPM_MAX_ACQ_CONTROLLERS; v0++) {
    fprintf(fp, "%f  ", dp->cbpm_find_delay_out.peaks[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "offsets:\n");
  for (v0 = 0; v0 < CBPM_MAX_ACQ_CONTROLLERS; v0++) {
    fprintf(fp, "%d  ", dp->cbpm_find_delay_out.offsets[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "prevADCRMSs:\n");
  for (v0 = 0; v0 < CBPM_MAX_ACQ_CONTROLLERS; v0++) {
    fprintf(fp, "%f  ", dp->cbpm_find_delay_out.prevADCRMSs[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "postADCRMSs:\n");
  for (v0 = 0; v0 < CBPM_MAX_ACQ_CONTROLLERS; v0++) {
    fprintf(fp, "%f  ", dp->cbpm_find_delay_out.postADCRMSs[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "num_retries: %d  \n", dp->cbpm_find_delay_out.num_retries);
  fprintf(fp, "numTurnsToCollectPeakData: %d  \n", dp->cbpm_find_delay_out.numTurnsToCollectPeakData);

  return CBI_F_SUCCESS;

}


