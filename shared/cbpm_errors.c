//-----------------------------------------------------------------------+
// File         :  cbpm_errors.c                                         |
//                                                                       |
// Description  :  This file contains the functions -                    |
//                 set_error, get_errors                                 |
//                                                                       |
// Prototypes   :  int cbpm_set_error(int)                               |
//                 int cbpm_get_errors(int*, int*)                       |
//                                                                       |
// Arguments    :  cbpm_set_error  - error enumeration value             |
//                 cbpm_get_errors - pointer to error word array         |
//                                - pointer to list which will receive   |
//                                  list of error enumeration values     |
//                                                                       |
// Return Values:  cbpm_set_error  - CBI_F_SUCCESS/CBI_F_FAILURE         |
//                 cbpm_get_errors - number of errors                    |
//                                                                       |
// Author       :  M. Stedinger                                          |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


#if defined(CBPM_INST_PLATFORM)
#include "cbpm_system.h"
#endif

//-----------------------------------------------------------------------+
// Definitions -                                                         |
//                                                                       |
//                                                                       |
//-----------------------------------------------------------------------+

#define BIT      0x01
#define LEN_WORD 32

//#if defined(CBPM_INST_PLATFORM)
	int cbpm_set_error(int error, volatile int *error_array) {
//#else
//	int cbpm_set_error(int error, int *error_array) {
//#endif

   int word;
   int shift;
   int mask;

  #if defined(CBPM_INST_PLATFORM)   
   // Set our routine ID in the trace when running on the DSP
   INST_stat.trace[INST_stat.num_levels++] = CBPM_SET_ERROR_ID;
  #endif
 
   if (error != ERR_CLEAR) {
   	
      *error_array &= ~CBI_NO_ERR;   	

      // Determine which word to use and which bit to set
      word  = (error - 1) / LEN_WORD;
      shift = (error - 1) % LEN_WORD;

      // Create bit mask
      mask  = BIT << shift;

      if (word < CBI_MAX_ERROR_WORDS) 
      	*(error_array + word) |= mask;

   } else {

      // Clear error array
      for (word = 0; word < CBI_MAX_ERROR_WORDS; word++) {
         *(error_array + word) = 0;
      }
      
      *error_array |=  CBI_NO_ERR;  
   }

  #if defined(CBPM_INST_PLATFORM)   
   // Decrement the trace when running on the DSP
   INST_stat.num_levels--;
  #endif

   return CBI_F_SUCCESS;
} 

// next routine is not needed on the DSP
#if !defined(CBPM_INST_PLATFORM)

int cbpm_get_errors(int* error_array, int *error_list) {

   int word;
   int bit;
   int n_errors = 0;
   int setbit;
   
   for (int i = 0; i<NUM_ERROR_IDS; i++) {
   	  *(error_list+i) = 0;
   }

   // If the NO_ERROR bit not set, then parse the error words for error
   // flags
   if ((*error_array & BIT) == 0) {
      for (word = 0; word < CBI_MAX_ERROR_WORDS; word++) {
         for (bit = 0; bit < LEN_WORD; bit++) {
            setbit = (*(error_array + word) >> bit) & BIT;
            if (setbit) {
               *(error_list + n_errors++) = LEN_WORD * word + bit + 1;
            }
         }
      }
   }

   return n_errors;
}

#endif
