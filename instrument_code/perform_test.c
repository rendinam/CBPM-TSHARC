//-----------------------------------------------------------------------+
// File         :  perform_test.c                                        |
//                                                                       |
// Description  :  Takes in value and compares to thresholds and         |
//                 determines error, warning, or success states.         |
//                                                                       |
// Author       :  M. Stedinger                                          |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int PerformTest(int test_index, int addr, float value, int test_step_index, int enable)
{
  volatile int *p_error = INST_cmd.error;  	
	
  // Add function ID to the trace
  int FID = PERFORM_TEST_ID;  
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  if (!enable || !INST_code_params.tests[test_index].enable) {
  	INST_stat.num_levels--;
  	return CBPM_TEST_ENUM_UNINITIALIZED;
  }
  	
	int errorTest;
	int warningTest;
	int testResult = CBPM_TEST_ENUM_UNINITIALIZED;

    	switch (INST_code_params.tests[test_index].type) {
    	case (CBPM_TEST_GREATERTHAN):
		if (value >= INST_code_params.tests[test_index].error_threshold)		
			testResult = CBPM_TEST_ENUM_ERROR;
		else if (value >= INST_code_params.tests[test_index].warning_threshold)	
			testResult = CBPM_TEST_ENUM_WARNING;
		else						
			testResult = CBPM_TEST_ENUM_SUCCESS;

		break;
	
	case (CBPM_TEST_LESSTHAN):
		if (value <= INST_code_params.tests[test_index].error_threshold)		
			testResult = CBPM_TEST_ENUM_ERROR;
		else if (value <= INST_code_params.tests[test_index].warning_threshold)	
			testResult = CBPM_TEST_ENUM_WARNING;
		else						
			testResult = CBPM_TEST_ENUM_SUCCESS;
	
		break;
	case (CBPM_TEST_EQUAL_ERROR):
		if (value == INST_code_params.tests[test_index].error_threshold)
			testResult = CBPM_TEST_ENUM_ERROR;
		else
			testResult = CBPM_TEST_ENUM_SUCCESS;
		break;
	case (CBPM_TEST_EQUAL_WARNING):
		if (value == INST_code_params.tests[test_index].error_threshold)
			testResult = CBPM_TEST_ENUM_WARNING;
		else
			testResult = CBPM_TEST_ENUM_SUCCESS;
		break;
		
	case (CBPM_TEST_TRUE_IS_WARNING):
		if (value == 1)
			testResult = CBPM_TEST_ENUM_WARNING;
		else
			testResult = CBPM_TEST_ENUM_SUCCESS;
		break;    
	case (CBPM_TEST_TRUE_IS_ERROR):
		if (value == 1)
			testResult = CBPM_TEST_ENUM_ERROR;
		else
			testResult = CBPM_TEST_ENUM_SUCCESS;
		break;							
	case (CBPM_TEST_FALSE_IS_ERROR):
		if (value == 0)
			testResult = CBPM_TEST_ENUM_ERROR;
		else
			testResult = CBPM_TEST_ENUM_SUCCESS;
		break;
									
	default:
		cbpm_set_error(PERFORM_TEST_ERROR, p_error);
		testResult = -990;
	}
	
	if (INST_test_results.Statuses[test_index][addr] < testResult) {
		INST_test_results.Statuses[test_index][addr] = testResult;
	}
	
	INST_btest_results.Statuseshist[test_step_index][test_index][addr] = testResult;
	
      INST_btest_results.StatusVals[test_step_index][test_index][addr] = value;
	  //INST_btest_results.WarnVals[test_index][addr] = INST_code_params.tests[test_index].warning_threshold;
	  //INST_btest_results.ErrorVals[test_index][addr] = INST_code_params.tests[test_index].error_threshold;      

      	INST_stat.num_levels--;
	return testResult;	
}

int PerformTest2Input(int test_index, int addr, float value1, float value2, int test_step_index, int enable)
// See if value1 is <inequality> value2*threshold
{
  volatile int *p_error = INST_cmd.error;  	
	
  int FID = PERFORM_TEST_ID;  
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  if (!enable || !INST_code_params.tests[test_index].enable) {
  	INST_stat.num_levels--;
  	return CBPM_TEST_ENUM_UNINITIALIZED;
  }
  	
	int testResult = CBPM_TEST_ENUM_UNINITIALIZED;

    	switch (INST_code_params.tests[test_index].type) {
    	case (CBPM_TEST_GREATERTHAN2INPUT):
		if (value1 >= value2*INST_code_params.tests[test_index].error_threshold)		
			testResult = CBPM_TEST_ENUM_ERROR;
		else if (value1 >= value2*INST_code_params.tests[test_index].warning_threshold)	
			testResult = CBPM_TEST_ENUM_WARNING;
		else						
			testResult = CBPM_TEST_ENUM_SUCCESS;

		break;
	
	case (CBPM_TEST_LESSTHAN2INPUT):
		if (value1 <= value2*INST_code_params.tests[test_index].error_threshold)		
			testResult = CBPM_TEST_ENUM_ERROR;
		else if (value1 <= value2*INST_code_params.tests[test_index].warning_threshold)	
			testResult = CBPM_TEST_ENUM_WARNING;
		else						
			testResult = CBPM_TEST_ENUM_SUCCESS;
	
		break;
	default:
		testResult = -990;
		cbpm_set_error(PERFORM_TEST2_ERROR, p_error);
	}
	
	if (INST_test_results.Statuses[test_index][addr] < testResult) {
		INST_test_results.Statuses[test_index][addr] = testResult;
	}
	
	INST_btest_results.Statuseshist[test_step_index][test_index][addr] = testResult;
	
  if (value2>0) {
    INST_btest_results.StatusVals[test_step_index][test_index][addr] = CBPM_TEST_DISPLAY_FACTOR*value1/value2;
  } else {
    INST_btest_results.StatusVals[test_step_index][test_index][addr] = -999;
  }
	
	INST_stat.num_levels--;
	return testResult;
}

int DetermineStatus(void) {
	
  volatile int *p_error = INST_cmd.error;  	
	
  // Add function ID to the trace
  int FID = DETERMINE_STAT_ID;  
  INST_stat.trace[INST_stat.num_levels++] = FID;
  	
  int maxStatus = CBPM_TEST_ENUM_UNINITIALIZED;
  int card, chan, test, addr;
	
  for (card = 0; card<CBPM_MAX_CARDS; card++) {
    for( chan = 0; chan < CBPM_MAX_CARD_ADCS_PER_BLOCK; chan++) {
      addr = (card * CBPM_MAX_CARD_ADCS_PER_BLOCK) + chan;			
      for(test = 0; test < CBPM_NUM_TESTS; test++) {
	if (maxStatus< INST_test_results.Statuses[test][addr])
	  {
	    maxStatus = INST_test_results.Statuses[test][addr];
	  }
      }
			
    }
  }

  int returnStatus;

  switch (maxStatus) {
  case CBPM_TEST_ENUM_UNINITIALIZED :
  case CBPM_TEST_ENUM_SUCCESS :
    returnStatus = CBI_F_SUCCESS;
    break;
  case CBPM_TEST_ENUM_WARNING :
    returnStatus = CBI_F_WARNING;
    break;
  case CBPM_TEST_ENUM_ERROR :
    returnStatus = CBI_F_FAILURE;
    //cbpm_set_error(PERFORM_TEST_ERROR, p_error);
    break;
  default:
    returnStatus = CBI_F_FAILURE;
    cbpm_set_error(DETERMINE_STATUS_ERR, p_error);
    break;
  }      

  INST_stat.num_levels--;
  return returnStatus;
}
