/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_load_instr_code_params.c                         |
*                                                                       |
* Description  :  load test thresholds and conditional branching flags  | 
                    from header file, into instrument comm structure    |
*                                                                       |
* Arguments    :  None                                                  | 										
*                                                                       |
* Author       :  M.Stedinger                                           |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"

char * strndup(const char * string, size_t n);

#include "cbpm_instrument_code_param.h"	

  void DisplayTestThresholds(void);

//Function called to set DSP debug level
/*int prompt_DSP_debug_level(CBPM_DATA *dp) {
	
  int debug_level = CTL_System.debug_verbosity;    	
	
  // Set a pointer to the data substructure of the temp master struct tree
  ;
  //dp = CTL_System.p_Module_temp->dsp_data;	

	  //-------------------------------------------------
	  // Define the list of prompts and their parameters
	  //-------------------------------------------------
	  CBI_INIT_PROMPTS(1);
	  int retval;
  
	  prompts[0].supp_text_ptr    = NULL;
	  prompts[0].prompt_text      = "Set DSP Debug to what Level ? ";
	  prompts[0].default_value    = debug_level;
	  prompts[0].destination      = &(dp->cbpm_code_params.execution_flags[DSP_DEBUG_LEVEL]);
	  prompts[0].last_val_default = TRUE;
    
	  retval = CBI_PROMPTER();      
	  //dp->cbpm_code_params.execution_flags[DSP_DEBUG_LEVEL] = debug_level;
	  
}*/

int cbpm_load_instr_code_params() {
	
//Structure containing error and warning thresholds for find_delays results
	
  char *func_name = (char *)__FUNCTION__;
  int debug_level = CTL_System.debug_verbosity;
  
  if (enable_time_scan_tests()) {
    printf("\n\n *********************************************** \n     New Time Scan Diagnostics Enabled  \n ***********************************************\n\n");
  } else {
    printf("\n\n *********************************************** \n     New Time Scan Diagnostics DISABLED \n ***********************************************\n\n");
  }  
  
  DisplayTestThresholds();

  // Pointers to 'data' substructure (destination & temporary)
  CBPM_DATA      *dp;

  int iidx, test, flag;
  
  for (iidx = 0; iidx < CTL_System.n_Modules; iidx++) {

    dp = CTL_System.p_Module[iidx]->dsp_data;
  		cbpm_copy_inst_params_for_prep(dp);

      for( test=0; test<CBPM_NUM_TESTS; test++) {
	    dp->cbpm_code_params.tests[test].type = tests[test].type;
	    dp->cbpm_code_params.tests[test].warning_threshold = tests[test].warning_threshold;
	    dp->cbpm_code_params.tests[test].error_threshold = tests[test].error_threshold;
	    dp->cbpm_code_params.tests[test].enable = tests[test].enable;
  	}//for test

  	for( flag=0; flag<CBPM_NUM_EXECUTION_FLAGS; flag++) {
	    dp->cbpm_code_params.execution_flags[flag] = execution_flags[flag];
  	}//for flag

    dp->cbpm_code_params.execution_flags[DSP_DEBUG_LEVEL] = debug_level;
    dp->cbpm_code_params.execution_flags[OPTION_FLAG] = CTL_System.option_mode;
    dp->cbpm_code_params.execution_flags[OPTION2_FLAG] = CTL_System.option_mode2;
    dp->cbpm_code_params.execution_flags[OPTION3_FLAG] = CTL_System.option_mode3;
    
    //dp->cbpm_code_params.execution_flags[SLOSH_DELAYS] = 0;

    //if (debug_level>= CBI_DEBUG_LEVEL_1) {
    //  prompt_DSP_debug_level(dp);
    //}   
  }//for iidx
  
  return CBI_F_SUCCESS;    
}

void DisplayTestThresholds(void)
{

  int debug_level = CTL_System.debug_verbosity;
  float dispFactor = CBPM_TEST_DISPLAY_FACTOR;
    
  if (enable_time_scan_tests() && debug_level > CBI_DEBUG_LEVEL_0) {
    printf("\nTest Parameters\n");
    printf("\n Value is  the input parameters fed to the PerformTest function\n");
    for(int test = 0; test < CBPM_NUM_TESTS; test++) {
      ERROR_TEST_STRUCT strct = tests[test];

      switch (strct.type) {
        case (CBPM_TEST_GREATERTHAN):
          printf("%2i) %-45s,                W:%9.3f    E:%9.3f <= value ",test, strct.name, strct.warning_threshold, strct.error_threshold);
          break;
        case (CBPM_TEST_LESSTHAN):
          printf("%2i) %-45s,       value <= E:%9.3f    W:%9.3f          ",test, strct.name, strct.error_threshold, strct.warning_threshold);
          break;
        case (CBPM_TEST_EQUAL_ERROR):
          printf("%2i) %-45s,        value = E:%9.3f   (W:%9.3f)         ",test, strct.name, strct.error_threshold, strct.warning_threshold);
          break;
        case (CBPM_TEST_EQUAL_WARNING):
          printf("%2i) %-45s,        value =(E:%9.3f)   W:%9.3f          ",test, strct.name, strct.error_threshold, strct.warning_threshold);
          break;
        case (CBPM_TEST_TRUE_IS_WARNING):
          printf("%2i) %-45s,                Warning, if value = TRUE    ",test, strct.name, strct.error_threshold, strct.warning_threshold);
          break;
        case (CBPM_TEST_TRUE_IS_ERROR):
          printf("%2i) %-45s,                Error, if value = TRUE      ",test, strct.name, strct.error_threshold, strct.warning_threshold);
          break;
        case (CBPM_TEST_FALSE_IS_ERROR):
          printf("%2i) %-45s,                Error, if value = FALSE     ",test, strct.name, strct.error_threshold, strct.warning_threshold);
          break;
        case (CBPM_TEST_GREATERTHAN2INPUT):
          printf("%2i) %-45s,             v2*W:%9.3f v2*E:%9.3f < %4.0f*v1",test, strct.name, CBPM_TEST_DISPLAY_FACTOR*strct.warning_threshold, CBPM_TEST_DISPLAY_FACTOR*strct.error_threshold, dispFactor);
          break;
        case (CBPM_TEST_LESSTHAN2INPUT):
          printf("%2i) %-45s, %4.0f*v1 <= v2*E:%9.3f v2*W:%9.3f         ",test, strct.name, dispFactor, CBPM_TEST_DISPLAY_FACTOR*strct.error_threshold, CBPM_TEST_DISPLAY_FACTOR*strct.warning_threshold);
          break;
        default :
          printf("UNKNOWN ERROR TYPE\n");
          break;
          
      }
      printf("  %s\n", strct.helptext);
    }
    printf("\n");

    printf("\nExecution flags\n");
    for (int i = 0; i< CBPM_NUM_EXECUTION_FLAGS; i++) {
      printf("%7i ", i);
    }  
    printf("\n");  
    for (int i = 0; i< CBPM_NUM_EXECUTION_FLAGS; i++) {
      printf("%7i ", execution_flags[i]);
    }
    printf("\n");
    
  }
  
  return;
}

int cbpm_copy_inst_params_for_prep(CBPM_DATA *dp) {
	
	int debug_level  = CTL_System.debug_verbosity; 
	
  int test, flag;
  for( test=0; test<CBPM_NUM_TESTS; test++) {
    dp->cbpm_code_params.tests[test].type = tests[test].type;
    dp->cbpm_code_params.tests[test].warning_threshold = tests[test].warning_threshold;
    dp->cbpm_code_params.tests[test].error_threshold = tests[test].error_threshold;
    dp->cbpm_code_params.tests[test].enable = tests[test].enable;
    dp->cbpm_code_params.tests[test].required_for_update_acceptance = tests[test].required_for_update_acceptance;
  }//for test  
  	
  for( flag=0; flag<CBPM_NUM_EXECUTION_FLAGS; flag++) {
    dp->cbpm_code_params.execution_flags[flag] = execution_flags[flag];
  }//for flag    	
  
    dp->cbpm_code_params.execution_flags[OPTION_FLAG] = CTL_System.option_mode;
    dp->cbpm_code_params.execution_flags[OPTION2_FLAG] = CTL_System.option_mode2;
    dp->cbpm_code_params.execution_flags[OPTION3_FLAG] = CTL_System.option_mode3;  
    dp->cbpm_code_params.execution_flags[DSP_DEBUG_LEVEL] = debug_level;
    
    dp->cbpm_code_params.execution_flags[SLOSH_DELAYS] = 1;
    //dp->cbpm_code_params.execution_flags[FIND_DELAYS_NUM_RETRIES] = 0;
    
    dp->cbpm_code_params.execution_flags[USE_TIME_SCAN_TESTS] = enable_time_scan_tests();

    return CBI_F_SUCCESS;
}
