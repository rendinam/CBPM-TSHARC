//-----------------------------------------------------------------------+
// File         :  sleep_n_turns.c                                       |
//                                                                       |
// Description  :  Causes the instrument to wait for the number of turns |
//                 specified to elapse.                                  |
//                                                                       |
// Arguments    :  nsturns - number of turns to wait                     |
//                                                                       |
// Author       :  E.Tanke                                               |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int sleep_n_turns(int nsturns) {  
	
   int status= CBI_F_SUCCESS;
   int gt_count, lc_loop;

   gt_count= *p_turn_counter;

   for (lc_loop = 0; lc_loop < nsturns ; lc_loop++) {
     while(1){
       if(*p_turn_counter != gt_count) break;
     }
     gt_count= *p_turn_counter;
   }
   
   return status;
}
