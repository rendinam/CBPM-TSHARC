//-----------------------------------------------------------------------+
// File         :  float_err_check.c                                     |
//                                                                       |
// Description  :  Provides routines to clear the "sticky" error bits,   |
//                 and to test the bits and set an error flag.           |
//                                                                       |
//                                                                       |
//-----------------------------------------------------------------------+
//#include "sysreg.h"  // Processor-specific system register address table
                       // Provided by Analog Devices
#include <defts101.h>
#include <builtins.h>  // For sysreg_read/write functions
                     
// Make global parameters available.
#include "cbi_core_inst_includes.h"
#include "cbpm_inst_includes.h"

void float_err_clear(void)
{
   int stky_dat;

   // Set our routine ID
   INST_stat.trace[INST_stat.num_levels++] = FLOAT_ERR_CLEAR_ID;

   // clear all bits, even the "read-only" bits 26..21
  //asm volatile ("R0 = 0x0;;");
  asm volatile ("R0 = 0x0;;xstat = R0;;");   

   // Decrement the trace
   INST_stat.num_levels--;

   return;
}



int float_err_check()
{
   volatile int *p_error = INST_cmd.error;
   int stky_dat = 0;
   int status = CBI_F_SUCCESS;

   // Set our routine ID
   int FID = FLOAT_ERR_CHECK_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;

   // read the XSTAT status register
	asm volatile("%0 = xstat;;" : "=x" (stky_dat) );   
  
   //       3         2          1
   //bit#: 1098765432109876 5432109876543210
   //       errors in XSTAT
   //         F   F   7   0    F   0   C   4 
   //      1111111101110000 1111000011000100
       
   //      sticky errors in XSTAT,   AI is NOT sticky
   //      0x F   F   0   0    8   0   0   0
   //      1111111100000000 1000000000000000
       
   //      Current error mask used
   //      0xAA001000
   //         A   A   0   0    1   0   0   0
   //      1010101000000000 0001000000000000
   //bit#: 1098765432109876 5432109876543210
   //       3         2         1         0

   // TSHARC MASK covers the following bits:
   // MIS - Multiplier floating-pt. invalid operation, sticky, bit 31
   // MVS - Multiplier floating-pt. overflow,          sticky, bit 29
   // AIS - ALU floating-pt.        invalid operation, sticky, bit 27
   // AVS - ALU floating-pt.        overflow,          sticky, bit 25
   //
   // AI  - ALU Floating-point invalid operation,      NOT sticky, bit 12
   
   // Documentation: https://wiki.lepp.cornell.edu/lepp/pub/ACC/Bunch/Hardware/ts101_programming_manual.pdf
   //                http://www.analog.com/static/imported-files/processor_manuals/34851204876822ts101_pgr.pdf
   //
   //          3         2         1         0
   //         10987654321098765432109876543210
   // mask    10101010000000000001000000000000
   //
   if ( (stky_dat & 0xAA001000) != 0) {
      // one or more error bits in mask detected
      
      //clear the error
      float_err_clear();
      
      //then report the error found
      add_debug( FID, 3545, CBI_DEBUG_LEVEL_0 );

      int i;
      for (i = 31; i>=0; i--) {
         
         //output to debug struct the bits set in XSTAT register
         if (stky_dat & 1<<i) {
            add_debug( FID, i, CBI_DEBUG_LEVEL_0 ); 
         }
      }
      INST_code_params.execution_flags[OUTPUT_ALU_ERROR_BITS] = stky_dat;
      
      add_debug( FID, -3545, CBI_DEBUG_LEVEL_0 );   

      cbpm_set_error(INST_MATH_ERR, p_error);
      
      status = CBI_F_FAILURE;
   }
   
   // Decrement the trace
   INST_stat.num_levels--;

   return status;
}
