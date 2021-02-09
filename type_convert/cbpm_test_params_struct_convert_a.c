//------------------------------------------------------------------------
//  D O     N O T     E D I T     T H I S     F I L E
//------------------------------------------------------------------------
// File         :  CBPM_TEST_PARAMS_STRUCT_convert_a.c
// Date Created :  Tue Apr 17 16:49:54 2012
//
// Description  :  This file was automatically generated by the BIParser.
//                 Converts between user-defined data structures by
//                 employing low-level data type conversions between
//                 the instrument's internal data representation and the
//                 control system machine's internal data representation
//                 if needed for each member of the structure.
//------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "CESR_platform.h"
#include "utilities.h"
#include "messages.h"

#include "cbi_constants.h"
#include "cbi_types_a.h"
#include "cbi_control_types.h"

#include "cbi_conv_protos.h"
#include "cbpm_types_a.h"
#include "cbpm_protos_a.h"




int * cbpm_test_params_struct_convert(int mode, int *p_buf, int dtype, int dcount,
                           int offset, void *p_data) {

//-----------------------------------------------------------------------+
// Local Variables -                                                     |
//                                                                       |
// func_name  -  Pointer to name of the local function                   |
//                                                                       |
//-----------------------------------------------------------------------+

   char *func_name = "cbpm_test_params_struct_convert";

   int i;
   CBPM_TEST_PARAMS_STRUCT *p_cbpm_test_params_struct = NULL;
   void *p_sub_data;

   int count;
   int idummy;

   //-----------------------------------------------------------------------+
   //                     EXECUTABLE CODE STARTS HERE                       |
   //-----------------------------------------------------------------------+
   if (dtype == CBI_STRUCT) {
      p_cbpm_test_params_struct = ((CBPM_TEST_PARAMS_STRUCT *) p_data + dcount * offset);
   } else {
      idummy = sprintf(message,
                       "Invalid data type specified:  %d\n",dtype);
      log_message(S_ERROR, func_name, message);
      return NULL;
   }

   // Don't need to check for mode, simply pass the individual
   // pieces of data in the struct to the relevant conversion 
   // routines
   for (i = 0; i < dcount; i++) {

      p_sub_data = (void *) &(p_cbpm_test_params_struct->type);
      count      = sizeof p_cbpm_test_params_struct->type / sizeof(int);
      p_buf      = cbi_int_convert(mode,p_buf,CBI_INT,count,0,p_sub_data);

      p_sub_data = (void *) &(p_cbpm_test_params_struct->error_threshold);
      count      = sizeof p_cbpm_test_params_struct->error_threshold / sizeof(float);
      p_buf      = cbi_float_convert(mode,p_buf,CBI_FLOAT,count,0,p_sub_data);

      p_sub_data = (void *) &(p_cbpm_test_params_struct->warning_threshold);
      count      = sizeof p_cbpm_test_params_struct->warning_threshold / sizeof(float);
      p_buf      = cbi_float_convert(mode,p_buf,CBI_FLOAT,count,0,p_sub_data);

      p_sub_data = (void *) &(p_cbpm_test_params_struct->enable);
      count      = sizeof p_cbpm_test_params_struct->enable / sizeof(int);
      p_buf      = cbi_int_convert(mode,p_buf,CBI_INT,count,0,p_sub_data);

      //Increment the data pointer
      p_cbpm_test_params_struct++;
   }

   return p_buf;
}