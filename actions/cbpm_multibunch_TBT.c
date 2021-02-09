/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_multibunch_TBT.c                                 |
*                                                                       |
* Description  :  Command for allowing one to take multibunch           |
*                 turn-by-turn data with arbitrary bunch patterns
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"


int cbpm_install_TBT_pattern(void) {

  // Set a pointer to the data substructure of the temp master struct tree
  CBPM_DATA *dp;
  dp = CTL_System.p_Module_temp->dsp_data;

  char input, retval;

  printf(" == Multibunch Turn-By-Turn Data Acquisition ==\n\n");

  // Read previously-stored bunch pattern from disk.
  int *pattern_array = CTL_System.meas_info.p_bunch_pattern;
  cbi_select_saved_pattern_cat( pattern_array );

  // Write the selected bunch pattern into the shared multibunch bunch pattern
  // file for use by simultaneous decoupled server instances.
    char *func_name = (char *)__FUNCTION__;

  int BP_array[4][10];
  int retcode, row, col, i = 0;
  int ele;
  FILE *fp;

  char *BP_category  = "multibunch_for_servers";
  int   BP_entry_num = 1;
  char *BP_name      = "pattern";

  printf("     Category  = %s\n", BP_category);
  printf("     Entry num = %d\n", BP_entry_num);
  printf("     Name      = %s\n", BP_name);
  
  for ( row = 0; row < 4; row++) {
    for (col = 0; col < 10; col++) {
      BP_array[row][col] = pattern_array[i];
      printf("%x ", BP_array[row][col] );
      i++;
    }
    printf("\n");
  }
  printf("\n");

  char element[5];
  char tempname[50];

  printf("Writing to file...\n");

  // Open file for overwrite.
  fp = fopen( "/nfs/cesr/online/instr/CBPM/ops/multibunch_server_pattern.cfg", "r+" );
  
  printf("        pattern name...\n");

  strcpy(element, "");
  sprintf(element, "C%d", BP_entry_num);
  //printf("Element = %s\n", element);
  retcode = faccess( FA_WRITE, fp, BP_category, element, 1, 1, STRING, BP_name );
  if (retcode != 0) {
    return CBI_F_FAILURE;
  }

  printf("        bunch pattern...\n");

  strcpy(element, "");
  sprintf(element, "%d", BP_entry_num);
  //printf("Element = %s\n", element);
  retcode = faccess( FA_WRITE, fp, BP_category, element, 4, 10, HEXINT, BP_array );
  if (retcode != 0) {
    return CBI_F_FAILURE;
  }

  fclose(fp);

  printf("\n\n");
  printf("*******************************************************************************\n");
  printf("  Bunch pattern has been installed for server use.\n" );
  printf("*******************************************************************************\n");


/*   //------------------------------------------------- */
/*   // Define the list of prompts and their parameters */
/*   //------------------------------------------------- */
/*   CBI_INIT_PROMPTS(1); */
/*   int num_turns = -1; */

/*   int range[2] = {1, 320000}; */
/*   prompts[0].supp_text_ptr    = NULL; */
/*   prompts[0].prompt_text      = "Number of turns to acquire: "; */
/*   prompts[0].data_type        = CBI_INT; */
/*   prompts[0].valid_inputs     = range; */
/*   prompts[0].default_value    = 1024; */
/*   prompts[0].destination      = &(num_turns); */
/*   prompts[0].last_val_default = FALSE; */

/*   retval = CBI_PROMPTER(); */

/*   fflush(stdout); */


  // Make call to standalone TBT request program, passing the number of
  // turns as an argument.
  
  return CBI_F_SUCCESS;


}

