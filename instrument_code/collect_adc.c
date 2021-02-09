//-----------------------------------------------------------------------+
// File         :  collect_adc.c  (DSP routine)                          |
//                                                                       |
// Description  :  Low level routine to activate the ADCs which then     |
//                 acquire data for each bunch that is marked in the     |
//                 selection registers.  This data is streamed directly  |
//                 into the hardware data buffers attached to each ADC   |
//                                                                       |
// Arguments    :  Instance of ..._RAW_PARAMS structure with acquisition |
//                 parameters.                                           |
//                                                                       |
// Contributors : J. Moffit, B. Rock, C. Strohman, M. Palmer, M. Rendina |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


int collect_adc( CBPM_RAW_PARAMS raw_params ) {

   // Add the function ID to the trace
   int FID = COLLECT_ADC_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;
   
  volatile int *p_error = INST_cmd.error;      
   
   int i, j, card, chan, sample;    // Counters
   int dsp_trig_mask;

   int status    = CBI_F_SUCCESS;

   int turn, done, trig_word;
   
   int nsturns = INST_cmd_params.trig_turns_delay; // Number of turns to wait after trigger masks
                                                   // are set but before acquisition starts.

   int tblock, idx, gain_setting;
   int temp_adc = 0;
   


   //----------------------------------------------
   // Set the number of samples (turns) to acquire.
   //  Always request from the hardware 
   //
   //    ONE EXTRA TURN.  
   //
   // This will be used to get all the requested 
   // data in the correct sampling order if a bunch
   // pattern wraps and the extra turn will simply 
   // be ignored during read-out if the bunch 
   // pattern does not wrap.  
   //      
   // Proper setting of Common TMD in non-14ns
   // timing setups now avoids scenarios where 
   // bunch patterns may wrap.
   //
   // May still be required for 14ns mode with
   // different species on each timing block.
   //----------------------------------------------
   // Break up the number of turns requested into 
   // two 16-bit portions, load those into the 
   // corresponding front-end registers.
   //----------------------------------------------
   short int num_turns_low  = 0;
   short int num_turns_high = 0;
   int num_turns_low_mask  = 0x0000FFFF;
   int num_turns_high_mask = 0xFFFF0000;
   num_turns_low  =  (raw_params.num_turns + 1) & num_turns_low_mask;
   num_turns_high = ((raw_params.num_turns + 1) & num_turns_high_mask) >> 16;


   int acq_con;
   for  (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
   	  *p_acq_turn_req[acq_con][REG_LOW]  = num_turns_low;
   	  *p_acq_turn_req[acq_con][REG_HIGH] = num_turns_high;
   }


   //--------------------------------------------------------------------
   // Wait for data enable flag to be set if use_data_enable is true
   //--------------------------------------------------------------------
   if (raw_params.use_data_enable == CBI_TRUE) {

      // Wait for enable bit to be set in command word
      done = CBI_FALSE;
      while (done == CBI_FALSE) {

         // get the data that comes encoded alongside the turns marker
         trig_word = *p_turn_data_word;
         
         // Check whether enable bit is set
         if (trig_word & DSP_COLLECT_ADC_ENABLE) {
            done = CBI_TRUE;
         }
             
         
        //If the control system sends a proper IDLE_CMD request, this if clause will break out of the loop.  
        //This can be used if a hangup occurs at this stage of acquisition.
		if (INST_cmd.handshake == CBI_CMD_NEW_HANDSHAKE &&    
		       INST_cmd.cmd  	== CBI_IDLE_CMD ) {            
		       add_debug( FID, 314159, CBI_DEBUG_LEVEL_0 );					   
		       INST_cmd.handshake = CBI_CMD_WORKING_HANDSHAKE; 
           cbpm_set_error(IDLE_COMMAND_LOOP_BREAK, p_error);
		       return CBI_F_FAILURE;                               
		}                                                     
         
      }
   }
   
   
   //----------------------------------------------------
   // Phase race-condition mitigation steps are numbered
   //
   //  1) Read the trigger mask from input structure
   //----------------------------------------------------
   // mask=0 => immediately read the data
   // mask=1 => wait for bit 0 in the trigger word to be set
   // mask=2 => wait for bit 1 in the trigger word to be set
   
   dsp_trig_mask = raw_params.trig_mask;
   
   // Apply that information
   if (dsp_trig_mask == TRIG_MASK_A) {   
      while (1){                                      // WAIT for turns marker trigger bit 'A' to come along
        if (*p_turn_data_word & TRIG_MASK_A) break;
	     
        //If the control system sends a proper IDLE_CMD request, this if clause will break out of the loop.  
        //This can be used if a hangup occurs at this stage of acquisition.      
        if (INST_cmd.handshake == CBI_CMD_NEW_HANDSHAKE &&    
          INST_cmd.cmd  	== CBI_IDLE_CMD ) {            
          add_debug( FID, 314159, CBI_DEBUG_LEVEL_0 );					   
          INST_cmd.handshake = CBI_CMD_WORKING_HANDSHAKE; 
          cbpm_set_error(IDLE_COMMAND_LOOP_BREAK, p_error);
          return CBI_F_FAILURE;                               
        }           
      }
      // wait for an additional nsturns; this can be of interest in order to
      // synchronize data taking to be synchronous with that of the BSM (note
      // that the BSM has a mechanical shutter that needs to be opened)       
      
      status = sleep_n_turns(nsturns);
   }

   if (dsp_trig_mask == TRIG_MASK_B) {   
    while (1){                                      // WAIT for turns marker trigger bit 'B' to come along
      if (*p_turn_data_word & TRIG_MASK_B) break;
	     
        //If the control system sends a proper IDLE_CMD request, this if clause will break out of the loop.  
        //This can be used if a hangup occurs at this stage of acquisition.      
      if (INST_cmd.handshake == CBI_CMD_NEW_HANDSHAKE &&    
        INST_cmd.cmd  	== CBI_IDLE_CMD ) {            
        add_debug( FID, 314159, CBI_DEBUG_LEVEL_0 );					   
        INST_cmd.handshake = CBI_CMD_WORKING_HANDSHAKE; 
        cbpm_set_error(IDLE_COMMAND_LOOP_BREAK, p_error);
        return CBI_F_FAILURE;                               
      }           
	  }
      // wait for an additional nsturns; this can be of interest in order to
      // synchronize data taking to be synchronous with that of the BSM (note
      // that the BSM has a mechanical shutter that needs to be opened)       
      status = sleep_n_turns(nsturns);
   }

   
   //--------------------------------------------
   // 2a) wait for new turns marker to arrive
   //--------------------------------------------
   int glob_tcounter;
   glob_tcounter = *p_turn_counter;

   // Accommodate roll-over condition (20-bit counter, maxval = 0xFFFFF = 1048575)
   if ( glob_tcounter == 0xFFFFF ) {
        while (*p_turn_counter != 0) { 
        	
        //If the control system sends a proper IDLE_CMD request, this if clause will break out of the loop.  
        //This can be used if a hangup occurs at this stage of acquisition.
		if (INST_cmd.handshake == CBI_CMD_NEW_HANDSHAKE &&    
		       INST_cmd.cmd  	== CBI_IDLE_CMD ) {            
		       add_debug( FID, 314159, CBI_DEBUG_LEVEL_0 );					   
		       INST_cmd.handshake = CBI_CMD_WORKING_HANDSHAKE; 
           cbpm_set_error(IDLE_COMMAND_LOOP_BREAK, p_error);
		       return CBI_F_FAILURE;                               
		} 	  
	}
   } else {
     while (*p_turn_counter != glob_tcounter + 1) { 
     	
        //If the control system sends a proper IDLE_CMD request, this if clause will break out of the loop.  
        //This can be used if a hangup occurs at this stage of acquisition.      
		if (INST_cmd.handshake == CBI_CMD_NEW_HANDSHAKE &&    
		       INST_cmd.cmd  	== CBI_IDLE_CMD ) {            
		       add_debug( FID, 314159, CBI_DEBUG_LEVEL_0 );					   
		       INST_cmd.handshake = CBI_CMD_WORKING_HANDSHAKE; 
           cbpm_set_error(IDLE_COMMAND_LOOP_BREAK, p_error);
		       return CBI_F_FAILURE;                               
		}        
     }
   }
      
   
   //--------------------------------------------
   // 2b) Tunable parameter here on a 
   //     per-instrument basis
   //
   //   Wait for ~1/2-turn ( ~1.25us ) or not.
   //--------------------------------------------
   //if (INST_cmd_params.phase_race_wait == CBI_TRUE) {
   if (raw_params.phase_race_wait == CBI_TRUE) {
     magic_wait( 20 );  // Verfied to wait for about 1.28 microseconds
   }
   
   
   //--------------------------------------------
   // 2c)
   // *******************************************
   // * Enable ADC buffers -- START ACQUISITION *
   // *******************************************
   //--------------------------------------------
   status = acquisition_control(ADC_CONTROL);
   
   glob_tcounter      = *p_turn_counter;
   // Save a snapshot of the turn counter value in the 
   // outgoing raw data heaader.
   INST_raw_data_header.turn_counter = glob_tcounter;

   // Check for data acquisition completion -
   // Stops acquisition as soon as all the done bits are high.
   int ph_counter = 0;
   
   
   // Place to tally up the acquisition controllers that have finished.
   int donecount;
   
   while (1) {
   	
     // Check for new turns marker arrival;
     // save the phase word in the turn slot, accommodate roll-over.
     if (glob_tcounter == 0xFFFFF) {
     	
       if ( *p_turn_counter == 0 ) {
          INST_ph_word_buf.ph_words[ph_counter] = *p_turn_data_word;
          glob_tcounter = *p_turn_counter;
          ph_counter++;
       } else {
	      continue;
       }
      
     } else {
     	
       if ( *p_turn_counter == glob_tcounter + 1 ) {
         INST_ph_word_buf.ph_words[ph_counter] = *p_turn_data_word;
         glob_tcounter = *p_turn_counter;
         ph_counter++;
       } else { 
	     continue;
       }
       
     }
     
    
     // Only check the done bits on the front-end cards if the
     // requested number of turns have elapsed to avoid interfering
     // with the cards during acquisition.
     //
     // Take into account whether or not the skip-turns feature has
     // been activated for this acquisition and multiply the total
     // number of turns to wait for by that factor.
     if ( ph_counter >= (raw_params.num_turns * (fpga_turns_to_skip+1) + 1) )  {
       // Break out if all cards report acquisition complete. 
       //if ( (*p_acq_control[0] & ACQ_DONE_BIT) &&
       //     (*p_acq_control[1] & ACQ_DONE_BIT) &&
       //     (*p_acq_control[2] & ACQ_DONE_BIT) &&
       //     (*p_acq_control[3] & ACQ_DONE_BIT) ) break; 
           
       // Tally up acquisition controllers that have finished.
       // Clear done status array before re-polling front ends.
       
       donecount = 0;
       for (acq_con = 0; acq_con < CBPM_MAX_ACQ_CONTROLLERS; acq_con++) {
       	  magic_wait(1); // Never returns from acquisition without a wait here!
       	  if ( (*(p_acq_control[acq_con]) & ACQ_DONE_BIT) ) {
       	  	donecount++;
       	  }
       }

       // Only drop out of this loop if all acquisition controllers are done.
       if (donecount == CBPM_MAX_ACQ_CONTROLLERS) {
       	  break;
       }
       
       
     }

        //If the control system sends a proper IDLE_CMD request, this if clause will break out of the loop.  
        //This can be used if a hangup occurs at this stage of acquisition.       
		if (INST_cmd.handshake == CBI_CMD_NEW_HANDSHAKE &&    
		       INST_cmd.cmd  	== CBI_IDLE_CMD ) {            
		       add_debug( FID, 314159, CBI_DEBUG_LEVEL_0 );					   
		       INST_cmd.handshake = CBI_CMD_WORKING_HANDSHAKE; 
           cbpm_set_error(IDLE_COMMAND_LOOP_BREAK, p_error);
		       return CBI_F_FAILURE;                               
		}      
           
   }

 
   //*******************************************
   //* Switch ADC buffers back to DSP control  *
   //*******************************************
   status = acquisition_control(DSP_CONTROL);


   // Decrement the trace and return
   INST_stat.num_levels--;
   return CBI_F_SUCCESS;
   
}
