//-----------------------------------------------------------------------+
// File         :  cbpm_query_bunches.c                                  |
//                                                                       |
// Description  :  Prompts user on the console to provide a bunch        |
//                 pattern via various means, and stores this in the     |
//                 pattern array passed in.                              |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M.Rendina                                             |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int tsetup_num_bunches_map[CBPM_MAX_TIMING_SETUPS] = {CBPM_4NS_MAX_BUNCHES,
						      CBPM_4NS_MAX_BUNCHES,
						      CBPM_14NS_MAX_BUNCHES/2,
						      CBPM_4NS_MAX_BUNCHES,
						      CBPM_4NS_MAX_BUNCHES,
						      CBPM_14NS_MAX_BUNCHES/2  };

int cbpm_query_bunch(int tsetup, int species, int pattern_array[] ) {

  // Wipe bunch pattern before starting
  cbi_clear_bp( pattern_array );
  
  int bit_offset = 0;
  int multiplier = 1;
  int bunch_requested, bunch_idx_to_set;
  int retval;
  

  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(1);

  int range[2] = {1, tsetup_num_bunches_map[tsetup]};
  char prompt[50];
  sprintf(prompt, "Which bunch do you want to examine (1-%d) ", tsetup_num_bunches_map[tsetup] );
  prompts[0].supp_text_ptr    = NULL;
  prompts[0].prompt_text      = prompt;
  prompts[0].data_type        = CBI_INT;
  prompts[0].validator        = cbi_int_range_validator;
  prompts[0].valid_inputs     = range;
  prompts[0].default_value    = 1;
  prompts[0].destination      = &(bunch_requested);
  prompts[0].last_val_default = CBI_TRUE;
  

  retval = CBI_PROMPTER();


  if ( (tsetup == s14ns_VAR_G || tsetup == s14ns_FIX_G) &&
       (species == ELECTRONS)                         ) {
    multiplier = 2;
    bit_offset = 1;
  }
  bunch_idx_to_set = bunch_requested + bit_offset;
  cbi_set_bunch( pattern_array, bunch_idx_to_set );
    
  return bunch_requested;
}




int cbpm_query_bunch_range(int pattern_array[]) {

  int bbucket = -1;
  int i = 0;
  int k;
  int retval;

  // Wipe bunch pattern before starting
  cbi_clear_bp( pattern_array );


  int x = 0;
  int y = 0;
  int pos = 0;


  int numbuckets;

  numbuckets = 640;

  int begin, end;

  printf("range query\n");

  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(2);

  int range[2] = {1, 640};

  prompts[0].supp_text_ptr    = "Which bunch range do you want to examine?\n";
  prompts[0].prompt_text      = "Begin : ";
  prompts[0].data_type        = CBI_INT;
  prompts[0].validator        = cbi_int_range_validator;
  prompts[0].valid_inputs     = range;
  prompts[0].default_value    = 1;
  prompts[0].destination      = &(begin);
  prompts[0].last_val_default = CBI_TRUE;


  prompts[1].supp_text_ptr    = NULL;
  prompts[1].prompt_text      = "End   : ";
  prompts[1].data_type        = CBI_INT;
  prompts[1].validator        = cbi_int_range_validator;
  prompts[1].valid_inputs     = range;
  prompts[1].default_value    = 2;
  prompts[1].destination      = &(end);
  prompts[1].last_val_default = CBI_TRUE;


  retval = CBI_PROMPTER();


  if (begin >= end) {
    cbi_set_bunch( pattern_array, begin);
  } else {
    printf("begin = %d         end = %d\n", begin, end);
    for (i = begin; i <= end; i++) {
      cbi_set_bunch( pattern_array, i );
    }
  }
  
  

  return CBI_F_SUCCESS;

}



int cbpm_query_bunches(int pattern_array[]) {

  int bbucket = -1;
  int i = 0;
  int k;


  // Wipe bunch pattern before starting
  cbi_clear_bp( pattern_array );


  int numbuckets;
  numbuckets = 640;
  

  printf("\n\nEnter the number of each bunch you wish to examine, \n");
  printf("each followed by a carriage return.\n");
  printf("Note: Bunches for two species are INTERLEAVED.\n");
  printf("      i.e. For e+ bunches 1,2 & 3  one would enter\n");
  printf("           1 <CR>  3<CR>  5<CR>  -1<CR>\n");
  printf("Enter -1 to end input session.\n");
  while (TRUE) {
    printf("> ");
    bbucket = cbi_get_int();
    if (bbucket == -1) return bbucket;
    cbi_set_bunch( pattern_array, bbucket );
  }
  printf("Done.\n");
  

  return bbucket;

}

