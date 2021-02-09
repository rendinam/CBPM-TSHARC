///-----------------------------------------------------------------------+
/// File         :  cbpm_print_pkt_tables.c                                |
///                                                                       |
/// Description  :  Dumps the contents of the packet address and packet   |
///                 size tables.                                          |
///                                                                       |
/// Arguments    :  Index of the master control structure for the         |
///                 instrument to control.                                |
///                                                                       |
/// Author       :  M. Stedinger                                          |
///-----------------------------------------------------------------------+

#include "cbpm_includes.h"

int cbpm_print_pkt_tables(FILE *fp, int iidx) {

  #define CBPM_MAX_PKTS_TOREAD FINAL_INVALID_CBPM_TAG 
  // Read all of the structures up to one less then the invalid place holder at the end, 
  // defined in CBPM-TSHARC/comm_include/cbpm_tags_a.h

  #define NUM_COLS 4

  int i, col;
  int status = CBI_F_SUCCESS;
  int val_per_col = ceilf( CBPM_MAX_PKTS_TOREAD/NUM_COLS )+1;
  int addr[NUM_COLS], size[NUM_COLS];
  int ind = 0;

  fprintf(fp, "    Packet Address and Size Table Contents\n");


  fprintf(fp, "Index    Addr     Size\n");
  for (i=0; i < val_per_col; i++) { 	

    col = 0;
    while (ind < CBPM_MAX_PKTS_TOREAD && col<NUM_COLS) {
	status = cbi_xbus_peek( iidx, CBI_6048_113_PKT_ADDR_TABLE+ind, &(addr[col]));
        if (status != CBI_F_SUCCESS) {
          printf("Failed cbi_xbus_peek for status: %i, iidx:%i, addr:%i, val back:%i\n",status, iidx, CBI_6048_113_PKT_SIZE_TABLE+i, (addr[col])); 
          return CBI_F_FAILURE;
        }  	
	
	status = cbi_xbus_peek( iidx, CBI_6048_113_PKT_SIZE_TABLE+ind, &(size[col]));
        if (status != CBI_F_SUCCESS) {
          printf("Failed cbi_xbus_peek for status: %i, iidx:%i, addr:%i, val back:%i\n",status, iidx, CBI_6048_113_PKT_SIZE_TABLE+i, (addr[col])); 
          return CBI_F_FAILURE;
        }  	

    	fprintf(fp, "%3d) %9d\t%4d\t\t", ind,   addr[col], size[col]);
	col++;
        ind++;
    }
    fprintf(fp,"\n");
    fflush(fp);
  }

  return status;

}
