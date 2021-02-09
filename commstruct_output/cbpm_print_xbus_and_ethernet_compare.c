///-----------------------------------------------------------------------+
/// File         :  cbpm_print_xbus_and_ethernet_compare.c                |
///                                                                       |
/// Description  :  Reads back a number of the smaller communication      |
///                 data structures, via both xbus and ethernet and       |
///                 compares them.                                        |
///                                                                       |
/// Arguments    :  Index of the master control structure for the         |
///                 instrument to control.                                |
///                                                                       |
/// Author       :  M. Stedinger                                          |
///-----------------------------------------------------------------------+

#include "cbpm_includes.h"

int cbpm_print_xbus_and_ethernet_compare(FILE *fp, int iidx) {

  fprintf(fp, "    Comparison of values retrieved from xbus and ethernet\n");

  char *func_name = (char *)__FUNCTION__;
  int debug_level = CTL_System.debug_verbosity;	
  
  char* StructNames[50];
  int same[50];
  int ethernetAllZeros[50];
  int xbusAllZeros[50];
  int sInd = 0;
  int status = CBI_F_SUCCESS;
  int XBusEnabled = 1;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBI_CMD_TAG\n");
  StructNames[sInd] = "CBI_CMD_TAG";
  int cbi_dump_cmd( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBI_CMD_TAG, 1, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbi_dump_cmd), &XBusEnabled);
  sInd++;
	  
  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBI_STAT_TAG\n");  
  StructNames[sInd] = "CBI_STAT_TAG";
  int cbi_dump_stat( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBI_STAT_TAG, 1, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbi_dump_stat), &XBusEnabled);
  sInd++;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBI_IDENT_TAG\n");  
  StructNames[sInd] = "CBI_IDENT_TAG";
  int cbi_dump_ident( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBI_IDENT_TAG, 1, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],  &(cbi_dump_ident), &XBusEnabled);
  sInd++;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBI_HEARTBEAT_TAG\n");  
  StructNames[sInd] = "CBI_HEARTBEAT_TAG";
  int cbi_dump_heartbeat( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBI_HEARTBEAT_TAG, 1, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],  &(cbi_dump_heartbeat), &XBusEnabled);
  sInd++;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBI_MODULE_CONFIG_TAG\n");  
  StructNames[sInd] = "CBI_MODULE_CONFIG_TAG";
  int cbi_dump_module_config( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBI_MODULE_CONFIG_TAG, 1, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],  &(cbi_dump_module_config), &XBusEnabled);
  sInd++;

  /*if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBPM_CMD_PARAMS_TAG\n");  
    StructNames[sInd] = "CBPM_CMD_PARAMS_TAG";
    int cbpm_dump_cmd_params( FILE *fp, int iidx );
    status = cbi_compare_xbus_and_ethernet(fp, iidx, CBPM_CMD_PARAMS_TAG, 1, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbpm_dump_cmd_params), &XBusEnabled);
    sInd++;*/

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBPM_TIMING_CONFIG_TAG\n");  
  StructNames[sInd] = "CBPM_TIMING_CONFIG_TAG";
  int cbpm_dump_timing_config( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBPM_TIMING_CONFIG_TAG, 1, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbpm_dump_timing_config), &XBusEnabled);
  sInd++;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBPM_GAIN_CONFIG_TAG\n");  
  StructNames[sInd] = "CBPM_GAIN_CONFIG_TAG";
  int cbpm_dump_gain_config( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBPM_GAIN_CONFIG_TAG, 1, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbpm_dump_gain_config), &XBusEnabled);
  sInd++;

  if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBPM_CAL_PARAMS_TAG\n");  
  StructNames[sInd] = "CBPM_CAL_PARAMS_TAG";
  int cbpm_dump_cal_params( FILE *fp, int iidx );
  status = cbi_compare_xbus_and_ethernet(fp, iidx, CBPM_CAL_PARAMS_TAG, 1, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbpm_dump_cal_params), &XBusEnabled);
  sInd++;

  /*if (debug_level > CBI_DEBUG_LEVEL_0) printf("Compared CBPM_PROC_BUF_TAG\n");  
    StructNames[sInd] = "CBPM_PROC_BUF_TAG";
    int cbpm_dump_proc_buf( FILE *fp, int iidx );
    status = cbi_compare_xbus_and_ethernet(fp, iidx, CBPM_PROC_BUF_TAG, 1, &same[sInd], &xbusAllZeros[sInd], &ethernetAllZeros[sInd],   &(cbpm_dump_proc_buf), &XBusEnabled);
    sInd++;*/

  int i;
	  
  fprintf(fp, "\n");

  fprintf(fp,   "Structure       : ");
  for (i=0; i<sInd; i++) {
    fprintf(fp, "%i ",i);
  }
  fprintf(fp, "\n");

  if (XBusEnabled) {
    fprintf(fp, "Same            : ");
    for (i=0; i<sInd; i++) {
      fprintf(fp, "%i ",same[i]);
    }
    fprintf(fp, "\n");

    fprintf(fp, "xbusAllZeros    : ");
    for (i=0; i<sInd; i++) {
      fprintf(fp, "%i ",xbusAllZeros[i]);
    }
    fprintf(fp, "\n");	  
    fflush(stdout);  
  }
	  

  fprintf(fp, "ethernetAllZeros: ");
  for (i=0; i<sInd; i++) {
    fprintf(fp, "%i ",ethernetAllZeros[i]);
  }
  fprintf(fp, "\n");

  fprintf(fp, "\nStructure Key : \n");
  for (i=0; i<sInd; i++) {
    fprintf(fp, "%i %s\n", i, StructNames[i]);
  }
	  
  fflush(stdout);
  fflush(fp);
  return status;
}
