/*!-----------------------------------------------------------------------+
* File         :  cbpm_query_pattern_type.c                               |
*                                                                         |
* Description  :  Prompt to get the type of bunch pattern entry method.   |
*                 Allows for three type of bunch pattern entry:           |
*                  1) Individual bunches to be selected one at at time    |
*                  2) A range of bunches to be selected a single time     |
*                  3) Load a pre-defined bunch pattern from a list        |
*                                                                         |
* Arguments    :  None                                                    |
*                                                                         |
* Author       :  M. Rendina                                              |
*-------------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_query_pattern_type( int pattern_array[] ) {


  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  int  inst, bpm, retval;
  char idx1, idx2;
  int  input;

  //-------------------------------------------------
  // Define the list of prompts and their parameters
  //-------------------------------------------------
  CBI_INIT_PROMPTS(1);

  int valid_inputs[5] = {4, (int)'l', (int)'i', (int)'r', (int)'c'};

  prompts[0].supp_text_ptr    = "   (l)oad predefined, \n"
                                "   (i)ndividual bunch selection,\n"
                                "   (r)ange of bunches \n";
  prompts[0].prompt_text      = "Specify the bunch pattern entry method:  ";
  prompts[0].data_type        = CBI_CHAR;
  prompts[0].valid_inputs     = valid_inputs;
  prompts[0].map_out          = NULL;
  prompts[0].default_value    = 'l';
  prompts[0].destination      = &(input);
  prompts[0].last_val_default = CBI_FALSE;


  retval = CBI_PROMPTER();

  printf("input = %c\n", input);

  switch ((char)input) {

  case 'l':    case 'L':
    //cbi_select_saved_pattern( pattern_array );
    cbi_select_saved_pattern_cat( pattern_array );
    break;

  case 'i':    case 'I':
    cbpm_query_bunches( pattern_array );
    break;

  case 'r':    case 'R':
    cbpm_query_bunch_range( pattern_array );
    break;

  default:
    break;
  }


  return retval;
  
}
