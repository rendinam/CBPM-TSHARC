//-----------------------------------------------------------------------+
// File         : cbpm_test_life.c                                       |
//                                                                       |
// Description  : Code to keep doing something until instrument produces |
//                error                                                  |
//                                                                       |
// Arguments    :  None                                                  |
//                                                                       |
// Author       :  M. Stedinger                                          |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_test_life_prep(void) {

  return CBI_F_SUCCESS;

}



int cbi_exit_loop_mode( void ) {

  int input = CBI_EMPTY_INPUT;

  while (TRUE) {
    input = cbi_get_char_local();
    if ((char)input == 'q') {
      continuous_mode = FALSE;
      break;
    }
  }

  return CBI_F_SUCCESS;
}


int continuous_mode;


int cbpm_test_life_post( int iidx ) {
	return CBI_F_SUCCESS;
}
	
int cbpm_test_life_final( void ) {
	
  char *func_name = (char *)__FUNCTION__;
  int debug_level = CTL_System.debug_verbosity;	
  int XBusEnabled = 1;
  int  status;
  int iidx;
  
  // Create thread to safely allow keyboard input without affecting CBInet operations
  // in progress.
  pthread_t  INPUT_thread;
  status = pthread_create( &(INPUT_thread), NULL, (void *)&(cbi_exit_loop_mode), NULL );  
  
  //--------------------------------------------------------------
  // Also install the SIGPIPE signal handler when entering
  // continuous polling mode to allow for a semi-graceful
  // recovery from any potential socket/pipe errors that appear.
  //--------------------------------------------------------------
  if (signal (SIGPIPE, continuous_mode_sigpipe_handler) == SIG_IGN) {
    signal (SIGPIPE, SIG_IGN);
  }
  
    // Hide cursor
    //printf("\033[?25l");
    //printf("\b\b\b\b\b\b\b\b");
    //fflush(stdout);  
    
	XBusEnabled = 1;
  
  //----------------------------------------------
  // Drop into infinite loop .
  //----------------------------------------------
  continuous_mode = TRUE;
  while(continuous_mode) {  

    for (iidx = 0; (iidx < CTL_System.n_Modules) && continuous_mode; iidx++) {
      if (CTL_System.p_Module[iidx]->active == CBI_ACTIVE) {
      	
    XBusEnabled = 1;
	int *ptr_orig, *ptr_Xbus, *ptr_Ethernet;
	int same[50];
	int ethernetAllZeros[50];
	int xbusAllZeros[50];
	char* StructNames[50];
	int sInd = 0;
	  
	printf("\n\n *********************************************************************************************************\n");

	FILE *fp;
	fp = stdout;	  
	  	  
  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBI_CMD_TAG\n");
  StructNames[sInd] = "CBI_CMD_TAG";
  int cbi_dump_cmd( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBI_CMD_TAG, 0, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbi_dump_cmd), &XBusEnabled);
  sInd++;
	  
  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBI_STAT_TAG\n");  
  StructNames[sInd] = "CBI_STAT_TAG";
  int cbi_dump_stat( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBI_STAT_TAG, 0, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbi_dump_stat), &XBusEnabled);
  sInd++;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBI_IDENT_TAG\n");  
  StructNames[sInd] = "CBI_IDENT_TAG";
  int cbi_dump_ident( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBI_IDENT_TAG, 0, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],  &(cbi_dump_ident), &XBusEnabled);
  sInd++;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBI_HEARTBEAT_TAG\n");  
  StructNames[sInd] = "CBI_HEARTBEAT_TAG";
  int cbi_dump_heartbeat( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBI_HEARTBEAT_TAG, 0, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],  &(cbi_dump_heartbeat), &XBusEnabled);
  sInd++;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBI_MODULE_CONFIG_TAG\n");  
  StructNames[sInd] = "CBI_MODULE_CONFIG_TAG";
  int cbi_dump_module_config( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBI_MODULE_CONFIG_TAG, 0, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],  &(cbi_dump_module_config), &XBusEnabled);
  sInd++;

  /*if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBPM_CMD_PARAMS_TAG\n");  
    StructNames[sInd] = "CBPM_CMD_PARAMS_TAG";
    int cbpm_dump_cmd_params( FILE *fp, int iidx );
    status = cbi_compare_xbus_and_ethernet(fp, iidx, CBPM_CMD_PARAMS_TAG, 0, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbpm_dump_cmd_params), &XBusEnabled);
    sInd++;*/

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBPM_TIMING_CONFIG_TAG\n");  
  StructNames[sInd] = "CBPM_TIMING_CONFIG_TAG";
  int cbpm_dump_timing_config( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBPM_TIMING_CONFIG_TAG, 0, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbpm_dump_timing_config), &XBusEnabled);
  sInd++;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBPM_GAIN_CONFIG_TAG\n");  
  StructNames[sInd] = "CBPM_GAIN_CONFIG_TAG";
  int cbpm_dump_gain_config( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBPM_GAIN_CONFIG_TAG, 0, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbpm_dump_gain_config), &XBusEnabled);
  sInd++;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBPM_CAL_PARAMS_TAG\n");  
  StructNames[sInd] = "CBPM_CAL_PARAMS_TAG";
  int cbpm_dump_cal_params( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBPM_CAL_PARAMS_TAG, 0, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbpm_dump_cal_params), &XBusEnabled);
  sInd++;

  /*if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBPM_PROC_BUF_TAG\n");  
    StructNames[sInd] = "CBPM_PROC_BUF_TAG";
    int cbpm_dump_proc_buf( FILE *fp, int iidx );
    status = cbi_compare_xbus_and_ethernet(fp, iidx, CBPM_PROC_BUF_TAG, 0, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbpm_dump_proc_buf), &XBusEnabled);
    sInd++;*/
	  
	  
	  
	int i;
	  
	printf("\n");
	
    fprintf(fp,   "Structure       : ");
    for (i=0; i<sInd; i++) {
      fprintf(fp, "%i ",i);
    }
    fprintf(fp, "\n");
  	
	if (XBusEnabled) {
	  printf("Same            : ");
	  for (i=0; i<sInd; i++) {
	    printf("%i ",same[i]);
	  }
	  printf("\n");

	  printf("xbusAllZeros    : ");
	  for (i=0; i<sInd; i++) {
	    printf("%i ",xbusAllZeros[i]);
	  }
	  printf("\n");	  
	  fflush(stdout);  
	}
	  

	printf("ethernetAllZeros: ");
	for (i=0; i<sInd; i++) {
	  printf("%i ",ethernetAllZeros[i]);
	}
	printf("\n");
	

    fprintf(fp, "\nStructure Key : \n");
    for (i=0; i<sInd; i++) {
      fprintf(fp, "%i %s\n", i, StructNames[i]);
    }		  
	fflush(stdout);  

	  
	struct tm  *loctime;
	time_t curtime;	  
  
	// Generate timestamp
	char mod_date[25];
	curtime = time(NULL);
	loctime = localtime(&curtime);
	strcpy( mod_date, asctime(loctime) );
	strip( mod_date );
	  
	printf("\n%5s (%s)   \n", 
	   CTL_System.p_Module[iidx]->det.location, 
	   CTL_System.p_Module[iidx]->comm.ethernet.hostname );	
	   fflush(stdout);
	cbi_sleep(1000);	  

	printf("%s - Type 'q' and then <ENTER> to exit loop: \n",mod_date);
	printf("\n****************************************************************************************\n\n");
	fflush(stdout);  
      }
  
      // Re-enable cursor
      //printf("\033[?25h");

    } //endIF active

  } //endFOR iidx
  
  //--------------------------------------------------------------
  // Disable current monitor-specific trapping of SIGPIPEs
  //--------------------------------------------------------------
  if (signal (SIGPIPE, std_sigpipe_handler) == SIG_IGN) {
    signal (SIGPIPE, SIG_IGN);
  }
  

  printf("\n\nPlease wait (3 sec)...");
  fflush(stdout);
  sleep(3);
    
  
  return CBI_F_SUCCESS;
  
}
