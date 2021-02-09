//-----------------------------------------------------------------------+
// File         :  cbpm_triplet_time_shift.c                             |
//                                                                       |
// Description  :  A SEQUENCE-type command.                              |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


#define   NUM_STEPS                12


#define   ORIGINAL_TIM_VALUE       -65500
#define   TIM_RANGE                10


int cbpm_triplet_time_shift(void) {
 
  CBPM_DATA *dpt;
  dpt = CTL_System.p_Module_temp->dsp_data;

  CBPM_CONTROL  *cpt;

  static int tim_value = ORIGINAL_TIM_VALUE - TIM_RANGE;

  int status;

  CBI_COMMAND_SEQUENCE_START(NUM_STEPS)
   

  CBI_COMMAND_SEQUENCE_STEP( 1 )
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);




  CBI_COMMAND_SEQUENCE_STEP( 2 )
    tim_value = tim_value + 2;
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);




  CBI_COMMAND_SEQUENCE_STEP( 3 )
    tim_value = tim_value + 2;
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);



  CBI_COMMAND_SEQUENCE_STEP( 4 )
    tim_value = tim_value + 2;
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);

 


  CBI_COMMAND_SEQUENCE_STEP( 5 )
    tim_value = tim_value + 2;
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);

 



  CBI_COMMAND_SEQUENCE_STEP( 6 )
    tim_value = tim_value + 2;
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);

  


  CBI_COMMAND_SEQUENCE_STEP( 7 )
    tim_value = tim_value + 2;
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);

  


  CBI_COMMAND_SEQUENCE_STEP( 8 )
    tim_value = tim_value + 2;
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);

  


  CBI_COMMAND_SEQUENCE_STEP( 9 )
    tim_value = tim_value + 2;
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);

 



  CBI_COMMAND_SEQUENCE_STEP( 10 )
    tim_value = tim_value + 2;
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);

  


  CBI_COMMAND_SEQUENCE_STEP( 11 )
    tim_value = tim_value + 2;
    status = vxputn_c("TIM CSR INIT", 10, 10, &tim_value );
    if (status != MPM_SUCCESS) {
      printf("ERROR writing value to TIM CSR INIT - ele 10!\n");
    }
    cbi_clear_bp( dpt->cbpm_cmd_params.bunch_pat );
    cbi_set_bunch( dpt->cbpm_cmd_params.bunch_pat, 1 );
    dpt->cbpm_cmd_params.species                    = POSITRONS;
    dpt->cbpm_cmd_params.num_turns                  = 1024;
    dpt->cbpm_cmd_params.use_data_enable            = CBI_TRUE;
    dpt->cbpm_cmd_params.trig_mask                  = TRIG_MASK_A;
    dpt->cbpm_cmd_params.scale_mode                 = PED_SUB_SCALED_DATA; // Changed 20-Jan-2010
    dpt->cbpm_cmd_params.delay_cal                  = CBI_FALSE; 
    dpt->cbpm_cmd_params.generate_delay_corrections = CBI_FALSE;
    dpt->cbpm_cmd_params.gain_cal                   = CBI_TRUE;
    dpt->cbpm_cmd_params.avg_mode                   = PROC_JITTER_SUPPRESSION_MODE; //PROC_SIMPLE_AVG_MODE;

    printf("Processed data buffer index will be reset to 0 for this command.\n");
    dpt->cbpm_cmd_params.reset_proc_buf_idx = CBI_TRUE;

    cbi_load_command(CTL_System.command_list,
		     CTL_System.itype.num_platform_commands,
		     "GET PROCESSED DATA",
		     CBI_FALSE);

 

    CBI_COMMAND_SEQUENCE_STEP(12)
      tim_value = ORIGINAL_TIM_VALUE;
      vxputn_c("TIM CSR INIT", 10, 10, &tim_value );



    CBI_COMMAND_SEQUENCE_END

 } 



