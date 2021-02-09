//-----------------------------------------------------------------------+
// File         :  cbpm_save_pedestals.c                                 |
//                                                                       |
// Description  :  Saves the pedestal parameters acquired for a device   |
//                 to disk.                                              |
//                                                                       |
// Arguments    :  Master struct array index of the BPM to which this    |
//                 command will be directed.                             |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"
#include "cbpm_param_tokens.h"


int cbpm_save_pedestals( void ) {
  
  int inst, mg_setup, status;
  int tblock, card, gsetting;
  char section[20];
  FILE *fpm, *fp;
  CBPM_DATA         *dp;

  char *func_name = (char *)__FUNCTION__;

  int debug_level = CTL_System.debug_verbosity;

  char filename[CBI_MAX_STRING_LENGTH]   = "";
  char full_fname[CBI_MAX_STRING_LENGTH] = "";

  char buf[CBI_MAX_SOCKDAT_SIZE];
  char PARAM_FILENAME_QUERY[CBI_MAX_STRING_LENGTH] = {"PARAM_FILENAME?\0"};
  int  next_ordered_section = 0;
  char sections[FA_MAX_FILE_SECTIONS][FA_MAX_STRING_LENGTH];
  int  sect_cnt;


  if (CTL_System.RunningMode != CBI_BACKEND_MODE &&
      CTL_System.RunningMode != CBI_MANAGER_MODE ) {


    //-------------------------------------------------
    // Define the list of prompts and their parameters
    //-------------------------------------------------
    int retval, input = FALSE;
    CBI_INIT_PROMPTS(1);
    
    
    int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
    int map_out[4]      = {TRUE, TRUE, FALSE, FALSE};
    prompts[0].supp_text_ptr    = NULL;
    prompts[0].prompt_text      = "\n\nWould you like to save the pedestal tables to an intermediate file? (y/n): ";
    prompts[0].data_type        = CBI_CHAR;
    prompts[0].valid_inputs     = valid_inputs;
    prompts[0].map_out          = map_out;
    prompts[0].default_value    = 'n';
    prompts[0].destination      = &(input);
    prompts[0].last_val_default = FALSE;
    
    
    retval = CBI_PROMPTER();
    
    if (input == FALSE) {
      printf("Tables not saved.\n");
      return CBI_F_FAILURE;
    }


    // ------- faccess session prep --------------
    //int next_ordered_section = 0;
    //char sections[FA_MAX_FILE_SECTIONS][FA_MAX_STRING_LENGTH];
    //int sect_cnt;
    // Wipe section order holding list.
    for (sect_cnt = 0; sect_cnt < FA_MAX_FILE_SECTIONS; sect_cnt++) {
      strcpy( sections[sect_cnt], "" );
    }
    
    fpm = fopen(appconfig.inst_param_file, "r");
    fa_extract_section_order(fpm, sections );
    if ( fclose(fpm) != 0) {
      perror("fclose: ");
    }
    
    if (debug_level > CBI_DEBUG_LEVEL_4) {
      printf( "\n\n\n\n----\n\n");
      for (sect_cnt = 0; sect_cnt < FA_MAX_FILE_SECTIONS; sect_cnt++) {
	if ( strcmp(sections[sect_cnt], "") != 0 ) {
	  printf("%3d %s\n", sect_cnt+1, sections[sect_cnt] );
	}
      }
    }
    //-----------------------------------

  
    fp = cbi_managed_file_open( filename, full_fname, (int)NULL, PARAMS_DIR, PARAMETERS, CBI_SINGLE_FILE, FALSE );
    if (fp == NULL) {
      sprintf(message, "Open of instrument parameters file failed!");
      log_message( S_ERROR, func_name, message );
      return CBI_F_FAILURE;
    }

  } else {


    printf("Sending filename query...%s\n", PARAM_FILENAME_QUERY);
    fflush(stdout);
    if(send(CTL_System.mgmt_socket_IDs[0], PARAM_FILENAME_QUERY, sizeof(PARAM_FILENAME_QUERY), 0) == -1) {
      perror("File_ID send() error");
      fflush(stdout);
      return CBI_F_FAILURE;
    }

    strcpy( buf, "" );
    status = recv(CTL_System.mgmt_socket_IDs[0], buf, CBI_MAX_SOCKDAT_SIZE, 0);


    // ------- faccess session prep --------------
    // Wipe section order holding list.
    for (sect_cnt = 0; sect_cnt < FA_MAX_FILE_SECTIONS; sect_cnt++) {
      strcpy( sections[sect_cnt], "" );
    }
    
    fpm = fopen(appconfig.inst_param_file, "r");
    fa_extract_section_order(fpm, sections );
    if ( fclose(fpm) != 0) {
      perror("fclose: ");
    }
    
    if (debug_level > CBI_DEBUG_LEVEL_4) {
      printf( "\n\n\n\n----\n\n");
      for (sect_cnt = 0; sect_cnt < FA_MAX_FILE_SECTIONS; sect_cnt++) {
	if ( strcmp(sections[sect_cnt], "") != 0 ) {
	  printf("%3d %s\n", sect_cnt+1, sections[sect_cnt] );
	}
      }
    }
    //-----------------------------------


    // Wait for filename info from manager, if NULL, then a filename has
    // not yet been generated.  Take this opportunity to do so and send
    // to manager.
    if ( strcmp( buf, "") == 0 ) {
      // Obtain file index # and perform a managed open on file.
      printf("Received empty filename spec.  Generating new filename...   ");
      fflush(stdout);
      fp = cbi_managed_file_open( filename, full_fname, 0, PARAMS_DIR, PARAMETERS, CBI_SINGLE_FILE, FALSE );
      printf("%s\n", full_fname);
      fflush(stdout);
    } else if ( strcmp(buf, "ABORT") == 0 ) {
      printf("\n\nReceived parameter file save abort request from manager.\n\n");
      return CBI_F_SUCCESS;
    } else {
      strcpy( full_fname, buf );
      printf("Using pre-existing file... %s\n", full_fname);
      fflush(stdout);
      fp = fopen(full_fname, "r+"); // Open for reading and writing.
    }
    
    printf("---> Sending full filename back: \"%s\"...\n", full_fname);
    fflush(stdout);
    if(send(CTL_System.mgmt_socket_IDs[0], full_fname, sizeof(full_fname), 0) == -1) {
     perror("File_ID send() error");
     fflush(stdout);
     return CBI_F_FAILURE;
    }
    
    
  } //endIF running mode


  printf("Saving pedestal tables to intermediate file:\n");
  printf(" %s... \n", full_fname );
  fflush(stdout);
  
  
  // Only write parameter file header if in standalone mode or if this is 
  // the first back-end instance.
  if ( CTL_System.RunningMode == CBI_SINGLE_MODE ||
       (CTL_System.RunningMode == CBI_BACKEND_MODE && CTL_System.instance_ID == 1) ) {
    
    cbi_write_param_file_header( fp, CTL_System.output_file_index, CBI_FALSE );
    
  }
  

  //---------------------------------------------------------------------------
  // Ensure faccess writes are performed in the order in which sections appear
  // in the parameter file.
  //---------------------------------------------------------------------------
  for (sect_cnt = 0; sect_cnt < FA_MAX_FILE_SECTIONS; sect_cnt++) {
    if (debug_level > CBI_DEBUG_LEVEL_3) {
      printf("SECTION = %s\n", sections[sect_cnt] );
    }

    if ( strcmp(sections[sect_cnt], "") == 0 ) {
      break;
    }


    for (inst = 0; inst < num_online(); inst++) {
      if (CTL_System.p_Module[inst]->active == CBI_ACTIVE) {
	
	dp = CTL_System.p_Module[inst]->dsp_data;
	
	strcpy( section, CTL_System.p_Module[inst]->det.name );

	// If next active instrument also appears next in file...
	if ( strcmp(section, sections[sect_cnt]) == 0 ) {
	  if (debug_level > CBI_DEBUG_LEVEL_3) {
	    printf("section = %s\n", section );
	  }
	  
	  // Section open
	  //printf("Accessing file section >%s<\n", section);
	  faccess( FA_WRITE, fp, section, NULL, 0, 0, 0, NULL);   // TODO: Check first argument protocol
	  
	  
	  printf("==== Saving pedestal tables for instrument %s:\n", section);
	  fflush(stdout);
	  
	  
	  // Save the pedestal tables for half of the defined timing setups since fixed gain
	  // and variable gain entries reside in different slots of the same tables.
	  for (mg_setup = 0; mg_setup < CBPM_MAX_MULTIGAIN_SETUPS; mg_setup++) {
	    
	    // Only save values for supported timing setups.
	    if ( mg_setup == s4ns_P_VAR_G || mg_setup == s4ns_E_VAR_G || mg_setup == s14ns_VAR_G ) {
	      
	      
	      status = faccess( FA_WRITE, 
				fp, 
				section, 
				pedestal_tokens[mg_setup][0], 
				4, 
				CBPM_MAX_GAINS, 
				FLOAT, 
				&(dp->cbpm_pedestal_config.tables[mg_setup].ped_table[0][0][0]) );
	      if (status != 0) {
		printf("ERROR! %s write status = %d\n", pedestal_tokens[mg_setup][0], status);
	      }
	      
	      status = faccess( FA_WRITE, 
				fp, 
				section, 
				pedestal_tokens[mg_setup][1], 
				4, CBPM_MAX_GAINS, FLOAT, 
				&(dp->cbpm_pedestal_config.tables[mg_setup].ped_table[1][0][0]) );
	      if (status != 0) {
		printf("ERROR! %s write status = %d\n", pedestal_tokens[mg_setup][1], status);
	      }
	      
	      status = faccess( FA_WRITE, 
				fp, 
				section, 
				pedestal_tokens[mg_setup][2], 
				4, 
				CBPM_MAX_GAINS, 
				FLOAT, 
				&(dp->cbpm_pedestal_config.tables[mg_setup].ped_rms_table[0][0][0]) );
	      if (status != 0) {
		printf("ERROR! %s write status = %d\n", pedestal_tokens[mg_setup][2], status);
	      }
	      
	      status = faccess( FA_WRITE, 
				fp, 
				section, 
				pedestal_tokens[mg_setup][3], 
				4, 
				CBPM_MAX_GAINS, 
				FLOAT, 
				&(dp->cbpm_pedestal_config.tables[mg_setup].ped_rms_table[1][0][0]) );
	      if (status != 0) {
		printf("ERROR! %s write status = %d\n", pedestal_tokens[mg_setup][3], status);
	      }
	      
	    }
	  }
	  
	} //endIF section name matches active instrument section key
	
      } //endIF active
    } //endFOR inst
    
    
  } //endFOR sect_cnt
  
  printf("Stopping faccess() session...\n");
  faccess( FA_WRITE, fp, NULL, NULL, 0, 0, 0, NULL);

  printf("Done saving pedestal tables to file:\n");
  printf(" %s... \n", full_fname );

  if( fclose(fp) != 0) {
    perror("fclose: ");
  }

  // If this is a back-end instance, signal the manager when the file
  // write is complete.
  if (CTL_System.RunningMode == CBI_BACKEND_MODE) {
    printf("---> Sending write completion notice...\n");
    fflush(stdout);
    if(send(CTL_System.mgmt_socket_IDs[0], "FILE WRITE DONE\0", 16, 0) == -1) {
      perror("File_ID send() error");
      fflush(stdout);
      return CBI_F_FAILURE;
    }
  }

  return CBI_F_SUCCESS;

}
