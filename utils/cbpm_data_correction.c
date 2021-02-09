//-----------------------------------------------------------------------+
// File         :  cbpm_data_correction.c                                |
//                                                                       |
// Description  :  Accepts a 32-bit floating point value, and the timing |
//                 block and channel from which it originated, then      |
//                 applies either pedestal subtraction, gain scaling, or |
//                 both before returning the corrected value.            |
//                 the provided word.                                    |
//                                                                       |
// Arguments    :  Pointer to platform-specific substructure tree for an |
//                 instrument.                                           |
//                                                                       |
// Author       :  M.Rendina                                             |
//                   FIXME TODO: Integrate into 'shared' directory of    |
//                               platform-specific code.                 |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


float cbpm_correct_value( CBPM_DATA *dp, float value, int mode, int gain_setting, int tblock, int card, int chan ) {
	
  int tsetup = dp->cbpm_op_timing.active_timing_setup;

  int ped_table_set;
  if (tsetup >= CBPM_MAX_MULTIGAIN_SETUPS) {
    ped_table_set = tsetup - CBPM_MAX_MULTIGAIN_SETUPS;
  } else {
    ped_table_set = tsetup;
  }

   if (mode == PED_SUB_DATA || mode == PED_SUB_SCALED_DATA) {
     value = value - dp->cbpm_pedestal_config.tables[ped_table_set].ped_table[tblock][card][gain_setting];
   }
	
   if (mode == SCALED_DATA || mode == PED_SUB_SCALED_DATA) {
      value = value * (1.0 / dp->cbpm_gain_config.gain_table[tblock][card][gain_setting]);
   }
	
   return value;
}
