/*!-----------------------------------------------------------------------+
* File         :  cbpm_write_find_bunch_dat.c                             |
*                                                                         |
* Description  :  Simple dump to disk of the raw buffer struct of a given |
*                 device.                                                 |
*                                                                         |
* Arguments    :  Index of device (integer)                               |
*                                                                         |
* Author       :  M. Rendina                                              |
*-------------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_write_find_bunch_dat( int iidx, FILE *fp, char *filename ) {

  int i, k;

  int NUM_FB_STEPS;

  CBPM_DATA  *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  int species = dp->cbpm_cmd_params.species;

  fprintf(fp, "#-------------------------------------------------------------------\n");
  fprintf(fp, "# Bunch pattern offset: %d \n", dp->cbpm_cmd_params.bun_pat_offset );
  fprintf(fp, "#-------------------------------------------------------------------\n");

  int   num_buckets;

  int timing_setup = dp->cbpm_op_timing.active_timing_setup;

  if ( timing_setup == s4ns_P_VAR_G || timing_setup == s4ns_E_VAR_G ) {
    NUM_FB_STEPS = 30;
    num_buckets  = CBPM_4NS_MAX_BUNCHES / 2;
  } else {
    NUM_FB_STEPS = 50;
    num_buckets  = CBPM_14NS_MAX_BUNCHES / 2;
  }
  
  printf("Dumping data to file '%s'.\n", filename);

  int   dstep, turn, bucket;
  int   num_turns = dp->cbpm_cmd_params.num_turns;
  float accum[CBPM_MAX_CARDS];
  float idx = 0.0;

  int card;
  int buf_idx;
  int value = 0;
  for (bucket = 0; bucket < num_buckets; bucket++) {
    for (dstep = 0; dstep < NUM_FB_STEPS; dstep++) {
      for (card = 0; card < CBPM_MAX_CARDS; card++) {


         accum[card] = 0.0;
         for (turn = 0; turn < num_turns; turn++) { 
           if (card == 0 ) {
	       if(timing_setup == s14ns_VAR_G && species == ELECTRONS) {
		 value = (float)dp->cbpm_adc_buffer1.raw_data[(dstep*num_buckets*num_turns) + (turn*num_buckets) + bucket];
	       } else {
		 value = (float)dp->cbpm_adc_buffer0.raw_data[(dstep*num_buckets*num_turns) + (turn*num_buckets) + bucket];
	       }
	   }
	   
           if (card == 1) {
	       if(timing_setup == s14ns_VAR_G && species == ELECTRONS) {
		 value = (float)dp->cbpm_adc_buffer3.raw_data[(dstep*num_buckets*num_turns) + (turn*num_buckets) + bucket];
	       } else {
		 value = (float)dp->cbpm_adc_buffer2.raw_data[(dstep*num_buckets*num_turns) + (turn*num_buckets) + bucket];
	       }
	   }

	   if (card == 2) {
	       if(timing_setup == s14ns_VAR_G && species == ELECTRONS) {
		 value = (float)dp->cbpm_adc_buffer5.raw_data[(dstep*num_buckets*num_turns) + (turn*num_buckets) + bucket];
	       } else {
		 value = (float)dp->cbpm_adc_buffer4.raw_data[(dstep*num_buckets*num_turns) + (turn*num_buckets) + bucket];
	       }
	   }

	   if (card == 3) {
	     if(timing_setup == s14ns_VAR_G && species == ELECTRONS) {
	       value = (float)dp->cbpm_adc_buffer7.raw_data[(dstep*num_buckets*num_turns) + (turn*num_buckets) + bucket];
	     } else {
	       value = (float)dp->cbpm_adc_buffer6.raw_data[(dstep*num_buckets*num_turns) + (turn*num_buckets) + bucket];
	     }
	   }
	   accum[card] += value;
	 } //endFOR turn
      } //endFOR card
      fprintf(fp, "%3.2f   %f   %f   %f   %f\n",
	      idx/NUM_FB_STEPS, 
	      accum[0]/(float)num_turns, 
	      accum[1]/(float)num_turns,
	      accum[2]/(float)num_turns,
	      accum[3]/(float)num_turns );

      idx++;
    } //endFOR dstep
  } //endFOR bucket
  
  fclose(fp);
  

  return CBI_F_SUCCESS;

}

