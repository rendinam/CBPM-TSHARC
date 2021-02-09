#ifndef CBPM_TRACE_H
#define CBPM_TRACE_H

//-----------------------------------------------------------------------+
// File         :  cbpm_trace.h                                           |
//                                                                       |
// Description  :  Structure definitions and enumerations that allow for |
//                 tracing of function calls and error conditions during |
//                 runtime of the software resident in the instruments   |
//                 themselves  Serves to aid in debugging and status     |
//                 notification.                                         |
//-----------------------------------------------------------------------+


//-----------------------------------------------------------------------+
//  The following provides for C++ compatibility (ie, C++ routines can   |
//  explicitly use this include file).                                   |
//-----------------------------------------------------------------------+
#if defined (__cplusplus)
extern "C" {
#endif

   /*typedef struct {
       int func_id;
       char *dsp_func;
   } CBI_FUNC_TRACE;

   typedef struct {
       int err_id;
       char *err_fmt;
   } CBI_ERR_TRACE;*/


//This list must have a 1-1 correspondence with the cbpm_instrument_error_name list in ..\utils\cbpm_defs.c
   enum ERROR_ID {
   ERR_CLEAR = 0,
   CBI_NO_ERR,
   ACQ_ALL_PED_PED_CAL_ERR, 
   ACQ_ALL_PED_SET_TIMING_SETUP_ERR, 
   AUTO_GAIN_COLLECT_PROC_ERR,
   AUTO_GAIN_CONVERGE_ERR,				//
   COLLECT_PROC_CR_ERR,
   COLLECT_PROC_PD_ERR,
   COLLECT_RAW_ERR,         
   DETERMINE_STATUS_ERR,
   DO_CAL_AUTO_FIND_DELAY1,
   DO_CAL_AUTO_FIND_DELAY2,				//
   DO_CAL_AUTO_GAIN_ERR1,
   DO_CAL_AUTO_GAIN_ERR2,
   DO_CAL_ERR,
   FB_COLLECT_ADC_ERR,			
   FB_UNKNOWN_TIMING_MODE_ERR,			//
   FIND_BTMD_FIND_BUNCH_ERR1,
   FIND_BTMD_FIND_BUNCH_ERR2,
   FIND_DELAYS_BLOCK_SCAN_ERR,   
   FIND_DELAYS_COLLECT_ADC_ERR1,
   FIND_DELAYS_COLLECT_ADC_ERR2,	
   FIND_DELAYS_COLLECT_RAW_ERR1,		//
   FIND_DELAYS_COLLECT_RAW_ERR2,
   FIND_DELAYS_PROCESS_DATA_ERR1,      
   FIND_DELAYS_COLLECT_RAW_RMS_ERR,
   FIND_DELAYS_PROCESS_DATA_RMS_ERR,
   GET_BETA_PHASE_DATA_ERR,
   GET_BETA_PHASE_ERR,
   GET_CONT_AUTO_GAIN_ERR,        	
   GET_PROC_DATA_ERR,         			//
   GET_PROC_DATA_CAL_ERR,
   GET_RAW_DATA_CAL_ERR,      
   GET_RAW_DATA_DAT_ERR,     
   GET_RAW_DATA_PROC_ERR,		
   IDLE_COMMAND_LOOP_BREAK,
   INST_MATH_ERR,						//
   PED_CAL_COLLECT_RAW_ERR,				
   PED_CAL_PROC_DATA_ERR,
   PERFORM_TEST_ERROR,
   PERFORM_TEST2_ERROR,
   PROCESS_ADC_AVG_ERR,
   //PROCESS_ADC_ERR,             	
   PROCESS_DATA_ERR,            		//
   STS_UNKNOWN_TIMING_MODE_ERR,			
   TIME_SCAN_DO_CAL_ERR1, 
   TIME_SCAN_DO_CAL_ERR2, 
   //INP_CHECKSUM_ERR,            // Input parameters errors //40
   //PED_ERR,                     // Start function errors				- not used
   //GET_RAW_DATA_FFT_ERR,        // error when processing FFT
   //OP_GAIN_INPUT_ERR,           // set_op_gain errors
   //OP_GAIN_COEFF_ERR,											//10
   //OP_GAIN_PED_ERR,
   //COLLECT_PROC_CB_ERR,         // collect_proc errors
   //CALIBRATE_BUNCH_LOAD_ERR,    // calibrate_bunch errors
   //CALIBRATE_BUNCH_AG_ERR,										//20
   //CALIBRATE_BUNCH_GAIN_ERR,
   //CALIBRATE_BUNCH_DELAY_ERR,
   //CALIBRATE_BUNCH_XCAL_ERR,
   //CALIBRATE_BUNCH_CONVERGE_ERR,
   //AUTO_GAIN_ERR,               // auto_gain errors				//25
   //AUTO_GAIN_SETOG_ERR,
   //CROSS_CALIBRATE_GAIN_ERR,    // cross_calibrate errors
   //CROSS_CALIBRATE_LOAD_XCAL_ERR,								//30
   //CROSS_CALIBRATE_SET_DELAY_ERR,
   //CROSS_CALIBRATE_RAW_ERR,
   //CROSS_CALIBRATE_PROC_ERR,
   //CROSS_CALIBRATE_LOAD_RESULTS_ERR,
   //GET_CONT_CB_ERR,             // get_cont_data errors			//35
   //GET_CONT_DATA_GAIN_ERR,
   //GET_CONT_DATA_ERR,
   //PEAK_FIND_ERR,               // peak_find errors
   //FIND_DELAYS_ERR,             // find_delays errors
   //FIND_DELAYS_GAIN_ERR,	
   //FD_CHAN_DELAY_CAL_ERR,
   //FD_PRELIM_ERR,
   //FD_CAL_LOC_OFFSET_ERR,
   //GET_BETA_PHASE_CAL_ERR,      // get_betatron_phase errors
   //DSP_MATH_ERR,
   //GET_BUNCH_DATA_CAL_ERR,										//50
   //GET_BUNCH_DATA_ERR,
   //GET_BUNCH_DATA_GAIN_ERR,
   //PROCESS_FFT_NUM_ERR,    // FFT called for turns != 2**n
   //PROCESS_FFT_HORZ_ERR,   // Error in horizontal FFT processing
   //PROCESS_FFT_VERT_ERR,   // Error in vertical FFT processing	//55
   //PROCESS_FFT_WINDOW_ERR, // Error in FFT windowing
   //PROCESS_FFT_OUTPUT_ERR, // Error in generating FFT spectrum
   //PROCESS_FFT_PEAK_ERR,   // Error in finding FFT spectral peaks
   //INIT_FFT_WIN_ERR,       // Error in initializing the FFT windowwing struct
   NUM_ERROR_IDS           // Total number of available error conditions 
  };

//#define NUM_ERROR_IDS 17


#if defined (__cplusplus)
}
#endif


#endif  //CBPM_TRACE_H
