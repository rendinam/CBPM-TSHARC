//-----------------------------------------------------------------------+
// File         :  cbpm_gain_scale_type.c                                |
//                                                                       |
// Description  :  Allows toggling between calculated gain scale factors |
//                 and a set of IDEAL gain scale factors for comparison. |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"

  const float CBPM_IDEAL_GAIN_FACTORS[CBPM_MAX_GAINS] = {
    4.980,
    0.0625,
    0.125,
    0.2500,
    0.500,
    1.000,
    2.000,
    4.000,
    8.000,
    28.480,
    56.960
  };


int cbpm_gain_scale_type_prep(void) {

  char iidx, input, tblock, card, gsetting;
  int  tsetup;

  CBPM_DATA         *dp;

  printf("   A) Bench-calibration gain scale factors\n");
  printf("   B) Ideal gain scale factors\n");
  printf("Load which set of gain scale factors? ");
  input = cbi_get_char();

  

  if (input == 'A' || input == 'a') {



    printf("Using timing setup of first active instrument encountered...\n");
    tsetup = cbpm_measurement_tsetup();
    //printf("Loading timing parameters for timing setup %s.\n", tsetup_names[tsetup]);
    printf("Applying all setup values...\n");
    for (iidx = 0; iidx < CBI_MAX_INSTRUMENTS; iidx++) {
      if (CTL_System.p_Module[iidx]->active == CBI_ACTIVE) {
	cbpm_set_timing_setup_post(iidx);
	printf("\n\n");
      }
    }



  } else if (input == 'B' || input == 'b') {



    for (iidx = 0; iidx < CBI_MAX_INSTRUMENTS; iidx++) {

      dp  = CTL_System.p_Module[iidx]->dsp_data;

      if (CTL_System.p_Module[iidx]->active == CBI_ACTIVE) {
	printf("Inst %d is ACTIVE\n", iidx);
	printf("Changing local copy of gain scale factors to IDEAL values for <%s>.\n",
	      CTL_System.p_Module[iidx]->comm.ethernet.hostname );

	// Load the appropriate gain scale factors into
	// each instrument's config_params structure and/or
	// gain_config structure and transmit.
	for (gsetting = 0; gsetting < CBPM_MAX_GAINS; gsetting++) {
	  printf("%f  ", CBPM_IDEAL_GAIN_FACTORS[gsetting]);
	  for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
	    for (card = 0; card < CBPM_MAX_CARDS; card++) {
	      dp->cbpm_gain_config.gain_table[tblock][card][gsetting];
	    }
	  }
	}
	printf("\n");
	cbpm_set_timing_setup_post(iidx);
	printf("\n\n\n");
      }
    }



  }  // endIF input

  return CBI_F_SUCCESS;

}

