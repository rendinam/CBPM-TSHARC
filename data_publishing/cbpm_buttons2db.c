//-----------------------------------------------------------------------+
// File         :  cbpm_buttons2db.c                                     |
//                                                                       |
// Description  :  Stores averaged buttons data in the CESR control      |
//                 system database.                                      |
//                                                                       |
//                 Designed to be run after a processed data collection. |
//                                                                       |
// Prototype    :  int cbpm_buttons2db(CBPM_CONTROL_MODULE*)             |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Return Value :  Status - F_SUCCESS/F_FAILURE                          |
//                                                                       |
// Author       :  M. Palmer  /  M. Rendina                              |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


#define POSITION_SCALE          0.000000001  // 1nm


int cbpm_buttons2db( int iidx ) {

  char *func_name = (char *)__FUNCTION__;
  int debug_level = CTL_System.debug_verbosity;

  int card;
  int status;

  int offset;

  // For MPM addressing
  char *node;
  int ele[2];

  int data[CBPM_MAX_CARDS];
  int data_idx[CBPM_MAX_CARDS];

  int newdata[CBPM_MAX_CARDS];


  CBPM_DATA    *dp;
  CBPM_CONTROL   *cp;

  // Pointer to platform-specific data sub-structure
  dp = CTL_System.p_Module[iidx]->dsp_data;
  // Pointer to platform-specific control sub-structure.
  cp = CTL_System.p_Module[iidx]->control;


  int species = dp->cbpm_cmd_params.species;


  // FIXME: Move this to a parameters header file.
  //                               Card-0     Card-1     Card-2     Card-3
  int ctl_button_orientation[] = {CBPM_TOP_INNER, CBPM_BOT_INNER, CBPM_BOT_OUTER, CBPM_TOP_OUTER};


  // Get the button orientation mapping
  /*
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    data_idx[card] = card;
  }
  */

  //for (species = POSITRONS; species <= ELECTRONS; species++) {
    
  // Publish the processed (avaraged) button data for this device/detector to
  // it's assigned MPM locations.
  node = cp->detector.button_data_node;

  ele[0] = 4 * (cp->detector.button_data_idx - 1) + 
                cp->detector.button_data_offset[species] + 1;
  if (debug_level > CBI_DEBUG_LEVEL_2) {
    printf("OFFSET : %d\n", cp->detector.button_data_offset[species]);
  }
  ele[1] = ele[0] + 3;

  // Get number of most recent processed data entry
  int active_buf = dp->cbpm_proc_buf_header.active_buf;

  int write_ptr = 0;

  //
  // The CESR control system database expects the button data in the 
  // order (BI, BO, TI, TO), arrange it as such.
  // 

  // FIXME: No button orientation mapping applied yet!
  //for (card = 0; card < CBPM_MAX_CARDS; card++) {
  //  data[card] = (int)
  //    ( 100 * fabs((double)(dp->proc_buf.proc_data[write_ptr].signal[card])) );
  //}

  // BPM front panel button mapping TO, BO, BI, TI
  // Correct mapping for database writes is  BI, BO, TI, TO

  data[0] = (int)( 100 * fabs((double)(dp->cbpm_proc_buf.proc_data[write_ptr].signal[1])) );
  data[1] = (int)( 100 * fabs((double)(dp->cbpm_proc_buf.proc_data[write_ptr].signal[2])) );
  data[2] = (int)( 100 * fabs((double)(dp->cbpm_proc_buf.proc_data[write_ptr].signal[0])) );
  data[3] = (int)( 100 * fabs((double)(dp->cbpm_proc_buf.proc_data[write_ptr].signal[3])) );

  newdata[0] = (int)( 100 * fabs((double)(dp->cbpm_proc_buf.proc_data[write_ptr].signal[0])) );
  newdata[1] = (int)( 100 * fabs((double)(dp->cbpm_proc_buf.proc_data[write_ptr].signal[1])) );
  newdata[2] = (int)( 100 * fabs((double)(dp->cbpm_proc_buf.proc_data[write_ptr].signal[2])) );
  newdata[3] = (int)( 100 * fabs((double)(dp->cbpm_proc_buf.proc_data[write_ptr].signal[3])) );
  

  if (!continuous_mode) {
    printf("\n\n   ORBIT BUTTON VALUES: %d  %d  %d  %d \n", data[0], data[1], data[2], data[3]);
  }

  float x_coeff = cp->detector.horz_coeff;  // mm
  float y_coeff = cp->detector.vert_coeff;  // mm
  float TO, BO, BI, TI;
  float Xpos; 
  float Ypos;

  TO = dp->cbpm_proc_buf.proc_data[write_ptr].signal[0]; 
  BO = dp->cbpm_proc_buf.proc_data[write_ptr].signal[1];
  BI = dp->cbpm_proc_buf.proc_data[write_ptr].signal[2];
  TI = dp->cbpm_proc_buf.proc_data[write_ptr].signal[3];

  Xpos = cbpm_compute_pos(iidx, CBPM_HORZ_DIM, TI, BI, BO, TO);
  Ypos = cbpm_compute_pos(iidx, CBPM_VERT_DIM, TI, BI, BO, TO);

  if (!continuous_mode) {
    printf("   Calculated position X = %f\n", Xpos );
    printf("   Calculated position Y = %f\n", Ypos );
  }

  if ( strcmp(INSTRUMENT_TEST_NAME, cp->detector.name) == 0 ) {
    
    //-----------------------------------------------------------
    // Perform the write of buttons values to the database node
    //-----------------------------------------------------------
    sprintf(message, "Instrument designated for testing.  Not publishing results to MPM database.");
    log_message( S_INFO, func_name, message );

  } else {

    if (!continuous_mode) {
      printf("-- Writing BUTTON data for %s (%s) / %s \n   to elements %3d through %3d in node %s \n",
	     CTL_System.p_Module[iidx]->core->cbi_ident.hostname,
	     CTL_System.p_Module[iidx]->core->cbi_ident.ipaddr,
	     cp->detector.name,
	     ele[0],
	     ele[1],
	     node );
    }

    // Publish the four averaged button signal values
    status = vxputn_c(node, ele[0], ele[1], data);
    if (status != MPM_SUCCESS) {
      sprintf(message, "ERROR writing buttons data!");
      log_message(S_ERROR, func_name, message);
    }


    //------------------------------------------------
    // New MPM arrays for receiving orbit BUTTON data
    //------------------------------------------------
    ele[0] = ((cp->detector.detec_id_ele - 1) * CBPM_ORBIT_ENTRY_SIZE) + 1;
    ele[1] = ele[0] + 3;
    //if (debug_level > CBI_DEBUG_LEVEL_2) {
    printf("-- Writing BUTTON data for %s (%s) / %s \n   to elements %3d through %3d in node %s \n",
	   CTL_System.p_Module[iidx]->core->cbi_ident.hostname,
	   CTL_System.p_Module[iidx]->core->cbi_ident.ipaddr,
	   cp->detector.name,
	   ele[0],
	   ele[1],
	   CBPM_ORBIT_DATA_NODE );
    //}
    status = vxputn_c(CBPM_ORBIT_DATA_NODE, ele[0], ele[1], newdata);
    if (status != MPM_SUCCESS) {
      sprintf(message, "ERROR writing new BUTTON data!");
      log_message(S_ERROR, func_name, message);
    }
   
    

  }


  //------------------------------------
  // Publish the X and Y position data 
  //------------------------------------
  node   = cp->detector.position_node;

  ele[0] = 2 * (cp->detector.position_idx - 1) + 
                cp->detector.position_offset[species] + 1;
  ele[1] = ele[0] + 1;

  data[0] = (int)(Xpos / POSITION_SCALE);
  data[1] = (int)(Ypos / POSITION_SCALE);

  //if (debug_level > CBI_DEBUG_LEVEL_2) {
    printf("-- Writing POSITION data for %s (%s) / %s \n   to elements %3d through %3d in node %s \n",
	   CTL_System.p_Module[iidx]->core->cbi_ident.hostname,
	   CTL_System.p_Module[iidx]->core->cbi_ident.ipaddr,
	   cp->detector.name,
	   ele[0],
	   ele[1],
	   node );
    //}
  status = vxputn_c(node, ele[0], ele[1], data);
  if (status != MPM_SUCCESS) {
    sprintf(message, "ERROR writing POSITION data!");
    log_message(S_ERROR, func_name, message);
  }
  

  //------------------------------------------
  // Write POSITION values to new MPM nodes
  //------------------------------------------
  ele[0] = ((cp->detector.detec_id_ele - 1) * CBPM_ORBIT_ENTRY_SIZE) + 5;
  ele[1] = ele[0] + 1;
  //if (debug_level > CBI_DEBUG_LEVEL_2) {
    printf("-- Writing POSITION data for %s (%s) / %s \n   to elements %3d through %3d in node %s \n",
	   CTL_System.p_Module[iidx]->core->cbi_ident.hostname,
	   CTL_System.p_Module[iidx]->core->cbi_ident.ipaddr,
	   cp->detector.name,
	   ele[0],
	   ele[1],
	   CBPM_ORBIT_DATA_NODE );
    //}
  status = vxputn_c(CBPM_ORBIT_DATA_NODE, ele[0], ele[1], data);
  if (status != MPM_SUCCESS) {
    sprintf(message, "ERROR writing POSITION data!");
    log_message(S_ERROR, func_name, message);
  }



  //--------------------------------------------------
  // Flag that entry as 'GOOD' in the "CSRBPM DSTAT"
  // node so CESRV will plot it after the measurement
  // completes.
  //
  //   TODO: Add call to a data examination 
  //         routine that can make an assessment of
  //         the values before flagging/writing.
  //--------------------------------------------------
  node = cp->detector.orbit_stat_node;
  ele[0] = ele[1] = cp->detector.orbit_stat_ele;

  int dataval;

  // If instrument has been designated a test unit, do not publish
  // results to MPM database.
  if ( strcmp(INSTRUMENT_TEST_NAME, cp->detector.name) == 0 ) {

    sprintf(message, "Instrument designated for testing.  Not flagging data in MPM database.");
    log_message(S_INFO, func_name, message);

  } else {

    if ( CTL_System.p_Module[iidx]->in_recovery == CBI_TRUE ) {
    
      sprintf( message, "Instrument presently in ERROR RECOVERY MODE, flagging data as 'BAD' in MPM database.");
      log_message(S_INFO, func_name, message);
      dataval = CBPM_BAD_DATA;
      
    } else {  // Otherwise, the data should be flagged for display in MPM database.
    
      dataval = CBPM_GOOD_DATA;
    }

    status = vxputn_c(node, ele[0], ele[1], &dataval);
    if (status != MPM_SUCCESS) {
      sprintf(message, "ERROR writing 'DATA GOOD' flag to the DSTAT node\n for instrument %s (%s) / %s",
	      CTL_System.p_Module[iidx]->comm.ethernet.hostname,
	      CTL_System.p_Module[iidx]->comm.ethernet.IP_address,
	      cp->detector.name );
      log_message(S_ERROR, func_name, message);
    }

  }


  // Fill n_sample, x_rms, and y_rms in the "CBPM DET SAVE" node (?)
    
  return CBI_F_SUCCESS;

}
