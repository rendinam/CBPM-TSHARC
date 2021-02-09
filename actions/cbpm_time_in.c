//-----------------------------------------------------------------------+
// File         :  cbpm_time_in.c                                        |
//                                                                       |
// Description  :  A SEQUENCE-type command.                              |
//                 Puts an instrument through a complete 'timing-in'     |
//                 procedure in order to determine the full complement   |
//                 of timing parameters necessary for operation of the   |
//                 instrument.                                           |
//                                                                       |
//                 Since machine conditions need to change in a          |
//                 prescribed way during this process, the program will  |
//                 instruct the user when to provide new beam conditions |
//                 and will prompt to continue the process in stages.    |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


 #define   NUM_STEPS                8 

// NUM_TIMING_SUBSETS is the number of different timing configurations
#define   NUM_TIMING_SUBSETS               12

enum TIMING_SETUP_SUBSETS {
  POS_4NS_A=1,
  POS_4NS_B,
  POS_14NS,
  ELE_4NS_A,
  ELE_4NS_B,
  ELE_14NS,

  POS_4NS_A_FG,  // Fixed-gain subsets begin here
  POS_4NS_B_FG,
  POS_14NS_FG,
  ELE_4NS_A_FG,
  ELE_4NS_B_FG,
  ELE_14NS_FG
};


char bnames[CBPM_MAX_TIMING_BLOCKS][2] = {"A", "B"};


//Function called to prompt for gain settings
int prompt_for_gain_setting(void) {
	
  int gain_setting = 0;
	
  CBI_INIT_PROMPTS(1);
  
  prompts[0].supp_text_ptr    = NULL;
  
  char output[80];
  sprintf(output, "Provide gain setting (0-%d) to use for this process ", CBPM_MAX_GAIN_SETTING);
  prompts[0].prompt_text = output;
  prompts[0].data_type        = CBI_INT;
  prompts[0].default_value    = 0;
  prompts[0].destination      = &(gain_setting);
  prompts[0].last_val_default = TRUE;
  CBI_PROMPTER();
  
  return gain_setting;	  
}


int cbpm_time_in(void) {
 
  CBPM_DATA *dpt, *dp;
  dpt = CTL_System.p_Module_temp->dsp_data;

  CBPM_CONTROL  *cp;

  // Counters
  int  card, tblock, inst;
  int  setup;         // which timing setup to calibrate
  int  valid_setup;
  char update_conf;
  int  prompt_for_gain = CBI_TRUE;

  // Temp vars for block B pre-calculation step
  int bTMD_B, GD_B, CDs_B[CBPM_MAX_CARDS];
  int status;
  int retval;

  //----------------------------------------------------------
  // Variables whose values need to be preserved across
  // multiple iterations (must be static)
  //----------------------------------------------------------
  static int TSETUP;
  static int GAIN_SETTING =  0;
  static int SPECIES      =  CBI_UNINITIALIZED_PARAM;
  static int BUNCH;
  static int TBLOCK;


  CBI_COMMAND_SEQUENCE_START(NUM_STEPS)
  // FIXME: Update this entire list.
    //   Get block A timings, then dead-reckon nearly all of block B's.
    ///  A timing scan for fine-adjustment and verification should be all that is
    // necessary for block B under appropriate 2nd-bunch machine conditions.

     CBI_COMMAND_SEQUENCE_STEP( 1 )
      valid_setup = FALSE;
      while (valid_setup == FALSE) {
         valid_setup = TRUE;  // assume success
        // Prompt the user for what conditions to time in
        printf("\n\n");
        printf("Select the desired timing conditions:\n");
        printf("   %d = Positrons, 4nSec, timing block A - Variable Gain\n",POS_4NS_A);
        printf("   %d = Positrons, 4nSec, timing block B - Variable Gain    Time-scan ONLY\n",POS_4NS_B);
        printf("   %d = Positrons, 14nSec - Variable Gain\n\n",             POS_14NS);

        printf("   %d = Electrons, 4nSec, timing block A - Variable Gain\n",ELE_4NS_A);
        printf("   %d = Electrons, 4nSec, timing block B - Variable Gain    Time-scan ONLY\n",ELE_4NS_B);
        printf("   %d = Electrons, 14nSec - Variable Gain\n\n",             ELE_14NS);

        printf("   %d = Positrons, 4nSec, timing block A - FIXED Gain\n",POS_4NS_A_FG);
        printf("   %d = Positrons, 4nSec, timing block B - FIXED Gain    Time-scan ONLY\n",POS_4NS_B_FG);
        printf("   %d = Positrons, 14nSec - FIXED Gain\n\n",             POS_14NS_FG);

        printf("   %d = Electrons, 4nSec, timing block A - FIXED Gain\n",ELE_4NS_A_FG);
        printf("   %d = Electrons, 4nSec, timing block B - FIXED Gain    Time-scan ONLY\n",ELE_4NS_B_FG);
        printf("   %d = Electrons, 14nSec - FIXED Gain\n\n",             ELE_14NS_FG);

	printf("\n \"T1B1\" bunch current should be in the neighborhood of 0.75mA--1.25mA for this process.\n\n");

        printf("  -1 = Done\n");
        printf("  Conditions: ");


	//-------------------------------------------------
	// Define the list of prompts and their parameters
	//-------------------------------------------------
	CBI_INIT_PROMPTS(1);
	
	int range[2] = {1, 12};
	
	prompts[0].supp_text_ptr    = "";
	prompts[0].prompt_text      = "Selection : ";
	prompts[0].data_type        = CBI_INT;
	prompts[0].validator        = cbi_int_range_validator;
	prompts[0].valid_inputs     = range;
	prompts[0].default_value    = 1;
	prompts[0].destination      = &(setup);
	prompts[0].last_val_default = CBI_TRUE;

	retval = CBI_PROMPTER();


        ////setup = cbi_get_int();
        ////printf("\n");


        // TODO: NEED TO KNOW HOW TO BREAK OUT AND END A SEQUENCE
        if (setup == -1) return F_SUCCESS;
      
        switch (setup) {
           case POS_4NS_A:
              //  Positrons, 4nSec, timing block A
              SPECIES = 0;
              printf(" 4ns 'LDINIT' --  LOAD  Train-1  Bunch-1  ('T1B1')  %s\n\n\n", cbi_species_names[SPECIES] );
              BUNCH = 1;
              TBLOCK = 0;
	      TSETUP = s4ns_P_VAR_G;
              break;
           case POS_4NS_B:
              //  Positrons, 4nSec, timing block B
              SPECIES = 0;
              printf(" LOAD  the bunch __4ns_AFTER__  Train-1  Bunch-1   %s \n", cbi_species_names[SPECIES] );
              BUNCH = 2;
              TBLOCK = 1;
	      TSETUP = s4ns_P_VAR_G;
              break;
           case POS_14NS:
              //  Positrons, 14nSec
              SPECIES = 0;
              printf(" 14ns 'LDINIT' --  LOAD  Train-1  Bunch-1  ('T1B1')  %s\n\n\n", cbi_species_names[SPECIES] );
              BUNCH = 1;
              TBLOCK = 0;
	      TSETUP = s14ns_VAR_G;
              break;

           case ELE_4NS_A:
              //  Electrons, 4nSec, timing block A
              SPECIES = 1;
              printf(" 4ns 'LDINIT' --  LOAD  Train-1  Bunch-1  ('T1B1')  %s\n\n\n", cbi_species_names[SPECIES] );
              BUNCH = 1;
              TBLOCK = 0;
	      TSETUP = s4ns_E_VAR_G;
              break;
           case ELE_4NS_B:
              //  Electrons, 4nSec, timing block B
              SPECIES = 1;
              printf(" LOAD  the bunch __4ns_AFTER__  Train-1  Bunch-1   %s \n", cbi_species_names[SPECIES] );
              BUNCH = 2;
              TBLOCK = 1;
	      TSETUP = s4ns_E_VAR_G;
              break;
           case ELE_14NS:
              //  Electrons, 14nSec
              SPECIES = 1;
              printf(" 14ns 'LDINIT' --  LOAD  Train-1  Bunch-1  ('T1B1')  %s\n\n\n", cbi_species_names[SPECIES] );
              BUNCH = 2;
              TBLOCK = 1;
	      TSETUP = s14ns_VAR_G;
              break;

           case POS_4NS_A_FG:
              //  Positrons, 4nSec, timing block A
              SPECIES = 0;
              printf(" 4ns 'LDINIT' --  LOAD  Train-1  Bunch-1  ('T1B1')  %s\n\n\n", cbi_species_names[SPECIES] );
              BUNCH = 1;
              TBLOCK = 0;
	      prompt_for_gain = CBI_FALSE;
	      TSETUP = s4ns_P_FIX_G;
              break;
           case POS_4NS_B_FG:
              //  Positrons, 4nSec, timing block B
              SPECIES = 0;
              printf(" LOAD  the bunch __4ns_AFTER__  Train-1  Bunch-1   %s \n", cbi_species_names[SPECIES] );
              BUNCH = 2;
              TBLOCK = 1;
	      TSETUP = s4ns_P_FIX_G;
	      prompt_for_gain = CBI_FALSE;
              break;
           case POS_14NS_FG:
              //  Positrons, 14nSec
              SPECIES = 0;
              printf(" 14ns 'LDINIT' --  LOAD  Train-1  Bunch-1  ('T1B1')  %s\n\n\n", cbi_species_names[SPECIES] );
              BUNCH = 1;
              TBLOCK = 0;
	      TSETUP = s14ns_FIX_G;
	      prompt_for_gain = CBI_FALSE;
              break;

           case ELE_4NS_A_FG:
              //  Electrons, 4nSec, timing block A
              SPECIES = 1;
              printf(" 4ns 'LDINIT' --  LOAD  Train-1  Bunch-1  ('T1B1')  %s\n\n\n", cbi_species_names[SPECIES] );
              BUNCH = 1;
              TBLOCK = 0;
	      TSETUP = s4ns_E_FIX_G;
	      prompt_for_gain = CBI_FALSE;
              break;
           case ELE_4NS_B_FG:
              //  Electrons, 4nSec, timing block B
              SPECIES = 1;
              printf(" LOAD  the bunch __4ns_AFTER__  Train-1  Bunch-1   %s \n", cbi_species_names[SPECIES] );
              BUNCH = 2;
              TBLOCK = 1;
	      TSETUP = s4ns_E_FIX_G;
	      prompt_for_gain = CBI_FALSE;
              break;
           case ELE_14NS_FG:
              //  Electrons, 14nSec
              SPECIES = 1;
              printf(" 14ns 'LDINIT' --  LOAD  Train-1  Bunch-1  ('T1B1')  %s\n\n\n", cbi_species_names[SPECIES] );
              BUNCH = 2;
              TBLOCK = 1;
	      TSETUP = s14ns_FIX_G;
	      prompt_for_gain = CBI_FALSE;
              break;

           default:
              valid_setup = FALSE;
        }
      }

      // Gain setting of 7 should provide sufficient signal amplitude under a variety of common bunch currents.
      // Default to that gain setting here.
      if (prompt_for_gain) {
      	
    GAIN_SETTING = prompt_for_gain_setting();
	//printf("Provide gain setting (0-%d) to use for this process and hit <ENTER> to begin: ", CBPM_MAX_GAIN_SETTING);
	//GAIN_SETTING = cbi_get_int();
	//printf("Setting variable gain acquisitions to use gain setting 7.\n");
	//GAIN_SETTING = 7;
      } else {
	GAIN_SETTING = 0;
      }

 
      // Scrub just the block-dependent timing params that will be updated during this time-in sequence.
      if (TBLOCK == 0 || TSETUP == s14ns_VAR_G || TSETUP == s14ns_FIX_G) {
	printf("\n\n\n-------------------------------------------------\n");
	printf("  Scrubbing timing parameters for block %s\n", bnames[TBLOCK] );
	printf("-------------------------------------------------\n");
	dpt->cbpm_cmd_params.tblock = TBLOCK;
	dpt->cbpm_op_timing.active_timing_setup = TSETUP;
	cbi_load_command(CTL_System.command_list,
			 CTL_System.itype.num_platform_commands,
			 "SCRUB TIMING PARAMS",
			 CBI_FALSE);
      } else {
	CBI_NEXT_SEQUENCE_STEP
      }
      


  CBI_COMMAND_SEQUENCE_STEP( 2 )
      printf("\n\n\n-------------------------------------------------\n");
      printf("  Setting timing setup to %s \n", tsetup_names[TSETUP] );
      printf("-------------------------------------------------\n");
      printf("%d instruments online \n", CTL_System.n_Modules );
      
      for (inst = 0; inst < num_online(); inst++) {
	if (CTL_System.p_Module[inst]->active == CBI_ACTIVE) {
	  cp = CTL_System.p_Module[inst]->control;
	  dp = CTL_System.p_Module[inst]->dsp_data;

 	  // Only initialize cTMD if this is a block A/0 pass or if it's 14ns mode.
	  if ( TBLOCK == 0 || (TSETUP == s14ns_VAR_G || TSETUP == s14ns_FIX_G) ) {
 	    printf("Initializing cTMD value to 0\n");
	    dp->cbpm_timing_config.setups[TSETUP].cTMD = 0;
 	    status = cbi_gp_struct(WRITE, CTL_System.comm_method, CBPM_TIMING_CONFIG_TAG, CTL_System.p_Module[inst], CBI_FULL_STRUCT);
 	  }

	}
      }
      
      dpt->cbpm_op_timing.active_timing_setup = TSETUP;
      dpt->cbpm_cmd_params.species            = SPECIES;
      cbi_load_command(CTL_System.command_list,
		       CTL_System.itype.num_platform_commands,
		       "SET TIMING SETUP",
		       CBI_FALSE);



  CBI_COMMAND_SEQUENCE_STEP( 3 )
    printf("\n\n\n-------------------------------------------------\n");
    printf("  Configuring gains for setting # %d.\n", GAIN_SETTING);
    printf("-------------------------------------------------\n");
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
      for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
        dpt->cbpm_op_gain.active_gain_settings[card][tblock] = GAIN_SETTING;
      }
    }
    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "SET GAINS",
		     CBI_FALSE);



  CBI_COMMAND_SEQUENCE_STEP( 4 )
     // Skip the initial bTMD determination if this is a BLOCK B timing-in procedure
     // or if this is done in 14ns mode.
     if (TBLOCK == 1 || TSETUP == s14ns_VAR_G || TSETUP == s14ns_FIX_G) {
        CBI_NEXT_SEQUENCE_STEP
     }
     printf("\n\n\n-------------------------------------------------\n");
     printf("  Determining INITIAL BLOCK TURNS MARKER DELAY for block %s...\n", bnames[TBLOCK]);
     printf("-------------------------------------------------\n");
     dpt->cbpm_cmd_params.species          = SPECIES;
     dpt->cbpm_cmd_params.tblock           = TBLOCK;
     dpt->cbpm_cmd_params.num_turns        = 2;
     // Routine internally uses all available bunch bits for repeated find_bunch runs.
     cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
     cbi_load_command(CTL_System.command_list,
		      CTL_System.itype.num_platform_commands,
		      "FIND bTMD",
		      CBI_FALSE);


   
  CBI_COMMAND_SEQUENCE_STEP( 5 )
     // Skip common TMD determination if this is a BLOCK B timing-in procedure
     // or if this is done in 14ns mode.
     if (TBLOCK == 1   ||  TSETUP == s14ns_VAR_G  ||  TSETUP == s14ns_FIX_G) {
       CBI_NEXT_SEQUENCE_STEP
     }
  dpt->cbpm_cmd_params.tblock = TBLOCK;
     printf("\n\n\n-------------------------------------------------\n");
     printf("  Determining COMMON TURNS MARKER DELAY \n");
     printf("-------------------------------------------------\n");
     cbi_load_command(CTL_System.command_list,
		      CTL_System.itype.num_platform_commands,
		      "FIND cTMD",
		      CBI_FALSE);



  CBI_COMMAND_SEQUENCE_STEP( 6 )
     // Skip final bTMD determination if this is a NON-14ns BLOCK B procedure.
     //if (TBLOCK == 1 && dpt->cbpm_timing_params.timing_mode == m4ns) {
     if (TBLOCK == 1 && (TSETUP == s4ns_P_VAR_G || TSETUP == s4ns_P_FIX_G || TSETUP == s4ns_E_VAR_G || TSETUP == s4ns_E_FIX_G)) {
       CBI_NEXT_SEQUENCE_STEP
     }
     printf("\n\n\n-------------------------------------------------\n");
     printf("  Determining FINAL BLOCK TURNS MARKER DELAY for block %s...\n", bnames[TBLOCK]);
     printf("-------------------------------------------------\n");
     dpt->cbpm_cmd_params.species          = SPECIES;
     dpt->cbpm_cmd_params.tblock           = TBLOCK;
     dpt->cbpm_cmd_params.num_turns        = 2;
     cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
     cbi_load_command(CTL_System.command_list,
		      CTL_System.itype.num_platform_commands,
		      "FIND bTMD",
		      CBI_FALSE);



  CBI_COMMAND_SEQUENCE_STEP( 7 )
    printf("\n\n\n-------------------------------------------------\n");
    printf("  Time scan (BUNCH %d)...\n", BUNCH);
    printf("-------------------------------------------------\n");
    dpt->cbpm_cmd_params.species                    = SPECIES;
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, BUNCH );
    dpt->cbpm_cmd_params.delay_cal                  = CBI_TRUE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_FALSE;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_FALSE;
    dpt->cbpm_cmd_params.trig_mask                  = 0;
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_TRUE;
    // Store the parameters needed for formatting the datafile in an external struct
    dspec.species         = dpt->cbpm_cmd_params.species;
    //dspec.first_index     = cdelay_low;
    dspec.bunch           = BUNCH-1;
    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "TIME SCAN",
		     CBI_FALSE);

    /*for (inst = 0; inst < num_online(); inst++) {
      if (CTL_System.p_Module[inst]->active == CBI_ACTIVE) {
        status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_BTEST_RESULTS_TAG,    CTL_System.p_Module[inst], CBI_FULL_STRUCT);	  
      }
    }  
    
    cbpm_time_scan_summary(1); // in post, read in CBPM_BTEST_RESULTS_TAG struct. this function needs it.*/


  // Summarize results and prompt for saving.
  CBI_COMMAND_SEQUENCE_STEP( 8 )
    printf("\n\n");
    for (inst = 0; inst < num_online(); inst++) {
      if (CTL_System.p_Module[inst]->active == CBI_ACTIVE) {
	dp = CTL_System.p_Module[inst]->dsp_data;
	cp = CTL_System.p_Module[inst]->control;

	if ( TSETUP != s14ns_VAR_G  &&  TSETUP != s14ns_FIX_G ) {
	  // Dead-reckon the expected values of bTMD, block, and channel delays
	  // for block B by using the block A results.
	  printf("Pre-computing BLOCK B values for bTMD, GD, and CDs...\n");
	  printf("Setting BLOCK B bTMD value to   bTMD(A) + 2...\n");
	  bTMD_B = dp->cbpm_timing_config.setups[TSETUP].bTMDs[0] + 2;
	  printf("Setting BLOCK B block delay to   gd(A) + 400...\n");
	  GD_B   = dp->cbpm_timing_config.setups[TSETUP].block_delays[0] + 400;
	  printf("Inheriting BLOCK B CHANNEL DELAYS \n");
	  for (card = 0; card < CBPM_MAX_CARDS; card++) {
	    CDs_B[card] = dp->cbpm_timing_config.setups[TSETUP].chan_delays[0][card];
	  }


	  // Copy these calculated values into place for BLOCK B
	  dp->cbpm_timing_config.setups[TSETUP].BP_offsets[1]  = dp->cbpm_timing_config.setups[TSETUP].BP_offsets[0];
	  dp->cbpm_timing_config.setups[TSETUP].bTMDs[1]       = bTMD_B;
	  dp->cbpm_timing_config.setups[TSETUP].block_delays[1] = GD_B;
	  for (card = 0; card < CBPM_MAX_CARDS; card++) {
	    dp->cbpm_timing_config.setups[TSETUP].chan_delays[1][card] = CDs_B[card];
	  }
	  
	}

	// Display all timing parameters for review.
	printf("   --- [%s]   -   %s ---\n", CTL_System.p_Module[inst]->comm.ethernet.hostname, cp->detector.name );
	printf("BP offsets               :  %d  %d\n",
	       dp->cbpm_timing_config.setups[TSETUP].BP_offsets[0], dp->cbpm_timing_config.setups[TSETUP].BP_offsets[1] );
	printf("Common turns marker delay:  %d\n", dp->cbpm_timing_config.setups[TSETUP].cTMD );
	printf("Block turns marker delays:  %d  %d  \n",
	       dp->cbpm_timing_config.setups[TSETUP].bTMDs[0], dp->cbpm_timing_config.setups[TSETUP].bTMDs[1] );
	printf("block_delays              :  %d  %d  \n",
	       dp->cbpm_timing_config.setups[TSETUP].block_delays[0], dp->cbpm_timing_config.setups[TSETUP].block_delays[1] );
	printf("Channel delays           :");
	for (card = 0; card < CBPM_MAX_CARDS; card++) {
	  printf("  %d", dp->cbpm_timing_config.setups[TSETUP].chan_delays[0][card]);
	}
	printf("\n                          ");
	for (card = 0; card < CBPM_MAX_CARDS; card++) {
	  printf("  %d", dp->cbpm_timing_config.setups[TSETUP].chan_delays[1][card]);
	}
	printf("\n\n\n");
      }
    } // endFOR inst

    // Prompt for confirmation of saving timing parameters to intermediate file.
    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "SAVE TIMING PARAMETERS",
		     CBI_TRUE);


    CBI_COMMAND_SEQUENCE_END

 } 



