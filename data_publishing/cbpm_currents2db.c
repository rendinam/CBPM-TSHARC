//-----------------------------------------------------------------------+
// File         :  cbpm_currents2db.c                                    |
//                                                                       |
// Description  :  Stores averaged current measurement from a single BPM | 
//                 after digitizing every available bunch in the CESR    |
//                 control system database.                              |
//                                                                       |
// Arguments    :  Index of instrument in management array (int)         |
//                                                                       |
// Return Value :  Status - F_SUCCESS/F_FAILURE                          |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


#define CURRENT_SUM_REPORT_THRESHOLD   6              // in uA, to accomodate offsets, etc.


int cbpm_currents2db( int iidx ) {

  char *func_name = (char *)__FUNCTION__;

  int debug_level = CTL_System.debug_verbosity;

  static int prior_species_state = CBI_UNINITIALIZED_PARAM;

  int card, tblock, bunch, species, sum;

  // For MPM addressing
  char node[CBI_MAX_NODE_CHARS];
  int ele[2];
  int write_counter = 0;


  CBPM_DATA   *dp;
  CBPM_CONTROL  *cp;

  dp = CTL_System.p_Module[iidx]->dsp_data;
  cp = CTL_System.p_Module[iidx]->control;

  //  species = dp->cbpm_cmd_params.species;
  // Get species flag from the injection control/handshake node
  ele[0] = ele[1] = 4;
  vmgetn_c( CBI_INJ_CONTROL_NODE, ele[0], ele[1], &species );
  ////printf("SPECIES from 'CSR 2NS CCON': %s     ", cbi_species_names[species] );


  float current_array[CBPM_4NS_MAX_BUNCHES*2];
  int   current_array_scaled[CBPM_4NS_MAX_BUNCHES*2];

  // Initialize the current measurement storage and conversion arrays
  for (bunch = 0; bunch < CBPM_4NS_MAX_BUNCHES*2; bunch++) {
    current_array[bunch]        = 0.0;
    current_array_scaled[bunch] = 0;
  }

  // Database addressing for publishing the total current for each species
  const int  sum_elements[CBI_NUM_SPECIES] = {5, 4};
  

  // Database addressing for publishing the processed (avaraged) button data
  // for this device/detector to its assigned MPM locations.
  strcpy(node, CBI_SIGNED_CURR_NODE);

  ele[0] = 1;
  ele[1] = CBPM_4NS_MAX_BUNCHES * 2;  // 1280 slots available in MPM array
                                        //  FIXME: Make separate named constant.

  int gain_setting;
  //---------------------------------------------------
  // Transfer the button sum data values into a local,
  // rarefied array (necessary for writing into the
  // database which has slots allocated for every
  // RF bucket) and apply scale factor to each.
  //---------------------------------------------------
  sum = 0;
  for (bunch = 0; bunch < CBPM_4NS_MAX_BUNCHES; bunch++) {
    current_array[bunch*2] = dp->cbpm_proc_sum_dat_buf.signal_sum[bunch] * cp->detector.current_coeff;
    current_array_scaled[bunch*2] = (int)current_array[bunch*2];

    if (debug_level > CBI_DEBUG_LEVEL_2) {
      printf(" [%3d]% 3d ", bunch+1, current_array_scaled[bunch*2] );
    }
    // Also sum everything to cross-check total current with the CERN current monitor.
    // If the scaled values (in uA) end up below the threshold, do not add them to
    // the running total.
    if ( abs(current_array[bunch*2]) > CURRENT_SUM_REPORT_THRESHOLD) {
      sum += current_array[bunch*2];
    }
  }


  if (debug_level > CBI_DEBUG_LEVEL_2) {
    printf("\n\n");
    for (bunch = 0; bunch < CBPM_4NS_MAX_BUNCHES; bunch++) {
      printf("%  f ", dp->cbpm_proc_sum_dat_buf.signal_sum[bunch] );
    }
 
    printf("\nfill_count = %d\n", dp->cbpm_proc_sum_dat_buf.fill_count );

    for (card = 0; card < CBPM_MAX_CARDS; card++) {    
      for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
	printf("%d ", dp->cbpm_proc_sum_dat_buf.gain_values[card][tblock]);
      }
      printf("\n");
    }

  }

  //----------------------------------------------
  // Perform the write to the database
  // Publish the entire current measurement array
  //----------------------------------------------
  int retval, i, num_xfers, leftover, first, next;
  int loc_pkt_size = 500;

  if (cbpm_currmon_server) {

    if (debug_level > CBI_DEBUG_LEVEL_2) {
      printf("preparing to vmputn to %s %d %d \n", CBI_CURRENT_SUM_NODE, sum_elements[species], sum_elements[species] );
      printf("sum_elements index = %d\n", species );
    }
    
    // Publish the sum of all bunch currents over threshold
    retval = vmputn_c(CBI_CURRENT_SUM_NODE, 
		      sum_elements[species], 
		      sum_elements[species], 
		      &sum );
    
    if (debug_level > CBI_DEBUG_LEVEL_2) {
      printf("vmputn to %s %d %d returned  %d\n", CBI_CURRENT_SUM_NODE, sum_elements[species], sum_elements[species],  retval);
    }

    if (retval != MPM_SUCCESS) {
      printf("vmputn to %s %d %d returned  %d\n", CBI_CURRENT_SUM_NODE, sum_elements[species], sum_elements[species],  retval);
      return F_FAILURE;
    }
    

    
    first = 1;
    next  = loc_pkt_size;
    num_xfers = ele[1] / loc_pkt_size;
    leftover  = ele[1] - (num_xfers * loc_pkt_size);
    
    // Transfer all full packets of values.
    for (i = 0; i < num_xfers; i++) {
       if (debug_level > CBI_DEBUG_LEVEL_2) {
          printf("C-array (%d-%d)  being sent to elements (%d-%d)...\n", first-1, next-1, first, next);
       }
       if (species == POSITRONS) {
	 retval = vmputn_c(CBI_POS_CURRMON_NODE, first, next, &current_array_scaled[first-1] );
       } else {
	 // Take abs of electron currents to conform to WINJ specification to only publish
	 // the sum of positive current values as the blue number in current display.
	 for (bunch = 0; bunch < CBPM_4NS_MAX_BUNCHES*2; bunch++) {
	   current_array_scaled[bunch] = abs(current_array_scaled[bunch]);
	 }
	 retval = vmputn_c(CBI_ELE_CURRMON_NODE, first, next, &current_array_scaled[first-1] );
       }
       if (debug_level > CBI_DEBUG_LEVEL_2) {
          printf("vmputn to %s %d %d returned  %d\n", CBI_SIGNED_CURR_NODE, first, next,  retval);
       }
       if (retval != MPM_SUCCESS) {
	 printf("vmputn to %s %d %d returned  %d\n", CBI_SIGNED_CURR_NODE, first, next,  retval);
	 return F_FAILURE;
       }
       first += loc_pkt_size;
       next  += loc_pkt_size;
    }

    if (leftover > 0) {
       // Transfer remainder that is less than one local packet's worth.
       if (debug_level > CBI_DEBUG_LEVEL_2) {
          printf("Leftovers: C-array (%d-%d)  being sent to elements (%d-%d).\n",
                 num_xfers*loc_pkt_size,
                 CBPM_4NS_MAX_BUNCHES*2,
                 num_xfers*loc_pkt_size+1,
                 ele[1]);
       }
       
       if (species == POSITRONS) {
	 retval = vmputn_c(CBI_POS_CURRMON_NODE, num_xfers*loc_pkt_size+1, ele[1], &current_array_scaled[num_xfers*loc_pkt_size] );
       } else {
	 retval = vmputn_c(CBI_ELE_CURRMON_NODE, num_xfers*loc_pkt_size+1, ele[1], &current_array_scaled[num_xfers*loc_pkt_size] );
       }
       if (debug_level > CBI_DEBUG_LEVEL_2) {
	 printf("vmputn to %s %d %d returned  %d\n", CBI_SIGNED_CURR_NODE, first, next,  retval);
       }
       if (retval != MPM_SUCCESS) {
	 printf("vmputn to %s %d %d returned  %d\n", CBI_SIGNED_CURR_NODE, first, next,  retval);
	 return F_FAILURE;
       }
    }
    
    // Increment current measurement write counter
    // FIXME: Grab counter from species-specific counter element.
    ele[0] = ele[1] = 1282;
    if (species == POSITRONS) {
      retval = vmgetn_c(CBI_POS_CURRMON_NODE, ele[0], ele[1], &write_counter);
    }
    if (species == ELECTRONS) {
      retval = vmgetn_c(CBI_ELE_CURRMON_NODE, ele[0], ele[1], &write_counter);
    }
    write_counter++;

    // Hide cursor
    printf("\033[?25l");
    printf("\b\b\b\b\b\b\b\b");
    fflush(stdout);
    printf("%8d", write_counter);
    if (write_counter == 0xFFFFFFFF) {
       write_counter = 0;
    }

    if (species == POSITRONS) {
       retval = vmputn_c(CBI_POS_CURRMON_NODE, ele[0], ele[1], &write_counter);
    } else {
       retval = vmputn_c(CBI_ELE_CURRMON_NODE, ele[0], ele[1], &write_counter);
    }
    if (retval != MPM_SUCCESS) {
      printf("vmputn to unsigned current node %d %d returned  %d\n", ele[0], ele[1],  retval);
      return F_FAILURE;
    }
    
  } else { // (currmon_server)
     
     printf(" TESTING -- NOT PUBLISHING RESULTS TO DATABASE  -- TESTING\n");
     
  } // endIF currmon_server
  
  
  if (debug_level > CBI_DEBUG_LEVEL_2) {

     printf("\n\n");
  }
  
  return F_SUCCESS;

}


