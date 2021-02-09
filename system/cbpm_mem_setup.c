//-----------------------------------------------------------------------+
// File         :  cbpm_mem_setup.c                                      |
//                                                                       |
// Description  :  Handles attachment of BPM-specific communication data |
//                 structures to the central management data structure   |
//                 as well as other platform-specific initializations    |
//                 for each instrument.                                  |
//                                                                       |
// Arguments    :                                                        |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_mem_setup( void ) {

  char *func_name = (char *)__FUNCTION__;

  int iidx;
  int status;

  //Initialize the temporary TSHARC structure tree for 'command imaging' 
  // and attach to temp master struct.
  CTL_System.p_Module_temp->dsp_data = (void *)(&(cbpm_comm_structs_temp));
  if ((status = cbpm_init_a(CTL_System.p_Module_temp)) != CBI_F_SUCCESS) {
    sprintf(message, "BPM tempstruct initialization failed -- Exiting!");
    log_message(S_ERROR,func_name,message);
    return CBI_F_FAILURE;
  }
  
  // Initialize the temporary TSHARC structure tree for platform-dependent
  // CONTROL structures and attach to temp master struct.
  CTL_System.p_Module_temp->control = &(tsharc_ctl_temp);

/*   CTL_System.p_Module_temp->analysis = (void *)&(cbpm_tbt_analysis_temp); */
/*   CTL_System.p_Module_temp->analysis.tbt_data = &(cbpm_tbt_data_temp); */
/*   CTL_System.p_Module_temp->analysis.pos_data = &(cbpm_pos_data_temp); */

  //-------------------------------------------------------------------------------


  // Attach device-specific communication structs to master tree.
  for (iidx = 0; iidx < CBI_MAX_INSTRUMENTS; iidx++) {
    
    // Initialize pointer key for platform-specific structs
    CTL_System.p_Module[iidx]->dsp_data = (void *)(&(cbpm_comm_structs[iidx])); //Attach data struct to master
    // Initialize all values in sub-structure
    memset( CTL_System.p_Module[iidx]->dsp_data, 0, sizeof(CBPM_DATA) );
    if ((status = cbpm_init_a(CTL_System.p_Module[iidx])) != CBI_F_SUCCESS) {
      sprintf(message, "BPM communications structure (TigerSHARC-based) initialization failed -- Exiting!");
      log_message(S_ERROR,func_name,message);
      return CBI_F_FAILURE;
    }
  }
  //printf("Got through CESR BPM (TigerSHARC-based) COMMUNICATION structures initialization.\n");



  // Attach instrument-specific CONTROL structs to master tree.
  for (iidx = 0; iidx < CBI_MAX_INSTRUMENTS; iidx++) {
    
    // Set pointer to the structure corresponding to the BPM to initialize.
    CTL_System.p_Module[iidx]->control = (void *)&(tsharc_ctl[iidx]);
    // Initialize communication identification fields.
    strcpy( (char *)CTL_System.p_Module[iidx]->comm.ethernet.hostname, "" );
    strcpy( (char *)CTL_System.p_Module[iidx]->comm.ethernet.IP_address, "" );

  }
  //printf("Got through CESR BPM CONTROL structures initialization.\n\n");



  // Attach instrument-specific ANALYSIS structures to master tree.
  for (iidx = 0; iidx < CBI_MAX_INSTRUMENTS; iidx++) {
    CTL_System.p_Module[iidx]->analysis = (void *)&(cbpm_tbt_data[iidx]);
    //CTL_System.p_Module[iidx]->analysis = (void *)&(cbpm_tbt_analysis[iidx]);
    //CTL_System.p_Module[iidx]->analysis.tbt_data = &(cbpm_tbt_data[iidx]);
    //CTL_System.p_Module[iidx]->analysis.pos_data = &(cbpm_pos_data[iidx]);
  }


  //----------------------------------------------------
  // Attach platform-specific bunch pattern storage
  // array in temporary instrument management structure
  // to master management structure for access by 
  // lower-level generalized routines.
  //----------------------------------------------------
  CBPM_DATA *dpt;
  dpt = CTL_System.p_Module_temp->dsp_data;
  CTL_System.meas_info.p_bunch_pattern   =  (int *)&(dpt->cbpm_cmd_params.bunch_pat );
  CTL_System.meas_info.p_num_turns       =  &(dpt->cbpm_cmd_params.num_turns );
  //CTL_System.meas_info.p_timing_setup    =  &(dpt->cbpm_op_timing.active_timing_setup );
  CTL_System.meas_info.p_turn_spacing    =  &(dpt->cbpm_cmd_params.spacex_turn );
  
  // Store the identifier for the instrument type to be controlled
  // by this program instance.
  strcpy( CTL_System.itype.instrument_type, "BPM" );


  //----------------------------------------------------
  // Attach platform-specific instrument executable 
  // routine name list to master management structure.  
  //----------------------------------------------------
  CTL_System.itype.instrument_routine_names = (char *)&( cbpm_routine_names[0] );
  CTL_System.itype.tsetup_names             = (char *)&( tsetup_names[0] );
  CTL_System.itype.instrument_error_names = (char *)&( cbpm_error_names[0] );
  CTL_System.itype.num_instrument_error_names = NUM_ERROR_IDS;    

  // Attach instrument-specific functions to central management structure for lower-level
  // library access.
  CTL_System.itype.tsetup_func_ptr    = &(cbpm_measurement_tsetup);
  CTL_System.itype.species_func_ptr   = &(cbpm_measurement_species);
  CTL_System.itype.supp_info_func_ptr = &(cbpm_print_inst_info);


  return CBI_F_SUCCESS;

}
