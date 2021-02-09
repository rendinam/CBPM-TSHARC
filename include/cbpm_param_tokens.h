#ifndef CBPM_PARAM_TOKENS_H
#define CBPM_PARAM_TOKENS_H

#define CBPM_MAX_TIM_TOKENS_PER_SETUP   5
#define CBPM_MAX_PED_TOKENS_PER_SETUP   4


#if defined (CBI_TSHARC_PARAM_TOKENS_NO_EXTERN)


char timing_tokens[CBPM_MAX_TIMING_SETUPS][CBPM_MAX_TIM_TOKENS_PER_SETUP][50] = {
  "4_NS_e+_BUNCH_PAT_OFFSETS",     // 0 -- Entries for 4ns POSITRONS
  "4_NS_e+_COM_TURNMARK_DELAY",    // 1
  "4_NS_e+_BLOCK_TURNMARK_DELAYS", // 2
  "4_NS_e+_GLOBAL_DELAYS",         // 3
  "4_NS_e+_CHAN_DELAYS",           // 4

  "4_NS_e-_BUNCH_PAT_OFFSETS",     // 0 -- Entries for 4ns ELECTRONS
  "4_NS_e-_COM_TURNMARK_DELAY",    // 1
  "4_NS_e-_BLOCK_TURNMARK_DELAYS", // 2
  "4_NS_e-_GLOBAL_DELAYS",         // 3
  "4_NS_e-_CHAN_DELAYS",           // 4

  "14_NS_BUNCH_PAT_OFFSETS",       // 0 -- Entries for 14ns DUAL SPECIES
  "14_NS_COM_TURNMARK_DELAY",      // 1
  "14_NS_BLOCK_TURNMARK_DELAYS",   // 2
  "14_NS_GLOBAL_DELAYS",           // 3
  "14_NS_CH_DELAYS",               // 4

  "4_NS_e+_FIX_BUNCH_PAT_OFFSETS",     // 0 -- Entries for 4ns POSITRONS FIXED GAIN
  "4_NS_e+_FIX_COM_TURNMARK_DELAY",    // 1
  "4_NS_e+_FIX_BLOCK_TURNMARK_DELAYS", // 2
  "4_NS_e+_FIX_GLOBAL_DELAYS",         // 3
  "4_NS_e+_FIX_CHAN_DELAYS",           // 4

  "4_NS_e-_FIX_BUNCH_PAT_OFFSETS",     // 0 -- Entries for 4ns ELECTRONS FIXED GAIN
  "4_NS_e-_FIX_COM_TURNMARK_DELAY",    // 1
  "4_NS_e-_FIX_BLOCK_TURNMARK_DELAYS", // 2
  "4_NS_e-_FIX_GLOBAL_DELAYS",         // 3
  "4_NS_e-_FIX_CHAN_DELAYS",           // 4

  "14_NS_FIX_BUNCH_PAT_OFFSETS",       // 0 -- Entries for 14ns DUAL SPECIES FIXED GAIN
  "14_NS_FIX_COM_TURNMARK_DELAY",      // 1
  "14_NS_FIX_BLOCK_TURNMARK_DELAYS",   // 2
  "14_NS_FIX_GLOBAL_DELAYS",           // 3
  "14_NS_FIX_CH_DELAYS"                // 4
};



char pedestal_tokens[CBPM_MAX_TIMING_SETUPS][CBPM_MAX_PED_TOKENS_PER_SETUP][50] = {
  "4_NS_e+_PED_TABLE_0", // 0 -- Entries for 4ns POSITRONS
  "4_NS_e+_PED_TABLE_1", // 1
  "4_NS_e+_PED_RMS_0",   // 2
  "4_NS_e+_PED_RMS_1",   // 3

  "4_NS_e-_PED_TABLE_0", // 0 -- Entries for 4ns ELECTRONS
  "4_NS_e-_PED_TABLE_1", // 1
  "4_NS_e-_PED_RMS_0",   // 2
  "4_NS_e-_PED_RMS_1",   // 3

  "14_NS_PED_TABLE_0",    // 0 -- Entries for 14ns DUAL SPECIES
  "14_NS_PED_TABLE_1",    // 1
  "14_NS_PED_RMS_0",      // 2
  "14_NS_PED_RMS_1",      // 3

  "4_NS_e+_PED_TABLE_0", // 0 -- Entries for 4ns POSITRONS FIXED GAIN
  "4_NS_e+_PED_TABLE_1", // 1    Same destination arrays as variable gain.
  "4_NS_e+_PED_RMS_0",   // 2
  "4_NS_e+_PED_RMS_1",   // 3

  "4_NS_e-_PED_TABLE_0", // 0 -- Entries for 4ns ELECTRONS FIXED GAIN
  "4_NS_e-_PED_TABLE_1", // 1
  "4_NS_e-_PED_RMS_0",   // 2
  "4_NS_e-_PED_RMS_1",   // 3

  "14_NS_PED_TABLE_0",    // 0 -- Entries for 14ns DUAL SPECIES FIXED GAIN
  "14_NS_PED_TABLE_1",    // 1
  "14_NS_PED_RMS_0",      // 2
  "14_NS_PED_RMS_1",      // 3
};



#else

extern char timing_tokens[CBPM_MAX_TIMING_SETUPS][CBPM_MAX_TIM_TOKENS_PER_SETUP][50];

extern char pedestal_tokens[CBPM_MAX_TIMING_SETUPS][CBPM_MAX_PED_TOKENS_PER_SETUP][50];

#endif // end EXTERN block


#endif // CBPM_PARAM_TOKENS_H
