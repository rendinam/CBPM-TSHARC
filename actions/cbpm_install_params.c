//-----------------------------------------------------------------------+
// File         :  cbpm_install_params.c                                 |
//                                                                       |
// Description  :  Copies the most recently saved intermediate           |
//                 operational parameter into the master operational     |
//                 parameter file position.                              |
//                                                                       |
// Arguments    :  none                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_install_params(void) {

  char filename[CBI_MAX_STRING_LENGTH]   = "";
  char full_fname[CBI_MAX_STRING_LENGTH] = "";
  // Compose the filename of the latest intermediate operational
  // parameters file.
  int idx = cbi_new_file_idx( PARAMS_DIR, CBI_FALSE );
  idx--;  // Decrement by one to get the actual latest file index.
  cbi_datafile_name_single( PARAMETERS, filename, idx );
  strcat( full_fname, appconfig.directories[PARAMS_DIR] );
  strcat( full_fname, "/" );

  // Determine target directory number
  int subdir_val;
  char subdir_string[6] = "";
  subdir_val = idx / CBI_FILES_PER_DIR;
  if (subdir_val < 100) {
    sprintf( subdir_string, "%02d/", subdir_val );
  } else {
    sprintf( subdir_string, "%03d/", subdir_val );
  }
  sprintf( subdir_string, "%02d/", subdir_val );
  strcat( full_fname, subdir_string );

  strcat( full_fname, filename );


  // ID info preserved during this process to catch if multiple
  // app sessions have written intermediate parameter files before
  // an installation has happened?
  

  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  int retval, input = CBI_FALSE;
  CBI_INIT_PROMPTS(1);


  char supp_text[400];
  sprintf(supp_text, "\n\nWould you like install the changes present in file:"
	  "\n  %s \n into the master operational parameters file: %s ?\n", 
  	  full_fname,
  	  appconfig.inst_param_file);
  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {CBI_TRUE, CBI_TRUE, CBI_FALSE, CBI_FALSE};
  prompts[0].supp_text_ptr    = supp_text;
  prompts[0].prompt_text      = "Enter [y/n]";
  prompts[0].data_type        = CBI_CHAR;
  prompts[0].valid_inputs     = valid_inputs;
  prompts[0].map_out          = map_out;
  prompts[0].default_value    = 'n';
  prompts[0].destination      = &(input);
  prompts[0].last_val_default = CBI_FALSE;


  retval = CBI_PROMPTER();

  // If in back-end mode, delegate this action to be performed only on the first back-end
  // because it only needs to happen a single time.
  if (input == CBI_TRUE ) {

    if ( CTL_System.RunningMode == CBI_SINGLE_MODE ||
	 (CTL_System.RunningMode == CBI_BACKEND_MODE && CTL_System.instance_ID == 1) ) {
      cbi_copy_file( appconfig.inst_param_file, full_fname );
      printf(" Done.\n");
    } else {
      printf("Changes not installed.\n");
    }

  } // endIF input
  
  return F_SUCCESS;

}


