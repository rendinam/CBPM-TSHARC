//-----------------------------------------------------------------------+
// File         :  acquisition_control.c                                 |
//                                                                       |
// Description  :  STARTS or FINALIZES a data acquisition operation by   |
//                 way of the digital board, which dispatches the start  |
//                 signal to all acquisition controllers. The parameters |
//                 governing the operation of which are assumed to have  |
//                 already been written to the relevant registers of     |
//                 each acquisition controller.                          |
//                                                                       |
// Arguments    :  ADC_CONTROL - Starts the acquisition                  |
//                 DSP_CONTROL - Finalizes the acquisition once all      |
//                               acquisition controllers have reported   |
//                               completion.                             |
//                                                                       |
// Author       :  M. Palmer / M. Rendina                                |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int acquisition_control(enum ADC_BUFFER_CONTROL control) {
	
   int i;
   int dsp_rw_control = 0XFFFFFFFF^ACQ_MODE_BIT; // Bitwise XOR
   
   
   if (control == ADC_CONTROL) {
   	  *p_ts_data_acq |= ACQ_MODE_BIT;
   	  
   } else if (control == DSP_CONTROL) { 
      *p_ts_data_acq &= dsp_rw_control;
      
   } else {
      return CBI_F_FAILURE;
   }

   
   return CBI_F_SUCCESS;
}

