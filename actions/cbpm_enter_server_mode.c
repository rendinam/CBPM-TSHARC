//-----------------------------------------------------------------------+
// File         :  enter_server_mode.c                                   |
//                                                                       |
// Description  :  Wrapper to call the BIS-provided cbi_cesr_interface   |
//                 function which constitutes the server mode command    |
//                 loop.  This must be able to see the platform-specific |
//                 command list array and pass it down to the            |
//                 cbi_cesr_interface call.                              |
//                                                                       |
// Arguments    :  none                                                  |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int enter_server_mode( void ) {

  char *func_name = (char *)__FUNCTION__;
  int debug_level = CTL_System.debug_verbosity;

  char SERVER_NOTIFY[CBI_MAX_STRING_LENGTH] = {"SERVER\0"};
  char *menu_ready = "MENU\0";


  cbi_close_sockets();


  //--------------------------------------------------------------
  // Override the SIGINT signal handler when entering server mode
  // to allow breaking out of server mode and back to application.
  //--------------------------------------------------------------
  if (signal (SIGINT, server_mode_sigint_handler) == SIG_IGN) {
    signal (SIGINT, SIG_IGN);
  }


  if (CTL_System.RunningMode == CBI_BACKEND_MODE) {
    printf("Sending server mode notification to manager...%s\n", SERVER_NOTIFY);
    fflush(stdout);
    if(send(CTL_System.mgmt_socket_IDs[0], SERVER_NOTIFY, 7, 0) == -1) {
      perror("Server notify send error");
      fflush(stdout);
      return CBI_F_FAILURE;
    }
    sleep( (CTL_System.instance_ID-1) ); // Simple collision avoidance for now.
                                         // Space out server mode entries in time.
  }


  cbi_cesr_interface( CTL_System.command_list,
		      CTL_System.itype.num_platform_commands, 
		      TOT_CBPM_COMM_STRUCTS );


  if (CTL_System.RunningMode == CBI_BACKEND_MODE) {
    printf("Sending menu ready indicator...\n");
    fflush(stdout);
    if(send(CTL_System.mgmt_socket_IDs[0], menu_ready, strlen(menu_ready)+1, 0) == -1) {
      perror("send() error");
      close(CTL_System.mgmt_socket_IDs[0]);
      exit(1);
    }
  }


  //--------------------------------------------------------------
  // Re-instate standard keyboard interrupt signal handling to 
  // application exit.
  //--------------------------------------------------------------
  if (signal (SIGINT, std_sigint_handler) == SIG_IGN) {
    signal (SIGINT, SIG_IGN);
  }


  return CBI_F_SUCCESS;
  
}
