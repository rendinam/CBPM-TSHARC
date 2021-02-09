/*!
*-----------------------------------------------------------------------+
* File         :  cbpm_diagnostic_dump.c                                |
*                                                                       |
* Description  :  Routines employed to create a diagnostic dump file    |
*                 with the contents of all of the cbpm communication    |
*                 data structures.                                      |
*                 The dump file is saved <Locationname>-<Hostname>.dump |
*                                                                       |
* Arguments    :  None                                                  |
*                                                                       |
* Author       :  M.Rendina                                             |
*-----------------------------------------------------------------------+*/

#include "cbpm_includes.h"

int cbpm_diagnostic_dump_prep(void) {
}



#define FRONT_END_REGISTER_MAX_OFFSET       62

 
int cbpm_diagnostic_dump_post( int iidx ) {

  int debug_level = CTL_System.debug_verbosity;

  printf("Creating instrument dump file:\n");
  FILE *fp;
  char filename[CBI_MAX_FILENAME_LEN];
  sprintf(filename, "%s-%s.dump", 
	  CTL_System.p_Module[iidx]->det.location,
	  CTL_System.p_Module[iidx]->comm.ethernet.hostname );
  
  fp = fopen(filename, "w");

  int status;

  int card_regvals[CBPM_MAX_CARDS][FRONT_END_REGISTER_MAX_OFFSET];
  int card, reg, addr, value;
  int xfer_size = 4;
  int data_count = FRONT_END_REGISTER_MAX_OFFSET + 1;
  fprintf(fp, "DIAGNOSTIC DUMP\n");
  fprintf(fp, "Location : %s\n", CTL_System.p_Module[iidx]->det.location );
  fprintf(fp, "Hostname : %s\n\n", CTL_System.p_Module[iidx]->comm.ethernet.hostname );
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading Reading Packet and Size tables\n");  
  status = cbpm_print_pkt_tables(fp, iidx);

  fprintf(fp, "\n=========================================================\n");
  fprintf(fp, "Front-end board registers:\n");
  fprintf(fp, "               Card-0               Card-1                Card-2                Card-3 \n");
  
  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading Front End Registers\n");

  for (reg = 0; reg < FRONT_END_REGISTER_MAX_OFFSET + 1; reg++) {
    fprintf(fp, "%3d)   ", reg);
    if (debug_level > CBI_DEBUG_LEVEL_0) printf("%i ",reg);
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
      addr = 0x09000000 + card*(0x100000) + reg;
      fprintf(fp, "[0x%x] ", addr);
      status = cbi_xbus_peek( iidx, addr, &(card_regvals[card][reg]) );
      fprintf(fp, "%8x  ", card_regvals[card][reg]);
    }
    fprintf(fp, "\n");
  }
  
  if (debug_level > CBI_DEBUG_LEVEL_0) printf("\nReading CBI_CMD_TAG\n");  
  fprintf(fp, "\n=========================================================\n");
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBI_CMD_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbi_dump_cmd( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBI_CMD_TAG, %i\n",status); fflush(fp);
    }
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBI_DEBUG_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBI_DEBUG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbi_dump_debug( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBI_DEBUG_TAG, %i\n",status);
    }
  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBI_HEARTBEAT_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBI_HEARTBEAT_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbi_dump_heartbeat( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBI_HEARTBEAT_TAG, %i\n",status);
    }
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBI_IDENT_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBI_IDENT_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbi_dump_ident( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBI_IDENT_TAG, %i\n",status);
    }
  
  fprintf(fp, "\n=========================================================\n");
  
  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBI_MODULE_CONFIG_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBI_MODULE_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbi_dump_module_config( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBI_MODULE_CONFIG_TAG, %i\n",status);
    }
  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBI_STAT_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBI_STAT_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbi_dump_stat( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBI_STAT_TAG, %i\n",status);
    }
  
  fprintf(fp, "\n=========================================================\n");
  fprintf(fp, "n================================================================\n\n");

  /*   status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER0_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT); */
  /*   cbpm_dump_adc_buffer0( fp, iidx ); */
  /*   fprintf(fp, "\n=========================================================\n"); */

  /*   status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER1_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT); */
  /*   cbpm_dump_adc_buffer1( fp, iidx ); */
  /*   fprintf(fp, "\n=========================================================\n"); */

  /*   status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER2_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT); */
  /*   cbpm_dump_adc_buffer2( fp, iidx ); */
  /*   fprintf(fp, "\n=========================================================\n"); */

  /*   status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER3_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT); */
  /*   cbpm_dump_adc_buffer3( fp, iidx ); */
  /*   fprintf(fp, "\n=========================================================\n"); */

  /*   status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER4_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT); */
  /*   cbpm_dump_adc_buffer4( fp, iidx ); */
  /*   fprintf(fp, "\n=========================================================\n"); */

  /*   status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER5_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT); */
  /*   cbpm_dump_adc_buffer5( fp, iidx ); */
  /*   fprintf(fp, "\n=========================================================\n"); */

  /*   status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER6_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT); */
  /*   cbpm_dump_adc_buffer6( fp, iidx ); */
  /*   fprintf(fp, "\n=========================================================\n"); */

  /*   status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_ADC_BUFFER7_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT); */
  /*   cbpm_dump_adc_buffer7( fp, iidx ); */
  /*   fprintf(fp, "\n=========================================================\n"); */

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_BETA_PHASE_OUT_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_BETA_PHASE_OUT_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_beta_phase_out( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_BETA_PHASE_OUT_TAG, %i\n",status);
    }
  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_CAL_PARAMS_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CAL_PARAMS_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_cal_params( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_CAL_PARAMS_TAG, %i\n",status);
    }
  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_CHAN_CAL_WORK_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CHAN_CAL_WORK_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_chan_cal_work( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_CHAN_CAL_WORK_TAG, %i\n",status);
    }
  
  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_CMD_PARAMS_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_CMD_PARAMS_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_cmd_params( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_CMD_PARAMS_TAG, %i\n",status);
    }
  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_DIAGNOSTIC_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_DIAGNOSTIC_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_diagnostic( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_DIAGNOSTIC_TAG, %i\n",status);
    }
  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_FIND_DELAY_OUT_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_FIND_DELAY_OUT_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_find_delay_out( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_FIND_DELAY_OUT_TAG, %i\n",status);
    }
  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_FIND_DELAY_PARAMS_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_FIND_DELAY_PARAMS_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_find_delay_params( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_FIND_DELAY_PARAMS_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_GAIN_CONFIG_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_GAIN_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_gain_config( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_GAIN_CONFIG_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_OP_GAIN_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_GAIN_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_op_gain( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_OP_GAIN_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_OP_TIMING_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_OP_TIMING_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_op_timing( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_OP_TIMING_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_PED_CAL_OUT_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PED_CAL_OUT_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_ped_cal_out( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_PED_CAL_OUT_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_PEDESTAL_CONFIG_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PEDESTAL_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_pedestal_config( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_PEDESTAL_CONFIG_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_PHASE_CONFIG_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PHASE_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_phase_config( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_PHASE_CONFIG_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_PH_WORD_BUF_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PH_WORD_BUF_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_ph_word_buf( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_PH_WORD_BUF_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_PROC_BUF_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_BUF_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_proc_buf( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_PROC_BUF_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_PROC_BUF_HEADER_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_BUF_HEADER_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_proc_buf_header( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_PROC_BUF_HEADER_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_PROC_BUF_IO_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_BUF_IO_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_proc_buf_io( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_PROC_BUF_IO_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_PROC_PARAMS_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_PARAMS_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_proc_params( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_PROC_PARAMS_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_PROC_SUM_DAT_BUF_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_PROC_SUM_DAT_BUF_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_proc_sum_dat_buf( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_PROC_SUM_DAT_BUF_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_RAW_DATA_HEADER_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_RAW_DATA_HEADER_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_raw_data_header( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_RAW_DATA_HEADER_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_RAW_PARAMS_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_RAW_PARAMS_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_raw_params( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_RAW_PARAMS_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_SIGNAL_STAT_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_SIGNAL_STAT_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_signal_stat( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_SIGNAL_STAT_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_SUMM_BUF_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_SUMM_BUF_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_summ_buf( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_SUMM_BUF_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_TEMPERATURES_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TEMPERATURES_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_temperatures( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_TEMPERATURES_TAG, %i\n",status);
    }
	  
  fprintf(fp, "\n=========================================================\n");

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Reading CBPM_TIMING_CONFIG_TAG\n");  
  status = cbi_gp_struct(READ, CTL_System.comm_method, CBPM_TIMING_CONFIG_TAG, CTL_System.p_Module[iidx], CBI_FULL_STRUCT);
  if (status == CBI_F_SUCCESS) {
    cbpm_dump_timing_config( fp, iidx );
  }
  else
    {
      fprintf(fp,"Error dumping CBPM_TIMING_CONFIG_TAG, %i\n",status);
    }
  
  fprintf(fp, "\n=========================================================\n");
  
  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Comparing structures read back by xbus and ethernet\n");  
  cbpm_print_xbus_and_ethernet_compare(fp, iidx);
  fprintf(fp, "\n=========================================================\n");

  fclose(fp);

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Done\n");  

  return CBI_F_SUCCESS;
}
