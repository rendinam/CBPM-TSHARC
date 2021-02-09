#ifndef CBPM_SYSTEM_H
#define CBPM_SYSTEM_H

//------------------------------------------------------------------------+
// File : tbpm_dsp_system.h                                               |
//                                                                        |
// Description : This header defines pointers, variables and funtion      |
// prototypes needed across the entire project.  It provides one place to |
// declare each variable, pointer or pointer array, and another to        |
// "extern" the definition.                                               |
//                                                                        |
// Authors : C. Strohman / M. Palmer / M. Rendina                         |
//------------------------------------------------------------------------+


#include "cbpm_inst_includes.h"

//-----------------------------------------------------------------------+
//  The following provides for C++ compatibility (ie, C++ routines can   |
//  explicitly use this include file).                                   |
//-----------------------------------------------------------------------+
#if defined (__cplusplus)
extern "C" {
#endif


//--------------------------------------------------------
// Macro to simplify accessing memory/register locations
// "value at:"
//--------------------------------------------------------
#define VAL_AT(reg) (*(volatile unsigned int *)(reg))


//---------------------------------------------------------------------
// Make available the horizontal and vertical phase word components
// bits 18-10: Vertical phase data   (bit 18=MSB, bit 10=LSB)
// bits 27-19: Horizontal phase data (bit 27=MSB, bit 19=LSB)
//---------------------------------------------------------------------
#define V_PHASE_MASK        0x7F80000
#define V_PHASE_MASK_SHIFT  9

#define H_PHASE_MASK        0x3FC00
#define H_PHASE_MASK_SHIFT  17

#define VERT_PHASE_DATA     ((*(volatile unsigned int *)(GLOBAL_TURN_DAT)) & V_PHASE_MASK) >> V_PHASE_MASK_SHIFT
#define HORZ_PHASE_DATA     ((*(volatile unsigned int *)(GLOBAL_TURN_DAT)) & H_PHASE_MASK) >> H_PHASE_MASK_SHIFT


//----------------------------------------------------------------
// Parameter name mapping for potential multi-platform capability
//----------------------------------------------------------------
#define BP_REG_WIDTH          CBPM_BP_REG_WIDTH
#define MAX_BP_REGS           CBPM_MAX_BP_REGS
#define LAST_BP_REG_WIDTH     CBPM_LAST_BP_REG_WIDTH
#define MAX_BP_REG_VAL        CBPM_MAX_BP_REG_VAL
#define MAX_SHORT_BP_REG_VAL  CBPM_MAX_SH_BP_REG_VAL
#define CHAN_SCAN_STEP        CBPM_CHAN_SCAN_STEP



//----------------------------------------
// Global and Pointer declarations
//
// Anything that the control system 
// can change must be declared "volatile"
// so that the compiler isn't presented
// with the opportunity to optimize it out. 
//----------------------------------------
#if defined (INST_NO_EXTERN)

   // Variables //
   //-------------

   // For receiving a copy of the turns count
   // register from the digital board FPGA.
   volatile int                  turncount;

   // Global variable for holding the timing
   // mode in which the device is operating
   int                           timing_mode_G;

   // A local place to store the bunch 
   // pattern rotation offset value
   int                           bun_pat_offset[CBPM_MAX_TIMING_BLOCKS];
   
   // Global variable to signify the 
   // maximum number of bunchess
   // available in the current bunch 
   // spacing mode
   int                           MAX_BUNCHES;
   
   // An array to hold the channel (timing block, ADC)
   // responsibe for digitizing each bunch selected, in order.
   int							 block_list[640];

   // Space for holding a count of the number
   // of bunches enabled on each timing block.
   int                           block_bunch_tally[CBPM_MAX_TIMING_BLOCKS];
   
   // sine and cosine lookup table memory for phase measurement
   float                         pm sin_table[CBPM_SIN_TABLE_SIZE];
   float                         pm cos_table[CBPM_SIN_TABLE_SIZE];

   // Range-checking flags for multibunch ADC threshold checks.
   int found_in_range[CBPM_MAX_TIMING_BLOCKS][CBPM_MAX_CARDS] = {CBI_FALSE, CBI_FALSE, CBI_FALSE, CBI_FALSE};
   
   // For passing info between bunch pattern reduction pre- and post- funtions
   int saved_bun_pat[MAX_BP_REGS];
   int saved_rot_bun_pat[640];   
     
   // Holds number of turns to skip if skip-turns feature on front-end FPGA is active.
   // Otherwise holds 0.
   int                           fpga_turns_to_skip;

   
   // Pointers and Arrays Thereof //
   //-------------------------------
   
   //----------------------------------------------
   //// Digital Board FPGA Register Pointers ////
   //----------------------------------------------
   volatile int *p_ts_adc_mem_first   = (int *)ADC_MEM_FIRST;
   volatile int *p_turn_counter       = (int *)GLOBAL_TURN_CNT;
   volatile int *p_turn_data_word     = (int *)GLOBAL_TURN_DAT;
   volatile int *p_tod_seconds        = (int *)TOD_SECONDS;
   volatile int *p_ts_data_acq        = (int *)DATA_ACQ;
 
   // Pointers to digital board SRAM locations that are populated by
   // the Coldfire communications board during boot-up.
   volatile int *sram_id_ptr          = (int *)(CF_IP_ADDR);
   volatile int *sram_id_ptr_last     = (int *)(CF_IP_ADDR+3);
   volatile int *sram_hostname        = (int *)(CF_HOSTNAME);
   
   
	volatile int *debug_heartbeat_reg  = (int *)(DEBUG_HEARTBEAT);
	volatile int *debug_run_reg        = (int *)(DEBUG_RUN);
	volatile int *debug_step_reg       = (int *)(DEBUG_STEP);
	volatile int *debug_location_reg   = (int *)(DEBUG_LOCATION);
	volatile int *breakpoint           = (int *)(DEBUG_BREAKPOINT);
	volatile int *input_reg            = (int *)(DEBUG_INPUT);
	volatile int *output_reg           = (int *)(DEBUG_OUTPUT);
	volatile int *value1_reg           = (int *)(DEBUG_VALUE1);
	volatile int *value2_reg           = (int *)(DEBUG_VALUE2);
	volatile int *value3_reg           = (int *)(DEBUG_VALUE3);
	volatile int *value4_reg           = (int *)(DEBUG_VALUE4);
	volatile int *value5_reg           = (int *)(DEBUG_VALUE5);
	volatile int *status_reg           = (int *)(DEBUG_STATUS);
	   
   
   //---------------------------------
   // Registers on the TIMING BOARD
   //---------------------------------
   // Block timing delay control registers (2 in series).  Each register 
   // is responsible for approximately 10ns of delay.
   volatile int *p_block_delays[CBPM_MAX_TIMING_BLOCKS][CBPM_NUM_BLOCK_DELAYS] = {
      (int*)CBPM_GDEL_A0, (int*)CBPM_GDEL_A1, 
      (int*)CBPM_GDEL_B0, (int*)CBPM_GDEL_B1
   };

   
   //--------------------------------------
   // Other signal-delay related registers
   //--------------------------------------
   // Final stage channel delays for each signal input.
   // BPM uses channel delays on TIMING board.
   // BSM uses channel delays on its respective front-end daughter boards.
   volatile int *p_chan_delays[CBPM_MAX_ACQ_CONTROLLERS][CBPM_MAX_TIMING_BLOCKS] = {
      (int*)CBPM_CDEL_C0_A, (int*)CBPM_CDEL_C0_B,
      (int*)CBPM_CDEL_C1_A, (int*)CBPM_CDEL_C1_B,
      (int*)CBPM_CDEL_C2_A, (int*)CBPM_CDEL_C2_B,
      (int*)CBPM_CDEL_C3_A, (int*)CBPM_CDEL_C3_B
   };
   
     
   //-----------------------------------
   // Registers on front-end cards
   //
   // One register bank exists in each
   // front-end acquisition controller
   // (FPGA).
   //-----------------------------------

   // Each acquisition controller has a primary control register that is used to enable
   // acquisitions, signal acquisition completion, and allow for the enabling and disabling
   // of other various operational modes as a series of flags in the bitfield.
   volatile int *p_acq_control[CBPM_MAX_ACQ_CONTROLLERS]  = {
   	 (int*)ACQ_CONTROL_0, 
   	 (int*)ACQ_CONTROL_1, 
   	 (int*)ACQ_CONTROL_2, 
   	 (int*)ACQ_CONTROL_3
   };
    
   volatile int *p_acq_bunch_patterns[CBPM_MAX_ACQ_CONTROLLERS] = {
     (int *)ACQ_BUNCH_PAT_0,
     (int *)ACQ_BUNCH_PAT_1,
     (int *)ACQ_BUNCH_PAT_2,
     (int *)ACQ_BUNCH_PAT_3
   };
   
   // Pseudo 32-bit registers are comprised of a set of 2, 16-bit high and low registers.
   volatile int *p_acq_turn_req[CBPM_MAX_ACQ_CONTROLLERS][REG_SPLIT_COUNT] = {
     (int*)ACQ_TURN_REQ_0_L, (int*)ACQ_TURN_REQ_0_H,
     (int*)ACQ_TURN_REQ_1_L, (int*)ACQ_TURN_REQ_1_H,
     (int*)ACQ_TURN_REQ_2_L, (int*)ACQ_TURN_REQ_2_H,
     (int*)ACQ_TURN_REQ_3_L, (int*)ACQ_TURN_REQ_3_H
   };

   
   volatile int *p_acq_mem_first[CBPM_MAX_ACQ_CONTROLLERS][REG_SPLIT_COUNT] = {
   	 (int *)ACQ_MEM_FIRST_0_L, (int *)ACQ_MEM_FIRST_0_H,
   	 (int *)ACQ_MEM_FIRST_1_L, (int *)ACQ_MEM_FIRST_1_H,
   	 (int *)ACQ_MEM_FIRST_2_L, (int *)ACQ_MEM_FIRST_2_H,
   	 (int *)ACQ_MEM_FIRST_3_L, (int *)ACQ_MEM_FIRST_3_H
   };
   
   // Skip-turns control registers
   volatile int *p_acq_skip_turns[CBPM_MAX_ACQ_CONTROLLERS] = {
   	 (int *)ACQ_SKIP_TURNS_0,
   	 (int *)ACQ_SKIP_TURNS_1,
   	 (int *)ACQ_SKIP_TURNS_2,
   	 (int *)ACQ_SKIP_TURNS_3
   };
   
   // Front-end buffer SRAM direct-write control register
   volatile int *p_acq_sram_ctrl[CBPM_MAX_ACQ_CONTROLLERS] = {
   	 (int *)ACQ_SRAM_CTRL_0,
   	 (int *)ACQ_SRAM_CTRL_1,
   	 (int *)ACQ_SRAM_CTRL_2,
   	 (int *)ACQ_SRAM_CTRL_3
   };
   
   // Front-end buffer SRAM direct-write DATA register
   volatile int *p_acq_sram_data[CBPM_MAX_ACQ_CONTROLLERS] = {
     (int *)ACQ_SRAM_DATA_0,
     (int *)ACQ_SRAM_DATA_1,
     (int *)ACQ_SRAM_DATA_2,
     (int *)ACQ_SRAM_DATA_3
   };
   
  volatile int *p_acq_temperature[CBPM_MAX_ACQ_CONTROLLERS] = {
  	(int *)ACQ_TEMPERATURE_0,
  	(int *)ACQ_TEMPERATURE_1,
  	(int *)ACQ_TEMPERATURE_2,
  	(int *)ACQ_TEMPERATURE_3
  }; 
  	
   
   // Registers used to set the signal gain via a gain code value that represents
   // a particular amplifier type/mode and setting within that type/mode.
   volatile int *p_gain_regs[CBPM_MAX_CARDS][CBPM_MAX_TIMING_BLOCKS] = {
      (int*)GAIN_C0_A, (int*)GAIN_C0_B,
      (int*)GAIN_C1_A, (int*)GAIN_C1_B,
      (int*)GAIN_C2_A, (int*)GAIN_C2_B,
      (int*)GAIN_C3_A, (int*)GAIN_C3_B
   };
   
   // Pointers to the base addresses of each ADC's incoming data buffer
   volatile int *p_adc_bufs[CBPM_MAX_CARDS][CBPM_MAX_TIMING_BLOCKS] = {
      (int*)BUF_C0_A_0, (int*)BUF_C0_B_0,
      (int*)BUF_C1_A_0, (int*)BUF_C1_B_0,
      (int*)BUF_C2_A_0, (int*)BUF_C2_B_0,
      (int*)BUF_C3_A_0, (int*)BUF_C3_B_0
   };
   
   
   
  

#else  // [[ INST_NO_EXTERN is NOT defined ]]



   extern volatile int  turncount;
   extern int           timing_mode_G;
   extern int           bun_pat_offset[CBPM_MAX_TIMING_BLOCKS];
   extern int           MAX_BUNCHES;
   extern int           block_list[640];
   extern int           block_bunch_tally[CBPM_MAX_TIMING_BLOCKS];
   extern float      pm sin_table[CBPM_SIN_TABLE_SIZE];
   extern float      pm cos_table[CBPM_SIN_TABLE_SIZE];
   extern int           found_in_range[CBPM_MAX_TIMING_BLOCKS][CBPM_MAX_CARDS];
   extern int           saved_bun_pat[];
   extern int           saved_rot_bun_pat[640];
   extern int           fpga_turns_to_skip;
   //===
   extern volatile int *p_ts_adc_mem_first;
   extern volatile int *p_turn_counter;
   extern volatile int *p_turn_data_word;
   extern volatile int *p_tod_seconds;
   extern volatile int *p_ts_data_acq;
   //---
   extern volatile int *sram_id_ptr;
   extern volatile int *sram_id_ptr_last;
   extern volatile int *sram_hostname;
   //---
   extern int *debug_heartbeat_reg;
   extern int *input_reg;
   extern int *output_reg;
   extern int *debug_run_reg;
   extern int *debug_step_reg;
   extern int *debug_location_reg;
   extern int *breakpoint;
   extern int *value1_reg;
   extern int *value2_reg;
   extern int *value3_reg;
   extern int *value4_reg;
   extern int *value5_reg;
   extern int *status_reg;

   //---
   extern volatile int *p_block_delays[CBPM_MAX_TIMING_BLOCKS][CBPM_NUM_BLOCK_DELAYS];
   extern volatile int *p_chan_delays[CBPM_MAX_ACQ_CONTROLLERS][CBPM_MAX_TIMING_BLOCKS];
   //---
   extern volatile int *p_acq_control[CBPM_MAX_ACQ_CONTROLLERS];
   extern volatile int *p_acq_bunch_patterns[CBPM_MAX_ACQ_CONTROLLERS];   
   extern volatile int *p_acq_turn_req[CBPM_MAX_ACQ_CONTROLLERS][REG_SPLIT_COUNT];
   extern volatile int *p_acq_mem_first[CBPM_MAX_ACQ_CONTROLLERS][REG_SPLIT_COUNT];
   extern volatile int *p_acq_skip_turns[CBPM_MAX_ACQ_CONTROLLERS];
   extern volatile int *p_acq_sram_ctrl[CBPM_MAX_ACQ_CONTROLLERS];
   extern volatile int *p_acq_sram_data[CBPM_MAX_ACQ_CONTROLLERS];
   extern volatile int *p_acq_temperature[CBPM_MAX_ACQ_CONTROLLERS];
  
   extern volatile int *p_gain_regs[CBPM_MAX_CARDS][CBPM_MAX_TIMING_BLOCKS];
   extern volatile int *p_adc_bufs[CBPM_MAX_CARDS][CBPM_MAX_TIMING_BLOCKS];
   
   
   
   
#endif //(INST_NO_EXTERN)





// FIXME: Have these prototypes generated by the BIParser program
//        They will reside in their own header files.
int   init_packet_tables( void ) ;
int   init_core_packet_tables( void );



//-----------------------
// function prototypes
//-----------------------

void  main( void );
void  dsp_init( void );

void  dispatch( void );
int   set_timing_mode( int mode );

// Interrupt handlers
void  timer0H_int_handler( int unused );

// Command functions
int   acquire_all_pedestals( void );
int   dump_adc_buf( void );
int   peak_find( void);
int   pedestal_cal( void );
int   user_set_gain( void );
int   user_set_delay( void );
int   get_raw_data( void );
int   get_proc_data( void );
int   get_bunch_data( int );
int   get_betatron_phase( void );
int   get_continuous_data( int collection_mode );
int   find_bTMD( void );
int   set_delays( int tsetup );
int   set_timing_setup( void );
int   test_raw_data( void );
int   time_scan( void );
int   delay_scan( void );
int   find_bunch( void );

// Acquisition control
int   load_bunch_pattern(int *);
int   acquisition_control(enum ADC_BUFFER_CONTROL control);

// get_raw_data utilities
int   collect_raw( CBPM_RAW_PARAMS params );
int   collect_adc( CBPM_RAW_PARAMS );

int   check_raw_buf(CBPM_RAW_PARAMS params, int buffer_offset);

// get_proc_data utilities
int   collect_proc(int, CBPM_RAW_PARAMS, CBPM_PROC_PARAMS, CBPM_CAL_PARAMS);
int   process_adc(int calc_avg_mode, int bunch, int num_turns, CBPM_PROC_DATA *proc_data, CBPM_SIGNAL_STAT *signal_stat);
int   process_data(int calc_avg_mode, CBPM_RAW_PARAMS raw_params, int summ_idx);

// Gain control utilities
int   set_gains(void);
int   do_calibration(CBPM_RAW_PARAMS, CBPM_CAL_PARAMS, int*);
int   calibrate_bunch(int, int, int, CBPM_RAW_PARAMS, CBPM_CAL_PARAMS);
int   load_cross_calib(int);
int   cross_calibrate(int, CBPM_RAW_PARAMS);
int   auto_gain(int single_pass, int collect_data, int max_gain_setting);

// Delay control utilities
int   set_delay(int,int);
int   dsp_sleep(int);
int   set_block_delay(int, int);

// Peak finding support
int   find_delays(CBPM_RAW_PARAMS );
int   iterate_find_delays( CBPM_RAW_PARAMS raw_params);

typedef struct {
	int delay_steps;
	int initial_delay;
	int block_step;
	int bunch_bit_parity;
	int species;
	CBPM_RAW_PARAMS local_raw_params;
}find_delays_input_struct;

typedef struct {
	float max[CBPM_MAX_ACQ_CONTROLLERS];
	int   Delay_at_peak[CBPM_MAX_ACQ_CONTROLLERS];
	int   single_peak;
}find_delays_output_struct;

// Betatron phase utilities
int   NormalBetatron(int, int);

// Miscellaneous utilities
int   add_debug(int, int, int);
int   clear_debug(void);
int   debug_point(int);
int   finish_handshake(int stat);
int   init_debug_registers(void);

float correct_value( float value, int mode, int tsetup, int gain_setting, int tblock, int card, int chan );
int   bitval( int word, int bit );
int   bunpat_reduce_pre( int tblock, int calib_bunche[] );
int   bunpat_reduce_pre_4ns(int calib_bunches[]);
int   bunpat_reduce_post(void);
int   cbpm_set_error(int error, volatile int *error_array);
int   cbpm_get_errors(int* error_array, int *error_list);
int   compose_block_list(void);
int   sleep_n_turns(int nsturns);
int   float_err_check(void);
void  float_err_clear(void);
void  magic_wait( int iter );
int   memfirsts( int offset );
int   parity( int number );
int   PerformTest(int index, int addr, float value, int test_step_index, int enable);
int   PerformTest2Input(int index, int addr, float value1, float value2, int test_step_index, int enable);
int   DetermineStatus(void);
int   publish_temps(void);
int   rotate_bunch_pattern_4ns(int, int wrapped_bits[]);
int   rotate_block_bunch_pattern(int, int);
int   setbit( int word, int bit );
int   set_flag0_high( void );
int   set_flag0_low( void );
int   sign_extend( int input );
int   set_chan_stat(int timing_block, int chan);
int   unset_chan_stat(int word, int timing_block, int chan);
int   get_chan_stat(int word, int timing_block, int chan);
int   pack( int word, int byte_idx, char c );
int   unpack(int, int);
int   unsetbit( int word, int bit );
float roundf(float val);
float clipf(float val, float min, float max);
int   clipi(int val, int min, int max);

#if defined (__cplusplus)
}
#endif



#endif // CBPM_SYSTEM_H
