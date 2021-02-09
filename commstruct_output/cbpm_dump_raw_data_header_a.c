//------------------------------------------------------------------------
// File         :  cbpm_dump_raw_data_header_a.c
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

int cbpm_dump_raw_data_header( FILE *fp, int iidx ) {

  CBPM_DATA *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  fprintf(fp, " Communication Structure Dump for: cbpm_raw_data_header\n");
  int  v0,  v1,  v2,  v3,  v4,  v5,  v6;
  int sv0, sv1, sv2, sv3, sv4, sv5, sv6;

  fprintf(fp, "tot_bunches: %d  \n", dp->cbpm_raw_data_header.tot_bunches);
  fprintf(fp, "num_bunches:\n");
  for (v0 = 0; v0 < CBPM_MAX_TIMING_BLOCKS; v0++) {
    fprintf(fp, "%d  ", dp->cbpm_raw_data_header.num_bunches[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "num_BP_bits_wrapped:\n");
  for (v0 = 0; v0 < CBPM_MAX_TIMING_BLOCKS; v0++) {
    fprintf(fp, "%d  ", dp->cbpm_raw_data_header.num_BP_bits_wrapped[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "num_turns: %d  \n", dp->cbpm_raw_data_header.num_turns);
  fprintf(fp, "skip_turns_active: %d  \n", dp->cbpm_raw_data_header.skip_turns_active);
  fprintf(fp, "turn_counter: %d  \n", dp->cbpm_raw_data_header.turn_counter);
  fprintf(fp, "trig_turns_delay: %d  \n", dp->cbpm_raw_data_header.trig_turns_delay);
  fprintf(fp, "spacex_turn: %d  \n", dp->cbpm_raw_data_header.spacex_turn);
  fprintf(fp, "scale_mode: %d  \n", dp->cbpm_raw_data_header.scale_mode);
  fprintf(fp, "ADC_zero_val_bitfields:\n");
  for (v0 = 0; v0 < CBPM_MAX_CARDS; v0++) {
    fprintf(fp, "%d  ", dp->cbpm_raw_data_header.ADC_zero_val_bitfields[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "ADC_lower_thresh_bitfields:\n");
  for (v0 = 0; v0 < CBPM_MAX_CARDS; v0++) {
    fprintf(fp, "%d  ", dp->cbpm_raw_data_header.ADC_lower_thresh_bitfields[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "ADC_low_thresh_bitfields:\n");
  for (v0 = 0; v0 < CBPM_MAX_CARDS; v0++) {
    fprintf(fp, "%d  ", dp->cbpm_raw_data_header.ADC_low_thresh_bitfields[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "ADC_high_thresh_bitfields:\n");
  for (v0 = 0; v0 < CBPM_MAX_CARDS; v0++) {
    fprintf(fp, "%d  ", dp->cbpm_raw_data_header.ADC_high_thresh_bitfields[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "ADC_saturation_bitfields:\n");
  for (v0 = 0; v0 < CBPM_MAX_CARDS; v0++) {
    fprintf(fp, "%d  ", dp->cbpm_raw_data_header.ADC_saturation_bitfields[v0] );
  }
  fprintf(fp, "\n");
  fprintf(fp, "gain:\n");
  for (v0 = 0; v0 < CBPM_MAX_CARDS; v0++) {
    for (v1 = 0; v1 < CBPM_MAX_TIMING_BLOCKS; v1++) {
      fprintf(fp, "%d  ", dp->cbpm_raw_data_header.gain[v0][v1] );
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");

  return CBI_F_SUCCESS;

}


