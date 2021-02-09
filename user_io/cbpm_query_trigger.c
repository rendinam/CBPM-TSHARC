//-----------------------------------------------------------------------+
// File         :  cbpm_query_trigger.c                                  |
//                                                                       |
// Description  :  Prompts the user on the console to enter the          |
//                 acquisition trigger type to use for the next          |
//                 acquisition.                                          |
//                                                                       |
// Arguments    :  Pointer to the 'dsp_data' substructure of an          |
//                 instrument master structure.                          |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


enum TRIG_OPTIONS {
  A_TRIG,
  B_TRIG,
  NO_TRIG
};


int cbpm_query_trigger( CBPM_DATA *dat_ptr ) {

  int input;
  int retval;
  int use_enable_gate = FALSE;
  dat_ptr->cbpm_cmd_params.use_data_enable = CBI_FALSE;

  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(1);

  int valid_inputs[4] = { 3, (int)'a', (int)'b', (int)'n' };
  int map_out[3]      = {    A_TRIG,   B_TRIG,   NO_TRIG};
  prompts[0].supp_text_ptr    = "   (a) 60Hz line-synchronous trigger \n"
                                "   (b) Injection trigger \n"
                                "   (n)o triggering \n";
  prompts[0].prompt_text      = "Select acquisition triggering type:  ";
  prompts[0].data_type        = CBI_CHAR;
  prompts[0].valid_inputs     = valid_inputs;
  prompts[0].map_out          = map_out;
  prompts[0].default_value    = 'n';
  prompts[0].destination      = &(input);
  prompts[0].last_val_default = CBI_FALSE;

  retval = CBI_PROMPTER();

  printf("input = %d\n", input );

  switch (input) {

  case (A_TRIG):
    printf("Setting hardware DATA ENABLE flag check in command parameters...\n\n");
    use_enable_gate = TRUE;
    dat_ptr->cbpm_cmd_params.use_data_enable  = TRUE;
    printf("Setting hardware   TRIGGER MASK A  (60Hz) in command parameters...\n\n");
    dat_ptr->cbpm_cmd_params.trig_mask        = TRIG_MASK_A;
    break;

  case (B_TRIG):
    printf("Setting hardware DATA ENABLE flag check in command parameters...\n\n");
    use_enable_gate = TRUE;
    dat_ptr->cbpm_cmd_params.use_data_enable  = TRUE;
    printf("Setting hardware   TRIGGER MASK B   (injection) in command parameters...\n\n");
    dat_ptr->cbpm_cmd_params.trig_mask        = TRIG_MASK_B;
    break;

  case (NO_TRIG):
    printf("Will immediately acquire the data with no trigger.\n\n");
    use_enable_gate = FALSE;
    dat_ptr->cbpm_cmd_params.use_data_enable  = FALSE;
    dat_ptr->cbpm_cmd_params.trig_mask        = 0;
    break;

  default:
    printf("ERROR: Problem processing prompt input.\n");
    break;

  }

  return use_enable_gate;

}
