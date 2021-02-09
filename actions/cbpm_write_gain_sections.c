//-----------------------------------------------------------------------+
// File         : cbpm_write_gain_sections.c                             |
//                                                                       |
// Description  : Allows writing of updated gain codes and other         |
//                gain-indexed tables in the configuration parameters    |
//                file.  Use this to reconfigure the parameter file when |
//                the dimensions of the parameter arrays changes due to  |
//                a change in the number of allowed gain settings.       |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_write_gain_sections( int iidx ) {


  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  int retval, input = FALSE;
  CBI_INIT_PROMPTS(1);


  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {TRUE, TRUE, FALSE, FALSE};
  prompts[0].supp_text_ptr    = NULL;
  prompts[0].prompt_text      = "\nWould you like to write ideal gain & ped tables into the master config file (y/n)? ";
  prompts[0].data_type        = CBI_CHAR;
  prompts[0].valid_inputs     = valid_inputs;
  prompts[0].map_out          = map_out;
  prompts[0].default_value    = 'n';
  prompts[0].destination      = &(input);
  prompts[0].last_val_default = FALSE;


  retval = CBI_PROMPTER();


  if (input == CBI_FALSE ) {
    printf("Tables not written.\n");
    return CBI_F_FAILURE;
  }



  CBPM_DATA  *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;


  int status;

  int gain_codes[CBPM_MAX_GAINS] = 
    {    0,   257,   258,   260,   264,   272,   288,   320,   383,    448,    511};
  float gain_table_vals[CBPM_MAX_CARDS][CBPM_MAX_GAINS] = 
    {4.980, 0.0625, 0.125, 0.250, 0.500, 1.000, 2.000, 4.000, 8.000, 28.480, 56.96001,
     4.980, 0.0625, 0.125, 0.250, 0.500, 1.000, 2.000, 4.000, 8.000, 28.480, 56.96001,
     4.980, 0.0625, 0.125, 0.250, 0.500, 1.000, 2.000, 4.000, 8.000, 28.480, 56.96001,
     4.980, 0.0625, 0.125, 0.250, 0.500, 1.000, 2.000, 4.000, 8.000, 28.480, 56.96001 };
  float ped_table_vals[CBPM_MAX_CARDS][CBPM_MAX_GAINS] = 
    {0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,  0.000,
     0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,  0.000,
     0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,  0.000,
     0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,  0.000 };


  char section[50] = "[";
  strcat( section, CTL_System.p_Module[iidx]->comm.ethernet.hostname );
  strcat( section, "]" );

  printf("Section = %s\n", section );
  printf("In file = %s\n", appconfig.param_file );

  FILE *fp;
  fp = fopen(appconfig.param_file, "r+"); // 'r+' To allow appending of data to file if necessary

  // Replace the list of gain codes
  status = faccess(WRITE,
		   fp,
		   section,
		   ".GAIN_CODES",
		   1,
		   CBPM_MAX_GAINS,
		   INT,
		   &gain_codes );


  // Replace the gain tables for each timing block
  status = faccess(WRITE,
		   fp,
		   section,
		   ".GAIN_TABLE_0",
		   CBPM_MAX_CARDS,
		   CBPM_MAX_GAINS,
		   FLOAT,
		   &gain_table_vals );

  status = faccess(WRITE,
		   fp,
		   section,
		   ".GAIN_TABLE_1",
		   CBPM_MAX_CARDS,
		   CBPM_MAX_GAINS,
		   FLOAT,
		   &gain_table_vals );


  // Replace the gain 'chan-to-chan' tables for each timing block
  status = faccess(WRITE,
		   fp,
		   section,
		   ".GAIN_CHAN_TO_CHAN_0",
		   CBPM_MAX_CARDS,
		   CBPM_MAX_GAINS,
		   FLOAT,
		   &gain_table_vals );

  status = faccess(WRITE,
		   fp,
		   section,
		   ".GAIN_CHAN_TO_CHAN_1",
		   CBPM_MAX_CARDS,
		   CBPM_MAX_GAINS,
		   FLOAT,
		   &gain_table_vals );


  // Replace the pedestal tables for each timing block
  status = faccess(WRITE,
		   fp,
		   section,
		   ".PEDESTAL_TABLE_0",
		   CBPM_MAX_CARDS,
		   CBPM_MAX_GAINS,
		   FLOAT,
		   &ped_table_vals );

  status = faccess(WRITE,
		   fp,
		   section,
		   ".PEDESTAL_TABLE_1",
		   CBPM_MAX_CARDS,
		   CBPM_MAX_GAINS,
		   FLOAT,
		   &ped_table_vals );


  // Replace the pedestal RMS tables for each timing block
  status = faccess(WRITE,
		   fp,
		   section,
		   ".PEDESTAL_RMS_0",
		   CBPM_MAX_CARDS,
		   CBPM_MAX_GAINS,
		   FLOAT,
		   &ped_table_vals );

  status = faccess(WRITE,
		   fp,
		   section,
		   ".PEDESTAL_RMS_1",
		   CBPM_MAX_CARDS,
		   CBPM_MAX_GAINS,
		   FLOAT,
		   &ped_table_vals );




  return CBI_F_SUCCESS;

}




