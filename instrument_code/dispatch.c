//-------------------------------------------------------------------------
// File         :  dispatch.c
//
// Description  :  Parks DSP in a loop that waits for a command identifier
//                 to be received.  Once that happens, actions common to 
//                 all commands are performed, such as the examination and
//                 processing of the bunch pattern.  Then the requested
//                 command routine is executed.  Once the command is 
//                 finished, the DSP returns to wait in this loop for the 
//                 next command request.
//
// Contributors :  M. Rendina
//-------------------------------------------------------------------------

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


volatile int kill_time_count;


void dispatch( void )
{

   int cmd;           // Local copy of the command identifier
   int stat;          // return value from a DSP command function
   int nkept;         // to hold number of bunches to examine

   int i;

   volatile int *p_error                    = INST_cmd.error;
   volatile CBPM_CMD_PARAMS *p_params = &INST_cmd_params;
   int found_errors;
   int checksum;
   int good_params = CBI_TRUE;
   int acq_con;
   
   // Add function ID to the trace
   int FID = DISPATCH_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;

   
   //--------------------------------------------------
   // Install timer interrupt handler
   //--------------------------------------------------
   interrupt ( SIGTIMER0HP, timer0H_int_handler);
   
   //-----------------------------------------------------
   // Load the timer sample duration in core clock-cycles
   // into the countdown timer registers TMRIN0(H/L).
   //-----------------------------------------------------
   __builtin_sysreg_write( __TMRIN0H, 0 );
   __builtin_sysreg_write( __TMRIN0L, ((float)CBPM_CORE_CLOCK_FREQUENCY * (float)CBPM_CLOCK_SAMPLE_DURATION) );
   
   ///----------------------------------------------------   
   init_debug_registers();

   int rtemp;
   //--------------------------------------------------
   // Start timer by setting the TMR0RN bit in SQCTL
   //--------------------------------------------------
   rtemp = __builtin_sysreg_read( __SQCTL );
   rtemp = rtemp | SQCTL_TMR0RN;
   __builtin_sysreg_write( __SQCTL, rtemp );
   
   
   //------------------------------
   // Main command dispatch loop
   //------------------------------
   while(1){

      INST_heartbeat.heartbeat = 0;
      int heartbeat = 0;

      // Grab snapshot of the present turn count value
      turncount = *p_turn_counter;
      
      
      //------------------------------------------
      // Unmask TIMER 0 High priority interrupt
      //------------------------------------------
      rtemp = __builtin_sysreg_read( __IMASKH );
      rtemp = rtemp | INT_TIMER0H;
      __builtin_sysreg_write( __IMASKH, rtemp );
      
      
   	  //------------------------------------------------------
      // Wait for the handshake that indicates a new command.
      //
      // In this loop, also arrange for a heartbeat status to
      // be provided. 
      // A timing integrity status is provided by an interrupt
      // handler installed above.  See that routine's 
      // documentation (timer0H_int_handler) for details on
      // the mechanism.
      // 
      // TODO: This should also be integrated into all 
      //       continuous sampling modes as well.
      //------------------------------------------------------
      while(INST_cmd.handshake != CBI_CMD_NEW_HANDSHAKE) {
      	
      	 //--------------------------------
      	 // Increment DSP heartbeat value
      	 //--------------------------------
      	 INST_heartbeat.heartbeat = heartbeat++;
      	 if (heartbeat == 0x7FFFFFFF) {
      	 	heartbeat = 0;
      	 }
 
      }
      
           
      //-----------------------------------------------
      // Mask timer interrupt to allow smooth running
      // during command execution.  Possibly modify
      // this for continuously running commands.
      //-----------------------------------------------
      rtemp = __builtin_sysreg_read( __IMASKH );
      rtemp = rtemp & (~INT_TIMER0H);
      __builtin_sysreg_write( __IMASKH, rtemp );
      
      
      //-------------------------------------------------------
      // Handshake received.   Extract the command identifier. 
      //------------------------------------------------------- 
      cmd = INST_cmd.cmd;
      
      //-----------------------------------------------------------
      // Clear the error value
      //-----------------------------------------------------------
      //if (cmd != CBI_IDLE_CMD) {
         ////float_err_clear();
         cbpm_set_error(ERR_CLEAR, p_error);
      //}


      //-------------------------------------
      // Only lookup a command if the input
      // parameters structure is valid.
      //-------------------------------------
      if (good_params == CBI_TRUE) {
      	
         //----------------------------------------------------------
      	 // Before running any command requiring it, extract the 
      	 // bunch pattern from the command parameters struct, rotate 
      	 // the pattern for this particular BPM's use, and load it 
      	 // into the bunch pattern registers.
      	 //----------------------------------------------------------
         if ( (cmd != CBI_IDLE_CMD) && (cmd != TEST_RAW_DATA_CMD) && (cmd != FIND_BUNCH_CMD)
              && (cmd != SET_TIMING_SETUP_CMD) )  {
         
           if (timing_mode_G == m4ns) { // FIXME: Expand to include other potentially supported single-species modes.
           	
           	  // CMD_PARAMS.bunch_pat --> CMD_PARAMS.rot_bunch_pat
           	  int temp_BP_bits_wrapped[CBPM_MAX_TIMING_BLOCKS] = {0, 0};
      	      rotate_bunch_pattern_4ns( bun_pat_offset[0], temp_BP_bits_wrapped );
      	      int tblock;
      	      for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
      	         INST_raw_data_header.num_BP_bits_wrapped[tblock] = temp_BP_bits_wrapped[tblock];
      	      }
           } 
		   else {
           	  INST_raw_data_header.num_BP_bits_wrapped[0] = rotate_block_bunch_pattern( 0, bun_pat_offset[0] );
           	  INST_raw_data_header.num_BP_bits_wrapped[1] = rotate_block_bunch_pattern( 1, bun_pat_offset[1] );
           }

           int bunches_temp[CBPM_MAX_CHANS_PER_CARD] = {0,0};
           
           // CMD_PARAMS.rot_bunch_pat --> REGISTERS
      	   nkept = load_bunch_pattern( bunches_temp );
      	   INST_raw_data_header.num_bunches[0] = bunches_temp[0];
      	   INST_raw_data_header.num_bunches[1] = bunches_temp[1];
      	   
      	         	   
           //----------------------------------------------------------
      	   // Skip more than 3 turns if the number of bunches on a 
      	   // single channel exceeds the firmware dataflow threshold.  
      	   //
      	   // Turn off skip turns functionality if the number of 
      	   // requested bunches on both channels falls below that 
      	   // threshold.  Store this state in the raw_data_header for
      	   // control system use.
      	   //----------------------------------------------------------
      	   int tc;
      	   if (INST_raw_data_header.num_bunches[0] > CBPM_MAX_NOSKIP_BUNCHES ||
      	   	   INST_raw_data_header.num_bunches[1] > CBPM_MAX_NOSKIP_BUNCHES)   {

	           fpga_turns_to_skip = CBPM_SKIP_TURNS_DEFAULT;

      	   	   INST_raw_data_header.skip_turns_active = CBI_TRUE;
      	   	   for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      	   	      *p_acq_skip_turns[acq_con] = CBPM_SKIP_TURNS_DEFAULT;
      	   	   }
      	   	   for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      	   	   	 tc = *p_acq_control[acq_con];
      	   	   	 tc = tc | ACQ_SKIP_TURN_BIT;
      	   	   	 *p_acq_control[acq_con] = tc;
      	   	   }

      	   } else {

	     fpga_turns_to_skip = 0;
	           
      	   	   INST_raw_data_header.skip_turns_active = CBI_FALSE;
      	   	   for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
      	   	      tc = *p_acq_control[acq_con];
      	   	      tc = tc &  ~(0xFFFF | ACQ_SKIP_TURN_BIT);
      	   	      *p_acq_control[acq_con] = tc;
      	   	   }
 
           }
      	
           
           // Save the total number of bunches to examine in cmd_params
      	   INST_raw_data_header.tot_bunches = nkept;   
      	   
      	   // Reset the visited bunch tallies for each channel
      	   block_bunch_tally[0] = 0;
      	   block_bunch_tally[1] = 0;
      	   
	     } //endIF cmd != ...
      	 
	     
	     
      	 //-------------------------------------
         // Change the handshake status before 
         // entering the command function.
         //-------------------------------------
         INST_cmd.handshake = CBI_CMD_WORKING_HANDSHAKE;

         //---------------------------------------
         // Make the data from the temperature
         // sensors available to the control 
         // system when executing each command.
         //---------------------------------------
         publish_temps();
         
         
      	 //-------------------------------------
         // Initialize status before next execution through loop
         //-------------------------------------
         stat = CBI_F_SUCCESS;


         switch (cmd) {
         	// Universal commands
            case CBI_IDLE_CMD:
            
               //Typeout to debug the values of the INST_code_params.execution_flags
               if (INST_code_params.execution_flags[DSP_DEBUG_LEVEL] > CBI_DEBUG_LEVEL_0) {
               		add_debug(FID, 12345, CBI_DEBUG_LEVEL_0);
            		for( i = 0; i<CBPM_NUM_EXECUTION_FLAGS; i++) {
            			add_debug(FID, INST_code_params.execution_flags[i], CBI_DEBUG_LEVEL_0);
            		}
               		add_debug(FID, -12345, CBI_DEBUG_LEVEL_0);
               }
               stat = CBI_F_SUCCESS;
               break;         	
         	
            // Instrument type-specific commands
            case ACQUIRE_ALL_PEDESTALS_CMD:
               stat = acquire_all_pedestals();
               break;
            case BETATRON_PHASE_CMD:
               stat = get_betatron_phase();
               break;
            case CONTINUOUS_PROC_CMD:
               stat = get_continuous_data( CBPM_CONT_PROC );
               break;
            case CURRENT_DATA_CMD:
               stat = get_continuous_data( CBPM_CONT_CURR );
               break;
            case FIND_BTMD_CMD:
               stat = find_bTMD();
               break;
            case FIND_BUNCH_CMD:
               stat = find_bunch();
               break;
            case PEDESTAL_CAL_CMD:
               stat = pedestal_cal();
               break;
            case PUBLISH_TEMPS_CMD:
               stat = publish_temps();
               break;
            case PROC_DATA_CMD:
               stat = get_proc_data();
               break;
            case RAW_DATA_CMD:
               stat = get_raw_data();
               break;
            case SET_TIMING_SETUP_CMD:
               stat = set_timing_setup();
               break;
            case SET_GAINS_CMD:
               stat = set_gains();
               break;
            case TEST_RAW_DATA_CMD:
               stat = test_raw_data();
               break;
            case TIME_SCAN_CMD:
               stat = time_scan();
               break;
            case CLEAR_DEBUG_CMD:
               stat = clear_debug();
               init_debug_registers();
               break;
               
            default:
               stat = CBI_F_FAILURE;
               break;
         }
         

         //------------------------
         // Finish the handshake.
         //------------------------
        // See if there were any math errors.  // FIXME: enable
        if (float_err_check() == CBI_F_FAILURE) {
        //   stat = CBI_F_FAILURE;
        }

        // Put the status flag in the cmd struct
        INST_cmd.cmd_status = stat;

        found_errors = CBI_FALSE;
        for (i = 0; i < CBI_MAX_ERROR_WORDS; i++) {
           if (*(INST_cmd.error + i) != 0) found_errors = CBI_TRUE;
        }
        if (found_errors == CBI_FALSE) {
           cbpm_set_error(CBI_NO_ERR,p_error);
        } 
        
        
        //--------------------------------------------------------------
        // Need to pause before setting the handshake in case the DSP
        // completes during the interval between when the xbus has
        // picked up the old error information and the xbus picks up
        // the handshake. Delay needs to be on the order of 250 usec?
        //--------------------------------------------------------------
        kill_time_count = 0;
        while (kill_time_count < 12500) {
           kill_time_count++;
        }

        
        //---------------------------------------------------
        // now set the handshake to reflect command outcome 
        //---------------------------------------------------
        if (stat == CBI_F_SUCCESS) {
           INST_cmd.handshake = CBI_CMD_DONE_HANDSHAKE;
        } else {
           INST_cmd.handshake = CBI_CMD_ERROR_HANDSHAKE;
        }
     
      } // end -- if (good_params....
      
   } // end -- while(1)

}



