//-----------------------------------------------------------------------+
// File         :  chan_stat.c                                           |
//                                                                       |
// Description  :  This file contains three functions -                  |
//                 set_chan_stat -which sets input signal threshold      |
//                                status bits for a given channel        |
//                 get_chan_stat -which returns a channel flag based on  |
//                                bits set in the status word            |
//                                                                       |
// Return Value :   SET - status bit to add to the overall word          |
//                UNSET - clears a particular status bit                 |
//                  GET - "worst" status flag (ie, saturated is worse    |
//                       than an out-of-tolerance                        |
//                                                                       |
// Author       :  M. Palmer / M. Rendina                                |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"



#define BIT 0x01



//-------------------------------------------------------------------
// Bits are arranged as follows:
//  Each card has its own isolated word for holding channel status
//  bitfields.
//              
//   ...       |     Timing Block 1    |     Timing Block 0    |
//   ...       | Chan-N | ... | Chan-0 | Chan-N | ... | Chan-0 |
//
// i.e. For a device with one channel per timing block per card,
//       ( 8 channels total)
//      Timing Block 0, channel 0 flagged would be,
//                              00000001
//      Timing Block 1, channel 0 flagged would be
//                              00000010
//
//
//      For a device with 8 channels in a single timing block per card,
//        ( 32 channels total)
//      Timing Block 0, channel 0 flagged would be,
//                              00000001
//      Timing Block 0, channel 6 flagged would be
//                              01000000
//-------------------------------------------------------------------
int set_chan_stat(int timing_block, int chan) {

   #if defined(CBPM_INST_PLATFORM)
   INST_stat.trace[INST_stat.num_levels++] = TSX_CHAN_STAT_ID;
   #endif
   
   int word = 0;
   word =  BIT << ((timing_block * CBPM_MAX_CARD_ADCS_PER_BLOCK) + chan);

   #if defined(CBPM_INST_PLATFORM)
     INST_stat.num_levels--; 
   #endif

   return word;
} 



// Unset the given bit in the channel status word that gets passed in.
int unset_chan_stat(int word, int timing_block, int chan) {
		
   #if defined(CBPM_INST_PLATFORM)
     INST_stat.trace[INST_stat.num_levels++] = TSX_CHAN_STAT_ID;
   #endif
   	
   
	int bitmask = BIT << ((timing_block * CBPM_MAX_CARD_ADCS_PER_BLOCK) + chan);
	
	word =  word & ~bitmask;
	
		
   #if defined(CBPM_INST_PLATFORM)
     INST_stat.num_levels--; 
   #endif
   	
	return word;
}



int get_chan_stat(int word, int timing_block, int chan) {

  #if defined(CBPM_INST_PLATFORM)
  INST_stat.trace[INST_stat.num_levels++] = TSX_CHAN_STAT_ID;
  #endif
  
  int mask = ( BIT << ((timing_block * CBPM_MAX_CARD_ADCS_PER_BLOCK) + chan) );


   #if defined(CBPM_INST_PLATFORM)
     INST_stat.num_levels--; 
   #endif
   
   if ( (mask & word) > 0 ) {
      return 1;
   } else {
      return 0;
   }
   
} 





