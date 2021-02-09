//-----------------------------------------------------------------------+
// File         :  cbpm_install_gain_tables.c                            |
//                                                                       |
// Description  :  Copies gain tables from calibration file to           |
//                 respective instruments in the master configuration    |
//                 file to disk.                                         |
//                                                                       |
//                 Input file format must be of the form:                |
//                 [BPM_XXY]                                             |
//                 .GAIN_TABLE_0    #.#####  #.##### ...                 |
//                                  #.#####  #.##### ...                 |
//                 .GAIN_TABLE_1    #.#####  #.##### ...                 |
//                                  #.#####  #.##### ...                 |
//                   ...                                                 |
//                                                                       |
// Arguments    :  Master struct array index of the BPM to which this    |
//                 command will be directed.                             |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"

#define GAIN_CAL_ELEC_FILE      "gain_cal_electronic.in"


int cbpm_install_gain_tables(void) {

  char *func_name = (char *)__FUNCTION__; 
  int debug_level = CTL_System.debug_verbosity;

  FILE *fp_in, *fp_out;
  char filename[CBI_MAX_STRING_LENGTH]   = "";
  char full_fname[CBI_MAX_STRING_LENGTH] = "";
  int status;
  int card, gsetting;
  

  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  int retval, input = FALSE;
  CBI_INIT_PROMPTS(1);

  char prompt_text[CBI_MAX_STRING_LENGTH*2];
  sprintf( prompt_text, "\n\nWould you like to replace all gain tables in the\n"
	   "existing set of parameters with those that appear in the file\n"
	   "  ./%s\n"
	   "and save the result to an intermediate file? (y/n): ", GAIN_CAL_ELEC_FILE );

  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {    TRUE,     TRUE,     FALSE,    FALSE};
  prompts[0].supp_text_ptr    = NULL;
  prompts[0].prompt_text      = prompt_text;
  prompts[0].data_type        = CBI_CHAR;
  prompts[0].valid_inputs     = valid_inputs;
  prompts[0].map_out          = map_out;
  prompts[0].default_value    = 'n';
  prompts[0].destination      = &(input);
  prompts[0].last_val_default = FALSE;


  retval = CBI_PROMPTER();


  if (input == FALSE ) {
    printf("Tables not installed.\n");
    return CBI_F_FAILURE;
  }



  fp_in  = fopen(GAIN_CAL_ELEC_FILE, "r");
  if (fp_in == NULL) {
   sprintf(message, "Cannot open input file ./%s ", GAIN_CAL_ELEC_FILE );
   log_message( S_ERROR, func_name, message );
   return CBI_F_FAILURE;
  }

  fp_out = cbi_managed_file_open( filename, full_fname, (int)NULL, PARAMS_DIR, PARAMETERS, CBI_SINGLE_FILE, FALSE );
  if (fp_out == NULL) {
    sprintf(message, "Open of instrument parameters file failed!");
    log_message( S_ERROR, func_name, message );
    return CBI_F_FAILURE;
  }

  cbi_write_param_file_header( fp_out, CTL_System.output_file_index, CBI_FALSE );

  printf("Filename  : %s\n", filename);
  printf("Full name : %s\n", full_fname);


  // Temp struct for holding gain tables
  CBPM_GAIN_CONFIG   temp_gain_config;


  // Get list of sections from the input file and the output file
  int insec, outsec, section;
  char sections[FA_MAX_FILE_SECTIONS][FA_MAX_STRING_LENGTH];
  char sections_out[FA_MAX_FILE_SECTIONS][FA_MAX_STRING_LENGTH];
  
  for (section = 0; section < FA_MAX_FILE_SECTIONS; section++) {
    strcpy(sections[section], "");
    strcpy(sections_out[section], "");
  }

  printf("Extracting file section order (%s)...\n", GAIN_CAL_ELEC_FILE);
  fa_extract_section_order(fp_in, sections );

  printf("Extracting file section order (%s)...\n", filename);
  fa_extract_section_order(fp_out, sections_out );

  // After reading section order, rewind file.
  rewind(fp_in);
  rewind(fp_out);


  // Cull sections list to those that only appear in BOTH the calibration input file
  // and the output parameter file.
  int found_match;
  for (insec = 0; insec < FA_MAX_FILE_SECTIONS; insec++) {

    found_match = FALSE;
    if ( strcmp(sections[insec], "") != 0 ) {
      for (outsec = 0; outsec < FA_MAX_FILE_SECTIONS; outsec++) {
	if ( strcmp(sections[insec], sections_out[outsec]) == 0 ) {
	  found_match = TRUE;
	}
      }
      if (!found_match) { // No match found for input instrument, remove it and notify.
	printf("Input section %s not found in master config file, skipping.\n",
	       sections[insec] );
	strcpy( sections[insec], "" );
      }
    } //endIF sections[insec] == ""

  } //endFOR insec

	
	  
  //------------------------------------------------------------------------------
  // For every section present in the both the input file and the master file,
  // read in the gain tables from input and replace-write them to the output file.
  //------------------------------------------------------------------------------
  for (section = 0; section < FA_MAX_FILE_SECTIONS; section++) {
    // Initialize
    for (gsetting = 0; gsetting < CBPM_MAX_GAINS; gsetting++) {
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
	temp_gain_config.gain_table[0][card][gsetting] = 0.0;
	temp_gain_config.gain_table[1][card][gsetting] = 0.0;
      }
    }

    if (strcmp(sections[section], "") != 0) {
      printf(" ---- \"%s\" ----\n", sections[section] );
      status = faccess(FA_READ, fp_in, sections[section], "GAIN_TABLE_0", 4, CBPM_MAX_GAINS, FLOAT, temp_gain_config.gain_table[0] );
      printf("read status = %d\n", status);
      status = faccess(FA_READ, fp_in, sections[section], "GAIN_TABLE_1", 4, CBPM_MAX_GAINS, FLOAT, temp_gain_config.gain_table[1] );
      printf("read status = %d\n", status);

      // Rewind inputfile since there is no guarantee that the instrument sections appear in both files in the same order.
      // For this process, the order of processing is driven by order of instrument sections as they appear in the OUTPUT file.
      rewind(fp_in);

      // Write gain table to intermediate file.
      status = faccess(FA_WRITE, fp_out, sections[section], "GAIN_TABLE_0", 4, CBPM_MAX_GAINS, FLOAT, temp_gain_config.gain_table[0] );
      printf("write status = %d\n", status);
      status = faccess(FA_WRITE, fp_out, sections[section], "GAIN_TABLE_1", 4, CBPM_MAX_GAINS, FLOAT, temp_gain_config.gain_table[1] );
      printf("write status = %d\n", status);
    }
  }


  if( fclose(fp_in) != 0) {
    perror("fclose: ");
  }
  if( fclose(fp_out) != 0) {
    perror("fclose: ");
  }


  return CBI_F_SUCCESS;

}

