#ifndef CBPM_INSTR_CODE_PARAM
#define CBPM_INSTR_CODE_PARAM_H

#define CBPM_MAX_NUM_TRIES_ALLOWED 4

int execution_flags[] = {
  0,				                	    // DSP_DEBUG_LEVEL is set to CTL_System.debug_verbosity	//0
  0,				                  		// LEAVE_FUNCTION_ON_FAILURE,
  0, 				                  		// OPTION_FLAG
  0, 		                  				// OPTION2_FLAG
  0, 			                  			// OPTION3_FLAG
  0,                              // USE_TIME_SCAN_TESTS,
  0,			                  			// OVERRIDE_TIMING_ROLLBACK		- Set by cbpm_time_scan prompt			
  CBPM_MAX_NUM_TRIES_ALLOWED, 		// FIND_DELAYS_NUM_RETRIES								//			
  1, 				                  		// SLOSH_DELAYS
  1024,	  	                  		// NUM_TURNS_IN_RMS
  0,                              // OUTPUT_TURN_COUNTER   - Output flag
  0,                              // OUTPUT_ALU_ERROR_BITS - Output flag
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

typedef struct {
		char *name;
		char *helptext;
		int  type;
		float  warning_threshold;
		float  error_threshold;
		int  enable;
    int required_for_update_acceptance;
		char *msg;
} ERROR_TEST_STRUCT;  

ERROR_TEST_STRUCT tests[] = {
	
	//TEST_COLLECT_RAW_ADC_SATURATED				
	{
		"ADC Saturation",
		//"                         Warning: 0.65 * CBPM_ADC_LIMIT, Error: 0.99 * CBPM_ADC_LIMIT, CBPM_ADC_LIMIT = 32768",
		"\n\tValue is if INST_raw_data_header.ADC_saturation_bitfields[card] == 1 for any point in collect_raw\n",
		CBPM_TEST_TRUE_IS_ERROR,
		0,
		0,
		1,
    1,
		"Saturation"
	},

	//TEST_SIGNAL_TOO_HIGH
	{
		"ADC Signal too large",
		//"                         Error if averaged ADC value across whole range ever exceeds high threshold",
		"\n\tValue is if INST_raw_data_header.ADC_high_threshold_bitfields[card] == 1 for any point during scan\n",
		CBPM_TEST_EQUAL_WARNING,
		1,
		1,
		1,
    0,
		"ADC Signal too large"
	},

	//TEST_SIGMA_IS_ZERO - Sigma is Zero					
	{
		"Sigma is Zero",
		"\n\tSometimes all 16 turns come back with the same value, so this is to monitor if that happens again in the future.\n",
		CBPM_TEST_EQUAL_WARNING,
		0.,
		0.,
		1,
    0,
		"Sigma is Zero"
	},

	//TEST_SIGNAL_TOO_LOW
	{
		"ADC Signal too small",
		//"                         Error if averaged ADC values across whole range never exceeded low threshold",
		"\n\tValue is if INST_raw_data_header.ADC_low_threshold_bitfields[card] == 1 for all points during scan\n",
		CBPM_TEST_TRUE_IS_ERROR,
		1,
		1,
		1,
    1,
		"ADC Signal too small"
	},

//TEST_BLOCK_DELAY_TOO_CLOSE_TO_HIGH_EDGE		
	{
		"Block Delay value too near high edge",
		"(Block Delay - maxspan) - 100, \n\tvalue computed before time scan run",
		CBPM_TEST_GREATERTHAN,
		200.,
		100.,
		0, // only enable this for time scan, not autotime
    0,
		"Block Delay value too near high edge"
	},		
	
	//TEST_BLOCK_DELAY_TOO_CLOSE_TO_LOW_EDGE		
	{
		"Block Delay value too near low edge",
		"Value is Block Delay\n",
		CBPM_TEST_LESSTHAN,
		0.+200.,
		0.+100.,
		1,
    0,
		"Block Delay value too near low edge"
	},	
	
	//TEST_OFFSET_TOO_CLOSE_TO_HIGH_EDGE - Offset too near high edge			
	{
		"Offset too near high edge",
		"Max chip value (1023) - 50, 100",
		CBPM_TEST_GREATERTHAN,
		CBPM_MAX_CHAN_DELAY_COUNTS-1.-100.,
		CBPM_MAX_CHAN_DELAY_COUNTS-1.-50.,
		1,
    0,
		"Offset too near high edge"
	},
	
	//TEST_OFFSET_TOO_CLOSE_TO_LOW_EDGE - Offset too near low edge			
	{
		"Offset too near low edge",
		"Min chip value (0) + 50, 100",
		CBPM_TEST_LESSTHAN,
		0.+100.,
		0.+50.,
		1,
    0,
		"Offset too near low edge"
	},	
	
	//TEST_CHIPS_TOO_FAR_APART - Chip offsets too far apart	
	{
		"Chip offsets too far apart",
		"",
		CBPM_TEST_GREATERTHAN,
		70.,
		200.,
		1,
    0,
		""
	},

	//TEST_OUTLIER_DETECTED - 
	{
		"Min too far from Peak",
		"fraction: v1/v2, v1 = maxAmp-minAmp, v2 = maxAmp\n",
		CBPM_TEST_GREATERTHAN2INPUT,
		0.11,
		0.12,
		1,
    0,
		"Min too far from Peak"
	},	
		
	//TEST_DIFF_OF_CROSS_SUM_TOO_BIG - (Difference of cross sums / Max Amp) too high		
	{
		"(Difference of cross sums/Max Amp) too high",
		"v1 = Difference of cross sums, v2 = Max Amp",
		CBPM_TEST_GREATERTHAN2INPUT,
		0.42, //    diff cross sum/ max amplitude
		0.6,
		1,
    0,
		""
	},

	//TEST_FIT_CONCAVITY_TERM_TOO_HIGH - 2nd order term in fit too high
	{
		"2nd order term in fit too high",
		"",
		CBPM_TEST_GREATERTHAN2INPUT,
		1./1800. + 2e-4, //1./(1800.*0.7),
		1./1800. + 3.2e-4, //1./(1800.*0.6),
		1,
    0,
		""
	},

	//TEST_FIT_CONCAVITY_TERM_TOO_LOW - 2nd order term in fit too low	
	{
		"2nd order term in fit too low",
		"\n\tratio of -1.0 * term to max amplitude should be roughly 1/1800 = 0.000556, so 1000/1800 ~= 0.555\n",
		CBPM_TEST_LESSTHAN2INPUT,
		1./1800. - 1.2e-4, //1./(1800.*1.3),
		1./1800. - 2.4e-4, //1./(1800.*1.4),
		1,
    0,
		""
	},

	//TEST_FIT_LINEAR_TERM_TOO_HIGH - Linear term in fit too high	
	{
		"Linear term in fit too high",
		"",
		CBPM_TEST_GREATERTHAN2INPUT,
		1./(80.) + 0.005 , //1./(80.*0.8),
		1./(80.) + 0.01 , //1./(80.*0.7),
		1,
    0,
		""
	},

	//TEST_FIT_LINEAR_TERM_TOO_LOW - Linear term in fit too low	
	{
		"Linear term in fit too low",
		"\n\tratio of term to max amplitude should be roughly 1/80 = 0.0125, so 1000/80 ~= 12.5\n",
		CBPM_TEST_LESSTHAN2INPUT,
		1./(80.) - 0.005 , //80.*1.3),
		1./(80.) - 0.01, //(80.*1.6),
		1,
    0,
		""
	},

	//TEST_FIT_CONSTANT_TERM_TOO_LOW - Constant term in fit too low	
	{
		"Constant term in fit too low",
		"\n\tratio of term to max amplitude should be roughly 1 so 1000*1 = 1000\n",
		CBPM_TEST_LESSTHAN2INPUT,
		1 - 0.14,
		1 - 0.28,
		1,
    0,
		""
	},

	//TEST_FITVALUE_TOO_CLOSE_TO_HIGH_EDGE - Fit value too near high edge			
	{
		"Fit value too near high edge",
		"Max fit value (2*CBPM_CAL_RADIUS) - 4",
		CBPM_TEST_GREATERTHAN,
		CBPM_CAL_RADIUS*2.-6.,
		CBPM_CAL_RADIUS*2.-4.,
		1,
    0,
		"Fit value too near high edge"
	},	
	
	//TEST_FITVALUE_TOO_CLOSE_TO_LOW_EDGE - Fit value too near low edge			
	{
		"Fit value too near low edge",
		"Min fit value (0) + 4\n",
		CBPM_TEST_LESSTHAN,
		0.+6.,
		0.+4.,
		1,
    0,
		"Fit value too near low edge"
	},

	//TEST_BEFORE_ADJUSTTIMING_ADC_RMS_TOO_BIG - Before adjusting the timing the RMS from 1024 turns is too big		
	{
		"RMS too big before timing adjust",
		"\n\tBefore adjusting the timing the RMS from 1024 turns is too big\n",
		CBPM_TEST_GREATERTHAN,
		700.,
		1400.,
		1,
    1,
		""
	},

	//TEST_AFTER_ADJUSTTIMING_ADC_RMS_TOO_BIG - After adjusting the timing the RMS from 1024 turns is too big		
	{
		"RMS too big after timing adjust",
		"\n\tAfter adjusting the timing the RMS from 1024 turns is too big\n",
		CBPM_TEST_GREATERTHAN,
		120.,
		400.,
		1,
    1,
		""
	},

	//TEST_CHIP_CHANGED_TOO_MUCH - Chipvalue changed too much		
	{
		"Chipvalue changed too much",
		"",
		CBPM_TEST_GREATERTHAN,
		40.,
		200.,
		1,
    0,
		""
	},

	//TEST_CHIP_TO_CHIP_VALUE_CHANGED_TOO_MUCH - Chip to chip value changed too much		
	{
		"Chip to chip value changed too much",
		"",
		CBPM_TEST_GREATERTHAN,
		4.,
		20.,
		1,
    0,
		""
	},

	//TEST_PEAK_FIT_SCAN_TOO_SLOW - peak scan took too long		
	{
		"Peak scan took too long",
		"",
		CBPM_TEST_GREATERTHAN,
		597.,
		603.,
		1,
    0,
		""
	},
		
  //TEST_TOO_MANY_TRIES - too many time scan tries
	{
		"Too many time scan tries",
		"",
		CBPM_TEST_GREATERTHAN,
		1.0 * (CBPM_MAX_NUM_TRIES_ALLOWED-1),
		1.0 * CBPM_MAX_NUM_TRIES_ALLOWED,
		1,
    0,
		""
	},
  
		//TEST_SLOSH_TOO_BIG - 	
	{
		"Absolute Value of Block Delay slosh too big",
		"",
		CBPM_TEST_GREATERTHAN,
		50.,
		500.,
		1,
    0,
		""
	},
		
  //TEST_ALU_ERROR - 
	{
		"ALU Error",
		"",
		CBPM_TEST_TRUE_IS_WARNING,
		0,
		0,
		1,
    0,
		""
	},  

};  

#endif
