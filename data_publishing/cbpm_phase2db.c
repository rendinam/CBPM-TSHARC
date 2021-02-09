//-----------------------------------------------------------------------+
// File         :  cbpm_phase2db.c    NEW                                |
//                                                                       |
// Description  :  Stores calculated betatron phase data in the CESR     |
//                 control system database.                              |
//                                                                       |
// Prototype    :  int cbpm_phase2db(FILE *fp, CBPM_CONTROL_MODULE*)     |
//                                                                       |
// Arguments    :  File pointer to use when displaying status output     |
//                 int instrument index.                                 |
//                                                                       |
// Return Value :  Status - F_SUCCESS/F_FAILURE                          |
//                                                                       |
// Author       :  M. Palmer  /  M. Rendina                              |
//                  FIXME: Trim responsibility down in this file to just |
//                         writing the info to the database.             |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


#define PHASE_ENTRIES                      16
#define PHASE_SCALE_FACTOR                 10000
#define PUBLISHED_PHASE_VALUES_PER_CARD    4


int cbpm_phase2db( FILE *fp, int iidx ) {
  
  char *func_name = (char *)__FUNCTION__;

  int debug_level = CTL_System.debug_verbosity;

  int card;
  int status;
  int offset, stored_offset;

  // For MPM addressing
  char *node;
  int ele[2];

  int    data[PUBLISHED_PHASE_VALUES_PER_CARD * CBPM_MAX_CARDS];
  int newdata[PUBLISHED_PHASE_VALUES_PER_CARD * CBPM_MAX_CARDS];

  int data_idx[CBPM_MAX_CARDS];


  CBPM_DATA      *dp;
  CBPM_CONTROL   *cp;

  dp = CTL_System.p_Module[iidx]->dsp_data;
  cp = CTL_System.p_Module[iidx]->control;

  
  int species = dp->cbpm_cmd_params.species;

  int tsetup = dp->cbpm_op_timing.active_timing_setup;
  if (debug_level > CBI_DEBUG_LEVEL_2) {
    printf("TIMING SETUP = %d\n", tsetup);
  }

  node   = cp->detector.phase_node;

  ele[0] = PHASE_ENTRIES * (cp->detector.phase_idx - 1) + 1;
  ele[1] = ele[0] + PHASE_ENTRIES - 1;

  int cesr_ctl_order[] = {CBPM_BOT_INNER, CBPM_BOT_OUTER, CBPM_TOP_INNER, CBPM_TOP_OUTER};


  float hcos, hsin, vcos, vsin;
  float hamp, hphase, vamp, vphase;
  float Xcos, Xsin;
  float Ycos, Ysin;
  float Xamp, Yamp, target_Xamp, target_Yamp;
  float Xphase, Yphase, target_Xphase, target_Yphase;

  float rad2deg = 180.0 / PI_;
  float deg2rad = PI_ / 180.0;
  float phi;

  int but_data[CBPM_MAX_CARDS];
  int but_data_new[CBPM_MAX_CARDS];

  int   button_id, tot_turns;
  int   ph_race_wait, stored_ph_race_wait;
  char  ph_race_wait_state[CBPM_HALF_TURN_OPTIONS][5] = {"OFF", "ON"};

  int publish;

  int bunch;
  printf("********************* Bunch = %d\n", CTL_System.phase_meas_bunch_index );
  bunch = CTL_System.phase_meas_bunch_index;
  

  int PHASE_MULT     = PHASE_SCALE_FACTOR;


  if (dp->cbpm_cmd_params.generate_phase_tables == TRUE) {
    fprintf(fp, "#  Instrument  %s  @   %s  \n", CTL_System.p_Module[iidx]->comm.ethernet.hostname, cp->detector.name );
    fprintf(fp, "#                   Phase Angle (deg)     |                Amplitudes           |         Plane      |   BO-TI     BI-TO  |");
    fprintf(fp, "   Written Angle-Angle\n");
    fprintf(fp, "#          0 (TI)  1 (BI)  2 (BO)  3 (TO) |                                     | Ang. (deg)    Amp. |                    |\n");
  


    //------------------------------------------------------------------------
    // Pre-calculation needed for creating [Written Angle - Angle] column.
    //
    // Calculate angles and amplitudes for the turns-offset entry that will 
    // be written to the database.  This will be used to compute an angle 
    // difference between all other entries that may be directly cross-checked 
    // against third-party application (i.e. CESRV) output.
    //------------------------------------------------------------------------

    //offset = cp->config_params.ph_turn_offsets_14ns[species][0][0] - CBPM_MIN_DP_TURNS_OFFSET;
    //    ph_race_wait = cp->config_params.ph_race_wait_flags[species];

    offset = cbpm_phase_turn_offset( iidx, species, bunch );    
    ph_race_wait = cbpm_phase_wait_value( iidx, species, bunch );

    
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
      hcos = dp->cbpm_beta_phase_out.phase_results[offset][ph_race_wait][CBPM_HORZ_DIM][CBPM_COS_COEFF][card];
      hsin = dp->cbpm_beta_phase_out.phase_results[offset][ph_race_wait][CBPM_HORZ_DIM][CBPM_SIN_COEFF][card];
      vcos = dp->cbpm_beta_phase_out.phase_results[offset][ph_race_wait][CBPM_VERT_DIM][CBPM_COS_COEFF][card];
      vsin = dp->cbpm_beta_phase_out.phase_results[offset][ph_race_wait][CBPM_VERT_DIM][CBPM_SIN_COEFF][card];
      
      hamp = sqrt(hcos*hcos + hsin*hsin);  
      vamp = sqrt(vcos*vcos + vsin*vsin);  
      
      hphase = rad2deg * atan2(hcos,hsin);  // NOTE: Arguments were (hsin,hcos) prior to 18:30 26-Mar-2009
      vphase = rad2deg * atan2(vcos,vsin);  //            "         (vsin,vcos)           "
                                          //***********************************************************************
                                          // ! NOTE !: 
                                          // CESRV is the consumer of the database-published data values
                                          // for each button.  It uses a convention that expects the supplied data
                                          // to come from an INCORRECT (arguments reversed) atan2() call.
                                          // 
                                          //  Phase tables written to disk for both CBPM_I and CBPM_II systems
                                          //  contain data only valid for system comparisons and turns-offset
                                          //  determination, the actual angles are INVALID prior to processing by
                                          //  CESRV.
                                          //***********************************************************************

      // Save each for later recall   Phase angles are in degrees at this point.
      cp->phase_calcs.horz_amp[card]   = hamp;
      cp->phase_calcs.horz_phase[card] = hphase;
      cp->phase_calcs.vert_amp[card]   = vamp;
      cp->phase_calcs.vert_phase[card] = vphase;
    }


    
    // Calculations to pre-generate the POSITION phase angles and amplitudes for each plane.
  Xcos = (cp->phase_calcs.horz_amp[3] * cos(deg2rad * cp->phase_calcs.horz_phase[3])) + 
          (cp->phase_calcs.horz_amp[2] * cos(deg2rad * cp->phase_calcs.horz_phase[2])) - 
           (cp->phase_calcs.horz_amp[1] * cos(deg2rad * cp->phase_calcs.horz_phase[1])) - 
            (cp->phase_calcs.horz_amp[0] * cos(deg2rad * cp->phase_calcs.horz_phase[0]));
  Xsin = (cp->phase_calcs.horz_amp[3] * sin(deg2rad * cp->phase_calcs.horz_phase[3])) + 
          (cp->phase_calcs.horz_amp[2] * sin(deg2rad * cp->phase_calcs.horz_phase[2])) - 
           (cp->phase_calcs.horz_amp[1] * sin(deg2rad * cp->phase_calcs.horz_phase[1])) - 
            (cp->phase_calcs.horz_amp[0] * sin(deg2rad * cp->phase_calcs.horz_phase[0]));
  
  
  Ycos = (cp->phase_calcs.vert_amp[3] * cos(deg2rad * cp->phase_calcs.vert_phase[3])) + 
          (cp->phase_calcs.vert_amp[0] * cos(deg2rad * cp->phase_calcs.vert_phase[0])) - 
           (cp->phase_calcs.vert_amp[2] * cos(deg2rad * cp->phase_calcs.vert_phase[2])) - 
            (cp->phase_calcs.vert_amp[1] * cos(deg2rad * cp->phase_calcs.vert_phase[1]));
  Ysin = (cp->phase_calcs.vert_amp[3] * sin(deg2rad * cp->phase_calcs.vert_phase[3])) + 
          (cp->phase_calcs.vert_amp[0] * sin(deg2rad * cp->phase_calcs.vert_phase[0])) - 
           (cp->phase_calcs.vert_amp[2] * sin(deg2rad * cp->phase_calcs.vert_phase[2])) - 
            (cp->phase_calcs.vert_amp[1] * sin(deg2rad * cp->phase_calcs.vert_phase[1]));
  
  target_Xamp   = sqrt( (Xcos * Xcos) + (Xsin * Xsin) );
  target_Yamp   = sqrt( (Ycos * Ycos) + (Ysin * Ysin) );
  
  target_Xphase = rad2deg * atan2( Xcos , Xsin );
  target_Yphase = rad2deg * atan2( Ycos , Ysin );
  
  printf("target_Xphase = %f\n", target_Xphase);
  printf("target_Yphase = %f\n", target_Yphase);
  

  } // endIF generate_phase_tables



  printf("generate tables = %d\n", dp->cbpm_cmd_params.generate_phase_tables );


    printf("**********************************************\n");
    stored_offset = cbpm_phase_turn_offset( iidx, species, bunch );
    printf("Stored Offset       = %d\n", stored_offset);
    
    stored_ph_race_wait = cbpm_phase_wait_value( iidx, species, bunch );
    printf("Stored Wait Value   = %d\n", stored_ph_race_wait );
    printf("**********************************************\n");


  //----------------------------------------------------------------
  // Calculate results for each of the turns offsets available for
  // each state of the phase race condition 1/2-turn wait setting.
  //----------------------------------------------------------------
  int ph_off = 0;
  for (ph_race_wait = 0; ph_race_wait < CBPM_HALF_TURN_OPTIONS; ph_race_wait++) {

    // If not generating a full table, skip all entries that don't correspond to the 
    // actual race wait value employed.
    if (dp->cbpm_cmd_params.generate_phase_tables == FALSE) {
      if (stored_ph_race_wait != ph_race_wait ) {
	continue;
      }
    } else {
      fprintf(fp, "# ~1/2-turn delay %3s: -------------------|-------------------------------------", ph_race_wait_state[ph_race_wait] );
      fprintf(fp, "|--------------------|--------------------------------\n");
    }

    
    int turn_offset_radius = abs(CBPM_MIN_DP_TURNS_OFFSET);
    // 'ph_off' index is for table entry labels. ( ranges  -X -- 0 -- +X )
    for (ph_off = -turn_offset_radius; ph_off < CBPM_NUM_DP_TURNS_OFFSET - turn_offset_radius; ph_off++) {

      // If not generating a full table, skip all entries that don't correspond to the 
      // actual race wait value employed.
      if ((dp->cbpm_cmd_params.generate_phase_tables == FALSE) &&
	  (stored_offset != ph_off) ) {
	continue;
      }
      
      // 'offset' index is for getting data out of arrays. ( ranges 0 -- +2X )
      offset = ph_off - CBPM_MIN_DP_TURNS_OFFSET;


      //------------------------------------------------------------
      // Calculate the BUTTON betatron phase angle and amplitude 
      // for each plane.
      //------------------------------------------------------------
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
      
	hcos = dp->cbpm_beta_phase_out.phase_results[offset][ph_race_wait][CBPM_HORZ_DIM][CBPM_COS_COEFF][card];
	hsin = dp->cbpm_beta_phase_out.phase_results[offset][ph_race_wait][CBPM_HORZ_DIM][CBPM_SIN_COEFF][card];
	vcos = dp->cbpm_beta_phase_out.phase_results[offset][ph_race_wait][CBPM_VERT_DIM][CBPM_COS_COEFF][card];
	vsin = dp->cbpm_beta_phase_out.phase_results[offset][ph_race_wait][CBPM_VERT_DIM][CBPM_SIN_COEFF][card];
      

	hamp = sqrt(hcos*hcos + hsin*hsin);
	vamp = sqrt(vcos*vcos + vsin*vsin);
      
	hphase = rad2deg * atan2(hcos,hsin); // NOTE: Arguments were (hsin,hcos) prior to 18:30 26-Mar-2009
	vphase = rad2deg * atan2(vcos,vsin); //            "         (vsin,vcos)           "
                                             //
                                             //***********************************************************************
                                             // ! NOTE !: 
                                             // CESRV is the consumer of the database-published data values
                                             // for each button.  It uses a convention that expects the supplied data
                                             // to come from an INCORRECT (arguments reversed) atan2() call.
                                             // 
                                             //  Phase tables written to disk for both CBPM_I and CBPM_II systems
                                             //  contain data only valid for system comparisons and turns-offset
                                             //  determination, the actual angles are INVALID prior to processing by
                                             //  CESRV.
                                             //***********************************************************************

	// Save each for later recall   Phase angles are in degrees at this point.
	cp->phase_calcs.horz_amp[card]   = hamp;
	cp->phase_calcs.horz_phase[card] = hphase;
	cp->phase_calcs.vert_amp[card]   = vamp;
	cp->phase_calcs.vert_phase[card] = vphase;

      }
    

      //------------------------------------------------------------
      // Calculate POSITION betatron phase angle and amplitude 
      // for each plane
      // 
      // Database expects:                   BPM button order:
      //   0   1   2   3                       0   1   2   3
      //  BI  BO  TI  TO                      TI  BI  BO  TO
      //------------------------------------------------------------
      Xcos = (cp->phase_calcs.horz_amp[3] * cos(deg2rad * cp->phase_calcs.horz_phase[3])) + 
	      (cp->phase_calcs.horz_amp[2] * cos(deg2rad * cp->phase_calcs.horz_phase[2])) - 
	       (cp->phase_calcs.horz_amp[1] * cos(deg2rad * cp->phase_calcs.horz_phase[1])) - 
		(cp->phase_calcs.horz_amp[0] * cos(deg2rad * cp->phase_calcs.horz_phase[0]));
      Xsin = (cp->phase_calcs.horz_amp[3] * sin(deg2rad * cp->phase_calcs.horz_phase[3])) + 
	      (cp->phase_calcs.horz_amp[2] * sin(deg2rad * cp->phase_calcs.horz_phase[2])) - 
	       (cp->phase_calcs.horz_amp[1] * sin(deg2rad * cp->phase_calcs.horz_phase[1])) - 
		(cp->phase_calcs.horz_amp[0] * sin(deg2rad * cp->phase_calcs.horz_phase[0]));


      Ycos = (cp->phase_calcs.vert_amp[3] * cos(deg2rad * cp->phase_calcs.vert_phase[3])) + 
	      (cp->phase_calcs.vert_amp[0] * cos(deg2rad * cp->phase_calcs.vert_phase[0])) - 
	       (cp->phase_calcs.vert_amp[2] * cos(deg2rad * cp->phase_calcs.vert_phase[2])) - 
		(cp->phase_calcs.vert_amp[1] * cos(deg2rad * cp->phase_calcs.vert_phase[1]));
      Ysin = (cp->phase_calcs.vert_amp[3] * sin(deg2rad * cp->phase_calcs.vert_phase[3])) + 
	      (cp->phase_calcs.vert_amp[0] * sin(deg2rad * cp->phase_calcs.vert_phase[0])) - 
	       (cp->phase_calcs.vert_amp[2] * sin(deg2rad * cp->phase_calcs.vert_phase[2])) - 
		(cp->phase_calcs.vert_amp[1] * sin(deg2rad * cp->phase_calcs.vert_phase[1]));

      Xamp   = sqrt( (Xcos * Xcos) + (Xsin * Xsin) );
      Yamp   = sqrt( (Ycos * Ycos) + (Ysin * Ysin) );

      Xphase = rad2deg * atan2( Xcos , Xsin );
      Yphase = rad2deg * atan2( Ycos , Ysin );
                                             //***********************************************************************
                                             // ! NOTE !: 
                                             // CESRV is the consumer of the database-published data values
                                             // for each button.  It uses a convention that expects the supplied data
                                             // to come from an INCORRECT (arguments reversed) atan2() call.
                                             // 
                                             //  Phase tables written to disk for both CBPM_I and CBPM_II systems
                                             //  contain data only valid for system comparisons and turns-offset
                                             //  determination, the actual angles are INVALID prior to processing by
                                             //  CESRV.
                                             //***********************************************************************


      //-----------------------------------------------------------------
      //          Write the phase measurement tables to disk
      //-----------------------------------------------------------------

      if (dp->cbpm_cmd_params.generate_phase_tables == TRUE) {
	//                 ------------------------ HORIZONTAL ------------------------
	// HORIZONTAL Button Phase Angles
	fprintf(fp, "%3d  HORZ", ph_off);
	for (card = 0; card < CBPM_MAX_CARDS; card++) {
	  fprintf(fp, "% 8.2f", cp->phase_calcs.horz_phase[card] );
	}
	fprintf(fp, " |");
	// HORIZONTAL Button Amplitudes
	for (card = 0; card < CBPM_MAX_CARDS; card++) {
	  fprintf(fp, " % 8.2f", cp->phase_calcs.horz_amp[card] );
	}
	// HORIZONTAL Plane Angle and Amp.
	fprintf(fp, " | % 8.2f  % 8.2f", Xphase, Xamp);
	//-------- Diagonal difference output   (BO - TI)   and   (BI - TO)   -------------
	fprintf(fp, " | % 8.2f  % 8.2f", cp->phase_calcs.horz_phase[CBPM_BOT_OUTER] - cp->phase_calcs.horz_phase[CBPM_TOP_INNER],
		cp->phase_calcs.horz_phase[CBPM_BOT_INNER] - cp->phase_calcs.horz_phase[CBPM_TOP_OUTER] );
	// HORIZONTAL (Written Angle - Angle)
	phi = target_Xphase - Xphase;
	if (phi > 180.0) {
	  phi = phi - 360.0;
	} else if (phi < -180.0) {
	  phi = phi + 360.0;
	}
	fprintf(fp, " | % 8.2f\n", phi );
	
	
	
	//                ------------------------ VERTICAL --------------------------
	// VERTICAL Button Phase Angles
	fprintf(fp, "     VERT");
	for (card = 0; card < CBPM_MAX_CARDS; card++) {
	  fprintf(fp, "% 8.2f", cp->phase_calcs.vert_phase[card] );
	}
	fprintf(fp, " |");
	// VERTICAL Button Amplitudes
	for (card = 0; card < CBPM_MAX_CARDS; card++) {
	  fprintf(fp, " % 8.2f", cp->phase_calcs.vert_amp[card] );
	} 
	// VERTICAL Plane Angle and Amplitude
	fprintf(fp, " | % 8.2f  % 8.2f", Yphase, Yamp);
	//-------- Diagonal difference output   (BO - TI)   and   (BI - TO)   -------------
	fprintf(fp, " | % 8.2f  % 8.2f", cp->phase_calcs.vert_phase[CBPM_BOT_OUTER] - cp->phase_calcs.vert_phase[CBPM_TOP_INNER],
		cp->phase_calcs.vert_phase[CBPM_BOT_INNER] - cp->phase_calcs.vert_phase[CBPM_TOP_OUTER] );
	// VERTICAL (Written Angle - Angle)
	phi = target_Yphase - Yphase;
	if (phi > 180.0) {
	  phi = phi - 360.0;
	} else if (phi < -180.0) {
	  phi = phi + 360.0;
	}
	fprintf(fp, " | % 8.2f\n", phi );

	
      } //endIF generate_phase_tables

      //--------------------------------------------------------------------------
      // Publish the phase measurement results from the selected 
      // 'ph_race_wait' setting and
      // turns offset   setting              4 per button
      // CESR Control system database expects data in the order (BI, BO, TI, TO)
      //--------------------------------------------------------------------------
      // NOTE: Only supports writes to database for measurements in 14ns mode.
      //--------------------------------------------------------------------------
      if (stored_offset == ph_off &&
	  stored_ph_race_wait == ph_race_wait) {

	int idx = 0;
	for (card = 0; card < CBPM_MAX_CARDS; card++) {
	  button_id   = cesr_ctl_order[card];
	  data[idx++] = (int) (PHASE_MULT * cp->phase_calcs.horz_amp[button_id]   );
	  data[idx++] = (int) (PHASE_MULT * cp->phase_calcs.horz_phase[button_id] );
	  data[idx++] = (int) (PHASE_MULT * cp->phase_calcs.vert_amp[button_id]   );
	  data[idx++] = (int) (PHASE_MULT * cp->phase_calcs.vert_phase[button_id] );
	}
	idx = 0;
	// Store phase data for each button in the order expected in the NEW MPM locations.
	for (card = 0; card < CBPM_MAX_CARDS; card++) {
	  //button_id   = cesr_ctl_order[card];
	  newdata[idx++] = (int) (PHASE_MULT * cp->phase_calcs.horz_amp[card]   );
	  newdata[idx++] = (int) (PHASE_MULT * cp->phase_calcs.horz_phase[card] );
	  newdata[idx++] = (int) (PHASE_MULT * cp->phase_calcs.vert_amp[card]   );
	  newdata[idx++] = (int) (PHASE_MULT * cp->phase_calcs.vert_phase[card] );
	}



	// For each entry in the phase reporting table, check the amplitude and
	// phase angle values about to be published too see if they're too high
	// owing to some potential readback or calculation problem that has been
	// seen before causing some of these values to be nonsensically large..
	int button;
	int entry = 0;
	publish = TRUE;
	for (entry = 0; entry < PHASE_ENTRIES; entry++) {
	  
	  // Amplitude value  (Amplitude and phase value are stored interleaved.)
	  if (entry % 2 == 0) {
	    if (abs(data[entry]) > CBPM_MAX_PUBLISH_PH_AMP) {
	      printf("Amplitude value out of range! [=%d]\n", data[entry] );
	      publish = FALSE;
	      break;
	    }

	    // Phase angle value
	  } else {
	    if (abs(data[entry]) > CBPM_MAX_PUBLISH_PH_ANG) {
	      printf("Angle value out of range! [=%d]\n", data[entry] );
	      publish = FALSE;
	    }
	  }

	} //endFOR entry



	if (publish == FALSE) {

	  sprintf(message, "Not publishing phase values for this instrument");
	  log_message( S_ERROR, func_name, message );

	} else {
	  
	  status = vxputn_c(node, ele[0], ele[1], data);
	  if (status == MPM_SUCCESS) {
	    if (dp->cbpm_cmd_params.generate_phase_tables == TRUE) {
	      fprintf(fp, "#      --wrote entries for  turn offset = %d  to database     PHASE_MULT = %d --\n", ph_off, PHASE_MULT );
	    }
	    sprintf(message, "X,Y amplitude,phase written for turns offset (%d) and phase_race_wait (%d) to node %s %d-%d",
		    stored_offset,
		    stored_ph_race_wait,
		    node,
		    ele[0],
		    ele[1]);
	    log_message(S_INFO, func_name, message);
	  } else {
	    fprintf(fp, "#    -- ERROR publishing entries for turn offset = %d  to database\n", ph_off );
	    log_message(S_ERROR, func_name, "ERROR writing phase values to CESR database\n");
	    sprintf(message, "for %s (%s) / %s \n   to elements %3d through %3d in node %s\n\n",
		    CTL_System.p_Module[iidx]->comm.ethernet.hostname,
		    CTL_System.p_Module[iidx]->comm.ethernet.IP_address,
		    cp->detector.name,
		    ele[0],
		    ele[1],
		    node );
	    log_message(S_ERROR, func_name, message);
	  }
	  
	  //--------------------------------------------------------------------------------
	  // Write horizontal and vertical phase and amplitude values to NEW MPM locations.
	  //--------------------------------------------------------------------------------
	  ele[0] = ((cp->detector.detec_id_ele - 1) * CBPM_BUTPHASE_ENTRY_SIZE) + 1;
	  ele[1] = ele[0] + CBPM_BUTPHASE_ENTRY_SIZE - 1;
	  status = vxputn_c(CBPM_BUTPHASE_DATA_NODE, ele[0], ele[1], newdata);
	  if (status == MPM_SUCCESS) {
	    sprintf(message, "X,Y amplitude,phase written for turns offset (%d) and phase_race_wait (%d) to node %s %d-%d",
		    stored_offset,
		    stored_ph_race_wait,
		    CBPM_BUTPHASE_DATA_NODE,
		    ele[0],
		    ele[1]);
	    log_message(S_INFO, func_name, message);
	  } else {
	    fprintf(fp, "#    -- ATTEMPTED to publish entries for turn offset = %d  to database  -- encountered ERROR\n", ph_off );
	    log_message(S_ERROR, func_name, "ERROR writing phase values to CESR database\n");
	    sprintf(message, "for %s (%s) / %s \n   to elements %3d through %3d in node %s\n\n",
		    CTL_System.p_Module[iidx]->comm.ethernet.hostname,
		    CTL_System.p_Module[iidx]->comm.ethernet.IP_address,
		    cp->detector.name,
		    ele[0],
		    ele[1],
		    CBPM_BUTPHASE_DATA_NODE );
	    log_message(S_ERROR, func_name, message);
	  }
	  
	  
	  
	  //-----------------------------------------------
	  // Flag that entry as 'GOOD' in the PSTAT node 
	  // so CESRV will plot it by default.
	  //-----------------------------------------------
	  node = cp->detector.phase_stat_node;
	  ele[0] = ele[1] = cp->detector.phase_stat_ele;
	  int dataval = CBPM_GOOD_DATA;
	  status = vxputn_c(node, ele[0], ele[1], &dataval);
	  if (status != MPM_SUCCESS) {
	    sprintf(message, "ERROR writing 'DATA GOOD' flag to the PSTAT node\n for instrument %s (%s) / %s",
		    CTL_System.p_Module[iidx]->comm.ethernet.hostname,
		    CTL_System.p_Module[iidx]->comm.ethernet.IP_address,
		    cp->detector.name );
	    log_message(S_ERROR, func_name, message);
	  }
	  
	} //endIF publish

      } // endIF  ph_off and ph_race_wait checks
        
    } // endFOR  phase turns offset
    
  } // endFOR  race_switch toggle
  


  //------------------------------------------------------
  // Publish the processed (avaraged) button data for
  // this device/detector to its assigned MPM locations.
  //------------------------------------------------------
  node = cp->detector.button_data_node;

  ele[0] = 4 * (cp->detector.button_data_idx - 1) + 
                cp->detector.button_data_offset[species] + 1;
  if (debug_level > CBI_DEBUG_LEVEL_1) {
    printf("OFFSET : %d\n", cp->detector.button_data_offset[species]);
  }
  ele[1] = ele[0] + 3;

  printf("Average signal amplitudes: %f   %f   %f   %f\n",
	 dp->cbpm_beta_phase_out.signal[0],
	 dp->cbpm_beta_phase_out.signal[1],
	 dp->cbpm_beta_phase_out.signal[2],
	 dp->cbpm_beta_phase_out.signal[3]  );

  printf("-- Writing BUTTON data for %s (%s) / %s \n   to elements %3d through %3d in node %s \n",
	 CTL_System.p_Module[iidx]->core->cbi_ident.hostname,
	 CTL_System.p_Module[iidx]->core->cbi_ident.ipaddr,
	 cp->detector.name,
	 ele[0],
	 ele[1],
	 node );
  
 
  //
  // The CESR control system database expects the button data in the 
  // order (BI, BO, TI, TO), arrange it as such.
  // 

  // FIXME: No button orientation mapping applied yet!
  //for (card = 0; card < CBPM_MAX_CARDS; card++) {
  //  but_data[card] = (int)
  //    ( 100 * fabs((double)(dp->cbpm_beta_phase_out.signal[1])) );
  //}

  // BPM front panel button mapping TO, BO, BI, TI
  // Correct mapping for database writes is  BI, BO, TI, TO
  // Multiply by 100 to get integer version of signal amplitudes 
  //  with 2 decimal places preserved.
  but_data[0] = (int)( 100 * fabs((double)(dp->cbpm_beta_phase_out.signal[1])) );
  but_data[1] = (int)( 100 * fabs((double)(dp->cbpm_beta_phase_out.signal[2])) );
  but_data[2] = (int)( 100 * fabs((double)(dp->cbpm_beta_phase_out.signal[0])) );
  but_data[3] = (int)( 100 * fabs((double)(dp->cbpm_beta_phase_out.signal[3])) );


  but_data_new[0] = (int)( 100 * fabs((double)(dp->cbpm_beta_phase_out.signal[0])) );
  but_data_new[1] = (int)( 100 * fabs((double)(dp->cbpm_beta_phase_out.signal[1])) );
  but_data_new[2] = (int)( 100 * fabs((double)(dp->cbpm_beta_phase_out.signal[2])) );
  but_data_new[3] = (int)( 100 * fabs((double)(dp->cbpm_beta_phase_out.signal[3])) );

  //------------------------------------------------
  // Publish button values to the CESR MPM
  //------------------------------------------------
  // Perform checks on button data about to be published
  int button;
  publish = TRUE;
  for (button = 0; button < CBPM_MAX_CARDS; button++) {
    if (abs(but_data_new[button]) > CBPM_MAX_PUBLISH_BUTTON) {
      sprintf(message, "Button-%d value out of range! [=%d]", button, but_data_new[button] );
      log_message( S_ERROR, func_name, message );
      publish = FALSE;
    }
  }

  if (publish == FALSE) {
    sprintf(message, "Not publishing button values for this instrument");
    log_message( S_ERROR, func_name, message );
  } else {
    status = vxputn_c(node, ele[0], ele[1], but_data);
    if (status != MPM_SUCCESS) {
      sprintf(message, "ERROR writing buttons data!");
      log_message(S_ERROR, func_name, message);
    }

    ele[0] = ((cp->detector.detec_id_ele - 1) * CBPM_ORBIT_ENTRY_SIZE) + 1;
    ele[1] = ele[0] + 3;
    printf("-- Writing BUTTON data for %s (%s) / %s \n   to elements %3d through %3d in node %s \n\n",
	   CTL_System.p_Module[iidx]->core->cbi_ident.hostname,
	   CTL_System.p_Module[iidx]->core->cbi_ident.ipaddr,
	   cp->detector.name,
	   ele[0],
	   ele[1],
	   CBPM_ORBIT_DATA_NODE );
    
    //-------------------------------------------------------
    // Publish button values to the CESR MPM - NEW LOCATIONS
    //-------------------------------------------------------
    status = vxputn_c(CBPM_ORBIT_DATA_NODE, ele[0], ele[1], but_data_new);
    if (status != MPM_SUCCESS) {
      sprintf(message, "ERROR writing buttons data!");
      log_message(S_ERROR, func_name, message);
    }

  } //endIF publish

  
  return CBI_F_SUCCESS;

}
