#ifndef CBPM_TAGS_A_H 
#define CBPM_TAGS_A_H 

//------------------------------------------------------------------------
//  D O     N O T     E D I T     T H I S     F I L E
//------------------------------------------------------------------------
// File         :  cbpm_tags_a.h 
// Date Created :  Fri Sep 14 14:08:01 2012
//
// Description  :  This file was automatically generated by the BIParser.
//                 It defines a unique tag ID # for each of the
//                 communication data structures employed for the given
//                 platform(s).
//                 This tag ID # is what is used as an index into the 
//                 instrument's on-board packet size/packet address tables
//                 for FPGA-mediated structure addressing.  It is also
//                 used to allow the user, within control system code, to 
//                 indicate which structure should be transferred during a
//                 cbi_gp_struct() READ/WRITE operation.
//------------------------------------------------------------------------

#include "cbi_core_inst_includes.h" 
#include "cbpm_types_a.h" 
#include "cbi_types_a.h" 
#include "cbi_tags_a.h" 


//================================================================
enum CBPM_TAG_ENUM {
  CBPM_CMD_PARAMS_TAG = CBI_FIRST_LOCAL_TAG,
  CBPM_TEMPERATURES_TAG,
  CBPM_TIMING_CONFIG_TAG,
  CBPM_GAIN_CONFIG_TAG,
  CBPM_PEDESTAL_CONFIG_TAG,
  CBPM_OP_TIMING_TAG,
  CBPM_OP_GAIN_TAG,
  CBPM_RAW_DATA_HEADER_TAG,
  CBPM_ADC_BUFFER0_TAG,
  CBPM_ADC_BUFFER1_TAG,
  CBPM_ADC_BUFFER2_TAG,
  CBPM_ADC_BUFFER3_TAG,
  CBPM_ADC_BUFFER4_TAG,
  CBPM_ADC_BUFFER5_TAG,
  CBPM_ADC_BUFFER6_TAG,
  CBPM_ADC_BUFFER7_TAG,
  CBPM_PH_WORD_BUF_TAG,
  CBPM_PROC_BUF_TAG,
  CBPM_PROC_BUF_HEADER_TAG,
  CBPM_PROC_SUM_DAT_BUF_TAG,
  CBPM_PROC_BUF_IO_TAG,
  CBPM_SUMM_BUF_TAG,
  CBPM_SIGNAL_STAT_TAG,
  CBPM_PHASE_CONFIG_TAG,
  CBPM_BETA_PHASE_OUT_TAG,
  CBPM_FIND_DELAY_OUT_TAG,
  CBPM_BFIND_DELAY_OUT_TAG,
  CBPM_FIND_DELAY_PARAMS_TAG,
  CBPM_CHAN_CAL_WORK_TAG,
  CBPM_PED_CAL_OUT_TAG,
  CBPM_RAW_PARAMS_TAG,
  CBPM_PROC_PARAMS_TAG,
  CBPM_CAL_PARAMS_TAG,
  CBPM_DIAGNOSTIC_TAG,
  CBPM_CODE_PARAMS_TAG,
  CBPM_TEST_RESULTS_TAG,
  CBPM_BTEST_RESULTS_TAG,
  FINAL_INVALID_CBPM_TAG          
};

//------------------------------------------------------
// Define the total number of platform-specific
// communication structures
//------------------------------------------------------
#define TOT_CBPM_COMM_STRUCTS          37



#endif
