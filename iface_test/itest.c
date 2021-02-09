//-----------------------------------------------------------------------+
// File         :  itest.c                                               |
//                                                                       |
// Description  :  Program for testing the MPM interface command mode of |
//                 the CBPM2 server.                                     |
//                                                                       |
//                                                                       |
// Arguments    :  '-d'/'-D' flag on command line to enable DEBUG mode.  |
//                 Allows for more verbose runtime output.               |
//                                                                       |
// Author       :  M. Rendina                                            |
//                                                                       |
//-----------------------------------------------------------------------+
//#define USE_MNET_MANAGER


//System includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   
#include <math.h>     
#include <time.h>
#include <signal.h>

//ACC release includes
#include "CESR_platform.h"
#include "utilities.h"
#include "messages.h"
#include "Mnet_client_c.h"

//Global
#include "cbi_params.h"
#include "cbi_dsp_comm_struct.h"
#include "cbi_ctl_types.h"
#define PKT_LIMIT_NO_EXTERN
#include "cbi_dsp_comm.h"
#include "cbi_global_config_a.h"

//Architecture-dependent library includes (CBPM-TSHARC)
#include "cbi_TSHARC_BPM_tags_a.h"  // includes types header too (best way to do this? Container header?)
#include "cbi_TSHARC_BPM_proto_a.h" 

//Local
#include "tbpm_dsp_params.h"
#include "tbpm_dsp_regs.h"
#define CBI_TSHARC_INI_NO_EXTERN
#define CBI_TSHARC_NAMES_NO_EXTERN
#include "names.h"
#define CBI_TSHARC_BPM_NO_EXTERN    // In general, Use these types of declarations for FIRST instance,
#include "ext_structs.h"            //    'extern' stanzas take care of all other files including targets.
#include "protos.h"
#include "cbic_params.h"

// For GUI stuff
#include <gtk/gtk.h>

#include "interface_mode.h"



//=======================================================================================
//=======================================================================================
//=======================================================================================
int main( int argc, char * argv[] ) {
 
  char *func_name = "itest"; // Pointer to name of the local function

  int istat;


  // Initialize the MPMnet (and/or) TCP/IP connection(s).
  istat = init_comms(); 
  if (istat == F_FAILURE) {
    printf("Communications initialization failure!\n");
    exit(1);
  }


  int action      = -1;
  int waitmode    =  0;
  int num_bunches =  1;
  int *p_species  =  NULL;
  int *p_train    =  NULL;
  int p_bunch     =  NULL;
  int num_samples =  0;
  int num_turns   =  1;
  int num_shots   =  0;
  int turn_select =  0;

  int elements[2] = {1, 2};
  
  istat = cbpm_request(action,      waitmode,  num_bunches, 
		       p_species,   p_train,   p_bunch,
		       num_samples, num_turns, num_shots, 
		       turn_select);
  


  return 0;
}
