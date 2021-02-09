/*!-----------------------------------------------------------------------+
* File         :  cbpm_write_chan_cal.c                                   |
*                                                                         |
* Description  :  Writes a data file of channel calibration values        |
*                 (i.e. the fitter window data)                           |
*                                                                         |
* Arguments    :  Struct of metadata to place in file header and to use   |
*                 in formatting the body of the data file                 |
*                                                                         |
* Author       :  M. Rendina                                              |
*-------------------------------------------------------------------------+*/

#include "cbpm_includes.h"

int CalRadius = CBPM_CAL_RADIUS;

int cbpm_write_chan_cal(int iidx, FILE *fp, char *filename, int *start_delays, int dat_err) {

  int card, i;
  int num_zeros[4];
  int del_lo = 65535;
  int del_hi = -65535; // FIXME: name these

  int chan = 0;

  int debug_level = CTL_System.debug_verbosity;

  CBPM_DATA  *dp;

  dp = CTL_System.p_Module[iidx]->dsp_data;
  

  for ( card = 0; card < CBPM_MAX_CARDS; card++ ) {
    if (debug_level >= CBI_DEBUG_LEVEL_2) {
      printf("delay[%i]: %d\n", card, start_delays[card]);
    }
    if (start_delays[card] < del_lo) {
      del_lo = start_delays[card];
    }
    if (start_delays[card] > del_hi) {
      del_hi = start_delays[card];
    }
  }
  if (debug_level >= CBI_DEBUG_LEVEL_2) {
    printf("\nlow channel delay  : %d\n", del_lo);
    printf("high channel delay : %d\n", del_hi);
  }

  int zctr[4];
 
  for (card = 0; card < CBPM_MAX_CARDS; card++ ) {
    num_zeros[card] = start_delays[card] - del_lo;
    if (debug_level >= CBI_DEBUG_LEVEL_2) {
      printf("num zeros: %d\n", num_zeros[card]);
    }
    zctr[card] = num_zeros[card];
  }
  
  if (dat_err == 0) {
    printf("Dumping CHANNEL CALIBRATION data to file '%s'.\n", filename);
  } else {
    printf("Dumping CHANNEL CALIBRATION ERROR data to file '%s'.\n", filename);
  }

  int datpoints = (2*CalRadius+1)/ CBPM_CHAN_SCAN_STEP;
  int numsteps = ( del_hi - del_lo + datpoints ) / CBPM_CHAN_SCAN_STEP;
  if (debug_level >= CBI_DEBUG_LEVEL_2) {
    printf("NUM STEPS: %d\n", numsteps);
  }

  int dat_z_ctr[4];
  for( card = 0; card < CBPM_MAX_CARDS; card++ ){
    dat_z_ctr[card] = datpoints + num_zeros[card];
  }

  for(i = 0; i < numsteps; i++) {

    fprintf(fp,  "%d\t", del_lo+(i*CBPM_CHAN_SCAN_STEP)  );

    if (zctr[0] == 0 && dat_z_ctr[0] > 0) {
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[0].data [ i-num_zeros[0] ]);
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[0].error[ i-num_zeros[0] ]);
    } else {
      fprintf(fp, "?\t\t?\t\t");
      zctr[0]--;
    }
    dat_z_ctr[0]--;

    if (zctr[1] == 0 && dat_z_ctr[1] > 0) {
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[1].data [ i-num_zeros[1] ]);
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[1].error[ i-num_zeros[1] ]);
    } else {
      fprintf(fp, "?\t\t?\t\t");
      zctr[1]--;
    }
    dat_z_ctr[1]--;

    if (zctr[2] == 0 && dat_z_ctr[2] > 0) {
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[2].data [ i-num_zeros[2] ]);
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[2].error[ i-num_zeros[2] ]);
    } else {
      fprintf(fp, "?\t\t?\t\t");
      zctr[2]--;
    }
    dat_z_ctr[2]--;

    if (zctr[3] == 0 && dat_z_ctr[3] > 0) {
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[3].data [ i-num_zeros[3] ]);
	fprintf(fp, "%f", dp->cbpm_chan_cal_work.chan_cal_data[3].error[ i-num_zeros[3] ]);
    } else {
      fprintf(fp, "?\t\t?\t\t");
      zctr[3]--;
    }
    dat_z_ctr[3]--;

    fprintf(fp, "\n");

  }

  fclose(fp);

  return F_SUCCESS;

}


int cbpm_write_chan_cal_parallelpost(int iidx, FILE *fp, char *filename, int *start_delays, int dat_err, float peaks[4]) {

  int card, i;

  int chan = 0;

  int debug_level = CTL_System.debug_verbosity;

  CBPM_DATA  *dp;

  dp = CTL_System.p_Module[iidx]->dsp_data;
  

  fprintf(fp, "Test\nPeaks      " );

  for (i=0; i<4; i++)
  {
    fprintf(fp, "%f, ", peaks[i]);
  }
  fprintf(fp,"\n");
  
  fprintf(fp, "Offsets    " );

  for (i=0; i<4; i++)
  {
    fprintf(fp, "%i, ", start_delays[i]);
  }
  fprintf(fp,"\n");


  if (dat_err == 0) {
    printf("Dumping CHANNEL CALIBRATION data to file '%s'.\n", filename);
  } else {
    printf("Dumping CHANNEL CALIBRATION ERROR data to file '%s'.\n", filename);
  }

  int numsteps = ( 2*CalRadius+1 ) / CBPM_CHAN_SCAN_STEP;
  if (debug_level >= CBI_DEBUG_LEVEL_2) {
    printf("NUM STEPS: %d\n", numsteps);
  }

  int addr;
  for(i = 0; i < numsteps; i++) {

    addr = i;
    fprintf(fp,  "%d\t", (i*CBPM_CHAN_SCAN_STEP)  );

	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[0].data [addr]);
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[0].error[ addr]);
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[1].data [ addr]);
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[1].error[ addr]);
 	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[2].data [ addr]);
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[2].error[ addr]);
	fprintf(fp, "%f\t", dp->cbpm_chan_cal_work.chan_cal_data[3].data [ addr]);
	fprintf(fp, "%f", dp->cbpm_chan_cal_work.chan_cal_data[3].error[ addr]);
    fprintf(fp, "\n");
  }

  fclose(fp);

  return F_SUCCESS;
}
