#ifndef CBPM_PROTOS_H
#define CBPM_PROTOS_H
//-----------------------------------------------------------------------+
// File         :  cbpm_protos.h                                         |
//                                                                       |
// Description  :  Function prototypes for the bulk of the routines      |
//                 provided in CBPM-TSHARC for instrument-type-specific  |
//                 functionality needed on the control system.           |
//-----------------------------------------------------------------------+

//-----------------------------------------------------------------------+
// The following provides for C++ compatibility (ie, C++ routines can    |
// explicitly use this include file).                                    |
//-----------------------------------------------------------------------+
#if defined (__cplusplus)
extern "C" {
#endif

  extern int   cbpm_triplet_time_shift(void);

  extern int   cbpm_mem_setup( void );
  extern int   cbpm_query_bunch(int tsetup, int species, int pattern_array[] );
  extern int   cbpm_query_bunch_range(int pattern_array[] );
  extern int   cbpm_query_bunches(int pattern_array[] );
  extern int   cbpm_query_pattern_type( int pattern_array[] );
  extern int   cbpm_query_species( int timing_setup );
  extern int   cbpm_query_trigger( CBPM_DATA *dat_ptr );

  extern int   cbpm_buttons2db( int iidx );
  extern int   cbpm_currents2db( int BPM_IDX );
  extern int   cbpm_phase2db( FILE *fp, int iidx );
  extern int   cbpm_phase_turn_offset( int iidx, int species, int bunch );
  extern int   cbpm_phase_wait_value( int iidx, int species, int bunch );

  extern int   cbpm_print_cmd_params_struct(int BPM_IDX, FILE *fp);
  extern int   cbpm_print_proc_buf_entries(int mode, int BPM_IDX, int first, int last, FILE *fp);
  extern int   cbpm_write_find_bunch_dat( int BPM_IDX, FILE *fp, char *filename );
  extern int   cbpm_measurement_species(void);
  extern int   cbpm_measurement_tsetup(void);
  extern int   cbpm_measurement_timing_mode(void);
  extern int   cbpm_set_timing_setup_prep( void );
  extern int   cbpm_set_timing_setup_post(int iidx);
  extern int   cbpm_set_gains_prep(void);
  extern int   cbpm_set_gains_post( int iidx );

  extern int   cbpm_prep_instrument_header(RDV3_INST_DATA *hdata, int iidx);
  
  extern int   cbpm_write_tscan_data(int BPM_IDX, int init_gd_count, FILE *fp, char *filename);
  extern int   cbpm_write_chan_cal(int iidx, FILE *fp, char *filename, int *start_delays, int dat_err);
  extern int   cbpm_write_chan_cal_parallelpost(int iidx, FILE *fp, char *filename, int *start_delays, int dat_err, float peaks[4]);

  extern int   cbpm_TB_to_BPslot( int train, int bunch );

  extern int   cbpm_acquire_pedestals_prep(void);
  extern int   cbpm_acquire_pedestals_post(int iidx);
  extern int   cbpm_beta_phase_prep(void);
  extern int   cbpm_beta_phase_prep_from_iface(void);
  extern int   cbpm_beta_phase_post(int iidx);
  extern int   cbpm_beta_phase_final(void);
  extern int   cbpm_calibrate_gains(void);
  extern int   cbpm_check_pulse_post( int iidx );
  extern int   cbpm_cont_proc_data_post( int iidx );
  extern int   cbpm_cont_proc_data_prep(void);
  extern int   cbpm_cont_proc_data_read( void );

  extern int   cbpm_copy_timing_params_prep(void);
  extern int   cbpm_set_zero_crossing_prep(void);

  extern int   cbpm_diagnostic_dump_prep(void);
  extern int   cbpm_diagnostic_dump_post( int iidx );

  extern int   cbpm_proc_data_final( void );

  extern int   cbpm_proc_data_currmon_prep(void);
  extern int   cbpm_proc_data_currmon_post( int iidx );
  extern int   cbpm_proc_data_cal_currmon_prep(void);
  extern int   cbpm_proc_data_cal_currmon_post( int iidx );  

  extern int   enter_server_mode(void);

  extern int   cbpm_find_bTMD_prep(void);
  extern int   cbpm_find_bTMD_post(int iidx);
  extern int   cbpm_find_bunch_prep(void);
  extern int   cbpm_find_bunch_post(int iidx);
  extern int   cbpm_find_cTMD_post(int iidx);
  extern int   cbpm_gain_scale_type_prep(void);
  extern int   cbpm_get_temps_post(int iidx);
  extern int   cbpm_install_gain_tables(void);
  extern int   cbpm_apply_gain_mappings(void);
  extern int   cbpm_install_params(void);
  extern int   cbpm_power_cycle_init(void);
  extern int   cbpm_proc_data_prep(void);
  extern int   cbpm_proc_data_prep_from_iface(void);
  extern int   cbpm_proc_data_post(int iidx);
  extern int   cbpm_proc_data_post_publish(int iidx);
  extern int   cbpm_orbit_data_block_publish(void);
  extern int   cbpm_push_full_config_post( int iidx );
  extern int   cbpm_raw_data_prep(void);
  extern int   cbpm_raw_data_prep_from_iface(void);

  extern int   cbpm_raw_data_post(int iidx);
  extern int   cbpm_raw_data_final(void);

  extern int   cbpm_scrub_timing_params_prep(void);
  extern int   cbpm_scrub_timing_params_post( int iidx );

  extern int   cbpm_install_TBT_pattern(void);

  extern int   cbpm_test_life_prep(void);
  extern int   cbpm_test_life_post( int iidx );
  extern int   cbpm_test_life_final(void);

  extern int   reset_coldfire( int iidx );
  extern int   reset_coldfire_final( void );
  
  extern int   reset_instrument( int iidx );

  extern int   cbpm_reset_and_init(void);
  extern int   cbpm_save_pedestals_prep(void);
  extern int   cbpm_save_pedestals(void);

  extern int   cbpm_save_timings_prep(void);
  extern int   cbpm_save_timings(int iidx);
  extern int   cbpm_save_timings_post(void);

  extern int   cbpm_set_gain_prep(void);
  extern int   cbpm_set_gain_post( int iidx );
  extern int   cbpm_test_data_prep(void);
  extern int   cbpm_test_data_post( int iidx );
  extern int   cbpm_time_in(void);

  extern int   parity( int number );

  extern int   cbpm_time_scan_prep(void);
  extern int   cbpm_time_scan_prompt_Time_Scan_Protection(void);
  extern int   cbpm_time_scan_prep_from_iface(void);
  extern int   cbpm_time_scan_post(int iidx);
  extern int   cbpm_time_scan_summary(int printf_level);
  extern int   cbpm_time_scan_final(void);
  extern int   enable_time_scan_tests(void);
  extern int   cbpm_set_timing_setup_prep(void);
  extern int   cbpm_set_timing_setup_post(int iidx);

  extern int    cbpm_exit_currmon_mode( void );


  // file_io
  extern int   cbpm_read_detector_params( char *filename );
  extern int   cbpm_read_instrument_params( char *filename );
  extern int   cbpm_reread_params(void);
  extern int   cbpm_write_gain_sections( int iidx );
  extern int   cbpm_rw_raw_data(FILE *fp, int mode, int iidx);
  extern int   cbpm_write_all_raw_data(void);

  extern int   cbpm_load_instr_code_params(void);

  extern int   cbpm_print_inst_info(int BPM_IDX, FILE *fp, int mode);
  extern int   cbpm_print_turn_count_summary(void);

  extern float cbpm_compute_pos( int iidx, int plane, float TI, float BI, float BO, float TO );


  // Fortran file I/O support
  extern int    cbpm_deref_( int *address, int *array, int *size );


  // Signal handlers
  extern int    cleanup_sockets(void);
  extern void   std_sigint_handler( int signum );
  extern void   server_mode_sigint_handler( int signum );
  extern void   continuous_mode_sigint_handler( int signum );
  extern void   std_sigpipe_handler( int signum );
  extern void   continuous_mode_sigpipe_handler( int signum );
  extern void   segfault_handler( int signum );


  // Utils
  extern float cbpm_correct_value( CBPM_DATA *dp, float value, int mode, int gain_setting, int tblock, int card, int chan );
  extern int   cbpm_currmon_modespec_to_tsetup( int timing_mode, int species );
  extern int   modegain_to_tsetup( int timing_mode, int gain_setting, int species );
  extern int   scrub_timing_params_prep(void);
  extern int   scrub_timing_params_post( int iidx );
  extern int   cbpm_print_patterns( int BP[], int RBP[], FILE *fp );
  extern int   cbpm_rf_bucket( int tsetup, int bunch );
  extern int   cbpm_find_delay_plot_peak( char *full_fname, int col_start, int col_end, PLOT_PARAMS pparams, char *plot_title, char titles[][CBI_MAX_STRING_LENGTH], int plot_fit, float fcoeffs[CBPM_MAX_FIT_COEFFS], int offsets[CBPM_MAX_ACQ_CONTROLLERS]);


//-----------------------------------------------------------------------+
// The following provides for C++ compatibility (ie, C++ routines can    |
// explicitly use this include file).                                    |
//-----------------------------------------------------------------------+
#if defined (__cplusplus)
}
#endif

#endif // CBPM_PROTOS_H
