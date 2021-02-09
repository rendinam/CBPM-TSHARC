/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_read_detector_params.c                           |
*                                                                       |
* Description  :  Read in the collection of general instrument          |
*                 operational parameters for each device that is online |
*                 and load up the appropriate control-side master       |
*                 structure members for each.                           |
*                                                                       |
* Arguments    :  -Pointer to operational parameters filename string    |
*                 -Pointer to holding array of master control structs   |
*                  to populate.                                         |
*              Use note: One must be sure to copy the values from       |
*              temporary input structure(s) to the final destination at |
*              the conclusion of the series of file accesses.           |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"


#define MAX_PARAMS              80   // FIXME: Relocate to managed file IO library and header.
#define MAX_SETUP_PARAMS        15
#define NUM_CONDITIONAL_PARAMS   1



int cbpm_read_detector_params( char *filename ) {
  
  char *func_name = (char *)__FUNCTION__;
  int debug_level = CTL_System.debug_verbosity;

  // Pointers to 'data' substructure (destination & temporary)
  CBPM_DATA      *dp,  *dpt;
  dpt = CTL_System.p_Module_temp->dsp_data;


  // Pointers to 'control' substructure (destination & temporary)
  CBPM_CONTROL  *cp,  *cpt;
  cpt = CTL_System.p_Module_temp->control;

  int status;

  // Arary of structures to hold file access info for each general instrument parameter
  PARAMETER_INFO pars[MAX_PARAMS];
  memset( &pars, 0, sizeof(pars) );


  //-----------------------------------------
  // Parameter file search token definitions
  //-----------------------------------------

  //                                      rows  cols
  // Detector Parameters ------------------------------------------------------------------------------------
  cbi_add_param( pars, "DETECTOR_NAME",      1, 1,               STRING, (void *)&cpt->detector.name );
  cbi_add_param( pars, "DETECTOR_LOCATION",  1, 1,               STRING, (void *)&CTL_System.p_Module_temp->det.location );
  cbi_add_param( pars, "SURVEY_NAME",        1, 1,               STRING, (void *)&cpt->detector.name_survey );
  cbi_add_param( pars, "CESRV_NAME",         1, 1,               STRING, (void *)&cpt->detector.name_cesrv );
  cbi_add_param( pars, "TYPE",               1, 1,               STRING, (void *)&cpt->detector.type );
  cbi_add_param( pars, "HORZ_POS_COEFF",     1, 1,               FLOAT,  (void *)&cpt->detector.horz_coeff );		
  cbi_add_param( pars, "VERT_POS_COEFF",     1, 1,               FLOAT,  (void *)&cpt->detector.vert_coeff );
  cbi_add_param( pars, "RING_POSITION",      1, 3,               FLOAT,  (void *)&cpt->detector.ring_position );
  cbi_add_param( pars, "RING_ROTATION",      1, 1,               FLOAT,  (void *)&cpt->detector.ring_rotation );
  cbi_add_param( pars, "LOCAL_AXES",         1, 2,               FLOAT,  (void *)&cpt->detector.local_axes );
  cbi_add_param( pars, "CURRENT_COEFF",      1, 1,               FLOAT,  (void *)&cpt->detector.current_coeff );

  cbi_add_param( pars, "XBUS_PKT_NODE",      1, 1,               STRING, (void *)&CTL_System.p_Module_temp->comm.xbus.pkt_node );
  cbi_add_param( pars, "XBUS_PKT_ELE",       1, 1,               INT,    (void *)&CTL_System.p_Module_temp->comm.xbus.element );

  cbi_add_param( pars, "DETEC_ID_NODE",      1, 1,               STRING, (void *)&cpt->detector.detec_id_node );
  cbi_add_param( pars, "DETEC_ID_ELE",       1, 1,               INT,    (void *)&cpt->detector.detec_id_ele );
  cbi_add_param( pars, "BUTTON_FLAG_NODE",   1, 1,               STRING, (void *)&cpt->detector.button_flag_node );
  cbi_add_param( pars, "BUTTON_FLAG_ELE",    1, 1,               INT,    (void *)&cpt->detector.button_flag_ele );
  cbi_add_param( pars, "BUTTON_DATA_NODE",   1, 1,               STRING, (void *)&cpt->detector.button_data_node );
  cbi_add_param( pars, "BUTTON_DATA_IDX",    1, 1,               INT,    (void *)&cpt->detector.button_data_idx );
  cbi_add_param( pars, "BUTTON_DATA_OFFSET", 1, CBI_NUM_SPECIES, INT,    (void *)&cpt->detector.button_data_offset );
  cbi_add_param( pars, "ORBIT_STAT_NODE",    1, 1,               STRING, (void *)&cpt->detector.orbit_stat_node );
  cbi_add_param( pars, "ORBIT_STAT_ELE",     1, 1,               INT,    (void *)&cpt->detector.orbit_stat_ele );
  cbi_add_param( pars, "POSITION_NODE",      1, 1,               STRING, (void *)&cpt->detector.position_node );
  cbi_add_param( pars, "POSITION_IDX",       1, 1,               INT,    (void *)&cpt->detector.position_idx );
  cbi_add_param( pars, "POSITION_OFFSET",    1, 2,               INT,    (void *)&cpt->detector.position_offset );
  cbi_add_param( pars, "PHASE_NODE",         1, 1,               STRING, (void *)&cpt->detector.phase_node );
  cbi_add_param( pars, "PHASE_IDX",          1, 1,               INT,    (void *)&cpt->detector.phase_idx );
  cbi_add_param( pars, "PHASE_STAT_NODE",    1, 1,               STRING, (void *)&cpt->detector.phase_stat_node );
  cbi_add_param( pars, "PHASE_STAT_ELE",     1, 1,               INT,    (void *)&cpt->detector.phase_stat_ele );

  cbi_add_param( pars, "14_NS_PHASE_RACE_WAIT_FLAGS", 1, 2,  INT, (void *)&cpt->config_params.ph_race_wait_flags[0] );
  cbi_add_param( pars, "14_NS_e+_PHASE_TURN_OFFSET",  5, 9,  INT, (void *)&cpt->config_params.ph_turn_offsets_14ns[0] );
  cbi_add_param( pars, "14_NS_e-_PHASE_TURN_OFFSET",  5, 9,  INT, (void *)&cpt->config_params.ph_turn_offsets_14ns[1] );
  
  cbi_add_param( pars, "14_NS_e+_PHASE_JUMP_BOUNDS", 1, 1, INT, (void *)&(cpt->config_params.phase_jump_boundaries[POSITRONS]) );
  cbi_add_param( pars, "14_NS_e+_PHASE_TURNS",       1, 2, INT, (void *)&(cpt->config_params.phase_turn_offsets[POSITRONS]) );
  cbi_add_param( pars, "14_NS_e+_PHASE_DELAYS",      1, 2, INT, (void *)&(cpt->config_params.phase_wait_values[POSITRONS]) );

  cbi_add_param( pars, "14_NS_e-_PHASE_JUMP_BOUNDS", 1, 1, INT, (void *)&cpt->config_params.phase_jump_boundaries[ELECTRONS] );
  cbi_add_param( pars, "14_NS_e-_PHASE_TURNS",       1, 2, INT, (void *)&cpt->config_params.phase_turn_offsets[ELECTRONS] );
  cbi_add_param( pars, "14_NS_e-_PHASE_DELAYS",      1, 2, INT, (void *)&cpt->config_params.phase_wait_values[ELECTRONS] );
  

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
	  printf("Reading location-dependent parameters for: %s\n", section);
	  
	  
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
	  memmove( cpt, cp, sizeof(CBPM_CONTROL) );
	  memmove( dpt, dp, sizeof(CBPM_DATA) );
	  
          
	  // For each parameter to harvest that is defined above, read it from the file and 
	  // load it into the appropriate place in the temporary structure tree.
	  int i = 0;
	  
	  
	  //----------------------------
	  // Read in general parameters
	  //----------------------------
	  i = 0;
	  while (pars[i].rows != 0) { 
	    status = faccess( FA_READ, fp, section, pars[i].name, pars[i].rows, pars[i].cols, pars[i].type, pars[i].pointer );
	    if (status != 0) {
	      printf(" !- Element (%s) NOT FOUND when reading parameters for section %s!\n", pars[i].name, section);
	    }
	    i++;
	  } // endWHILE general parameters
	  
	  
	  
	  // Copy values for core structure tree elements instead of a wholesale memory imaging
	  // to prevent corruption of pointers in the central structure tree.
	  strcpy( CTL_System.p_Module[inst]->det.location, CTL_System.p_Module_temp->det.location );
	  
	  //---------------------------------------------------------------------
	  // Copy populated read-in structures to instrument's structure targets
	  //---------------------------------------------------------------------
	  memmove( cp, cpt, sizeof(CBPM_CONTROL) );
	  memmove( dp, dpt, sizeof(CBPM_DATA) );
	  memcpy( &(CTL_System.p_Module[inst]->comm.xbus), &(CTL_System.p_Module_temp->comm.xbus), sizeof(XBUS_INFO) );
	  
	  //Replace the underscores in BPM mnenomic names
	  replace( (char *) &(cp->detector.button_data_node), '_', ' ');
	  replace( (char *) &(cp->detector.position_node), '_', ' ');
	  replace( (char *) &(cp->detector.orbit_stat_node), '_', ' ');
	  replace( (char *) &(cp->detector.phase_node), '_', ' ');
	  replace( (char *) &(cp->detector.phase_stat_node), '_', ' ');
	  replace( (char *) &(cp->detector.button_data_node), '_', ' ');

	  	  // Process/propagate XBUS addressing info.
	  // 1) Strip out underscores until faccess is intelligent enough to acquire quoted strings.
	  // 2) Replace substrings as necessary to turn PKT node name into ADR and DAT node names.
	  replace( CTL_System.p_Module[inst]->comm.xbus.pkt_node, '_', ' ' );
	  strcpy( CTL_System.p_Module[inst]->comm.xbus.adr_node, 
		  (char *)replace_str(CTL_System.p_Module[inst]->comm.xbus.pkt_node, "PKT", "ADR") );
	  strcpy( CTL_System.p_Module[inst]->comm.xbus.dat_node, 
		  (char *)replace_str(CTL_System.p_Module[inst]->comm.xbus.pkt_node, "PKT", "DAT") );

	} //endIF strcmp(section...
      } //endIF det.name != ""
    } //endFOR inst (n_Modules)
  } //endFOR sect_cnt < FA_MAX_FILE_SECTIONS

  
  printf("Stopping faccess() session...\n");
  faccess( FA_READ, fp, NULL, NULL, 0, 0, 0, NULL);

  fclose(fp);

  return F_SUCCESS;
  
}

