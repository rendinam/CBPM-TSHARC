/*!-----------------------------------------------------------------------+
* File         :  cbpm_defs.c                                             |
*                                                                         |
* Description  :  Variable definitions that will be 'extern'ed in a header|
*                 file elsewhere, but for compatibility cannot themselves |
*                 be contained in a header file.                          |
*                                                                         | 
* Author       :  M. Rendina                                              |
*-------------------------------------------------------------------------+*/

#define CBPM_NO_EXTERN 
#include "cbpm_includes.h"
#include "cbpm_trace.h"

char tmode_names[CBI_MAX_TIMING_MODES][8] = {
  "4ns",
  "14ns"
};

char tsetup_names[CBPM_MAX_TIMING_SETUPS][CBI_MAX_TSETUP_NAME_LENGTH] = {
  "4ns_P_VAR_G",
  "4ns_E_VAR_G",
  "14ns_VAR_G",
  "4ns_P_FIX_G",
  "4ns_E_FIX_G",
  "14ns_FIX_G"
};


char cbpm_cmd_names[CBPM_MAX_COMMANDS+CBI_FIRST_PLATFORM_COMMAND][28] = {
  "CBI_INVAILD_CMD_0",
  "CBI_IDLE_CMD",
  "ACQUIRE_ALL_PEDESTALS_CMD",
  "PEDESTAL_CAL_CMD",
  "SET_GAINS_CMD",
  "SET_DELAYS_CMD",
  "RAW_DATA_CMD",
  "PROC_DATA_CMD",
  "CONT_DATA_CMD",
  "BETATRON_PHASE_CMD",
  "INJECTION_DATA_CMD",
  "TEST_RAW_DATA_CMD",
  "FIND_BUNCH_CMD",
  "FIND_BTMD_CMD",
  "TIME_SCAN_CMD",
  "PUBLISH_TEMPS_CMD",
  "SET_BUN_PAT_OFFSETS_CMD",
  "SET_TIMING_MODE_CMD",
  "CURRENT_DATA_CMD",
  "CONTINUOUS_PROC_CMD",
  "SET_TIMING_SETUP_CMD"
};


//This list must have a 1-1 correspondence with the CBPM_ROUTINE_IDS list in ..\include\cbpm_constants.h
char cbpm_routine_names[NUM_ROUTINE_IDS][CBI_MAX_ROUTINE_NAME_LENGTH] = {
  "",
  "acquire_all_pedestals",
  "add_debug",
  "auto_gain",
  "backup_matrix",
  "bunch_pattern_reduce",
  "bunch_pattern_reduce_4nS",   
  "bunch_pattern_post",
  "cal_loc_offset",       
  "cbpm_get_errors",
  "cbpm_set_error",
  "chan_delay_cal",
  "check_raw_buf",
  "check_validity",  
  "clear_debug",  
  "coeff_matrix",         
  "collect",              
  "collect_adc",
  "collect_proc",
  "collect_raw",
  "determine_stat",
  "decode_stat",  
  "dispatch",             
  "do_block_scan",
  "do_calibration",       
  "dsp_sleep_turns",
  "encode_stat",  
  "find_bTMD",
  "find_bunch",
  "find_delays",          
  "float_err_check",
  "float_err_clear",
  "get_betatron_phase",   
  "get_bits",  
  "get_continuous_data",
  "get_proc_data",
  "get_raw_data",
  "init",                 
  "iterate",              
  "iterate_find_delays",
  "load_bunch_pattern",
  "lu_decomp",
  "lu_save_sol",
  "lu_solve",             
  "main",                 
  "mat_mult",
  "measure_adc_rms",
  "normal_betatron",
  "pedestal_cal",
  "perform_test",
  "perform_test_2input",
  "process_adc",          
  "process_data",         
  "publish_temps",
  "rotate_bunch_pattern",
  "set_bits",  
  "set_delays",
  "set_gains",            
  "set_timing_mode",
  "set_timing_setup",      
  "test_raw_data",
  "time_scan",
  "ts_set_buffer_control",
  "tsx_chan_stat",        
  "update_sol",           
};

//This list must have a 1-1 correspondence with the ERROR_ID list in ..\include\cbpm_trace.h
//0        1         2         3         4         5
//12345678901234567890123456789012345678901234567890
char cbpm_error_names[NUM_ERROR_IDS][CBI_MAX_ROUTINE_ERROR_LENGTH] = {
  "Internal use: Error Clear command", 
  "Internal use: Indicator for: No Error",				
  "acquire_all_pedestals: pedestal_cal error",		// - ACQ_ALL_PED_PED_CAL_ERR",
  "acquire_all_pedestals: set_timing_setup error",	// - ACQ_ALL_PED_SET_TIMING_SETUP_ERR",
  "auto_gain: collect_proc error",					// - AUTO_GAIN_COLLECT_PROC_ERR",
  "auto_gain: converge error",						// - AUTO_GAIN_CONVERGE_ERR",					//
  "collect_proc: collect_raw error",				// - COLLECT_PROC_CR_ERR",
  "collect_proc: process_data error",				// - COLLECT_PROC_PD_ERR",
  "collect_raw: collect_adc error",					// - COLLECT_RAW_ERR",        
  "determine_status: error",                        // - DETERMINE_STATUS_ERR,
  "do_calibration: find_delay call 1 error",		// - DO_CAL_AUTO_FIND_DELAY1",
  "do_calibration: find_delay call 2 error",		// - DO_CAL_AUTO_FIND_DELAY2",					//
  "do_calibration: auto_gain call 1 error",			// - DO_CAL_AUTO_GAIN_ERR1",
  "do_calibration: auto_gain call 2 error",			// - DO_CAL_AUTO_GAIN_ERR2",
  "do_calibration error",							// - DO_CAL_ERR",
  "find_bunch: collect_adc error",					// - FB_COLLECT_ADC_ERR",						
  "find_bunch: unknown timing mode in timing_mode_G",//- FB_UNKNOWN_TIMING_MODE_ERR",				//
  "find_bTMD: find_bunch call 1 error",				// - FIND_BTMD_FIND_BUNCH_ERR1",
  "find_bTMD: find_bunch call 2 error",				// - FIND_BTMD_FIND_BUNCH_ERR2",
  "find_delays: block scan error",					// - FIND_DELAYS_BLOCK_SCAN_ERR
  "find_delays: collect_adc call 1 error",			// - FIND_DELAYS_COLLECT_ADC_ERR1",
  "find_delays: collect_adc call 2 error",			// - FIND_DELAYS_COLLECT_ADC_ERR2",
  "find_delays: collect_raw call 1 error",			// - FIND_DELAYS_COLLECT_RAW_ERR1",				//
  "find_delays: collect_raw call 2 error",			// - FIND_DELAYS_COLLECT_RAW_ERR2",
  "find_delays: process_data call 1 error",			// - FIND_DELAYS_PROCESS_DATA_ERR1,  
  "find_delays: collect_raw:Measure_ADC_RMS error",	// - FIND_DELAYS_COLLECT_RAW_RMS_ERR,  
  "find_delays: process_data:Measure_ADC_RMS error",// - FIND_DELAYS_PROCESS_DATA_RMS_ERR,
  "get_betatron_phase_A: collect_raw error",        // - GET_BETA_PHASE_DATA_ERR",
  "get_betatron_phase_A: NormalBetatron error",     // - GET_BETA_PHASE_ERR",
  "get_continuous_data: auto_gain error",           // - GET_CONT_AUTO_GAIN_ERR",       
  "get_proc_data: collect_proc error",				// - GET_PROC_DATA_ERR",           				//
  "get_proc_data: do_calibration error",		    // - GET_PROC_DATA_CAL_ERR",
  "get_raw_data: do_calibration error",				// - GET_RAW_DATA_CAL_ERR",       
  "get_raw_data: collect_raw error",				// - GET_RAW_DATA_DAT_ERR",    
  "get_raw_data: process_data error",				// - GET_RAW_DATA_PROC_ERR",
  "Warning: loop broken by Idle Command",   // - IDLE_COMMAND_LOOP_BREAK,
  "float_err_check: ALU floating point error",		// - INST_MATH_ERR",							//
  "pedestal_cal: collect_raw error",				// - PED_CAL_COLLECT_RAW_ERR",					
  "pedestal_cal: process_data error",				// - PED_CAL_PROC_DATA_ERR",
  "perform_test: errors",							// - PERFORM_TEST_ERROR
  "perform_test2: errors",							// - PERFORM_TEST2_ERROR
  "process_adc: unknown calc_avg_mode",				// - PROCESS_ADC_AVG_ERR",
  //"process_adc error",								// - PROCESS_ADC_ERR",             
  "process_data: process_adc error",				// - PROCESS_DATA_ERR",            				//
  "set_timing_setup: unknown timing mode",			// - STS_UNKNOWN_TIMING_MODE_ERR",				
  "time_scan: do_calibration call 1 error",			// - TIME_SCAN_DO_CAL_ERR1",
  "time_scan: do_calibration call 2 error",			// - TIME_SCAN_DO_CAL_ERR2",
  };


char cbpm_data_corrections[CBPM_MAX_CORRECTION_MODES][30] = {
  "NO CORRECTION (RAW ADC)",
  "PEDESTALS SUBTRACTED",
  "GAIN SCALED",
  "PED SUB & SCALED"
};


char cbpm_trigger_setup_names[5][10] = {
  "NONE",
  "A no enab",
  "A+enable",
  "B no enab",
  "B+enable"
};


char cbpm_avg_mode_names[3][20] = {
  "NONE",
  "SIMPLE AVG",
  "JITTER SUPPRESSION"
};
