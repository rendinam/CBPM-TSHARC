//-----------------------------------------------------------------------+
// File         :  publish_temps.c                                       |
//                                                                       |
// Description  :  Function to read all available temperature sensors,   |
//                 convert their output to degrees centigrade and store  |
//                 their data in the temperature output struct.          |
//                                                                       |
// Author       :  M.Rendina                                             |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific

#include "cbpm_inst_includes.h"


int publish_temps() {
    
    int card, acq_con, i, j=0;
    
    // Intiate temperature reading refresh
    *(volatile unsigned int *)(TEMPERATURE_CSR) = 1;
    
    // Wait for temperature read to complete
    while( *(volatile unsigned int *)(TEMPERATURE_CSR) % 2 != 0 ) { }
    
      INST_temperatures.dig_temp = ((float) VAL_AT(DIGITAL_TEMP)) / 4.0; // In degrees C
      INST_temperatures.tim_temp = ((float) VAL_AT(TIMING_TEMP))  / 4.0;
      
      // Write to each FE-board temperature register to request an update of registers
      for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
          *p_acq_temperature[acq_con] = 10;
      }


      // Wait a moment for update to complete
      magic_wait( 2000 );

      
      // Copy temperature values (div by 4 to get C) into temperatures structure.
      for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
          INST_temperatures.fe_temps[acq_con] = ((float) (*p_acq_temperature[acq_con]) / 4.0);
      }
      
      
    return CBI_F_SUCCESS;
}

