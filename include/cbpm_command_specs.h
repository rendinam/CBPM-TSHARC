#ifndef CBPM_COMMAND_SPECS_H
#define CBPM_COMMAND_SPECS_H
//-----------------------------------------------------------------------+
// File         :  cbi_PT_command_specs.h                                |
//                                                                       |
// Description  :  Defines the collection of all actions that can be     |
//                 requested of a control apllication/server session     |
//                 configured for TigerSHARC BPM Series-3 operation.     |
//-----------------------------------------------------------------------+

#include "cbpm_protos.h"


//-----------------------------------------------------------------------+
//  The following provides for C++ compatibility (ie, C++ routines can   |
//  explicitly use this include file).                                   |
//-----------------------------------------------------------------------+
#if defined (__cplusplus)
extern "C" {
#endif



#if defined (CBPM_NO_EXTERN)


CBI_COMMAND cbpm_commands[] = {

  // Find block Turns Marker Delay
  { 
    "FIND bTMD",                                        // (*name)          Name string
    "Determine best block turns marker delay settings", // (*helptext)      Help text string
    FIND_BTMD_CMD,                                      // (command)        DSP Command ID, if applicable
    NOT_AVAILABLE_VIA_INTERFACE,                        // (flag)           Flag for invocation by interface
    SINGLE_TYPE,                                        // (type)           Command type (single or sequence)
    &(cbpm_find_bTMD_prep),                             // (*prep_func)     Function pointer to '_prep()'
    &(cbpm_find_bTMD_post),                             // (*post_func)     Function pointer to '_post()'
    NULL,                                               // (*final_func)    Function pointer to follow-up routine.
    FALSE,                                              // (parallel_post)  Perform all post calls simultaneously.
    0,                                                  // (menu_mask)
    0,                                                  // (menuID)         Menu ID #
    0                                                   // (group_mask)     Group mask
  },


  // Find Bunch
  { 
    "FIND BUNCH",
    "Determine which bunch bucket in the machine contains charge\n  (Single bunch only)",
    FIND_BUNCH_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_find_bunch_prep),
    &(cbpm_find_bunch_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Get stat and debug structs
  { 
    "GET STATUS AND DEBUG",
    "Read back the status and debug structures from each insturment",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    0,
    NULL,
    &(cbi_get_debug_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Test raw data
  {
    "TEST DATA BUFFERS",
    "Test command to verify buffer filling and other things",
    TEST_RAW_DATA_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_test_data_prep),
    &(cbpm_test_data_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Raw data
  { 
    "GET TURN-BY-TURN DATA",
    "Collect turn-by-turn data",
    RAW_DATA_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_raw_data_prep),
    &(cbpm_raw_data_post),
    &(cbpm_raw_data_final),
    TRUE,
    0,
    0,
    0
  },


  // Processed data
  { 
    "GET PROCESSED DATA",
    "Collect data averaged over some number of turns",
    PROC_DATA_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_proc_data_prep),
    &(cbpm_proc_data_post),
    &(cbpm_proc_data_final), //&(cbpm_print_turn_count_summary),
    FALSE,
    0,
    0,
    0
  },

  // 4ns bunch current monitor
  { 
    "MEASURE 4ns BUNCH CURRENTS",
    "Collect data averaged over some number of turns, calculate sum of buttons, publish in current nodes.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_proc_data_currmon_prep),
    &(cbpm_proc_data_currmon_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },

  // Calculate the COMMON turns-marker delay
  // from an initial bunch pattern offset provided
  // after a successful find_bTMD or find_bunch command.
  { 
    "FIND cTMD",
    "Calculate the COMMON turns-marker delay setting.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    NULL,
    &(cbpm_find_cTMD_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Betatron phase
  { 
    "GET BETATRON PHASE",
    "Collect betatron phase data",
    BETATRON_PHASE_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_beta_phase_prep),
    &(cbpm_beta_phase_post),
    &(cbpm_beta_phase_final), ////NULL,
    TRUE,
    0,
    0,
    0
  },


  // Save timing values to disk
  { 
    "SAVE TIMING PARAMETERS",
    "Save timing values to disk",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_save_timings_prep),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Save pedestal values to disk
  { 
    "SAVE PEDESTAL PARAMETERS",
    "Save pedestal values to disk",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_save_pedestals),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Time scan
  { 
    "TIME SCAN",
    "Perform a global delay scan",
    TIME_SCAN_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_time_scan_prep),
    &(cbpm_time_scan_post),
    &(cbpm_time_scan_final),
    FALSE,
    0,
    0,
    0
  },


  // Set gains
  { 
    "SET GAINS",
    "Set all gain values on board instrument",
    SET_GAINS_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_set_gains_prep),
    &(cbpm_set_gains_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Get temperatures
  { 
    "GET TEMPERATURES",
    "Read back all temperature sensors data",
    CBI_IDLE_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    NULL,
    &(cbpm_get_temps_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Reread Parameter Files
  {
    "REREAD PARAMETER FILES",
    "Reload the detector and instrument parameters files.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_reread_params),
    &(cbpm_push_full_config_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Toggle active instruments
  { 
    "SELECT ACTIVE INSTRUMENTS",
    "Toggle the activity state of any online instruments",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbi_toggle_active_instruments),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Idle
  { 
    "IDLE COMMAND",
    "Send the 'IDLE COMMAND' to the instruments.",
    CBI_IDLE_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    NULL,
    &(cbi_idle_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Enter server mode
  { 
    "ENTER SERVER MODE",
    "Accept requests via the MPM interface array",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(enter_server_mode),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Dump contents of MPM interface array
  { 
    "DUMP MPM INTERFACE STRUCTURE",
    "Dump the contents of the MPM interface array to the screen",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbi_dump_interface),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Acquire pedestal values
  { 
    "ACQUIRE PEDESTALS",
    "Acquire pedestal values on all cards, channels, and gain settings for the active timing setup.",
    ACQUIRE_ALL_PEDESTALS_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_acquire_pedestals_prep),
    &(cbpm_acquire_pedestals_post),
    NULL,
    TRUE, //FALSE,
    0,
    0,
    0
  },


  // DUMP Raw data
  { 
    "DUMP RAW DATA",
    "Dump to disk the most recent turn-by-turn data without new acquisition.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    NULL,
    &(cbpm_raw_data_post),
    &(cbpm_write_all_raw_data),
    FALSE,
    0,
    0,
    0
  },


  // Check heartbeat status
  { 
    "CHECK PULSE",
    "Read and verify the DSP heartbeat and timing integrity signals.",
    (int)NULL,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    NULL,
    &(cbpm_check_pulse_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Set the timing setup for each instrument
  { 
    "SET TIMING SETUP",
    "Configure the timing setup for each ONLINE instrument.",
    SET_TIMING_SETUP_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_set_timing_setup_prep),
    &(cbpm_set_timing_setup_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Install updated parameter values
  { 
    "INSTALL UPDATED PARAMETERS",
    "Copy the latest parameter set with outstanding changes in this session to the master operational parameters file.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_install_params),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Reset Coldfire ethernet communications module
  { 
    "RESET COLDFIRE",
    "Reset the Coldfire communications board",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    NULL,
    &(reset_coldfire),
    &(reset_coldfire_final),
    FALSE,
    0,
    0,
    0
  },


  // Reprogram instrument's flash RAM
  { 
    "PROGRAM FLASH",
    "Reload an instrument's on-board software",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbi_program_flash_prep),
    &(cbi_program_flash_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Reset instrument, push configuration parameters, and
  // set state (gain and timing setup) to that in place
  // before the reset 
  { 
    "RESET AND INIT",
    "Reset and reinitialize instruments",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SEQUENCE_TYPE,
    &(cbpm_reset_and_init),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Cycle power, reset instrument, push configuration parameters,
  // and set state (gain and timing setup) to that in place before
  // the reset 
  { 
    "POWER CYCLE - RESET AND INIT",
    "Cycle power before performing reset and init sequence.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SEQUENCE_TYPE,
    &(cbpm_power_cycle_init),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Time-in Procedure (multiple-instrument capable)
  { 
    "TIME-IN",
    "Perform a mostly-automated initial timing-in procedure on the instruments.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SEQUENCE_TYPE,
    &(cbpm_time_in),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


/*   // Beam-based calibration process */
/*   {  */
/*     "CALIBRATE ELECTRONIC GAINS", */
/*     "Perform a beam-based gain calibration procedure.", */
/*     CBI_INVALID_CMD_0, */
/*     NOT_AVAILABLE_VIA_INTERFACE, */
/*     SEQUENCE_TYPE, */
/*     &(cbpm_calibrate_gains), */
/*     NULL, */
/*     NULL, */
/*     FALSE, */
/*     0, */
/*     0, */
/*     0 */
/*   }, */


  // Install (via-overwrite) gain scale factors obtained
  // through NTR external analysis program.
  // Provide new tables in a complete intermediate config file
  // that can be examined before installing.
  { 
    "INSTALL ELECTRONIC GAIN CORRECTIONS",
    "Overwrite all relevant gain tables from scale factor file storing in intermediate config file.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_install_gain_tables),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },

  // Apply (incorporate into existing by mulitiplication) the DLR-provided
  // gain mapping correction to the existing gain correction tables for all gain
  // settings.  Apply changes to intermediate config file that can be examined
  // before installing.
  { 
    "APPLY GAIN MAPPING CORRECTIONS",
    "Multiply each existing gain scale factor by the provided gain mapping correction values.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_apply_gain_mappings),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Change gain section of configuration parameters file
  { 
    "COMPOSE BUNCH PATTERN",
    "Compose a new bunch pattern graphically.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbi_prompt_pattern_mode),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Clear Debug Struct
  { 
    "Clear Debug Struct",
    "Clear Debug index and array",
    CLEAR_DEBUG_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    NULL,
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },   


  // Change gain section of configuration parameters file
  { 
    "SCRUB TIMING PARAMS",
    "Scrub all block-dependent timing params to an uninitialized state.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_scrub_timing_params_prep),
    &(cbpm_scrub_timing_params_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Change gain section of configuration parameters file
  { 
    "COPY TIMING PARAMS A-->B",
    "Copies all timing params from timing block A to block B for calibration work.",
    SET_TIMING_SETUP_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_copy_timing_params_prep),
    &(cbpm_set_timing_setup_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },

/*   {  */
/*     "TRIPLET TIME SHIFT", */
/*     "Runs a triplet time shift test", */
/*     CBI_INVALID_CMD_0, */
/*     NOT_AVAILABLE_VIA_INTERFACE, */
/*     SEQUENCE_TYPE, */
/*     &(cbpm_triplet_time_shift), */
/*     NULL, */
/*     NULL, */
/*     FALSE, */
/*     0, */
/*     0, */
/*     0 */
/*   }, */

  // Diagnostic Dump of Instrument
  {
    "Diagnostic Dump",
    "Dump full diagnostic information for all active instruments to a file.",
    SET_TIMING_SETUP_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_diagnostic_dump_prep),
    &(cbpm_diagnostic_dump_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },

  //----------------------------------------------------------------------------

  // Exit application
  { 
    "EXIT",
    "Exit this application",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbi_exit_app),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Set zero-crossing sample point for Fixed-Gain operation
  {
    "Enable FG Zero-Crossing Sample Point",
    "Change all active instrument sample points to 0.4ns later for FixedGain zero-crossing measurement.",
    SET_TIMING_SETUP_CMD,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_set_zero_crossing_prep),
    &(cbpm_set_timing_setup_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Prompt for a bunch pattern and acquire a multibunch turn-by-turn data set.
  { 
    "Install Turn-By-Turn Bunch Pattern",
    "Prompt for a bunch pattern and acquire a multibunch turn-by-turn data set.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_install_TBT_pattern),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },
  

  // Test Life
  { 
    "Test Life",
    "Command test running till death or error",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_test_life_prep),
    &(cbpm_test_life_post),
    &(cbpm_test_life_final),
    FALSE,
    0,
    0,
    0
  },   

  
  // Calibrate 4ns bunch current monitor
  { 
    "Calibrate 4ns BUNCH CURRENTS",
    "Calibrate the Current monitor at different gain settings",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    SINGLE_TYPE,
    &(cbpm_proc_data_currmon_prep),
    &(cbpm_proc_data_cal_currmon_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },

  //----------------------------------------------------------------------------
  //  'Hidden' commands for CBPM MPM interface dispatching and other purposes
  //             These do not show up in user menus by default.
  //  Once menu mask mechanism is in place, HIDDEN_TYPE flag can be removed.
  //
  //  Each 'CBPM_xxx_FLAG' value may have a corresponding command defined here.
  //----------------------------------------------------------------------------

  // Reset entire instrument via DSP
  { 
    "RESET INSTRUMENT",
    "Reset the DSP and the Coldfire communications module",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    HIDDEN_TYPE,
    NULL,
    &(reset_instrument),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Reset entire instrument via DSP
  { 
    "POWER CYCLE",
    "Cycle power to an instrument, nothing more.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    HIDDEN_TYPE,
    &(cbi_power_cycle_prep),
    &(cbi_power_cycle_post),
    &(cbi_power_cycle_final),
    FALSE,
    0,
    0,
    0
  },


  // Orbit - Special case of Processed data
  { 
    "GET ORBIT VIA MPM",
    "Collect data averaged over some number of turns, dispatched from MPM interface.",
    PROC_DATA_CMD,
    CBPM_ORBIT_FLAG,
    HIDDEN_TYPE,
    &(cbpm_proc_data_prep_from_iface),
    &(cbpm_proc_data_post),
    &(cbpm_proc_data_final), // Block publishing routine here to take advantage of new MPM nodes
    TRUE,  //Make false for orbit stability on triplet.  All data goes to one file that way.
    0,
    0,
    0
  },

  // Betatron Phase Measurement
  { 
    "GET BETATRON PHASE VIA MPM",
    "Collect betatron phase data, dispatched from MPM interface.",
    BETATRON_PHASE_CMD,
    CBPM_PHASE_FLAG,
    HIDDEN_TYPE,
    &(cbpm_beta_phase_prep_from_iface),
    &(cbpm_beta_phase_post),
    &(cbpm_beta_phase_final),
    TRUE,
    0,
    0,
    0
  },


  // Turn-by-turn (Raw) data acquisition
  // Using SHAKER_DATA flag temporarily for compatibility with legacy CBPM-I code.
  { 
    "GET RAW VIA MPM",
    "Collect turn-by-turn (raw) data, dispatched from MPM interface.",
    RAW_DATA_CMD,
    CBPM_SHAKER_DATA_FLAG,
    HIDDEN_TYPE,
    &(cbpm_raw_data_prep_from_iface),
    &(cbpm_raw_data_post),
    &(cbpm_write_all_raw_data),
    TRUE, //////FALSE,  // File index race issue with concurrent standalone servers!
    0,
    0,
    0
  },


  // Pedestal acquisition
  { 
    "ACQUIRE PEDESTALS VIA MPM",
    "Acquire all pedestal table contents, dispatched from MPM interface.",
    ACQUIRE_ALL_PEDESTALS_CMD,
    CBPM_PEDESTALS_FLAG,
    HIDDEN_TYPE,
    &(cbpm_acquire_pedestals_prep),
    &(cbpm_acquire_pedestals_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Timing adjustment in server mode
  { 
    "TIME SCAN - AUTO TIME",
    "Perform a time scan, display channel delay adjustment summary.  Dispatched from MPM interface.",
    TIME_SCAN_CMD,
    CBPM_AUTO_TIME_FLAG,
    HIDDEN_TYPE,
    &(cbpm_time_scan_prep_from_iface),
    &(cbpm_time_scan_post),
    &(cbpm_time_scan_summary),
    FALSE,
    0,
    0,
    0
  },


  // Push gain config structure
  // Temporary until reorganized/optimized structure scheme can 
  // be fully implemented.  TODO: Remove in cleanup pass.
  { 
    "PUSH FULL CONFIG",
    "Simply transmits the gain config structure to the remote device to allow for gain setting after a recovery.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    HIDDEN_TYPE,
    NULL,
    &(cbpm_push_full_config_post),
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Switch between IDEAL gain scale factors and calculated factors
  { 
    "SET GAIN SCALE TYPE",
    "Toggles between calculated and IDEAL gain scale factors.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    HIDDEN_TYPE,
    &(cbpm_gain_scale_type_prep),
    NULL,
    NULL,
    FALSE,
    0,
    0,
    0
  },


  // Change gain section of configuration parameters file
  { 
    "WRITE GAIN SECTIONS",
    "Write a new set of gain codes and scale factors to the tables in the configuration file.",
    CBI_INVALID_CMD_0,
    NOT_AVAILABLE_VIA_INTERFACE,
    HIDDEN_TYPE,
    NULL,
    &(cbpm_write_gain_sections),
    NULL,
    FALSE,
    0,
    0,
    0
  },


};

  //---------------------------------------------------------------------------
  // The total number of supported commands for this instrumentation platform.
  //---------------------------------------------------------------------------
  int cbpm_num_commands = sizeof(cbpm_commands)/sizeof(CBI_COMMAND);


#else

  extern CBI_COMMAND cbpm_commands[];
  
  extern int cbpm_num_commands;

#endif
      
    




#if defined (__cplusplus)
}
#endif


#endif // CBPM_COMMAND_SPECS_H

