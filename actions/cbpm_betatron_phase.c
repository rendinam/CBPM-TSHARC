/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_beta_phase.c                                     |
*                                                                       |
* Description  :  Sets up all the necessary structures for performing a |
*                 betatron phase measurement in a central, temporary    |
*                 master struct.                                        |
*                                                                       |
* Arguments    :  None                                                  |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_beta_phase_prep(void) {

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  printf("********* GET BETATRON PHASE *********\n");
  printf("This will calculate the betatron phase coefficients for a single bunch of\n");
  printf("either species, and generate setup/comparison coefficient tables for\n");
  printf("instrument installation and diagnostic purposes.\n");

  int species, use_enable_gate;
  int retval;

  int tsetup;
  tsetup = cbpm_measurement_tsetup();
  cbpm_query_species(tsetup);
  species = dp->cbpm_cmd_params.species;

  // Set bunch pattern ------------------------------------------------------
  //cbpm_query_bunches(dp->cbpm_cmd_params.bunch_pat);
  CTL_System.phase_meas_bunch_index = cbpm_query_bunch( tsetup, species, dp->cbpm_cmd_params.bunch_pat );

  dp->cbpm_cmd_params.num_turns = 40960;
  printf("Will acquire %d turns of data.\n", dp->cbpm_cmd_params.num_turns );

  dp->cbpm_cmd_params.delay_cal  = FALSE;
  dp->cbpm_cmd_params.gain_cal   = FALSE;

  cbpm_query_trigger(dp);
  dp->cbpm_cmd_params.scale_mode      = PED_SUB_DATA;

  dp->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;
  dp->cbpm_cmd_params.avg_mode           = PROC_SIMPLE_AVG_MODE;


  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(1);

  int valid_inputs[5] = {4, (int)'y', (int)'Y', (int)'n', (int)'N'};
  int map_out[4]      = {     TRUE,     TRUE,    FALSE,    FALSE};

  prompts[0].supp_text_ptr    = "Answering this question will start the acquistiion.\n\n";
  prompts[0].prompt_text      = "Generate phase tables for setup/comparison? [y/n]";
  prompts[0].data_type        = CBI_CHAR;
  prompts[0].valid_inputs     = valid_inputs;
  prompts[0].map_out          = map_out;
  prompts[0].default_value    = 'y';
  prompts[0].destination      = &(dp->cbpm_cmd_params.generate_phase_tables);
  prompts[0].last_val_default = TRUE;

  retval = CBI_PROMPTER();


  // Set the maximum number of hanshake completion checks to make.
  // Only manifests if there is a problem that causes a handshake timeout.
  cbi_set_handshake_tries( 60 );


  if (use_enable_gate) {
    return CBI_F_SUCCESS_W_GATE;
  } else {
    return CBI_F_SUCCESS_NO_GATE;
  }


}




int cbpm_beta_phase_prep_from_iface(void) {

  CBPM_DATA *dp;

  dp = CTL_System.p_Module_temp->dsp_data;
 
  int use_enable_gate = TRUE;
  int turns;

  // CESR MPM Interface array provided paramters ------  

  // Clear the bunch pattern
  cbi_clear_bp( dp->cbpm_cmd_params.bunch_pat );


  dp->cbpm_cmd_params.num_turns          = 40960;
  dp->cbpm_cmd_params.species            = CTL_CBPM_Intf_Params.species;



  int NEW_BUNCH_OVERRIDE_TESTING_MODE = FALSE;

  int tempbunch, internal_index;
  int ele[2];
  int status;


  if (NEW_BUNCH_OVERRIDE_TESTING_MODE == FALSE) {


    // Get bunch to examine:  HARDCODED to 'T1B1' (first bunch).
    if (dp->cbpm_cmd_params.species == POSITRONS) {
      cbi_set_bunch( dp->cbpm_cmd_params.bunch_pat, 1 );
      CTL_System.phase_meas_bunch_index = 1;
    }
    if (dp->cbpm_cmd_params.species == ELECTRONS) {
      cbi_set_bunch( dp->cbpm_cmd_params.bunch_pat, 2 );
      CTL_System.phase_meas_bunch_index = 2;
    }



  } else {

    printf("\n\nUsing bunch override mode for phase measurement!\n");
    printf("Taking bunch index override from MPM node 'CBI CONTROL2' element '1'\n\n");
    // Get bunch to examine from TEMPORARY MPM location
    ele[0] = ele[1] = 1;
    status = vxgetn_c( "CBI CONTROL2", ele[0], ele[1], &tempbunch );
    if (status != MPM_SUCCESS) {
      sprintf(message, "ERROR obtaining bunch value from temporary location! %s ele:%d",
	      CBPM_CONTROL_NODE,
	      ele[0]  );
    }
    CTL_System.phase_meas_bunch_index = tempbunch;
    
    if (dp->cbpm_cmd_params.species == POSITRONS) {
      internal_index = (tempbunch*2)-1;
      cbi_set_bunch( dp->cbpm_cmd_params.bunch_pat, internal_index );
    }
    if (dp->cbpm_cmd_params.species == ELECTRONS) {
      internal_index = (tempbunch*2);
      cbi_set_bunch( dp->cbpm_cmd_params.bunch_pat, internal_index );
    }
    printf("Internal Bunch index = %d\n", internal_index );


  } //endIF NEW_BUNCH_OVERRIDE_TESTING_MODE




  // Set the trigger mask
  dp->cbpm_cmd_params.use_data_enable    = TRUE;
  dp->cbpm_cmd_params.trig_mask          = TRIG_MASK_A;

  dp->cbpm_cmd_params.gain_cal           = TRUE;
  dp->cbpm_cmd_params.delay_cal          = FALSE;

  dp->cbpm_cmd_params.avg_mode           = PROC_SIMPLE_AVG_MODE; // Not needed.
  dp->cbpm_cmd_params.scale_mode         = PED_SUB_DATA;

  dp->cbpm_cmd_params.reset_proc_buf_idx    = CBI_TRUE;
 
  // FALSE = Fast method for use once turn offsets are known (i.e. after initial deployment)
  dp->cbpm_cmd_params.generate_phase_tables = FALSE; 


  printf("\n\n  betatron_phase_prep_from_iface() values obtained from MPM:\n");
  printf("    species     : %d\n", dp->cbpm_cmd_params.species );


  // Set the maximum number of hanshake completion checks to make.
  // Only manifests if there is a problem that causes a handshake timeout.
  cbi_set_handshake_tries( 60 );


  return CBI_F_SUCCESS_W_GATE;

}




int cbpm_beta_phase_post(int iidx) {

  FILE *fp, *rfp;
  char filename[CBI_MAX_STRING_LENGTH]   = "";
  char full_fname[CBI_MAX_STRING_LENGTH] = "";

  int i, j, k;
  int species, nturns;

  CBPM_DATA     *dp;

  // Pointer to platform-specific data sub-structure
  dp = CTL_System.p_Module[iidx]->dsp_data;

 
  // Command is done.  Read back status and payload
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CMD_PARAMS_TAG,      CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_BETA_PHASE_OUT_TAG,  CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_RAW_DATA_HEADER_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }

  // New, for obtaining averaged data alongside the betatron phase calculation
  // results and/or comparison tables.
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_BUF_HEADER_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }
  k=cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_BUF_TAG,        CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (k == CBI_F_FAILURE) {
    return CBI_F_COMM_ERROR;
  }
  
  //printf("num_bunches[0] = %d\n", dp->cbpm_raw_data_header.num_bunches[0] );
  //printf("num_bunches[1] = %d\n", dp->cbpm_raw_data_header.num_bunches[1] );

  nturns  = dp->cbpm_cmd_params.num_turns;
  int block_0_words = dp->cbpm_raw_data_header.num_bunches[0] * nturns;
  int block_1_words = dp->cbpm_raw_data_header.num_bunches[1] * nturns;


  species = dp->cbpm_cmd_params.species;

  return CBI_F_SUCCESS;

}



int cbpm_beta_phase_final(void) {

  int retval, iidx;
  CBPM_DATA     *dp;

  FILE *fp;
  char filename[CBI_MAX_STRING_LENGTH]   = "";
  char full_fname[CBI_MAX_STRING_LENGTH] = "";


  for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {
    if (CTL_System.p_Module[iidx]->active == CBI_ACTIVE) {
      
      strcpy( filename, "" );
      strcpy( full_fname, "" );

      dp = CTL_System.p_Module[iidx]->dsp_data;

      if (dp->cbpm_cmd_params.generate_phase_tables == TRUE) {
	// Write phase turns offset scan table to disk and selected entry to database
	fp = cbi_managed_file_open( filename, full_fname, iidx, PHASE_DIR, PHASE_TABLES, CBI_MULTI_FILE, FALSE );
	cbpm_phase2db( fp, iidx );
	fclose(fp);
      } else {
	fp = stdout;
	// Only write selected phase table entry to MPM database
	cbpm_phase2db( stdout, iidx );
      }

    }
  }

  cbpm_print_turn_count_summary();

  return CBI_F_SUCCESS;

}
