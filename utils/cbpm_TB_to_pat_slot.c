/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_TB_to_BPslot.c                                   |
*                                                                       |
* Description  :  Maps a train number and bunch number combination to a |
*                 bunch pattern register slot.                          |
*                 Returns the slot number as an integer.                |
*                                                                       |
* Arguments    :  Train number                                          |
*                 Bunch number                                          |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_TB_to_BPslot( int train, int bunch ) {

  int BP_slot;

  BP_slot = (train-1)*20 + (int)((train-1)/3) + bunch - 1;

  return BP_slot;
  
}
