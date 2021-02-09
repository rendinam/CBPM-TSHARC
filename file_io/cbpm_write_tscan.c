/*!
*-------------------------------------------------------------------------+
* File         :  cbpm_write_tscan_data.c                                 |
*                                                                         |
* Description  :  Writes a data file formatted to reflect a time scan     |
*                 acquisition.                                            |
*                                                                         |
* Arguments    :                                                          |
*                 iidx     - Index of BPM where this data orginated       |
*                 int init_gd_count - Initial block delay count, used     |
*                    registering data over the appropriate delay range.   |
*                 FILE *fp - Pointer to file where data will be archived  |
*                 char *filename - Name of the same file                  |
*                                                                         |
*                 RETURN args:                                            |
*                 - datname - Pointer to string holding filename where    |
*                    data will be written.                                |
*                                                                         |
* Author       :  M. Rendina                                              |
*-------------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_write_tscan_data(int iidx, int init_gd_count, FILE *fp, char *filename) {

  int   sample, t_max, k, contr;
  int   dstep = 0, card, turn;
  int   idx;
  float ns_delay;
  int   tblock;

  CBPM_DATA  *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  int BlockStep = CBPM_BLOCK_STEP;

  int num_turns = CBPM_SCAN_TURNS;

  float sum[CBPM_MAX_ACQ_CONTROLLERS];

  t_max = dp->cbpm_find_delay_out.max_block_delay;

  if (dp->cbpm_raw_data_header.num_bunches[0] != 0) {
    tblock = 0;
  } else if (dp->cbpm_raw_data_header.num_bunches[1] != 0) {
    tblock = 1;
  } else {
    printf("ERROR!: Timing block could not be determined!\n");
    return F_FAILURE;
  }
      
  int acq_con;
  printf("max block delay  = %d\n", dp->cbpm_find_delay_out.max_block_delay);
  printf("init block delay = %d\n", init_gd_count);
  printf("block step size  = %d\n", BlockStep);
  int DELAY_STEPS = (dp->cbpm_find_delay_out.max_block_delay - init_gd_count)/BlockStep;
  switch (CTL_System.p_Module[iidx]->core->cbi_stat.state) {
  case (m4ns):
    DELAY_STEPS = 800;
    break;
  case (m14ns):
    DELAY_STEPS = 1400;
    break;
  }
  printf("writing # delay steps = %d\n", DELAY_STEPS);

  fprintf(fp, "#-------------------------------------------------------------------\n");
  fprintf(fp, "#G Delay  Card-0\t  Card-1          Card-2          Card-3\n");
  fprintf(fp, "#-------------------------------------------------------------------\n");
  
  printf("Dumping TIMING SCAN data to file '%s'.\n", filename);
  printf("Each point is a %d-turn average.\n", num_turns);
  
  
  for (dstep = 0; dstep < DELAY_STEPS; dstep++) {
    
    for (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      sum[acq_con] = 0.0;
    }
    
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
	
      for (turn = 0; turn < num_turns; turn++) {
	idx = (dstep*num_turns) + turn;
	sum[card] += cbpm_adc_buffer_get(dp, card, tblock, idx);
      }
      sum[card] = sum[card] / num_turns;
      
    } //endFOR card

    ns_delay = (dstep*BlockStep + init_gd_count)/100.00;
    fprintf(fp, "%.2f\t%f\t", ns_delay, sum[0] );
    fprintf(fp, "%f\t", sum[1] );
    fprintf(fp, "%f\t", sum[2] );
    fprintf(fp, "%f\n", sum[3] );

  } //endFOR dstep

  
  fclose(fp);

  return F_SUCCESS;

}
