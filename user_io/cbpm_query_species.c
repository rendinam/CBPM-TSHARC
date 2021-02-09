//-----------------------------------------------------------------------+
// File         :  cbpm_query_species.c                                  |
//                                                                       |
// Description  :  Allows for automatic determination or user entry of   |
//                 species.                                              |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_query_species( int timing_setup ) {

  int retval, species;

  
  CBI_INIT_PROMPTS(1);
  

  // Define the list of prompts and their parameters
  int range[2] = {0,1};
  prompts[0].supp_text_ptr    = NULL;
  prompts[0].prompt_text      = "Enter species [POSITRONS=0, ELECTRONS=1] ";
  prompts[0].data_type        = CBI_INT;
  prompts[0].validator        = cbi_int_range_validator;
  prompts[0].valid_inputs     = range;
  prompts[0].default_value    = 0;
  prompts[0].destination      = &(species);
  prompts[0].last_val_default = TRUE;


  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  switch(timing_setup) {
  case (s4ns_P_VAR_G):
    printf("Timing setup is for 4ns POSITRONS.\n");
    species = POSITRONS;
    prompts[0].active = FALSE;
    break;
  case (s4ns_E_VAR_G):
    printf("Timing setup is for 4ns ELECTRONS.\n");
    species = ELECTRONS;
    prompts[0].active = FALSE;
    break;
  case (s14ns_VAR_G):
    break;
  case (s4ns_P_FIX_G):
    printf("Timing setup is for 4ns POSITRONS.\n");
    species = POSITRONS;
    prompts[0].active = FALSE;
    break;
  case (s4ns_E_FIX_G):
    printf("Timing setup is for 4ns ELECTRONS.\n");
    species = ELECTRONS;
    prompts[0].active = FALSE;
    break;
  case (s14ns_FIX_G):
    break;
  }


  retval = CBI_PROMPTER();


  dp->cbpm_cmd_params.species          = species;
  printf("species = (%d) %s\n", species, cbi_species_names[species] );

  return CBI_F_SUCCESS;

}

