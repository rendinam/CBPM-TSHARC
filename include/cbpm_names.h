//------------------------------------------------
// String constants for accessing DSP command 
// and function names.
//------------------------------------------------
#ifndef CBPM_NAMES_H
#define CBPM_NAMES_H



#if defined (CBPM_NO_EXTERN)


#else


extern char tmode_names[CBI_MAX_TIMING_MODES][8];

extern char tsetup_names[CBPM_MAX_TIMING_SETUPS][CBI_MAX_TSETUP_NAME_LENGTH];

extern char cbpm_cmd_names[CBPM_MAX_COMMANDS][25];

extern char cbpm_routine_names[50][25];

extern char cbpm_error_names[50][CBI_MAX_ROUTINE_ERROR_LENGTH];

extern char cbpm_error_names[50][CBI_MAX_ROUTINE_ERROR_LENGTH];

extern char cbpm_data_corrections[CBPM_MAX_CORRECTION_MODES][30];

extern char cbpm_trigger_setup_names[5][10];

extern char cbpm_avg_mode_names[3][20];


#endif // end EXTERN block



#endif // CBPM_NAMES_H
