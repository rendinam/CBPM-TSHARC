//-----------------------------------------------------------------------+
// File         :  find_delays.c                                         |
//                                                                       |
// Description  :  Algorithms to set and fine tune the block             |
//                 (timing block-level) and channel (ADC-level) delays   |
//                                                                       |
// Author       :  J. Moffit / C. Strohman / M.Rendina / M. Stedinger    |
//-----------------------------------------------------------------------+

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"

int   chan_delay_cal(int species);
void  Collect(int species);
int   ProcessData(int species, int card);
int   CoeffMatrix(int card);
void  BackUpMatrix(void);
void  LUDecomp(void);
void  LUSolve(void);
void  SaveSol(void);		
void  Iterate(void);
void  UpdateSol(void);
int   CalLocOffset(int species, int card);
int   CheckValidity(int species);
void  MatMult(void);
int   Do_Block_Scan(find_delays_input_struct input, find_delays_output_struct* output);
int   Measure_ADC_RMS(float rms[CBPM_MAX_ACQ_CONTROLLERS]);
int   SloshDelays(int block_span, int* block_delay, int card_delays[4]);

// Amount to increase the block delay range scan by
// on each duplicate peak-finding pass.
#define  BLOCK_DELAY_RANGE_STEP   40

//-----------------------------------------------------------
// Global arrays needed for the curve fitter function chain
//-----------------------------------------------------------
float alpha[3][3];
float cAlpha[3][3];
float beta[3];
float cBeta[3];
float polyV[3];
float vSol[3];
//------------------------------------------------------------

//---------------------------------
// Other globals
//---------------------------------
int   tsetup;
int   bunch_bit_parity;
int   slosh;
int   prevChipValues[CBPM_MAX_ACQ_CONTROLLERS];
float prevPeakValues[CBPM_MAX_ACQ_CONTROLLERS];
int   prevBlockValue;
int   prevOffsetvalues[CBPM_MAX_ACQ_CONTROLLERS];
//----------------------------------



int find_delays( CBPM_RAW_PARAMS raw_params) {
	
  // Add function ID to the trace
  int FID = FIND_DELAYS_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  int   status = CBI_F_SUCCESS; // status returned by functions called
  int   new_status = CBI_F_SUCCESS; // status returned by next function called
  slosh = 0;

  int i, j, turn, card, bit, test;
  int   lblock, block_span, target_delay;
  int   dstep, buffer_offset;
  int   Delay_at_peak[CBPM_MAX_ACQ_CONTROLLERS] = {0,0,0,0};
  
  tsetup = INST_op_timing.active_timing_setup;
  
  int   delayval;
  float biggest;
  int   initial_bdelay;
  
  float max[CBPM_MAX_ACQ_CONTROLLERS] = {0,0,0,0};  // Max-value holding array for searching purposes
  float sum[CBPM_MAX_ACQ_CONTROLLERS] = {0,0,0,0};
  float avg[CBPM_MAX_ACQ_CONTROLLERS] = {0,0,0,0};
  
  float floatinputmax = 0.0;
  int inputmax = 0;
  int cardwithinputmax = 0;
  
  volatile int *p_error = INST_cmd.error;
  
  int species = INST_cmd_params.species;
  
  CBPM_RAW_PARAMS local_raw_params = {
    CBPM_SCAN_TURNS,             //num_turns
    RAW_DATA,                    //scale_mode
    0,                           //trig_turns_delay
    0,                           //spacex_turn
    CBI_FALSE,                   //use_data_enable
    TRIG_MASK_NONE,   //trig_mask
    0                            //phase_race_wait
  };
  
  //------------------------------------------------------------
  // Determine the time scan bunch bit's parity.
  // This is the channel on which the data will appear.
  // Remember, there is only a single bit enabled for timing 
  // scans and timing calibrations in general.
  //------------------------------------------------------------
  for (bit = 0; bit < MAX_BUNCHES; bit++) {
    if(INST_cmd_params.rot_bunch_pat[bit] == 1) {
      if (bit % 2 == 0) {
        bunch_bit_parity = 0;
        break;
      } 
      else {
        bunch_bit_parity = 1;
        break;
      }
    }
  }   
  
  int block_delays_AreInitialized = INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity] >= 0;
    
  
  if (INST_cmd_params.generate_delay_corrections) {
    //MEASURE RMS First thing
    Measure_ADC_RMS(INST_find_delay_out.prevADCRMSs);
  }
  
  set_delays(tsetup);    
  	
  //Initialize Previous value arrays
  for (card = 0; card< CBPM_MAX_ACQ_CONTROLLERS; card++) {
    prevChipValues[card]    =  INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][card];
    
    prevPeakValues[card]    =  INST_find_delay_out.peaks[card];
    prevOffsetvalues[card]  =  INST_find_delay_out.offsets[card];
  }
  prevBlockValue          =  INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity];
  
    //---------------------------------------------------------------
  // Set the initial block delay scan range depending on the 
  // timing mode.  Start just shy of the minimum since the loop
  // below increments block delays for the scan at the beginning 
  // of each iteration.
  //---------------------------------------------------------------
   int fallback_span;
   if ( timing_mode_G == m4ns ) {   
     block_span = fallback_span = 800;
   } else {
     block_span = fallback_span = 1400;  
   } 

  
  if (INST_code_params.execution_flags[USE_TIME_SCAN_TESTS] && 
      block_delays_AreInitialized && 
  	  INST_code_params.execution_flags[SLOSH_DELAYS] && 
  	  INST_cmd_params.generate_delay_corrections) {
 	
	  int cardDelays[4];
	  for (i = 0; i<4; i++) {
	    cardDelays[i] =  INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][i];
	  } 
	  int blockDelay = INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity];

    SloshDelays(fallback_span, &blockDelay, cardDelays);

    for (i = 0; i<4; i++) {
      INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][i] = cardDelays[i];
    	*p_chan_delays[i][bunch_bit_parity] = cardDelays[i]; 
	  }

	  INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity] = blockDelay;
	  set_block_delay( bunch_bit_parity, blockDelay );
  }//endif block_delays_AreInitialized, Sloch, Generate_Delay
  

  // Only determine the ideal block delay scan window if the block
  // delay sampling point has not yet been determined as these multiple
  // scans are rather time consuming.
  //--------------------
  //  BLOCK DELAY SCAN - when block delay UNinitialized
  //--------------------
  if (!block_delays_AreInitialized) {		
    
    //---------------------------------------------------------------
    // Determine the ideal block scan range.  This will perform
    // multiple block delay scans to find the maximum range that
    // does not result in multiple copies of the same waveform
    // peak due to digitization timing wrap-around.  The maximization 
    // of this scan range is a necessary precaution because of 
    // variation in the nominal 10ps delay steps provided by the 
    // timing hardware that, when smaller than expected due to normal
    // variation, may result in the timing algorithm missing the 
    // detection of a signal peak on a given card.
    //
    // Approach:  Repeatedly scan the block delay starting at a range
    // minumum appropriate for the timing mode.  
    // Once a second peak appears based on the criteria below for any
    // card, reduce the scan range by some sensible value to again 
    // remove the second copy of the peak.  This is the new ideal 
    // scan range for the instrument under conditions present during
    // the scan.
    //
    // This process will only have to run when
    //   a block delay needs to be initially determined
    //      (first-pass)
    //
    // Otherwise, this initial range-determining step and the
    // subsequent waveform-acquiring steps should be SKIPPED by
    // default, as it needlessly lengthens the time required to
    // obtain fine-tuned channel delay updates.
    //---------------------------------------------------------------
    find_delays_input_struct input = {
      block_span / CBPM_BLOCK_STEP,		//number of delay_steps
      0,								//initial_bdelay
      CBPM_BLOCK_STEP,
      bunch_bit_parity,
      species,
      local_raw_params
    };
    find_delays_output_struct output;
	
    output.single_peak = CBI_TRUE;
    while ( output.single_peak ) {
      
      // Increase the scan range for the next pass
      block_span += BLOCK_DELAY_RANGE_STEP;
      
      if ( block_span > 1900 ) {
        block_span = fallback_span + BLOCK_DELAY_RANGE_STEP;
        break;
      }
      
      // Clear the comparison array
      for (i = 0; i < CBPM_MAX_ACQ_CONTROLLERS; i++) {
        max[i] = 0.0;
      }
      
      input.delay_steps = block_span / CBPM_BLOCK_STEP;

      status = Do_Block_Scan(input, &output);
      if (new_status > status) {
        status = new_status;
      } 	   

      for (i = 0; i < CBPM_MAX_ACQ_CONTROLLERS; i++) {
        max[i] = output.max[i];
        Delay_at_peak[i] = output.Delay_at_peak[i];
      }	
      
    } //endWHILE single_peak
    
    // Back off by one block delay range step to avoid
    // the second copy of the signal peak that was just discovered.
    // This should leave the range at a tenable setting for scanning
    // the block delays without multiple peak interference.
    block_span -= BLOCK_DELAY_RANGE_STEP;
    
  } // endIF block delay uninitialized
  
  
  // Publish block delay scan range in communications structure 
  // to give control system access to it.
  INST_find_delay_out.max_block_delay = block_span;
  
  
  //------------------
  //  BLOCK DELAY SCAN - when block delay ALREADY initialized
  //------------------
  
  //if block_delays is     initialized, the default block_span set above is used.  
  //											We seem to be assuming there will be only one peak in the range.
  if (block_delays_AreInitialized) { 	

    initial_bdelay = INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity] - (fallback_span/2);
    
    if (initial_bdelay < 0) {
      initial_bdelay = 0;       // FIXME: look into this for waveform alignment with BD sample point
    }
   
    find_delays_input_struct input = {
      block_span / CBPM_BLOCK_STEP,		//number of delay_steps
      initial_bdelay,
      CBPM_BLOCK_STEP,
      bunch_bit_parity,
      species,
      local_raw_params
    };
    find_delays_output_struct output;
    
    new_status = Do_Block_Scan(input, &output);   
    
    if (new_status > status) {
      status = new_status;
    }       	   	     
	           
    for (i = 0; i < CBPM_MAX_ACQ_CONTROLLERS; i++) {
      max[i] = output.max[i];
      Delay_at_peak[i] = output.Delay_at_peak[i];
    }   
  }// endIF block_delays_AreInitialized

  // Determine the input (acquisition controller) that saw the largest absolute signal
  for (i = 0; i < CBPM_MAX_ACQ_CONTROLLERS; i++) {	
    if (max[i] > inputmax) {
      inputmax = max[i];
      cardwithinputmax = i;
    }
  }        
 

  //---------------------------------------------------------------
  // If the block delay values in INST_find_delay_work have 
  // already been provided, either by a previous run of 
  // find_delays  -OR-  by being loaded explicitly from the 
  // control system via the timing_params struct, use that value
  // (the sum of two chained block delay chip values) instead of
  // the block delay determined in the steps above.
  //---------------------------------------------------------------
  if (!block_delays_AreInitialized) { 
  
	// Simple single-card max find
    target_delay = Delay_at_peak[cardwithinputmax];
    
    //---------------------------------------------------------
    // If target block delay is within 2*(cal radius)+1 of 0,
    // 'slosh' delay counts uniformly out of the channel delay
    // and into the GD to move the peak and the sampling point
    // together away from 0 by enough that the channel delay
    // tuning can proceed.
    //---------------------------------------------------------
    if (target_delay < (2*CBPM_CAL_RADIUS+1)) {
      add_debug( FID,666, CBI_DEBUG_LEVEL_0 ); 
      target_delay += (2*CBPM_CAL_RADIUS+1);
      add_debug( FID,target_delay, CBI_DEBUG_LEVEL_0 ); 
      for (card = 0; card < CBPM_MAX_CARDS; card++) {
        INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][card] -= (2*CBPM_CAL_RADIUS+1);
        *p_chan_delays[card][bunch_bit_parity] = INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][card];
        add_debug( FID,INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][card], CBI_DEBUG_LEVEL_0 ); 
      } //endFOR card
      add_debug( FID,666, CBI_DEBUG_LEVEL_0 );       
    }
    //---------------------------------------------------------
    
    target_delay = clipi(target_delay, 0, block_span);      
    INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity] = target_delay;
    
  } else {     

    // Cached block delay value should be used
    target_delay = INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity];
  }
  
  set_block_delay( bunch_bit_parity, target_delay );
  
  
  
  //--------------------------------------------------------------
  // Adjust each channel delay to bring the max roughly into
  // position at the center block delay determined above.
  // Set each channel to a time slightly before these new
  // channel delay values (CBPM_CAL_RADIUS) and adjust as necessary if
  // this falls too close to the min or max delay values allowed.
  // 
  // This grouping of peaks together is necessary because the
  // channel delay adjustments can only adjust a maximum of some
  // fraction of calwidth (= CBPM_CAL_RADIUS*2).  If the peaks are
  // initially greater than that many counts away from the block
  // delay sampling point, a single pass will be insufficient to
  // align every peak onto the sampling point.  Thus, this first
  // rough pass ensures that the second fine pass will always
  // succeed.
  //--------------------------------------------------------------
  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    // This realigns the channel delays to group peaks of the waveforms
    // nearer to the central block delay.
    //
    // OFFSET = Starting Channel Delay for each card - (difference between target GD and GD of the peak on this card)
    //
    INST_find_delay_out.offsets[card] = INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][card] - (target_delay - Delay_at_peak[card]);
    INST_find_delay_out.offsets[card] = clipi(INST_find_delay_out.offsets[card], 2 * CBPM_CAL_RADIUS +1, (CBPM_MAX_CHAN_DELAY_COUNTS-1) - (2 * CBPM_CAL_RADIUS +1));  
    
    // This sets the channel delays to one side of the peak to scan across 
    INST_find_delay_out.offsets[card] = INST_find_delay_out.offsets[card] - CBPM_CAL_RADIUS; 

    
    if (INST_cmd_params.generate_delay_corrections) {
      // write the starting delay to the channel delay register
      *p_chan_delays[card][bunch_bit_parity] = INST_find_delay_out.offsets[card];
      
      INST_diagnostic.pre_pf_peak_block_delays[card]  = Delay_at_peak[card];
      INST_diagnostic.pre_pf_offset_chan_delays[card] = INST_find_delay_out.offsets[card];
    }
        
  } //endFOR card
  
  //set_delays(tsetup);  
  
  if (INST_cmd_params.generate_delay_corrections) {
    //--------------------------------------------
    // now trim the individual channels 
    //--------------------------------------------
    
    new_status = chan_delay_cal(species);  // Perform fine-adjustment window data acquisition /
    if (new_status > status) {
      status = new_status;
    }   	
  
    Measure_ADC_RMS(INST_find_delay_out.postADCRMSs);
    
    for (card = 0; card < CBPM_MAX_ACQ_CONTROLLERS; card++) {
      INST_bfind_delay_out.peakshist[INST_find_delay_params.hist_index][card] = INST_find_delay_out.peaks[card];
      INST_bfind_delay_out.offsetshist[INST_find_delay_params.hist_index][card] = INST_find_delay_out.offsets[card];
      INST_bfind_delay_out.chipshist[INST_find_delay_params.hist_index][card] = INST_find_delay_out.offsets[card] + INST_bfind_delay_out.peakshist[INST_find_delay_params.hist_index][card];
      INST_bfind_delay_out.blockhist[INST_find_delay_params.hist_index] = INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity];
    }	     
      

    new_status = CheckValidity(species);
    if (new_status > status) {
      status = new_status;
    }	  
	
    INST_btest_results.statushist[INST_find_delay_params.hist_index] = status;
	       

    //Determine whether to accept new timings or not
	   int timingUpdateIsGood = 1;
  
	  for (card = 0; card<CBPM_MAX_CARDS; card++) {
    
      for(test = 0; test < CBPM_NUM_TESTS; test++) {
        if (INST_code_params.tests[test].required_for_update_acceptance) {
          timingUpdateIsGood = timingUpdateIsGood && (INST_test_results.Statuses[test][card] < CBPM_TEST_ENUM_ERROR);
        }
      }//fot test
      
    }
                
    int rollback_changes = INST_code_params.execution_flags[USE_TIME_SCAN_TESTS] && !timingUpdateIsGood && !INST_code_params.execution_flags[OVERRIDE_TIMING_ROLLBACK];

    INST_bfind_delay_out.timingUpdateIsGoodhist[INST_find_delay_params.hist_index] = timingUpdateIsGood;
    INST_bfind_delay_out.rollbackhist[INST_find_delay_params.hist_index] = rollback_changes; 


    if (rollback_changes) { //Put chip settings back
   
      for(card = 0; card < CBPM_MAX_CARDS; card++) {
        INST_diagnostic.pre_pf_offset_chan_delays[card] 								        = prevOffsetvalues[card];

        INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][card] 	= prevChipValues[card];
        *p_chan_delays[card][bunch_bit_parity]                                  = prevChipValues[card];
      } //endFOR
      	   
      INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity]  	  		= prevBlockValue;
      set_block_delay(bunch_bit_parity, prevBlockValue );
    } //endIF rollback_changes 
	   
  }//endIF INST_cmd_params.generate_delay_corrections    
  
  set_delays(tsetup); 
  
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
  
}


int Do_Block_Scan(find_delays_input_struct input, find_delays_output_struct* output) {
  //---------------------------------------------------------------------
  // Scan over the entire specified GLOBAL (block) delay range 
  // for the selected timing block (A or B).
  // Find the BLOCK delay settings for the peak of each channel. 
  //---------------------------------------------------------------------
  // Request an acquisition for each delay setting in the range of the
  // block delay scan.  ADC buffers get filled in the following fashion:
  //
  //  Dx  = delay step
  //  Bx  = bunch
  //  Tux = turn
  //
  //  |             |                         |           |            |
  //  +---+---+---+ +---+---+---+  ---+---+---+---+---+---+ ---+---+---+
  //  | D0| D0| D0| | D0| D0| D0|   D1| D1| D1| D1| D1| D1|  D2| D2| D2| 
  //  |B0 |B..|BN | |B0 |B..|BN |  B0 |B..|B0 |B1 |B..|B1 | B0 |B1.|B0 | o o o 
  //  |Tu0|Tu0|Tu0| |Tu1|Tu1|Tu1|  Tu0|Tu0|Tu0|Tu1|Tu1|Tu1| Tu0|Tu0|Tu1|
  //  +---+---+---+ +---+---+---+  ---+---+---+---+---+---+ ---+---+---+
  //
  //---------------------------------------------------------------------	
   
  //   Double peak presence criteria:
  //     If, after finding one max on a given input, another value
  //     is found that is   BOTH   
  //     a)  > 90% of that input's maximum
  //              - AND - 
  //     b)  more than 300 delay counts to the right of the first 
  //         maximum.
		
  // Add function ID to the trace
  int FID = DO_BLOCK_SCAN_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;

  int status = CBI_F_SUCCESS;
  int dstep;
  int i, card, turn, buffer_offset;
  
  volatile int *p_error = INST_cmd.error;  
  
  float sum[CBPM_MAX_ACQ_CONTROLLERS];  
  float avg[CBPM_MAX_ACQ_CONTROLLERS];

  //   Double peak presence criteria:
  //     If, after finding one max on a given input, another value
  //     is found that is   BOTH   
  //     a)  > AmpThreshold of that input's maximum
  //              - AND - 
  //     b)  more than DistThreshold delay counts to the right of the first 
  //         maximum.  
  float AmpThreshold = 0.9;
  int DistThreshold  = 300;
  int alwaysLow[CBPM_MAX_ACQ_CONTROLLERS];// = {1,1,1,1};
    
  //Initialize output struct
  (*output).single_peak = CBI_TRUE;
  for (i = 0; i < CBPM_MAX_ACQ_CONTROLLERS; i++) {
    (*output).max[i] = -CBPM_ADC_LIMIT;
    (*output).Delay_at_peak[i] = 0;
    alwaysLow[i] = 1;
  }     
	
    int ISsaturated[CBPM_MAX_ACQ_CONTROLLERS];
    int IsTooHigh[CBPM_MAX_ACQ_CONTROLLERS]; 
    
    
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
	    ISsaturated[card] = 0;
	    IsTooHigh[card] = 0;
    }
	
    int delay = input.initial_delay;   
    for(dstep = 0;  dstep < input.delay_steps;  dstep++) {
     
    //increment delay
    delay += input.block_step;										
 	
    //set block delay in HW
    set_block_delay(input.bunch_bit_parity, delay );				
    
    // Set mem_first, the starting cardess of the 
    // next acquisition on the front-end cards.
    buffer_offset = dstep * input.local_raw_params.num_turns;
    memfirsts( buffer_offset );
 
 
    //------------------------------
    // *** FILL THE ADC BUFFERS ***
    //------------------------------
    status = collect_raw(input.local_raw_params);
    
     if (status == CBI_F_FAILURE) {
       cbpm_set_error(FIND_DELAYS_COLLECT_ADC_ERR1,p_error);
       //INST_stat.num_levels--;
       //return CBI_F_FAILURE;
     }	
     
    status = check_raw_buf(input.local_raw_params,buffer_offset);
    //status = process_adc(PROC_THRESHOLDS_ONLY, 0, input.local_raw_params.num_turns, NULL, NULL);
    
     if (status == CBI_F_FAILURE) {
       cbpm_set_error(FIND_DELAYS_PROCESS_DATA_ERR1,p_error);
       //INST_stat.num_levels--;
       //return CBI_F_FAILURE;
     }
    
    int stat;	
    
	//--------------------------------------------
	// Clear the accumulators used for
	// amplitude averaging over turns.
	//--------------------------------------------
	for (i = 0; i < CBPM_MAX_ACQ_CONTROLLERS; i++) {
	  sum[i] = 0.0;
	}	  
    	
    //---------------------------------------------------
    // Sum all the data collected at this delay setting
    // Average it over the number of turns acquired.
    //---------------------------------------------------
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
          
      stat = get_chan_stat(INST_raw_data_header.ADC_saturation_bitfields[card], bunch_bit_parity,0);
      ISsaturated[card] = ISsaturated[card] || stat;
           
      stat = get_chan_stat(INST_raw_data_header.ADC_high_thresh_bitfields[card], bunch_bit_parity,0);
      IsTooHigh[card] = IsTooHigh[card] || stat;
      
      stat = get_chan_stat(INST_raw_data_header.ADC_lower_thresh_bitfields[card], bunch_bit_parity,0);
      alwaysLow[card] = alwaysLow[card] && stat;
           
      //Initialize summation array
      sum[card] = 0.0;
      //compute sums
      for (turn = 0; turn < input.local_raw_params.num_turns; turn++) {
        sum[card] += sign_extend( *(p_adc_bufs[card][bunch_bit_parity] + buffer_offset + turn)  ); 
      }
      
      // Average amplitude over all turns
      avg[card] = sum[card] / input.local_raw_params.num_turns; 
           
      //update max seen so far
      int val;
      if (input.species == POSITRONS) {
        val = avg[card];
      }
      else
      {
        val = -avg[card];
      }
      
      if ( val  > (*output).max[card]) {
        (*output).Delay_at_peak[card]  = delay;
        (*output).max[card]         = val;
      }
         
      // Check for presence of a second peak
      if (val > (AmpThreshold * (*output).max[card])) { 
        if (delay - (*output).Delay_at_peak[card] > DistThreshold) {
          (*output).single_peak = CBI_FALSE;
          break;
        }
      }
	   
    } //endFOR card
 
  } //endFOR dstep  (block delay scan)
   
  // Reset mem_first regsiters
  memfirsts( 0 );  

  for (card = 0; card < CBPM_MAX_CARDS; card++) {
    //Checks ADC Saturation
    PerformTest(TEST_COLLECT_RAW_ADC_SATURATED, card, ISsaturated[card], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);  

    //Checks ADC High values
    PerformTest(TEST_SIGNAL_TOO_HIGH, card, IsTooHigh[card], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);	

    PerformTest(TEST_SIGNAL_TOO_LOW, card, (float) alwaysLow[card], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
  } //endFOR card 
  
  INST_stat.num_levels--;
  if (status == CBI_F_FAILURE) {
    cbpm_set_error(FIND_DELAYS_BLOCK_SCAN_ERR, p_error);
  }
  return status;
}


//========================================================================
// Support functions for curve fitting below
//========================================================================



int chan_delay_cal(int species) {
  int card;
  int status, new_status;
  int i;
  volatile int *p_error = INST_cmd.error;
  
  // Add function ID to the trace
  int FID = CHAN_DELAY_CAL_ID;
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  status = CBI_F_SUCCESS;
  
  Collect(species);
  
  for(card=0; card < CBPM_MAX_CARDS; card++) {
      
    float *data  = &INST_chan_cal_work.chan_cal_data[card].data[0];
    float *error = &INST_chan_cal_work.chan_cal_data[card].error[0];  
    for(i=0; i<2*CBPM_CAL_RADIUS+1; i = i + CHAN_SCAN_STEP) {  
      *data++  = 0.0;
      *error++ = 0.0;
    }
    
    new_status = ProcessData(species,card);
    if (new_status > status) {
      status = new_status;
    }	    		
  } //endFOR card     
  
  for(card=0; card < CBPM_MAX_CARDS; card++) {
    new_status = CoeffMatrix(card);
    if (new_status > status) {
      status = new_status;
    }
    
    BackUpMatrix();
    LUDecomp();
    LUSolve();
    SaveSol();
    Iterate();
    UpdateSol();
    new_status = CalLocOffset(species, card);
    
    if (new_status > status) {
      status = new_status;
    }
        
  } //endFOR card
  
  INST_stat.num_levels--;
  return status;
}


//--------------------------------------------------------------------------------------



void Collect(int species)
{   
  int i, turn, card;
  int status = CBI_F_SUCCESS;
  
  volatile int *p_error = INST_cmd.error;    
  
  // Add function ID to the trace
  int FID = COLLECT_ID;  
  INST_stat.trace[INST_stat.num_levels++] = FID;

  CBPM_RAW_PARAMS raw_params = {CBPM_FIT_TURNS,
				RAW_DATA,      // scale mode
				1,             // was fd_trig_turns_delay, // check, was 0
				0,             // was fd_spacex_turn,   
				CBI_FALSE,     // use_data_enable 
				TRIG_MASK_NONE};  //TRIG_MASK_A};  // CBPM_TRIG_MASK_A}; was 0
  
  CBPM_RAW_PARAMS raw_params60Hz = {CBPM_FIT_TURNS,							//Synchronize to 60 Hz
				    RAW_DATA,      // scale mode
				    1,             // was fd_trig_turns_delay, // check, was 0
				    0,             // was fd_spacex_turn,   
				    CBI_FALSE,     // use_data_enable 
				    TRIG_MASK_A};  // CBPM_TRIG_MASK_A}; was 0       

  CBPM_RAW_PARAMS raw_params_start60Hz = {CBPM_FIT_TURNS,							//Synchronize to 60 Hz
		RAW_DATA,      // scale mode
		1,             // was fd_trig_turns_delay, // check, was 0
		0,             // was fd_spacex_turn,   
		INST_find_delay_params.use_data_enable,     // use_data_enable 
		TRIG_MASK_A};//,   // CBPM_TRIG_MASK_A}; was 0       
					  //0};            //phase_race_wait
            
  status = collect_raw( raw_params_start60Hz );
  INST_code_params.execution_flags[OUTPUT_TURN_COUNTER] = *p_turn_counter;    
  
  if (status == CBI_F_FAILURE) {
    cbpm_set_error(FIND_DELAYS_COLLECT_RAW_ERR1,p_error);
    //INST_stat.num_levels--;
    //return CBI_F_FAILURE;
  }	   
  
  //-------------------------------------------------------------------------   
  // collect data at each delay setting. We are already CBPM_CAL_RADIUS before the
  // peak, so run till we are CBPM_CAL_RADIUS after the peak
  //-------------------------------------------------------------------------
  INST_find_delay_out.numTurnsToCollectPeakData = *p_turn_counter;  
  
  for(i=-1; i<2*CBPM_CAL_RADIUS+1; i = i + CHAN_SCAN_STEP) {	
    //start at minus 1, throwing away first measurement there is often a glitch there. not sure why, but discarding it works, the rest of the data is usually fine
    
    //-------------------------------------------------------------
    // Get raw ADC data at this delay setting
    //-------------------------------------------------------------
    int buffer_offset = (i+1) * CBPM_FIT_TURNS;
    memfirsts( buffer_offset );
    
    status = collect_raw( raw_params );
    
    if (status == CBI_F_FAILURE) {
      cbpm_set_error(FIND_DELAYS_COLLECT_RAW_ERR2,p_error);
      //INST_stat.num_levels--;
      //return CBI_F_FAILURE;
    }    
    
    // increment this channel's delay for the next pass
    for(card=0; card < CBPM_MAX_CARDS; card++) {
      *p_chan_delays[card][bunch_bit_parity] = INST_find_delay_out.offsets[card] + i;
    }
    
  }
  
  INST_find_delay_out.numTurnsToCollectPeakData = *p_turn_counter - INST_find_delay_out.numTurnsToCollectPeakData;  
  
  memfirsts( 0 );
  
  INST_stat.num_levels--;
  return;
}


int  ProcessData(int species, int card)
{   
  int i, turn;
  float temp, tempSquare, avg, avgSquare, meanSquare, tempError;
  
  float *data  = &INST_chan_cal_work.chan_cal_data[card].data[0];
  float *error = &INST_chan_cal_work.chan_cal_data[card].error[0];
  
  int avg_mode = INST_cmd_params.avg_mode;
  int loc_tmp, loc_tmpfrac;
  
  // Add function ID to the trace
  int FID = PROCESS_DATA_ID; 
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  
  //-------------------------------------------------------------------------   
  // collect data at each delay setting. We are already CBPM_CAL_RADIUS before the
  // peak, so run till we are CBPM_CAL_RADIUS after the peak
  //-------------------------------------------------------------------------
  for(i=0; i<2*CBPM_CAL_RADIUS+1; i = i + CHAN_SCAN_STEP) {
    
    // clear the accumulators
    temp = 0.0;
    tempSquare = 0.0;   
    
    //-----------------------------------      
    // Sum the ADC data for this buffer
    //-----------------------------------
    for(turn = 0; turn < CBPM_FIT_TURNS; turn++) {   
      // Use sign extension fix when reading from ADC buffers.
      loc_tmp = sign_extend( *(p_adc_bufs[card][bunch_bit_parity] + (i+1) * CBPM_FIT_TURNS + turn) );
      
      temp       += loc_tmp;
      tempSquare += loc_tmp * loc_tmp;
    }
    
    
    // Fitter DIAGNOSTIC:
    // Union to allow delivery of float data through the debug list structure
    // that is only capable of handling integers right now.
    // These raw bit patterns can be cast back to floats once they are delivered
    // to the control program.
    union { int i; float f; } uvar;
    
    
    avg = temp / CBPM_FIT_TURNS;
    avgSquare  = tempSquare / CBPM_FIT_TURNS;		
    meanSquare = avgSquare - (avg * avg);     //RMS^2 (single sample noise)
    tempError  = sqrt(meanSquare);
    
    // Fitter DIAGNOSTIC:
    // Store floats in union to preserve entire bit patterns
    // Deliver as ints.
    // For each card:
    // a) avg
    // b) avgSquare
    // c) meanSquare
    // d) tempError
    uvar.f = tempSquare;
    uvar.f = meanSquare;
    
    
    if (species == ELECTRONS) {
      avg = -avg;
    }
    
    // data in the chan_cal_work array
    *data++  = avg;
    *error++ = tempError;
  } 
  
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
}



//-----------------------------------------------------------------------
int  CoeffMatrix(int card)
{
  float x, y, sig2i, sig;
  int i,j,k;
  
  float *data  = &INST_chan_cal_work.chan_cal_data[card].data[0];
  float *error = &INST_chan_cal_work.chan_cal_data[card].error[0];
  int status = CBI_F_SUCCESS;
  int sigmaNeverZero = 1;
  
  
  // Add function ID to the trace
  INST_stat.trace[INST_stat.num_levels++] = COEFF_MATRIX_ID;
  
  for(j=0; j<3; j++) {
    beta[j] = 0.0;
    for(k=0; k<3; k++) {
      alpha[j][k] = 0.0;
    }
  }
  
  for(i=0; i<(2*CBPM_CAL_RADIUS+1)/CHAN_SCAN_STEP; i++) {	
    x = (i*CHAN_SCAN_STEP);
    y = *(data + i);
    sig = *(error + i);  //RMS^2
    if (sig == 0.0) {
      sig = 1.0; //If for some reason, the error is zero, set sigma to 1.0 so that the fit doesn't blow up.
      sigmaNeverZero = 0;
    }    
    sig2i = 1.0/(sig);				
    
    polyV[0] = x*x;
    polyV[1] = x;
    polyV[2] = 1;
    
    for(j=0; j<3; j++)
    {
      beta[j] += sig2i*polyV[j]*y;
      for(k=0; k<3; k++) {
        alpha[j][k] += sig2i*polyV[j]*polyV[k];
      }
    }
    
  }
  
  if (sigmaNeverZero) {
	PerformTest(TEST_SIGMA_IS_ZERO, card, sig, INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
  }
  else
  {
  	PerformTest(TEST_SIGMA_IS_ZERO, card, 0.0, INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
  }
  
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
}



//-------------------------------------------------------------------
void BackUpMatrix()
{
  int i, j;
  
  // Add function ID to the trace
  INST_stat.trace[INST_stat.num_levels++] = BACKUP_MATRIX_ID;
  
  for(i=0; i<3; i++) {
    cBeta[i] = beta[i];
    for(j=0; j<3; j++) {
      cAlpha[i][j] = alpha[i][j];
    }
  }
  
  INST_stat.num_levels--;
  return;
}



//----------------------------------------------------------------
void LUDecomp(void)
{
  int i, j, k;	
  
  // Add function ID to the trace
  INST_stat.trace[INST_stat.num_levels++] = LU_DECOMP_ID;
  
  for(j=0; j<3; j++)
    {
      for(i=1; i<j+1; i++) {
	for(k=0; k<i; k++) {
	  alpha[i][j] -= alpha[i][k]*alpha[k][j];
	}
      }
      
      for(i=j+1; i<3; i++) {	
	for(k=0; k<j; k++) {
	  alpha[i][j] -= alpha[i][k]*alpha[k][j];
	}
	alpha[i][j] /=alpha[j][j];	
      }
    }
  
  INST_stat.num_levels--;
  return;
}



//---------------------------------------------------------------------
void LUSolve(void)
{
  int i, j;
  
  // Add function ID to the trace
  INST_stat.trace[INST_stat.num_levels++] = LU_SOLVE_ID;
  
  for(i=1; i<3; i++){
    for(j=0; j<i; j++){
      beta[i] -= alpha[i][j]*beta[j];
    }
  }
  
  for(i=2; i>=0; i--) {
    for(j=i+1; j<3; j++) {
      beta[i] -= alpha[i][j]*beta[j];
    }
    beta[i] /= alpha[i][i];
  }
  
  
  INST_stat.num_levels--;
  return;
}



//----------------------------------------------------------------
void SaveSol(void)
{
  int i;
  
  // Add function ID to the trace
  INST_stat.trace[INST_stat.num_levels++] = LU_SAVE_SOL_ID;
  
  for(i=0; i<3; i++) {
    vSol[i] = beta[i];
  }	
  
  INST_stat.num_levels--;
  return;
}



//----------------------------------------------------------------
void Iterate(void)
{
  int i;
  
  // Add function ID to the trace
  INST_stat.trace[INST_stat.num_levels++] = ITERATE_ID;
  
  MatMult();
  for(i=0; i<3; i++) {	
    beta[i] -= cBeta[i];
  }	
  LUSolve();
  
  INST_stat.num_levels--;
  return;
}



//----------------------------------------------------------------
void MatMult(void)
{
  int i, j;
  float temp[3];
  
  // Add function ID to the trace
  INST_stat.trace[INST_stat.num_levels++] = MAT_MULT_ID;
  
  for(i=0; i<3; i++) {
    temp[i] = beta[i];
    beta[i] = 0.0;
  }
  
  for(i=0; i<3; i++){
    for(j=0; j<3; j++) {
      beta[i] += cAlpha[i][j]*temp[j];
    }
  }
  
  INST_stat.num_levels--;
  return;
}



//---------------------------------------------------------------
void UpdateSol(void)
{
  int i;
  
  // Add function ID to the trace
  INST_stat.trace[INST_stat.num_levels++] = UPDATE_SOL_ID;
  
  for(i=0; i<3; i++) {
    vSol[i] -= beta[i];
    beta[i] = vSol[i];
  }
  
  INST_stat.num_levels--;
  return;
}



//----------------------------------------------------------------
// Final stage of the curve-fitter
//----------------------------------------------------------------
int CalLocOffset(int species, int card)
{
  
  volatile int *p_error = INST_cmd.error;
  int status = CBI_F_SUCCESS;
  float peaks[CBPM_MAX_ACQ_CONTROLLERS];  
  int new_offsets[CBPM_MAX_ACQ_CONTROLLERS];    
  
  // Add function ID to the trace
  int FID = CAL_LOC_OFFSET_ID;  
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  if (vSol[0] == 0) {
    peaks[card] = CBPM_CAL_RADIUS;
  } else {
    peaks[card] = -vSol[1]/(2*vSol[0]);
  }
  peaks[card] = clipf(peaks[card], 0., (2.*CBPM_CAL_RADIUS));
    
  // Publish fit coefficients for control side use. -------------
  INST_find_delay_out.fcoeffs[card*3    ] = vSol[0];
  INST_find_delay_out.fcoeffs[card*3 + 1] = vSol[1];
  INST_find_delay_out.fcoeffs[card*3 + 2] = vSol[2];
  
  new_offsets[card] = (int) roundf(peaks[card]) + INST_find_delay_out.offsets[card];
  new_offsets[card] = clipi(new_offsets[card], 0, (CBPM_MAX_CHAN_DELAY_COUNTS-1));  
  
  //---------------------------------------------------------------
  // Write the final corrected channel delay values to the timing
  // registers and store in the appropriate communications 
  // structure if explicitly requested.  Otherwise, just return.
  //---------------------------------------------------------------
  *p_chan_delays[card][bunch_bit_parity] 										= new_offsets[card];
  INST_timing_config.setups[tsetup].chan_delays[bunch_bit_parity][card]         = new_offsets[card];
  
  INST_find_delay_out.peaks[card] = peaks[card]; // Store the peaks the fitter returned
  
  INST_stat.num_levels--;
  return status;
}

//----------------------------------------------------------------
// Other Functions
//----------------------------------------------------------------
int CheckValidity(int species)
{
  int status = CBI_F_SUCCESS;
  int i, j;
  int card;
  float fcoeffs[3*CBPM_MAX_ACQ_CONTROLLERS];
  int ChipValues[CBPM_MAX_ACQ_CONTROLLERS];
  float maxAmp[CBPM_MAX_ACQ_CONTROLLERS] = {0.,0.,0.,0.};
  float minAmp[CBPM_MAX_ACQ_CONTROLLERS];
	
  // Add function ID to the trace
  int FID = CHECK_VALIDITY_ID;  
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  int min_start_delay = INST_find_delay_out.offsets[0];
  int max_start_delay = min_start_delay;

  for (card = 0; card<CBPM_MAX_CARDS; card++) {
    int test;
    for( test = 0; test < CBPM_NUM_TESTS; test++) {
      INST_btest_results.StatusVals[ INST_find_delay_params.hist_index][test][card] = 0;
    }
          
    ChipValues[card] = (int) roundf(INST_find_delay_out.peaks[card] + INST_find_delay_out.offsets[card]);
    
    //TODO MGS - put this test around line 571, where the value gets set
    //Check to see if new chip value is too close to edges at 0 and 1023
    PerformTest(TEST_OFFSET_TOO_CLOSE_TO_HIGH_EDGE, card, (float) INST_find_delay_out.offsets[card], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
    PerformTest(TEST_OFFSET_TOO_CLOSE_TO_LOW_EDGE,  card, (float) INST_find_delay_out.offsets[card], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);

    //Check to see if new chip fitter peak is too close to edges at 0 and 20		  
    PerformTest(TEST_FITVALUE_TOO_CLOSE_TO_HIGH_EDGE, card, INST_find_delay_out.peaks[card], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
    PerformTest(TEST_FITVALUE_TOO_CLOSE_TO_LOW_EDGE,  card, INST_find_delay_out.peaks[card], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
    
    PerformTest(TEST_BEFORE_ADJUSTTIMING_ADC_RMS_TOO_BIG, card, INST_find_delay_out.prevADCRMSs[card], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);	
    PerformTest(TEST_AFTER_ADJUSTTIMING_ADC_RMS_TOO_BIG,  card, INST_find_delay_out.postADCRMSs[card], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);	
    
    float y;
    float *data  = &INST_chan_cal_work.chan_cal_data[card].data[0];
    int LowestAmplitudeParam = 5;
    
    minAmp[card] = *(data + 0);		
    maxAmp[card] = *(data + 0);	
    //Step through all the points across the peak window
    for(i=0; i<(2*CBPM_CAL_RADIUS+1); i++) {
			
      y = *(data + i);

      if (y>maxAmp[card]) {
        maxAmp[card] = y;
      }
      if (y<minAmp[card]) {
        minAmp[card] = y;
      }				
    }

    //Check to see if minimum ever drops before a certain fraction of peak amplitude
    PerformTest2Input(TEST_OUTLIER_DETECTED, card, fabs(maxAmp[card]-minAmp[card]),fabs(maxAmp[card]), INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);

    
    int delay = INST_find_delay_out.offsets[card];
    if (delay<min_start_delay) {
      min_start_delay = delay; 
    }
    if (max_start_delay<delay) {
      max_start_delay = delay; 
    }
    
    //Check to see if 2nd order term in fit is within a reasonable range
    PerformTest2Input(TEST_FIT_CONCAVITY_TERM_TOO_HIGH,    card, -1.0*INST_find_delay_out.fcoeffs[card*3 + 0], fabs(maxAmp[card]), INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
    PerformTest2Input(TEST_FIT_CONCAVITY_TERM_TOO_LOW,     card, -1.0*INST_find_delay_out.fcoeffs[card*3 + 0], fabs(maxAmp[card]), INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);

    //Check to see if Linear term in fit is within a reasonable range			
    PerformTest2Input(TEST_FIT_LINEAR_TERM_TOO_HIGH,       card, INST_find_delay_out.fcoeffs[card*3 + 1], fabs(maxAmp[card]), INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
    PerformTest2Input(TEST_FIT_LINEAR_TERM_TOO_LOW,        card, INST_find_delay_out.fcoeffs[card*3 + 1], fabs(maxAmp[card]), INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);

    //Check to see if Constant term in fit is within a reasonable range			
    //PerformTest(TEST_FIT_CONSTANT_TERM_TOO_HIGH,     card, INST_find_delay_out.fcoeffs[card*3 + 2], maxAmp[card], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
    PerformTest2Input(TEST_FIT_CONSTANT_TERM_TOO_LOW,      card, INST_find_delay_out.fcoeffs[card*3 + 2], fabs(maxAmp[card]), INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);			
  
    //Checks to see if chip value has changed too much since last time
    PerformTest(TEST_CHIP_CHANGED_TOO_MUCH, card, (int) abs( prevChipValues[card] - ChipValues[card] - slosh ), INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
    
    //Checks to see if chip value difference from Channel 0 has changed too much since last time - probably obsolete this?
    if (card !=0 ){
      PerformTest(TEST_CHIP_TO_CHIP_VALUE_CHANGED_TOO_MUCH, card, 
      abs( (prevChipValues[0]-prevChipValues[card]) - (ChipValues[0]-ChipValues[card]) ), INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
    }
    
  }
  
  //Check to see if Diff of Crossed sum is too far from zero
  int DiffOfCrossSum = abs(maxAmp[0] + maxAmp[2] - ( maxAmp[1] + maxAmp[3] ));
  int AvgOfPeaks = abs(maxAmp[0] + maxAmp[1] + maxAmp[2] + maxAmp[3])/4;
  PerformTest2Input(TEST_DIFF_OF_CROSS_SUM_TOO_BIG, 0, DiffOfCrossSum, AvgOfPeaks, INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);	

  PerformTest(TEST_SLOSH_TOO_BIG,                      0,  (float) fabs(slosh),                                                      INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
  PerformTest(TEST_BLOCK_DELAY_TOO_CLOSE_TO_HIGH_EDGE, 0,  (float) INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
  PerformTest(TEST_BLOCK_DELAY_TOO_CLOSE_TO_LOW_EDGE,  0,  (float) INST_timing_config.setups[tsetup].block_delays[bunch_bit_parity], INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);         
  
  PerformTest(TEST_PEAK_FIT_SCAN_TOO_SLOW,  0, (float) INST_find_delay_out.numTurnsToCollectPeakData, INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);  
	
  //Check to see if all 4 cards choose a chip value somewhere near each other
  PerformTest(TEST_CHIPS_TOO_FAR_APART, 0, max_start_delay - min_start_delay, INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);
  
  PerformTest(TEST_ALU_ERROR, 0, float_err_check() == CBI_F_FAILURE, INST_find_delay_params.hist_index, INST_cmd_params.generate_delay_corrections);     
	
  INST_stat.num_levels--;
  status =  DetermineStatus();
  return status; //CBI_F_SUCCESS;
}

int Measure_ADC_RMS(float rms[CBPM_MAX_ACQ_CONTROLLERS])
{
	
  volatile int *p_error = INST_cmd.error;	
	
  int FID = MEASURE_ADC_RMS_ID;  
  INST_stat.trace[INST_stat.num_levels++] = FID;
  
  int i = 0;
  int status = CBI_F_SUCCESS;
  float min[CBPM_MAX_ACQ_CONTROLLERS] = {1.e5, 1.e5, 1.e5, 1.e5};
  float max[CBPM_MAX_ACQ_CONTROLLERS] = {0., 0., 0., 0.};
  float comparison;
  
  INST_raw_data_header.num_BP_bits_wrapped[0] = 0;
  INST_raw_data_header.num_BP_bits_wrapped[1] = 0;	
  
  CBPM_PROC_DATA   proc_data;  
  CBPM_SIGNAL_STAT signal_stat;  
  CBPM_RAW_PARAMS raw_params60Hz = {INST_code_params.execution_flags[NUM_TURNS_IN_RMS],							//Synchronize to 60 Hz
				    RAW_DATA,      // scale mode
				    1,             // was fd_trig_turns_delay, // check, was 0
				    0,             // was fd_spacex_turn,   
				    CBI_FALSE,     // use_data_enable 
				    TRIG_MASK_A};  // CBPM_TRIG_MASK_A}; was 0     
	
  memfirsts( 0 );
  
	for (i = 0; i<4; i++) {
    	rms[i] = 0.0;
	}  
    	  
  int ind = 0;
  int number_of_rms_interations = 10;
  for (ind = 0; ind<number_of_rms_interations; ind++) {
	  status = collect_raw( raw_params60Hz );					
	  
	  if (status == CBI_F_FAILURE) {
	    cbpm_set_error(FIND_DELAYS_COLLECT_RAW_RMS_ERR,p_error);
	    //INST_stat.num_levels--;
	    //return CBI_F_FAILURE;
	  }		  
	  	  
	  status = process_data(PROC_SIMPLE_AVG_MODE, raw_params60Hz, 0);
	  	  
	  if (status == CBI_F_FAILURE) {
	    cbpm_set_error(FIND_DELAYS_PROCESS_DATA_RMS_ERR,p_error);
	    //INST_stat.num_levels--;
	    //return CBI_F_FAILURE;
	  }		  
	  
	  for (i = 0; i<4; i++) {
	  	comparison = INST_signal_stat.signal_rms[i];
	  	if (comparison>max[i]) {
	  		max[i] = comparison;
	    }
	  	if (comparison<min[i]) {
	  		min[i] = comparison;
	    }
	  }
	  
      if (rms != NULL) {
  		for (i = 0; i<4; i++) {
    		rms[i] += INST_signal_stat.signal_rms[i];
        }
      }	  
	  
  }
	
  for (i = 0; i<4; i++) {
    if (rms != NULL) {
    	rms[i] /= number_of_rms_interations;
    }
  }
  
  INST_stat.num_levels--;
  return CBI_F_SUCCESS;
}

//Function to shift block_delay and card_delays to maximize distance from the edge of all values
int SloshDelays(int block_span, int* block_delay, int card_delays[CBPM_MAX_ACQ_CONTROLLERS]) {
	
	int FID = FIND_DELAYS_ID; 
  
	int i;
	int MaxCardDelay = CBPM_MAX_CHAN_DELAY_COUNTS-1;
	
	*block_delay = clipi(*block_delay, 0, block_span);
	
	int card_delay;
	int maxCard = 0;
	int minCard = MaxCardDelay;
	for( i = 0; i<CBPM_MAX_ACQ_CONTROLLERS; i++) {
		
		card_delay = clipi(card_delays[i], 0, CBPM_MAX_CHAN_DELAY_COUNTS-1);
		
		if (card_delay < minCard) {
			minCard = card_delay;
		}
		if (card_delay > maxCard) {
			maxCard = card_delay;
		}
	}
	
	if ((maxCard-minCard) > 700) {
		return CBI_F_WARNING;
	}

	int medianCard = roundf((maxCard+minCard)/2);
	int cardDistFromMiddle =  medianCard - roundf((CBPM_MAX_CHAN_DELAY_COUNTS-1)/2);
	int blockDistFromMiddle = *block_delay - block_span/2;
	int cardSlosh = roundf((cardDistFromMiddle - blockDistFromMiddle)/2);
        slosh = cardSlosh;
	
	for( i = 0; i<CBPM_MAX_ACQ_CONTROLLERS; i++) {
		card_delays[i] -= cardSlosh;
		card_delays[i] = clipi(card_delays[i], 0, CBPM_MAX_CHAN_DELAY_COUNTS-1);
	}

	*block_delay += cardSlosh;
	*block_delay = clipi(*block_delay, 0, block_span);
  
	return CBI_F_SUCCESS;
}
