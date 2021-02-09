//-----------------------------------------------------------------------+
// File         :  cbpm_test_functions.h                                 |
//                                                                       |
// Description  :  Include find for the cbpm_test_functions              |
//                 Contains:                                             |
//                                                                       |
// Contributors :  M. Stedinger                                          |
//-----------------------------------------------------------------------+

void encode_status(int Status[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS], int EncodedStatus[CBPM_NUM_TESTS_ENCODED]);
void decode_status(int EncodedStatus[CBPM_NUM_TESTS_ENCODED], int Status[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS]);
