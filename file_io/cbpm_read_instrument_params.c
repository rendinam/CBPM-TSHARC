/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_read_instrument_params.c                         |
*                                                                       |
* Description  :  Read in all timing-setup-dependent instrument         |
*                 operational parameters for each instrument that is    |
*                 online and load up the appropriate data structure for |
*                 for each.                                             |
*                                                                       |
* Arguments    :  -Pointer to operational parameters filename string    |
*                 -Pointer to holding array of master control structs   |
*                  to populate.                                         |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"
#include "cbpm_param_tokens.h"


#define MAX_PARAMS              80   // FIXME: Relocate to managed file IO library and header.
#define MAX_SETUP_PARAMS        65
#define NUM_CONDITIONAL_PARAMS   1


int cbpm_read_instrument_params( char *filename ) {
  
  char *func_name = (char *)__FUNCTION__;
  int debug_level = CTL_System.debug_verbosity;

  // Pointers to 'data' substructure (destination & temporary)
  CBPM_DATA      *dp,  *dpt;
  dpt = CTL_System.p_Module_temp->dsp_data;


  // Pointers to 'control' substructure (destination & temporary)
  CBPM_CONTROL  *cp,  *cpt;
  cpt = CTL_System.p_Module_temp->control;

  //int dev;
  int stat;

  // Array of structures to hold Identification/communication parameters
  PARAMETER_INFO pars[MAX_SETUP_PARAMS];
  memset( &pars, 0, sizeof(pars) );


  //-----------------------------------------
  // Parameter file search token definitions
  //-----------------------------------------

  // ID parameters
  cbi_add_param( pars, "INSTRUMENT_HOSTNAME",  1, 1,  STRING, (void *)&CTL_System.p_Module_temp->comm.ethernet.hostname );


  // Bunch pattern offsets, timing parameters, and delays --------------------------------------------------
  // Parameters for the available timing setups           --------------------------------------------------

  // Parameters to be read for Timing setup: s4ns_P
  cbi_add_param( pars, timing_tokens[s4ns_P_VAR_G][0],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_P_VAR_G].BP_offsets  );
  cbi_add_param( pars, timing_tokens[s4ns_P_VAR_G][1],  1, 1,                       INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_P_VAR_G].cTMD );
  cbi_add_param( pars, timing_tokens[s4ns_P_VAR_G][2],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_P_VAR_G].bTMDs       );
  cbi_add_param( pars, timing_tokens[s4ns_P_VAR_G][3],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_P_VAR_G].block_delays );
  cbi_add_param( pars, timing_tokens[s4ns_P_VAR_G][4],  2, 4,                       INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_P_VAR_G].chan_delays );


  // Parameters to be read for Timing setup: s4ns_E
  cbi_add_param( pars, timing_tokens[s4ns_E_VAR_G][0],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_E_VAR_G].BP_offsets  );
  cbi_add_param( pars, timing_tokens[s4ns_E_VAR_G][1],  1, 1,                       INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_E_VAR_G].cTMD );
  cbi_add_param( pars, timing_tokens[s4ns_E_VAR_G][2],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_E_VAR_G].bTMDs       );
  cbi_add_param( pars, timing_tokens[s4ns_E_VAR_G][3],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_E_VAR_G].block_delays );
  cbi_add_param( pars, timing_tokens[s4ns_E_VAR_G][4],  2, 4,                       INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_E_VAR_G].chan_delays );


  // Parameters to be read for Timing setup: s14ns
  cbi_add_param( pars, timing_tokens[s14ns_VAR_G][0],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s14ns_VAR_G].BP_offsets  );
  cbi_add_param( pars, timing_tokens[s14ns_VAR_G][1],  1, 1,                       INT, (void *)&dpt->cbpm_timing_config.setups[s14ns_VAR_G].cTMD );
  cbi_add_param( pars, timing_tokens[s14ns_VAR_G][2],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s14ns_VAR_G].bTMDs       );
  cbi_add_param( pars, timing_tokens[s14ns_VAR_G][3],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s14ns_VAR_G].block_delays );
  cbi_add_param( pars, timing_tokens[s14ns_VAR_G][4],  2, 4,                       INT, (void *)&dpt->cbpm_timing_config.setups[s14ns_VAR_G].chan_delays );


  // Timing parameters for the FIXED GAIN timing setups.  Necessary because the signal propagation time within the instrument
  // differs between the variable-gain amplifier and fixed-gain amplifier settings.

  // Parameters to be read for Timing setup: s4ns_P_FIX_G
  cbi_add_param( pars, timing_tokens[s4ns_P_FIX_G][0],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_P_FIX_G].BP_offsets  );
  cbi_add_param( pars, timing_tokens[s4ns_P_FIX_G][1],  1, 1,                       INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_P_FIX_G].cTMD );
  cbi_add_param( pars, timing_tokens[s4ns_P_FIX_G][2],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_P_FIX_G].bTMDs       );
  cbi_add_param( pars, timing_tokens[s4ns_P_FIX_G][3],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_P_FIX_G].block_delays );
  cbi_add_param( pars, timing_tokens[s4ns_P_FIX_G][4],  2, 4,                       INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_P_FIX_G].chan_delays );

  // Parameters to be read for Timing setup: s4ns_E_FIX_G
  cbi_add_param( pars, timing_tokens[s4ns_E_FIX_G][0],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_E_FIX_G].BP_offsets  );
  cbi_add_param( pars, timing_tokens[s4ns_E_FIX_G][1],  1, 1,                       INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_E_FIX_G].cTMD );
  cbi_add_param( pars, timing_tokens[s4ns_E_FIX_G][2],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_E_FIX_G].bTMDs       );
  cbi_add_param( pars, timing_tokens[s4ns_E_FIX_G][3],  1, CBPM_MAX_TIMING_BLOCKS,  INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_E_FIX_G].block_delays );
  cbi_add_param( pars, timing_tokens[s4ns_E_FIX_G][4],  2, 4,                       INT, (void *)&dpt->cbpm_timing_config.setups[s4ns_E_FIX_G].chan_delays );

  // Parameters to be read for Timing setup: s14ns_FIX_G
  cbi_add_param( pars, timing_tokens[s14ns_FIX_G][0],  1, CBPM_MAX_TIMING_BLOCKS,   INT, (void *)&dpt->cbpm_timing_config.setups[s14ns_FIX_G].BP_offsets  );
  cbi_add_param( pars, timing_tokens[s14ns_FIX_G][1],  1, 1,                        INT, (void *)&dpt->cbpm_timing_config.setups[s14ns_FIX_G].cTMD );
  cbi_add_param( pars, timing_tokens[s14ns_FIX_G][2],  1, CBPM_MAX_TIMING_BLOCKS,   INT, (void *)&dpt->cbpm_timing_config.setups[s14ns_FIX_G].bTMDs       );
  cbi_add_param( pars, timing_tokens[s14ns_FIX_G][3],  1, CBPM_MAX_TIMING_BLOCKS,   INT, (void *)&dpt->cbpm_timing_config.setups[s14ns_FIX_G].block_delays );
  cbi_add_param( pars, timing_tokens[s14ns_FIX_G][4],  2, 4,                        INT, (void *)&dpt->cbpm_timing_config.setups[s14ns_FIX_G].chan_delays );


  // Gain-related parameters -------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------------------------
  cbi_add_param( pars, "GAIN_CODES",          1,              CBPM_MAX_GAINS, INT,   (void *)&dpt->cbpm_gain_config.gain_codes );
  cbi_add_param( pars, "GAIN_TABLE_0",        CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_gain_config.gain_table[0] ); 
  cbi_add_param( pars, "GAIN_TABLE_1",        CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_gain_config.gain_table[1] ); 

  cbi_add_param( pars, pedestal_tokens[s4ns_P_VAR_G][0], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s4ns_P_VAR_G].ped_table[0]     );
  cbi_add_param( pars, pedestal_tokens[s4ns_P_VAR_G][1], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s4ns_P_VAR_G].ped_table[1]     );
  cbi_add_param( pars, pedestal_tokens[s4ns_P_VAR_G][2], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s4ns_P_VAR_G].ped_rms_table[0] );
  cbi_add_param( pars, pedestal_tokens[s4ns_P_VAR_G][3], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s4ns_P_VAR_G].ped_rms_table[1] );

  cbi_add_param( pars, pedestal_tokens[s4ns_E_VAR_G][0], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s4ns_E_VAR_G].ped_table[0]     );
  cbi_add_param( pars, pedestal_tokens[s4ns_E_VAR_G][1], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s4ns_E_VAR_G].ped_table[1]     );
  cbi_add_param( pars, pedestal_tokens[s4ns_E_VAR_G][2], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s4ns_E_VAR_G].ped_rms_table[0] );
  cbi_add_param( pars, pedestal_tokens[s4ns_E_VAR_G][3], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s4ns_E_VAR_G].ped_rms_table[1] );

  cbi_add_param( pars, pedestal_tokens[s14ns_VAR_G][0], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s14ns_VAR_G].ped_table[0]     );
  cbi_add_param( pars, pedestal_tokens[s14ns_VAR_G][1], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s14ns_VAR_G].ped_table[1]     );
  cbi_add_param( pars, pedestal_tokens[s14ns_VAR_G][2], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s14ns_VAR_G].ped_rms_table[0] );
  cbi_add_param( pars, pedestal_tokens[s14ns_VAR_G][3], CBPM_MAX_CARDS, CBPM_MAX_GAINS, FLOAT, (void *)&dpt->cbpm_pedestal_config.tables[s14ns_VAR_G].ped_rms_table[1] );


  FILE *fp;
  fp = fopen(filename, "r");

  // Check for file existence
  if (fp == (int)NULL ) {
    printf("\nInstrument parameter file not found!\n");
    printf("Could not find: %s\n\n", filename);
    printf("Please check that the file entry reflects the\n");
    printf("path of a valid instrument parameter file.\n");
    exit(1);
  }


  char hostname[100];
  char section[20];

  //-----------------------------------------------------------

  /// faccess() session management stuff
  FILE *fpm;
  int inst, next_ordered_section = 0;
  char sections[FA_MAX_FILE_SECTIONS][FA_MAX_STRING_LENGTH];
  int sect_cnt;
  // Wipe section order holding list.
  for (sect_cnt = 0; sect_cnt < FA_MAX_FILE_SECTIONS; sect_cnt++) {
    strcpy( sections[sect_cnt], "" );
  }
  // Get ordered list of sections from file
  fpm = fopen(filename, "r");
  fa_extract_section_order(fpm, sections );
  if ( fclose(fpm) != 0) {
    perror("fclose: ");
  }
  if (debug_level > CBI_DEBUG_LEVEL_4) {
    for (sect_cnt = 0; sect_cnt < FA_MAX_FILE_SECTIONS; sect_cnt++) {
      if ( strcmp(sections[sect_cnt], "") != 0 ) {
	printf("%3d %s\n", sect_cnt+1, sections[sect_cnt] );
      }
    }
  }

  //-----------------------------------------------------------
 
  /// faccess() session START

  //---------------------------------------------------------------------------
  // Ensure faccess writes are performed in the order in which sections appear
  // in the parameter file.
  //---------------------------------------------------------------------------
  for (sect_cnt = 0; sect_cnt < FA_MAX_FILE_SECTIONS; sect_cnt++) {

    if ( strcmp(sections[sect_cnt], "") == 0 ) {
      break;
    }

    for (inst = 0; inst < CTL_System.n_Modules; inst++) {
      if ( strcmp(CTL_System.p_Module[inst]->det.name, "") != 0 ) {


	strcpy( section, CTL_System.p_Module[inst]->det.name );

	// If next active instrument also appears next in file...
	if ( strcmp(section, sections[sect_cnt]) == 0 ) {
	  printf("Reading instrument-dependent parameters for: %s\n", section);
	  
	  
	  // Open section
	  faccess( FA_READ, fp, section, NULL, 0, 0, 0, NULL);


	  // Set pointers to instrument's structure targets
	  cp = CTL_System.p_Module[inst]->control;
	  dp = CTL_System.p_Module[inst]->dsp_data;


	  // Initialize temp structs to known values before populating
	  memset( dpt, 0, sizeof(CBPM_DATA) );
	  memset( cpt, 0, sizeof(CBPM_CONTROL) );

	  //----------------------------------------------------------------------
	  // Copy any data already in place from each instrument's structure tree
	  // to the temporary structure prior to updating the temporary 
	  // structure tree.  Changes to temporary tree get pushed back to 
	  // instrument tree after the updates.
	  //----------------------------------------------------------------------
	  memmove( cpt,  cp,  sizeof(CBPM_CONTROL) );
	  memmove( dpt,  dp,  sizeof(CBPM_DATA) );


	  // For each parameter to harvest that is defined above, read it from the file and 
	  // load it into the appropriate place in the temporary structure tree.
	  int i = 0;
	  

	  //-----------------------------------------------------
	  // Read in parameters
	  //-----------------------------------------------------
	  i = 0;
	  while (pars[i].rows != 0) { 
	    stat = faccess( FA_READ, fp, section, pars[i].name, pars[i].rows,
			    pars[i].cols, pars[i].type, pars[i].pointer );
	    if (stat != 0) {
	      printf(" !- Element (%s) NOT FOUND when reading parameters for section %s!\n", pars[i].name, section);
	    }
	    i++;
	  } //endWHILE pars


	  //---------------------------------------------------------------------------------
	  // Copy CHANGES TO populated read-in structures to instrument's structure targets
	  //---------------------------------------------------------------------------------
	  memcpy( cp,  cpt,  sizeof(CBPM_CONTROL) );
	  memcpy( dp,  dpt,  sizeof(CBPM_DATA) );
	  // Only copy relevant fields of the ethernet info structure.
	  strcpy( (char *)&(CTL_System.p_Module[inst]->comm.ethernet.hostname), (char *)&(CTL_System.p_Module_temp->comm.ethernet.hostname) );
	  strcpy( (char *)&(CTL_System.p_Module[inst]->comm.ethernet.IP_address), (char *)&(CTL_System.p_Module_temp->comm.ethernet.IP_address) );


	} //endIF strcmp(section...
      } //endIF det.name != ""
    } //endFOR inst (n_Modules)
  } //endFOR sect_cnt < FA_MAX_FILE_SECTIONS
  

  printf("Stopping faccess() session...\n");
  faccess( FA_READ, fp, NULL, NULL, 0, 0, 0, NULL);

  fclose(fp);

  return F_SUCCESS;
  
}
