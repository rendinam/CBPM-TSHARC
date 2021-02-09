///*!-----------------------------------------------------------------------+
//* File         :  cbpm_prep_instrument_header.c        
//*/

#include "cbpm_includes.h"

int cbpm_prep_instrument_header(RDV3_INST_DATA *hdata, int iidx) {

  CBPM_DATA  *dp;
  dp = CTL_System.p_Module[iidx]->dsp_data;

  CBPM_CONTROL *cp;
  cp = CTL_System.p_Module[iidx]->control;

  int species = dp->cbpm_cmd_params.species;
  int tsetup = dp->cbpm_op_timing.active_timing_setup;
  strcpy((*hdata).timing_setup, tsetup_names[tsetup]);

  int card, tblock, ped_table_set;
  int ch_val, ch_stat;
  int trigger_setup;
  if (dp->cbpm_cmd_params.trig_mask == TRIG_MASK_A) {
    if (dp->cbpm_cmd_params.use_data_enable) {
      trigger_setup = 2;
    } else {
      trigger_setup = 1;
    }
  } else {
    if (dp->cbpm_cmd_params.use_data_enable) {
      trigger_setup = 4;
    } else {
      trigger_setup = 3;
    }
  }
  // Fill staging arrays for ADC threshold flags.
  // These values are encoded here.
  // Compose a single integer for each bank of channels
  // on a timing block (4 at a time).  
  // Each 10's place in the 4-digit number is a 1 or 0 to 
  // signify the flag for that channel.  The pow() function
  // spaces those digits appropriately so they can be stored
  // in the single word.
  //  A similar decoding procedure is necessary on the
  //  reading side of things.  
  // TODO: Change to space-separated fields to allow easier
  //       reading in by scanf functions in next file version.
  for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
    (*hdata).adc_saturation[tblock][0] = 0;
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
      ch_val  = (1 << (tblock*CBPM_MAX_CARD_ADCS_PER_BLOCK)) & dp->cbpm_raw_data_header.ADC_saturation_bitfields[card];
      if (ch_val > 0) {
	(*hdata).adc_saturation[tblock][0] = pow(10, card);
      }
    }
  }
  for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
    (*hdata).adc_high[tblock][0] = 0;
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
      ch_val  = (1 << (tblock*CBPM_MAX_CARD_ADCS_PER_BLOCK)) & dp->cbpm_raw_data_header.ADC_high_thresh_bitfields[card];
      if (ch_val > 0) {
	(*hdata).adc_high[tblock][0] = pow(10, card);
      }
    }
  }
  for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
    (*hdata).adc_low[tblock][0] = 0;
    for (card = 0; card < CBPM_MAX_CARDS; card++) {
      ch_val  = (1 << (tblock*CBPM_MAX_CARD_ADCS_PER_BLOCK)) & dp->cbpm_raw_data_header.ADC_low_thresh_bitfields[card];
      if (ch_val > 0) {
	(*hdata).adc_low[tblock][0] = pow(10, card);
      }
    }
  }
  if (tsetup >= CBPM_MAX_MULTIGAIN_SETUPS) {
    ped_table_set = tsetup - CBPM_MAX_MULTIGAIN_SETUPS;
  } else {
    ped_table_set = tsetup;
  }


  /*   // Compose field mapping */
  fio_addrs addrs;
  addrs[0] = (long)&(CTL_System.p_Module[iidx]->det.location);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 0, addrs);
  
  addrs[0] = (long)&(CTL_System.p_Module[iidx]->comm.ethernet.hostname);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 1, addrs);

  addrs[0] = (long)&(CTL_System.p_Module[iidx]->comm.ethernet.IP_address);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 2, addrs);

  addrs[0] = (long)&(cp->detector.type);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 3, addrs);

  addrs[0] = (long)&(cp->detector.horz_coeff);
  addrs[1] = (long)&(cp->detector.vert_coeff);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 4, addrs);

  addrs[0] = (long)&(CTL_System.p_Module[iidx]->core->cbi_module_config.ldr_name);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 5, addrs);

  addrs[0] = (long)&(CTL_System.p_Module[iidx]->core->cbi_module_config.build_timestamp);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 6, addrs);

  addrs[0] = (long)&(CTL_System.p_Module[iidx]->core->cbi_ident.fpga_maj);
  addrs[1] = (long)&(CTL_System.p_Module[iidx]->core->cbi_ident.fpga_min);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 7, addrs);

  addrs[0] = (long)&(CTL_System.p_Module[iidx]->core->cbi_ident.fe_fpga_id[0]);
  addrs[1] = (long)&(CTL_System.p_Module[iidx]->core->cbi_ident.fe_fpga_id[1]);
  addrs[2] = (long)&(CTL_System.p_Module[iidx]->core->cbi_ident.fe_fpga_id[2]);
  addrs[3] = (long)&(CTL_System.p_Module[iidx]->core->cbi_ident.fe_fpga_id[3]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 8, addrs);

  addrs[0] = (long)&((*hdata).timing_setup);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 9, addrs);

  addrs[0] = (long)&(dp->cbpm_raw_data_header.num_turns);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 10, addrs);

  addrs[0] = (long)&(dp->cbpm_raw_data_header.turn_counter);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 11, addrs);

  addrs[0] = (long)&(dp->cbpm_raw_data_header.spacex_turn);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 12, addrs);

  addrs[0] = (long)&(cbpm_trigger_setup_names[trigger_setup]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 13, addrs);

  addrs[0] = (long)&(dp->cbpm_timing_config.setups[tsetup].BP_offsets[0]);
  addrs[1] = (long)&(dp->cbpm_timing_config.setups[tsetup].BP_offsets[1]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 14, addrs);

  addrs[0] = (long)&(dp->cbpm_timing_config.setups[tsetup].cTMD);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 15, addrs);

  addrs[0] = (long)&(dp->cbpm_timing_config.setups[tsetup].bTMDs[0]);
  addrs[1] = (long)&(dp->cbpm_timing_config.setups[tsetup].bTMDs[1]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 16, addrs);

  addrs[0] = (long)&(dp->cbpm_timing_config.setups[tsetup].block_delays[0]);
  addrs[1] = (long)&(dp->cbpm_timing_config.setups[tsetup].block_delays[1]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 17, addrs);

  addrs[0] = (long)&(dp->cbpm_timing_config.setups[tsetup].chan_delays[0][0]);
  addrs[1] = (long)&(dp->cbpm_timing_config.setups[tsetup].chan_delays[0][1]);
  addrs[2] = (long)&(dp->cbpm_timing_config.setups[tsetup].chan_delays[0][2]);
  addrs[3] = (long)&(dp->cbpm_timing_config.setups[tsetup].chan_delays[0][3]);
  addrs[4] = (long)&(dp->cbpm_timing_config.setups[tsetup].chan_delays[1][0]);
  addrs[5] = (long)&(dp->cbpm_timing_config.setups[tsetup].chan_delays[1][1]);
  addrs[6] = (long)&(dp->cbpm_timing_config.setups[tsetup].chan_delays[1][2]);
  addrs[7] = (long)&(dp->cbpm_timing_config.setups[tsetup].chan_delays[1][3]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 18, addrs);

  addrs[0] = (long)&(dp->cbpm_op_gain.active_gain_settings[0][0]);
  addrs[1] = (long)&(dp->cbpm_op_gain.active_gain_settings[0][1]);
  addrs[2] = (long)&(dp->cbpm_op_gain.active_gain_settings[0][2]);
  addrs[3] = (long)&(dp->cbpm_op_gain.active_gain_settings[0][3]);
  addrs[4] = (long)&(dp->cbpm_op_gain.active_gain_settings[1][0]);
  addrs[5] = (long)&(dp->cbpm_op_gain.active_gain_settings[1][1]);
  addrs[6] = (long)&(dp->cbpm_op_gain.active_gain_settings[1][2]);
  addrs[7] = (long)&(dp->cbpm_op_gain.active_gain_settings[1][3]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 19, addrs);

  addrs[0] = (long)&(dp->cbpm_gain_config.gain_codes[ dp->cbpm_op_gain.active_gain_settings[0][0] ]);
  addrs[1] = (long)&(dp->cbpm_gain_config.gain_codes[ dp->cbpm_op_gain.active_gain_settings[0][1] ]);
  addrs[2] = (long)&(dp->cbpm_gain_config.gain_codes[ dp->cbpm_op_gain.active_gain_settings[0][2] ]);
  addrs[3] = (long)&(dp->cbpm_gain_config.gain_codes[ dp->cbpm_op_gain.active_gain_settings[0][3] ]);
  addrs[4] = (long)&(dp->cbpm_gain_config.gain_codes[ dp->cbpm_op_gain.active_gain_settings[1][0] ]);
  addrs[5] = (long)&(dp->cbpm_gain_config.gain_codes[ dp->cbpm_op_gain.active_gain_settings[1][1] ]);
  addrs[6] = (long)&(dp->cbpm_gain_config.gain_codes[ dp->cbpm_op_gain.active_gain_settings[1][2] ]);
  addrs[7] = (long)&(dp->cbpm_gain_config.gain_codes[ dp->cbpm_op_gain.active_gain_settings[1][3] ]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 20, addrs);

  addrs[0] = (long)&(dp->cbpm_gain_config.gain_table[0][0][ dp->cbpm_op_gain.active_gain_settings[0][0] ]);
  addrs[1] = (long)&(dp->cbpm_gain_config.gain_table[0][1][ dp->cbpm_op_gain.active_gain_settings[0][1] ]);
  addrs[2] = (long)&(dp->cbpm_gain_config.gain_table[0][2][ dp->cbpm_op_gain.active_gain_settings[0][2] ]);
  addrs[3] = (long)&(dp->cbpm_gain_config.gain_table[0][3][ dp->cbpm_op_gain.active_gain_settings[0][3] ]);
  addrs[4] = (long)&(dp->cbpm_gain_config.gain_table[1][0][ dp->cbpm_op_gain.active_gain_settings[1][0] ]);
  addrs[5] = (long)&(dp->cbpm_gain_config.gain_table[1][1][ dp->cbpm_op_gain.active_gain_settings[1][1] ]);
  addrs[6] = (long)&(dp->cbpm_gain_config.gain_table[1][2][ dp->cbpm_op_gain.active_gain_settings[1][2] ]);
  addrs[7] = (long)&(dp->cbpm_gain_config.gain_table[1][3][ dp->cbpm_op_gain.active_gain_settings[1][3] ]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 21, addrs);

  // FIXME: Verify that these values are stored and written correctly.
  addrs[0] = (long)&(dp->cbpm_pedestal_config.tables[ped_table_set].ped_table[0][0][ dp->cbpm_op_gain.active_gain_settings[0][0] ]);
  addrs[1] = (long)&(dp->cbpm_pedestal_config.tables[ped_table_set].ped_table[0][1][ dp->cbpm_op_gain.active_gain_settings[0][1] ]);
  addrs[2] = (long)&(dp->cbpm_pedestal_config.tables[ped_table_set].ped_table[0][2][ dp->cbpm_op_gain.active_gain_settings[0][2] ]);
  addrs[3] = (long)&(dp->cbpm_pedestal_config.tables[ped_table_set].ped_table[0][3][ dp->cbpm_op_gain.active_gain_settings[0][3] ]);
  addrs[4] = (long)&(dp->cbpm_pedestal_config.tables[ped_table_set].ped_table[1][0][ dp->cbpm_op_gain.active_gain_settings[1][0] ]);
  addrs[5] = (long)&(dp->cbpm_pedestal_config.tables[ped_table_set].ped_table[1][1][ dp->cbpm_op_gain.active_gain_settings[1][1] ]);
  addrs[6] = (long)&(dp->cbpm_pedestal_config.tables[ped_table_set].ped_table[1][2][ dp->cbpm_op_gain.active_gain_settings[1][2] ]);
  addrs[7] = (long)&(dp->cbpm_pedestal_config.tables[ped_table_set].ped_table[1][3][ dp->cbpm_op_gain.active_gain_settings[1][3] ]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 22, addrs);

  addrs[0] = (long)&(dp->cbpm_temperatures.dig_temp);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 23, addrs);

  addrs[0] = (long)&(dp->cbpm_temperatures.fe_temps[0]);
  addrs[1] = (long)&(dp->cbpm_temperatures.fe_temps[1]);
  addrs[2] = (long)&(dp->cbpm_temperatures.fe_temps[2]);
  addrs[3] = (long)&(dp->cbpm_temperatures.fe_temps[3]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 24, addrs);


  addrs[0] = (long)&((*hdata).adc_saturation[0][0]);
  addrs[1] = (long)&((*hdata).adc_saturation[1][0]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 25, addrs);

  addrs[0] = (long)&((*hdata).adc_high[0][0]);
  addrs[1] = (long)&((*hdata).adc_high[1][0]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 26, addrs);

  addrs[0] = (long)&((*hdata).adc_low[0][0]);
  addrs[1] = (long)&((*hdata).adc_low[1][0]);
  cbpmfio_map_addresses_to_field(&RDV3_inst_header_specs, 27, addrs);
  // End field mapping

  return CBI_F_SUCCESS;

}

