//------------------------------------------------------------------------
// File         :  cbpm_dump_proc_buf_a.c
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

int cbpm_dump_proc_buf( FILE *fp, int iidx ) {

  CBPM_DATA *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  fprintf(fp, " Communication Structure Dump for: cbpm_proc_buf\n");
  int  v0,  v1,  v2,  v3,  v4,  v5,  v6;
  int sv0, sv1, sv2, sv3, sv4, sv5, sv6;

  fprintf(fp, "proc_data:\n");
  for (v0 = 0; v0 < CBPM_MAX_BUNCHES; v0++) {


    fprintf(fp, "msmt_and_bun_id: %d  \n", dp->cbpm_proc_buf.proc_data[v0].msmt_and_bun_id);
    fprintf(fp, "data_counter: %d  \n", dp->cbpm_proc_buf.proc_data[v0].data_counter);
    fprintf(fp, "turn_counter: %d  \n", dp->cbpm_proc_buf.proc_data[v0].turn_counter);
    fprintf(fp, "signal:\n");
    for (sv0 = 0; sv0 < CBPM_MAX_CARDS; sv0++) {
      fprintf(fp, "%f  ", dp->cbpm_proc_buf.proc_data[v0].signal[sv0] );
    }
    fprintf(fp, "\n");
    fprintf(fp, "signal_rms:\n");
    for (sv0 = 0; sv0 < CBPM_MAX_CARDS; sv0++) {
      fprintf(fp, "%f  ", dp->cbpm_proc_buf.proc_data[v0].signal_rms[sv0] );
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");

  return CBI_F_SUCCESS;

}


