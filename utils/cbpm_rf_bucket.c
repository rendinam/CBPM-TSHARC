/*!---------------------------------------------------------------------------+
* File         :  cbpm_rf_bucket.c                                            |
*                                                                             |
* Description  :  Returns the CESR RF bucket number for the timing setup and  |
*                 bunch index provided.                                       |
*                                                                             |
*                 bunch index argument starts at 1                            |
*                 RF buckets are indexed here starting from 1                 |
*  If bunch index is out of range for the timing setup found, return -1.      |
*-----------------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_rf_bucket( int tsetup, int bunch ) {

  int retval;

  if (bunch < 1) {
    retval = -1;
  }

  // timing setup is 4ns
  if (tsetup == s4ns_P_FIX_G ||
      tsetup == s4ns_E_FIX_G ||
      tsetup == s4ns_P_VAR_G ||
      tsetup == s4ns_E_VAR_G   ) {

    if (bunch > 640) {
      retval = -1;
    }
    retval = bunch * 2;

  }

  // timing setup is 14ns
  if (tsetup == s14ns_VAR_G ||
      tsetup == s14ns_FIX_G   )   {
    
    if (bunch > 183) {
      retval = -1;
    }
    retval = (((bunch-1)*14) / 2) + 1;
    
  }

  return retval;

}
