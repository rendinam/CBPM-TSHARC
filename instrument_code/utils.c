//-----------------------------------------------------------------------
// File         :  utils.c
//
// Description  :  Various utility functions:
//                 Contains:
//
//                 set_block_delay
//                 correct_value  
//                 unpack
//                 pack
//                 bitval
//                 parity
//                 setbit
//                 unsetbit
//                 sign_extend
//                 memfirsts
//                 magic_wait
//                 set_flag0_low
//                 set_flag0_high
//                 roundf
//
// Contributors :  M. Rendina
//-----------------------------------------------------------------------

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


//--------------------------------------------------------------------
// set_block_delay
//
// Sets the twin, chained global delay registers for a given timing
// block, taking care of dual register spillover automatically.
//
// block: 0 = A
//        1 = B
// value  : number of 10ps increments required to obtain total delay.
//--------------------------------------------------------------------
int set_block_delay(int block, int value) {
  if (value > 1023) {
   	 *p_block_delays[block][0] = 1023;           // All 10 bits set.
     *p_block_delays[block][1] = value - 1023;   // leftovers.
  } else {
   	 *p_block_delays[block][0] = value;  // Does not spill over into second register.
   	 *p_block_delays[block][1] = 0;
  }
  return CBI_F_SUCCESS;  
}



//--------------------------------------------
// correct_value
//
// Accepts a 32-bit floating point value,
// and the timing block and channel from
// which it originated.  Then this applies 
// either pedestal subtraction, gain scaling, 
// or both before returning the corrected 
// value.
//---------------------------------------------
float correct_value( float value, int mode, int tsetup, int gain_setting, int tblock, int card, int chan ) {
	
	
   if (mode == PED_SUB_DATA || mode == PED_SUB_SCALED_DATA) {
      value = value - INST_pedestal_config.tables[tsetup/CBPM_MAX_GAIN_MODES].ped_table[tblock][card][gain_setting];
   }
	
   if (mode == SCALED_DATA || mode == PED_SUB_SCALED_DATA) {
      value = value * (1.0 / INST_gain_config.gain_table[tblock][card][gain_setting]);
   }
	
   return value;
}




//---------------------------------------
// unpack
//
// Returns the requested 8-bit
// segment of a 32-bit word that is
// passed in.
// Index 0 is LSByte   3 is MSByte
//---------------------------------------
int unpack( int word, int byte_idx ) {
	
	int mask = 0x000000FF;
	mask = mask << (byte_idx*8);
	
	return ( (word & mask) >> (byte_idx*8) );
}



//---------------------------------------
// pack
//
// Inserts a given character (byte)
// into the byte-idx position of the 
// 32-bit word that is passed in.
// Returns the modified word.
//
// Index 0 is LSByte   3 is MSByte
//---------------------------------------
int pack( int word, int byte_idx, char c ) {
	
	word = word | (c << byte_idx*8);
	return word;
}
	

//---------------------------------------
// bitval
//
// Returns the value of the bit at the 
// given position in the provided word. 
//---------------------------------------
int bitval( int word, int bit ) {
	
  int bitmask = 1;
  int value = 0;

  bitmask = ( bitmask << bit );
  value = (word & bitmask);

  if (value > 0) {
    return 1;
  } else {
    return 0;
  }

}



//----------------------------
// parity
//
// Returns 0 for EVEN parity
//         1 for  ODD parity
//----------------------------
int parity( int number ) {
	return (number % 2);
}


//---------------------------------------
// setbit
//
// Set a particular bit in the specified
// word high, return the changed value.
//---------------------------------------
int setbit( int word, int bit ) {
	
	int i;
	int compose = 1;
	
    compose = compose << bit;

	word = word | compose;
	
	return word;

}


//---------------------------------------
// unsetbit
//
// Set a particular bit in the specified
// word low, return the changed value.
//---------------------------------------
int unsetbit( int word, int bit ) {
	
	int i;
	int compose = 1;
	
    compose = compose << bit;

	word = word & ~compose;
	
	return word;

}	



//---------------------------------------
// sign_extend
//
// Low-level hardware sign-extension fix:
//
//  Arguments:  Value to read from 
//              custom hardware register
//              or data location.
//
// Returns 32-bit sign-extended version
// of the input.  Use this to guard 
// against invalid reads from hardware
// subsystems in an instrument.
//---------------------------------------
int sign_extend( int input ) {
	
	int temp;
	
	temp = input;
	if ( temp & 0x00008000 ) {
       temp = (temp | 0xffff0000);
    } else {
       temp = (temp & 0x0000ffff);
    }

    return temp;
}



//---------------------------------------
// memfirsts
//
// Sets the value of the mem_first
// registers on all four front-end cards.
//---------------------------------------
int memfirsts( int offset ) {

	short int lower, upper;
	int temp, acq_con;
	
	lower = offset & 0x0000FFFF;
	temp  = offset & 0xFFFF0000;
	upper = temp >> 16;
	
	for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
	   *p_acq_mem_first[acq_con][0] = lower;
	   *p_acq_mem_first[acq_con][1] = upper;
	}
	
	return CBI_F_SUCCESS;
}
	

//-----------------------------------------
// Magic wait:  Useful for providing
//  a controllable delay.
//
//  Arguments:  number of iterations
// 
// This has been calibrated roughly with 
// a digital oscilloscope to give real-time
// equivalents.  Repeatability was within
// a few nanoseconds.
//
// --------------------------------
//  Argument  Measured Delay (ns)
// --------------------------------
//     0          220
//     1          260
//     2          320
//     3          380
//     4          440
//     5          480
//    10          760
//    20         1280  ( 1.28us)
//    50         2880  ( 2.88us)
//   100         5520  ( 5.52us)
//  1000        53200  (53.20us)
// The code used to produce this behavior
// must not change to retain the accuracy
// of the above table.  It is reproduced
// here for reference.
//-----------------------------------------
// void magic_wait( int iter ) {
//	
//	int waiter   = 0;
//	int wcounter = 0;
//	
//  for (waiter = 0; waiter < iter; waiter ++) { 
//       wcounter++; // Something simple to do
//       wcounter--;
//    }
//
//}
//-----------------------------------------
void magic_wait( int iter ) {
	
	int waiter   = 0;
	int wcounter = 0;
	
    for (waiter = 0; waiter < iter; waiter ++) { 
       wcounter++; // Something simple to do
       wcounter--;
    }

}



//--------------------------------------
// set_flag0_low
//
// Sets a flag output pin HIGH
//--------------------------------------
int set_flag0_low( void ) {

   int temp = 0;
   int f0_out_bitmask = MAKE_BITMASK_(SQCTL_FLAG0_OUT_P);
		
   temp = __builtin_sysreg_read( __SQCTL );
   temp = (temp & (~f0_out_bitmask) );
   __builtin_sysreg_write( __SQCTL, temp );
   
   return CBI_F_SUCCESS;
}




//--------------------------------------
// set_flag0_high
//
// Sets a flag output pin HIGH
//--------------------------------------
int set_flag0_high( void ) {
	
   int systemp = 0;
   int f0_out_bitmask = MAKE_BITMASK_(SQCTL_FLAG0_OUT_P);
   
   systemp = __builtin_sysreg_read( __SQCTL );
   systemp = (systemp | f0_out_bitmask);
   __builtin_sysreg_write( __SQCTL, systemp );

   return CBI_F_SUCCESS;
}

//--------------------------------------
// Round
//
// rounds a float to an integer
//--------------------------------------
float roundf(float val)
{
   if (val>=0)
      return val + 0.5;
   else
      return val - 0.5;
      
}

//--------------------------------------
// Clipf
//
// clips a float between max and min
//--------------------------------------
float clipf(float val, float min, float max)
{
   if (val<min)
      return min;
   else if (val>max)
      return max;
   else
   	  return val;
      
}

//--------------------------------------
// Clipi
//
// clips a int between max and min
//--------------------------------------
int clipi(int val, int min, int max)
{
   if (val<min)
      return min;
   else if (val>max)
      return max;
   else
   	  return val;
      
}

