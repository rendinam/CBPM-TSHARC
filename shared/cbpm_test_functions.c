//-----------------------------------------------------------------------+
// File         :  cbpm_test_functions.c                                 |
//                                                                       |
// Description  :  This file contains four functions -                   |
//				   setbits		 -which sets bits at a specified offset  |
//																		 |
//				   getbits		 -which gets a specified number of bits  |
//								   at a specified offset			     |
//				   encode_status - take a 2 dimensional array of  		 |
//					status values: 0:3 and converts into to a 			 |
//					single array of 32bit integers						 |
//                                                                       |
// Return Value :   SET - the changed 32bit word			             |
//                  GET - the specificed number of bits at the specified | 
//						offset											 |
//                  ENCODE, DECODE - none								 |
//                                                                       |
// Author       :  M. Stedinger			                                 |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"
#include "math.h"


//-------------------------------------------------------------------
// Bits are arranged as follows:
//              
// First byte of first 32bit word:
//  |     Test     0                                            |
//  | Card3 2 bits | Card2 2 bits | Card1 2 bits | Card0 2 bits |
//
// Second byte of first 32bit word:
//  |     Test     1                                            |
//  | Card3 2 bits | Card2 2 bits | Card1 2 bits | Card0 2 bits |
//	. . .
// First byte of second 32bit word:
//  |     Test     5                                            |
//  | Card3 2 bits | Card2 2 bits | Card1 2 bits | Card0 2 bits |
//  . . .
//
//-------------------------------------------------------------------

unsigned int set_bits(unsigned int word, int offset, unsigned int input) {

   #if defined(CBPM_INST_PLATFORM)
   INST_stat.trace[INST_stat.num_levels++] = SET_BITS_ID;
   #endif
   	
	unsigned int compose = input << offset;
	unsigned int val = word | compose;

   #if defined(CBPM_INST_PLATFORM)
     INST_stat.num_levels--; 
   #endif	
	return val;
}



unsigned int get_bits(unsigned int word, int offset, int numbits) {

   #if defined(CBPM_INST_PLATFORM)
   INST_stat.trace[INST_stat.num_levels++] = GET_BITS_ID;
   #endif
   	
	unsigned int bitmask = (1 << numbits) - 1;
	bitmask = bitmask << offset;
	unsigned int val = word & bitmask;
	val = val >> offset;
	
   #if defined(CBPM_INST_PLATFORM)
     INST_stat.num_levels--; 
   #endif	
	return (int) val;
}



/* void encode_status(int Status[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS], int EncodedStatus[CBPM_NUM_TESTS_ENCODED]) {

	int test, card;
	
   #if defined(CBPM_INST_PLATFORM)
   INST_stat.trace[INST_stat.num_levels++] = ENCODE_STAT_ID;
   #endif

	for(test = 0; test<CBPM_NUM_TESTS_ENCODED; test++) {
		EncodedStatus[test] = 0;
	}

	for (test = 0; test<CBPM_NUM_TESTS; test++) {

		int wordNum = floor(test/4);
		int offset = (test*8) % 32;
		for (card = 0; card<4; card++) {
			EncodedStatus[wordNum] = set_bits(EncodedStatus[wordNum],card*2+offset,Status[test][card]);
		}
	}

   #if defined(CBPM_INST_PLATFORM)
     INST_stat.num_levels--; 
   #endif
   return;	
}




void decode_status(int EncodedStatus[CBPM_NUM_TESTS_ENCODED], int Status[CBPM_NUM_TESTS][CBPM_MAX_ACQ_CONTROLLERS]) {

	int test, card;

   #if defined(CBPM_INST_PLATFORM)
   INST_stat.trace[INST_stat.num_levels++] = DECODE_STAT_ID;
   #endif

   	for(card= 0; card < CBPM_MAX_CARDS; card++) {
		for(test = 0; test < CBPM_NUM_TESTS; test++) {
			Status[test][card] = 0;
		}
	}

	for ( test = 0; test<CBPM_NUM_TESTS; test++) {
        	int wordNum = floor(test/4);
        	int offset = (test*8) % 32;
        	for ( card = 0; card<4; card++) {
            		Status[test][card] = (int) get_bits(EncodedStatus[wordNum],card*2+offset,2);
        	}
    	}

   #if defined(CBPM_INST_PLATFORM)
     INST_stat.num_levels--; 
   #endif
   return;	
} */
