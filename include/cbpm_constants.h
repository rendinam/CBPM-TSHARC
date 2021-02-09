#ifndef CBPM_CONSTANTS_H
#define CBPM_CONSTANTS_H


#include "cbi_constants.h"
#include "cbpm_io_constants.h"
#include "cbi_types_a.h"


//-----------------------------------------------------------------------+
// File         :  cbpm_constants.h      [TigerSHARC-based BPM]          |
//                                                                       |
// Description  :  This header file defines a set of Fast BPM            |
//                 system configuration parameters for SHARC-based       |
//                 instrumentation units.                                |
//                                                                       |
// Author       :  M. Palmer / C. Strohman / M. Rendina / M. Stedinger   |
//-----------------------------------------------------------------------+

//-----------------------------------------------------------------------+
//  The following provides for C++ compatibility (ie, C++ routines can   |
//  explicitly use this include file).                                   |
//-----------------------------------------------------------------------+
#if defined (__cplusplus)
extern "C" {
#endif


//-------------------------------------------
// TigerSHARC TS101 processor parameters
//
// Multiplier setting converts system clock 
// into DSP core clock frequency 
//        (LCLKRAT) = 6  (1 1 0)
//-------------------------------------------
#define CBPM_SYSTEM_CLOCK_FREQUENCY    50000000    // Hz
#define CBPM_CORE_CLOCK_FREQUENCY     300000000    // Hz


//----------------------------------------
// Clock integrity check parameters
//----------------------------------------
#define CBPM_CLOCK_SAMPLE_DURATION    0.1          // In seconds


//===========================================================+
// 2nd-generation Timing Board Parameters                    |
// FIXME: Add board ID number for doc reference.
//===========================================================+
#define CBPM_4NS_FREQ_CODE         60
#define CBPM_4NS_PULSEWIDTH_CODE   1

#define CBPM_6NS_FREQ_CODE         58
#define CBPM_6NS_PULSEWIDTH_CODE   1
  
#define CBPM_10NS_FREQ_CODE        54
#define CBPM_10NS_PULSEWIDTH_CODE  1 

#define CBPM_14NS_FREQ_CODE        57
#define CBPM_14NS_PULSEWIDTH_CODE  2

//------------------------------------------------------------------------+
// Position Montior, TigerSHARC-based  - Data Parameters                  |
//---------------------------------------                                 |
// CBPM_MAX_BUNCHES       Maximum number of bunches to be digitized       |
//                                                                        |
// CBPM_MAX_NOSKIP_BUNCHES                                                |
//                      Maximum number of bunches that may be             |
//                      requested in a single timing block (channel)      |
//                      before skipping acquisition turns is necessary to |
//                      prevent data corruption due to front-end FPGA bus |
//                      bandwidth.  Characteristic of front-end firmware. |
//                                                                        |
// CBPM_MAX_CARDS         Number of front-end cards per module            |
//                                                                        |
// CBPM_MAX_BUTTONS       Number of buttons that can be read out per      |
//                          module                                        |
//                                                                        |
// CBPM_MAX_ADC_PER_CARD  Number of ADC channels per front-end card       |
// CBPM__MAX_CHANS        Number of digitizer channels per front-end      |
//                          card                                          |  
//                                                                        |
// CBPM_ADC__BUF_SIZ      ADC buffer words for each front end digitizer   |
//                                                                        |
// CBPM_ADC_BUF_USE       ADC buffer depth to be used for standard BPM    |
//                          data  acquisition [presently set for          |
//                      (640 bunches|2)*1024 turns = 327680 words per     |
//                      digitizer]                                        |
//                                                                        |
// CBPM_RAW_BUF_USE       Total active raw buffer size [presently set     |
//                          for (640 bunches * 1024 turns) ]              |
//                                                                        |
// CBPM_MAX_TURNS         Maximum number of turns for a single bunch      |
//                          which can be read out (NOTE:  Cannot store    |
//                          the full number of turns for all possible     |
//                          bunches)                                      |
//                                                                        |
// CBPM_MAX_RAW_SAMPLES   Maximum number of raw data samples that can     |
//                          be acquired                                   |
//                                                                        |
// CBPM_MAX_PROC_SAMPLES  Maximum number of processed data samples that   |
//                       can be acquired                                  |
//                                                                        |
// CBPM_MAX_AUTO_SAMPLES  Maximum number of auto-sample data samples that |
//                      can be acquired (presently set to be the same as  |
//                      the number of processed samples)                  |
//                                                                        |
// CBPM_MAX_1B_PHASE_SAMPLES Maximum number of betatron phase samples     |
//                     that can be acquired for a single bunch (same as a |
//                      single ADC buffer depth)                          |
//                                                                        |
// CBPM_MAX_MB_PHASE_SAMPLES Maximum number of betatron phase samples     |
//                      that can be acquired for multiple bunches (same   |
//                      as the raw buffer depth)                          |
//------------------------------------------------------------------------+
#define CBPM_MAX_BUNCHES              640
#define CBPM_4NS_MAX_BUNCHES          640  // Single species
#define CBPM_6NS_MAX_BUNCHES          426  // Single species
#define CBPM_10NS_MAX_BUNCHES         256  // Single species
#define CBPM_14NS_MAX_BUNCHES         366  // Dual species

#define CBPM_MAX_NOSKIP_BUNCHES       140

#define	CBPM_MAX_CARDS                4
#define CBPM_MAX_CARD_ADCS_PER_BLOCK  1
#define CBPM_MAX_CHANS_PER_CARD       2
#define CBPM_MAX_TIMING_BLOCKS        2

#define CBPM_ADC_BUF_SIZ              512000
#define CBPM_ADC_BUF_USE              327680   // = (1024 * CBPM_MAX_BUNCHES / TS_MAX_ADC_PER_CARD)

#define CBPM_RAW_BUF_USE             (1024 * CBPM_MAX_BUNCHES)
#define CBPM_MAX_TURNS                CBPM_ADC_BUF_USE

#define CBPM_MAX_RAW_SAMPLES          40960
#define CBPM_MAX_TSCAN_SAMPLES        1900
#define CBPM_MAX_PROC_SAMPLES         64
#define CBPM_MAX_AUTO_SAMPLES         640

#define CBPM_MAX_CTMD_VALUE           70  // FIXME: Verify this value from timing board docs.
#define CBPM_MAX_BTMD_VALUE           9

#define CBPM_TBT_DATA_COLUMNS         6

//--------------------------------------------------------------
// This is the total number of acquisition controllers that
// exist in a given instrument.  The acquisition controller is
// the front-end FPGA device.
//   For BPMs, there is one acquisition controller device for
//   each PAIR of ADCs and data buffers.
//--------------------------------------------------------------
#define CBPM_MAX_ACQ_CONTROLLERS      4
#define CBPM_MAX_FIT_COEFFS           12

//------------------------------------------------------
// Find Bunch parameters
//------------------------------------------------------
#define CBPM_4NS_FIND_BUNCH_STEPS     120  //30
#define CBPM_6NS_FIND_BUNCH_STEPS     30 // FIXME Check
#define CBPM_10NS_FIND_BUNCH_STEPS    40 // FIXME Check
#define CBPM_14NS_FIND_BUNCH_STEPS    200 //50



//===========================================================+
// Front-end card control values                             |
//===========================================================+

//------------------------------------------------------------------------+
// CBPM_SRAM_WRITE_ENAB_0    When written to the SRAM control register,   |
//                          allows address-incremented writes to the next |
//                          slot in SRAM for the first SRAM on the card.  |
//                                                                        |
// CBPM_SRAM_WRITE_ENAB_1    Same thing for the second SRAM on the card.  |
//                                                                        |
// CBPM_SKIP_TURNS_DEFAULT   Default value to use in skipping turns       |
//                           whenever the skip-turns feature on the       |
//                           front-end card FPGA is activated.            |
//                             (See dispatch.c for details.)              |
//------------------------------------------------------------------------+
#define CBPM_SRAM_WRITE_ENAB_0       0x0001
#define CBPM_SRAM_WRITE_ENAB_1       0x0002

#define CBPM_SRAM_WRITE_DISABLE      0x0

#define CBPM_SKIP_TURNS_DEFAULT      5

//===========================================================+
// Functionality (Command) Parameters:                       |
//===========================================================+

//------------------------------------------------------------------------+
// Timing Scan:                                                           |
//---------------                                                         |
//                                                                        | 
// CBPM_SCAN_TURNS          Number of turns to average together when      |
//                         scanning through delays. For global delays and |
//                         initial channel delay                          |
//                                                                        |
// CBPM_FIT_TURNS           For final channel delay fitting               |
//                                                                        |
//                                                                        |
// CBPM_MAX_DELAY_COUNTS     Largest value of a delay chip register       |
//------------------------------------------------------------------------+
#define CBPM_SCAN_TURNS              10

#define CBPM_FIT_TURNS               16

#define CBPM_TURN_TIME              (BT*NUM_BUCKETS)
#define CBPM_MAX_CHAN_DELAY_COUNTS   1024
#define CBPM_MAX_BLOCK_DELAY_COUNTS  2048
#define CBPM_CH_DEL_MIN_THRESH       25
#define CBPM_CH_DEL_MAX_THRESH       102
#define CBPM_INVALID_DELAY_SETTING   -1
#define CBPM_MAX_GLOB_DEL_COUNTS     1500   // corresponds to 15ns

#define CBPM_4NS_BUCKET_LEN          1000   // was 800, delay chips may provide only 
#define CBPM_6NS_BUCKET_LEN          800    
#define CBPM_10NS_BUCKET_LEN         1200    
#define CBPM_14NS_BUCKET_LEN         1700   // Length of a single bucket in delay chip counts

#define CBPM_CHAN_SCAN_STEP          1      // = 10ps
#define CBPM_CHAN_SCAN_STEP_IN_PS	 10*CBPM_CHAN_SCAN_STEP

#define CBPM_DELAY_LOW               0
#define CBPM_DELAY_HIGH              1023
#define CBPM_DELAY_STEP              1

#define CBPM_DELAY_MID               512
#define CBPM_CAL_RADIUS              10  // Window HALF-width (radius) of channel delay scan.
                                         // Keep it small to stay on roughly parabolic top of peak
#define CBPM_BLOCK_STEP				 1


//------------------------------------------------------------------------+
// Gain Calibration:            FIXME: Documentation here needs updating. |
//--------------------                                                    |
//                                                                        |
// CBPM_INIT_PEDESTAL           Initial pedestal value                    |
//                                                                        |
// CBPM_ADC_LIMIT               Largest count value possible              |
//                                                                        |
// TBMPM_LOWER_GAIN_THRESHOLD   Value at or below which collected ADC     |
//                              values will trigger an increase in the    |
//                              gain setting when auto-gain is active.    |
// CBPM_UPPER_GAIN_THRESHOLD     Value at or above which collected ADC    |
//                              values will trigger an increase in the    |
//                              gain setting when auto-gain is active.    |
//                                                                        |
// CBPM_NEAR_ADC_SATURATION      Value at or above which collected ADC    |
//                              values will trigger a saturation flag.    |
//                                                                        |
// CBPM_PED_CALIB_TOL           Max. ADC count difference allowed from    |
//                               ped_table                                |
//                                                                        |
// CBPM_GAIN_CALIB_TOL          Max. fractional change allowed in auto    |
//                              gain calibration                          |
//                                                                        |
//                                                                        |
// CBPM_VGA_STEP             Step size to use when cycling through all    |
//                              variable gain amplifier settings.         |
//                                                                        |
// MX_CBI_GAINS                 Max number of states (ie, gain settings)  |
//                              available for each button.                |
//                                                                        |
// CBPM_MIN_GAIN      Lowest possible overall gain setting                |
//                                                                        |
// CBPM_MAX_GAIN      Highest possibel overall gain setting  (Comes       |
//                      from turning off attenuator circuit which can     |
//                      only be done on all channels simultaneously.)     |
//                                                                        |
// MAX_DVGA_GAIN      Highest possible DVGA gain setting. (Overall gain   |
//                    depends on DVGA gain AND attenuator setting.)       |
//                                                                        |
// INIT_GAIN          Initial gain setting for DSP initialization         |
//                                                                        |
// INIT_GAIN_COEFF    Initial gain coefficient for DSP initialization     |
//                                                                        |
//------------------------------------------------------------------------+
#define CBPM_ADC_LIMIT                    32768                      // 16-bit ADC
#define CBPM_ADC_LOWER_THRESHOLD          1600                      // - used by Time Scan
#define CBPM_ADC_LOW_THRESHOLD            (0.25 * CBPM_ADC_LIMIT)  //  8,190     (30% was 9,830)
#define CBPM_ADC_HIGH_THRESHOLD           (0.65 * CBPM_ADC_LIMIT)  //  21,300     (75% was 24,576)
#define CBPM_ADC_SAT_THRESHOLD            (0.99 * CBPM_ADC_LIMIT)  //

// The number of status bitfields required = ceil( (CBPM_NUM_CARDS * CBPM_MAX_ADC_PER_CARD) / 32 )

#define CBPM_BITFIELD_ALL_HIGH_32         0xFFFFFFFF

#define CBPM_MAX_GAIN_MODES               2   // Fixed-gain aplifier mode vs. variable-gain amplifier mode
#define CBPM_MAX_MULTIGAIN_SETUPS         3

#define CBPM_MIN_GAIN_SETTING             1
#define CBPM_MAX_GAIN_SETTING             10
#define CBPM_MAX_GAINS                    11

#define CBPM_MAX_CURRMON_GAIN_SETTING     8
#define CBPM_DEFAULT_GAIN_CODE            262   // Lowest variable-gain setting.


//------------------------------------------------------------------------+
// Betatron Phase:                                                        |
//------------------                                                      |
// 
// 
//------------------------------------------------------------------------+
#define CBPM_MAX_PHASE_DIM          2  // Horz/Vert
#define CBPM_NUM_PHASE_COEFFS       2  // Cosine/Sine

#define CBPM_NUM_DP_TURNS_OFFSET    5    // was  9
#define CBPM_MIN_DP_TURNS_OFFSET   -2    // was -4

#define CBPM_HALF_TURN_OPTIONS      2
#define CBPM_COS_COEFF              0
#define CBPM_SIN_COEFF              1
  //#define CBPM_HORZ_DIM               0
  //#define CBPM_VERT_DIM               1
  //#define CBPM_LONG_DIM               2
#define CBPM_SIN_TABLE_SIZE         512

  // The maximum expected number of times a particular BPM location
  // will jump between correct phase angle reporting and discontinuous
  // phase angle reporting as the bunch number is scanned from 1 to the maximum.
#define CBPM_MAX_PHASE_JUMPS        3



//---------------------------
// Bunch pattern parameters
//---------------------------
#define CBPM_MAX_BP_REGS            40
#define CBPM_14NS_BUNCH_PAT_REGS    22
#define CBPM_BP_REG_WIDTH           16     // Width of BP registers in bits
#define CBPM_LAST_BP_REG_WIDTH      16     //
#define CBPM_MAX_BP_REG_VAL         0xFFFF // Max val of 16 bit int
#define CBPM_MAX_SH_BP_REG_VAL      0xFFFF // No short regs, so same value
#define CBPM_14NS_SH_BP_REG_VAL     0x3FFF


//--------------------------
// Timing board parameters
//--------------------------
//#define CBPM_MAX_TIM_VAL            1024  // 10-bit delay register value limit
#define CBPM_NUM_BLOCK_DELAYS       2     // Max. global delays in block



//========================================================================+
// Data Structure-Specific Parameters:                                    |
//========================================================================+


//------------------------------------------------------------------------+
// Processed Data Buffer (PROC_BUF):                                      |
//------------------------------------                                    |
//                                                                        |
//                                                                        |
// NUM_PROC_BUFS defines how many circular buffers exist. Typically, the  |
// DSP will be writing to one buffer while the control system is reading  |
// out another. The third can be used to hold an "interesting" event that |
// the DSP detects.                                                       |
//                                                                        |
// NUM_PROC_BUF_PKTS is a calculated value that defines how many packets  |
// are required to cover the entire set of processed data buffers. Since  |
// we don't want a partial packet, the remainder of                       |
// (NUM_PROC_BUFS * PROC_BUF_ITEMS * PROC_BUF_SAMPLES) / PROC_BUF_PKT_SIZ |
// should be zero.                                                        |
//                                                                        |
//------------------------------------------------------------------------+
#define CBPM_MAX_PROC_BUFS         3


// Maximum number of BPMs supported in MPM publishing arrays.
#define CBPM_MAX_BPMS              120

//------------------------------------------------------------------------+
// MPM Database node names                                                |
//                                                                        |
//  ORBIT_DATA_BLOCK_NODE    Node name of block of continguous MPM memory |
//                           set aside for BPM orbit data publishing      |
//------------------------------------------------------------------------+
#define CBPM_ORBIT_DATA_NODE       "BPM ORB DATA"
#define CBPM_ORBIT_ENTRY_SIZE      7
#define CBPM_ORBIT_DATA_SIZE       (ORBIT_ENTRY_SIZE * CBPM_MAX_BPMS)

#define CBPM_ORBSTAT_NODE          "BPM ORB STAT"
#define CBPM_ORBSTAT_ENTRY_SIZE    16
#define CBPM_ORBSTAT_SIZE          (ORBSTAT_ENTRY_SIZE * CBPM_MAX_BPMS)

#define CBPM_BUTPHASE_DATA_NODE    "BTN PHA DATA"
#define CBPM_BUTPHASE_ENTRY_SIZE   16
#define CBPM_BUTPHASE_DATA_SIZE    (BUTPHASE_ENTRY_SIZE * CBPM_MAX_BPMS)

#define CBPM_MAX_PUBLISH_BUTTON    32768000
#define CBPM_MAX_PUBLISH_PH_ANG    32768000
#define CBPM_MAX_PUBLISH_PH_AMP   327680000  // 3276800 with threshold checking
                                             //  results in most instruments not
                                             //  publishing phase amplitude values
                                             //  with no beam.  This might be OK.
                                             // The value that seems to indicate
                                             // a digital problem of some sort
                                             // is -2147483648.


//========================================================================+
// Enumerations:                                                          |
//========================================================================+

// Distinct timing setups:
enum TIMING_SETUPS {
  s4ns_P_VAR_G,
  s4ns_E_VAR_G,
  s14ns_VAR_G,
  s4ns_P_FIX_G,
  s4ns_E_FIX_G,
  s14ns_FIX_G,
  CBPM_TOTAL_TIMING_SETUPS
};
//--------------------------------------------------
// FIXME: Manually sync these numbers for now.  
// BIParser does not understand enough C syntax to
// determine the value of enumeration tails like 
// CBPM_TOTAL_TIMING_SETUPS above.
//--------------------------------------------------
#define CBPM_MAX_TIMING_SETUPS              6


// Commands
// Inherit universal CBI command(s) from
// support library, then append all architecture-specific
// commands.
enum DSP_COMMANDS {
  ACQUIRE_ALL_PEDESTALS_CMD = CBI_FIRST_PLATFORM_COMMAND,
  PEDESTAL_CAL_CMD, 
  SET_GAINS_CMD,
  SET_DELAYS_CMD,           
  RAW_DATA_CMD,
  PROC_DATA_CMD,
  CONT_DATA_CMD,
  BETATRON_PHASE_CMD,  
  INJECTION_DATA_CMD,  
  TEST_RAW_DATA_CMD,
  FIND_BUNCH_CMD,
  FIND_BTMD_CMD,
  TIME_SCAN_CMD,
  PUBLISH_TEMPS_CMD,
  SET_BUN_PAT_OFFSETS_CMD,
  SET_TIMING_MODE_CMD,
  CURRENT_DATA_CMD,
  CONTINUOUS_PROC_CMD,
  SET_TIMING_SETUP_CMD,
  CLEAR_DEBUG_CMD,
  CBPM_MAX_COMMANDS
};


// collect_adc modes
enum COLLECT_ADC_MODE {
  RAW_DATA,
  PED_SUB_DATA,
  SCALED_DATA,
  PED_SUB_SCALED_DATA,
  CBPM_MAX_CORRECTION_MODES
};


// Types of averaging available when processing data
// For code consolidation
// and ease of management, process_data handles cases
// where one may want to obtain both signal level flags
// and computed averages, or just the former, which is
// less computationally intensive.  (For turn-by-turn
// data acquisitions, for instance.)
enum PROC_CALC_AVG_MODE {
  PROC_THRESHOLDS_ONLY,
  PROC_SIMPLE_AVG_MODE,
  PROC_JITTER_SUPPRESSION_MODE
};


// Enumeration for ADC Memory Buffer Control (for TS101 series 
// front-end memory buffers 
enum ADC_BUFFER_CONTROL {
   ADC_CONTROL,
   DSP_CONTROL
};


//-----------------------------------------------------------------------+
// BUTTON_INDEX     Describes the indexing scheme for the 4 detector     |
//                  buttons.                                             |
//                                                                       |
// DSP_BOOLEAN      False and true variables                             |
//                                                                       |
// CBI_POS_CUR      Specifies indices for 3 positions and a current for  |
//                  use in processing BPM information.                   |
//                                                                       |
// CONTINUOUS_DATA_MODES  Supported types of continous-monitoring data   |
//                        collection modes.                              |
//                  CONT_PROC - Continuous processed data                |
//                               (single bunch orbits)                   |
//                  CONT_CURR - Continuous multibunch current monitoring |
//                                                                       |
// STRUCT_TAGS      Tags for specific structures which can be passed     |
//                  back and forth between the control system and a      |
//                  BPM readout module                                   |
//                                                                       |
// ERROR_ID         ID nums to track down errors and define the number   |
//                  of error words required to hold individual bits for  |
//                  each error.                                          |
//                                                                       |
//-----------------------------------------------------------------------+
enum CBPM_BUTTON_INDEX { 
  CBPM_TOP_INNER, 
  CBPM_BOT_INNER, 
  CBPM_BOT_OUTER, 
  CBPM_TOP_OUTER 
};


enum CONTINUOUS_DATA_MODES {
  CBPM_CONT_PROC,          
  CBPM_CONT_CURR
};



// Assign an integer to every function that can be
// called onboard in the course of DSP operation.

////This list must have a 1-1 correspondence with the cbpm_routine_names list in ..\utils\cbpm_defs.c
enum CBPM_ROUTINE_IDS {
  INVALID_ROUTINE_0,
  ACQUIRE_ALL_PEDESTALS_ID,
  ADD_DEBUG_ID,
  AUTO_GAIN_ID,
  BACKUP_MATRIX_ID,
  BUNCH_PATTERN_REDUCE_ID, 
  BUNCH_PATTERN_REDUCE_4NS_ID,   
  BUNCH_PATTERN_POST_ID,
  CAL_LOC_OFFSET_ID,       
  CBPM_GET_ERRORS_ID,
  CBPM_SET_ERROR_ID,
  CHAN_DELAY_CAL_ID,
  CHECK_RAW_BUF_ID,   
  CHECK_VALIDITY_ID,  
  CLEAR_DEBUG_ID,         
  COEFF_MATRIX_ID,         
  COLLECT_ID,              
  COLLECT_ADC_ID,
  COLLECT_PROC_ID,
  COLLECT_RAW_ID,          
  DETERMINE_STAT_ID,
  DECODE_STAT_ID,  
  DISPATCH_ID,             
  DO_BLOCK_SCAN_ID,
  DO_CALIBRATION_ID,       
  DSP_SLEEP_TURNS_ID,
  ENCODE_STAT_ID,  
  FIND_BTMD_ID,
  FIND_BUNCH_ID,           
  FIND_DELAYS_ID,          
  FLOAT_ERR_CHECK_ID,          
  FLOAT_ERR_CLEAR_ID,          
  GET_BETATRON_PHASE_ID,
  GET_BITS_ID,  
  GET_CONTINUOUS_DATA_ID,
  GET_PROC_DATA_ID,        
  GET_RAW_DATA_ID,         
  INIT_ID,                 
  ITERATE_ID,              
  ITERATE_FIND_DELAYS_ID,
  LOAD_BUNCH_PATTERN_ID,
  LU_DECOMP_ID,            
  LU_SAVE_SOL_ID,          
  LU_SOLVE_ID,             
  MAIN_ID,                 
  MAT_MULT_ID,
  MEASURE_ADC_RMS_ID,
  NORMALBETATRON_ID,       
  PEDESTAL_CAL_ID,         
  PERFORM_TEST_ID,
  PERFORM_TEST_2INPUT_ID,
  PROCESS_ADC_ID,          
  PROCESS_DATA_ID,         
  PUBLISH_TEMPS_ID,
  ROTATE_BUNCH_PATTERN_ID, 
  SET_BITS_ID,  
  SET_DELAYS_ID,           
  SET_GAINS_ID,            
  SET_TIMING_MODE_ID,
  SET_TIMING_SETUP_ID,      
  TEST_RAW_DATA_ID,        
  TIME_SCAN_ID,            
  TS_SET_BUFFER_CONTROL_ID,
  TSX_CHAN_STAT_ID,        
  UPDATE_SOL_ID,           
  NUM_ROUTINE_IDS
};

enum CBPM_TEST_ENUM {
	CBPM_TEST_ENUM_UNINITIALIZED,	//0
	CBPM_TEST_ENUM_SUCCESS,			//1
	CBPM_TEST_ENUM_WARNING, 		//2
	CBPM_TEST_ENUM_ERROR,			//3
};

enum CBPM_TEST_TYPE {
	CBPM_TEST_GREATERTHAN,		//0
	CBPM_TEST_LESSTHAN,			  //1
	CBPM_TEST_EQUAL_ERROR,		//2
	CBPM_TEST_EQUAL_WARNING,	//3
	CBPM_TEST_TRUE_IS_WARNING,//4
	CBPM_TEST_TRUE_IS_ERROR,	//5	
	CBPM_TEST_FALSE_IS_ERROR,	//6	
	CBPM_TEST_GREATERTHAN2INPUT,//7
	CBPM_TEST_LESSTHAN2INPUT,	//8
};

enum ERROR_TEST_STRUCT_ELEMENTS { // 
	TEST_COLLECT_RAW_ADC_SATURATED,			//  0 
	TEST_SIGNAL_TOO_HIGH,           	    	// 
	TEST_SIGMA_IS_ZERO,                		// 
	TEST_SIGNAL_TOO_LOW,           	    	// 
	TEST_BLOCK_DELAY_TOO_CLOSE_TO_HIGH_EDGE, // 
	TEST_BLOCK_DELAY_TOO_CLOSE_TO_LOW_EDGE,  //5 
	TEST_OFFSET_TOO_CLOSE_TO_HIGH_EDGE,		// 
	TEST_OFFSET_TOO_CLOSE_TO_LOW_EDGE,  		// 
	TEST_CHIPS_TOO_FAR_APART,        		// 
	TEST_OUTLIER_DETECTED,	                // 
	TEST_DIFF_OF_CROSS_SUM_TOO_BIG,    		// 10
	TEST_FIT_CONCAVITY_TERM_TOO_HIGH,		// 
	TEST_FIT_CONCAVITY_TERM_TOO_LOW,			// 
	TEST_FIT_LINEAR_TERM_TOO_HIGH,			// 
	TEST_FIT_LINEAR_TERM_TOO_LOW,			// 
	TEST_FIT_CONSTANT_TERM_TOO_LOW,			// 15
	TEST_FITVALUE_TOO_CLOSE_TO_HIGH_EDGE,   	// 
	TEST_FITVALUE_TOO_CLOSE_TO_LOW_EDGE,   	// 
	TEST_BEFORE_ADJUSTTIMING_ADC_RMS_TOO_BIG,//
	TEST_AFTER_ADJUSTTIMING_ADC_RMS_TOO_BIG, // 
	TEST_CHIP_CHANGED_TOO_MUCH,   			// 
	TEST_CHIP_TO_CHIP_VALUE_CHANGED_TOO_MUCH, // 
	TEST_PEAK_FIT_SCAN_TOO_SLOW,			//
  TEST_TOO_MANY_TRIES,
	TEST_SLOSH_TOO_BIG,           	    	// 
	TEST_ALU_ERROR,           	    	// 
		//Card 0 will never get this cause the others are compared to it
};

#define CBPM_NUM_TESTS          26
#define CBPM_NUM_TESTS_ENCODED  6 //(int) ceil(CBPM_NUM_FIND_DELAYS_TESTS/4)

#define CBPM_TEST_DISPLAY_FACTOR 10000

enum EXECUTION_FLAG_ELEMENTS {
	DSP_DEBUG_LEVEL,
	LEAVE_FUNCTION_ON_FAILURE,
	OPTION_FLAG,
	OPTION2_FLAG,
	OPTION3_FLAG,
	USE_TIME_SCAN_TESTS,
	OVERRIDE_TIMING_ROLLBACK,
	FIND_DELAYS_NUM_RETRIES,						
	SLOSH_DELAYS,
	NUM_TURNS_IN_RMS,
	OUTPUT_TURN_COUNTER,
	OUTPUT_ALU_ERROR_BITS,
//	FLAG1,
//	FLAG2,
//	FLAG3,
//	FLAG4,								
//	FLAG5,
//	FLAG6,
//	FLAG7,
//	FLAG8,								
//	FLAG9,
};

#define CBPM_NUM_EXECUTION_FLAGS    12
#define CBPM_MAX_NUM_EXECUTION_FLAGS    20

#define CBPM_FIND_DELAYS_HIST_LENGTH  8

#if defined (__cplusplus)
}
#endif



#endif // CBPM_CONSTANTS_H 


