//------------------------------------------------------------------------
//  D O     N O T     E D I T     T H I S     F I L E
//------------------------------------------------------------------------
// File         :  CBPM_PEDESTALS_convert_a.c
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




int * cbpm_pedestals_convert(int mode, int *p_buf, int dtype, int dcount,
                           int offset, void *p_data) {

//-----------------------------------------------------------------------+
// Local Variables -                                                     |
//                                                                       |
// func_name  -  Pointer to name of the local function                   |
//                                                                       |
//-----------------------------------------------------------------------+

   char *func_name = "cbpm_pedestals_convert";

   int i;
   CBPM_PEDESTALS *p_cbpm_pedestals = NULL;
   void *p_sub_data;

   int count;
   int idummy;

   //-----------------------------------------------------------------------+
   //                     EXECUTABLE CODE STARTS HERE                       |
   //-----------------------------------------------------------------------+
   if (dtype == CBI_STRUCT) {
      p_cbpm_pedestals = ((CBPM_PEDESTALS *) p_data + dcount * offset);
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

      p_sub_data = (void *) &(p_cbpm_pedestals->ped_table);
      count      = sizeof p_cbpm_pedestals->ped_table / sizeof(float);
      p_buf      = cbi_float_convert(mode,p_buf,CBI_FLOAT,count,0,p_sub_data);

      p_sub_data = (void *) &(p_cbpm_pedestals->ped_rms_table);
      count      = sizeof p_cbpm_pedestals->ped_rms_table / sizeof(float);
      p_buf      = cbi_float_convert(mode,p_buf,CBI_FLOAT,count,0,p_sub_data);

      //Increment the data pointer
      p_cbpm_pedestals++;
   }

   return p_buf;
}
