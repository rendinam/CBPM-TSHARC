#ifndef CBPM_CTL_TYPES_H
#define CBPM_CTL_TYPES_H
//-----------------------------------------------------------------------+
// File         :  cbpm_ctl_types.h                                      |
//                                                                       |
// Description  :  This header file defines the CBPM-TSHARC platform     |
//                 data structures used exclusively in control system    |
//                 code.                                                 |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+
#include "cbpm_constants.h"
//#include "cbpm_ctl_types.h"


//-----------------------------------------------------------------------+
//  CBPM_DETECTOR:  Contains all data relevant to a given installation   |
//                  of physical BPM detector/transducer hardware.        |
//                                                                       |
//                                                                       |
// name          - Descriptive detector name                             |
// name_survey   - Detector name as specified in survey data             |
// name_cesrv    - Detector name as specified by CESRV                   |
// type          - Detector type - BPM geometry types to be mapped onto  |
//                 identifiers.  Identifier goes here.                   |
// det_id_node   - ??
// det_id_ele    - ??
// f_index       - floating point detector index  (Isn't this redundant?)|
// ring_position - (x,y,s) coordinates of detector center (units?)       |
// ring_rotation - Rotation of buttons around beam axis                  |
// local_axes    - (x,y) horizontal distance from center to being        |
//                 between top/bottom button pair and vertical distance  |
//                 from center to being between in/out button pair       |
// curr_coeff    - Coefficients for current calc (?)                     |
// horz_coeff    - Coefficient(s) for horizontal pos.                    |
// vert_coeff    - Coefficient(s) for vertical pos.                      |
//                                                                       |
//-----------------------------------------------------------------------+
typedef struct {
  char  name[CBI_MAX_NAME_CHARS];
  char  location[CBI_MAX_NAME_CHARS];
  char  name_survey[CBI_MAX_NAME_CHARS];
  char  name_cesrv[CBI_MAX_NAME_CHARS];
  char  type[CBI_MAX_NAME_CHARS];
  char  detec_id_node[CBI_MAX_NODE_CHARS];
  int   detec_id_ele;
  char  button_flag_node[CBI_MAX_NODE_CHARS];
  int   button_flag_ele;
  char  button_data_node[CBI_MAX_NODE_CHARS];
  int   button_data_idx;
  int   button_data_offset[CBI_NUM_SPECIES];
  char  orbit_stat_node[CBI_MAX_NODE_CHARS];
  int   orbit_stat_ele;
  char  position_node[CBI_MAX_NODE_CHARS];
  int   position_idx;
  int   position_offset[CBI_NUM_SPECIES];
  char  phase_node[CBI_MAX_NODE_CHARS];
  int   phase_idx;
  char  phase_stat_node[CBI_MAX_NODE_CHARS];
  int   phase_stat_ele;
  float ring_position[3];
  float ring_rotation;
  float local_axes[2];        
  float current_coeff;
  float horz_coeff;
  float vert_coeff;
} CBPM_DETECTOR;



//-----------------------------------------------------------------------+
//  CBIC_PHASE_CALCS  - Holds output of phase angle/amplitude calcs.     |
//-----------------------------------------------------------------------+
typedef struct {
  int   bunch_i;
  int   tot_turns;
  float horz_amp[CBPM_MAX_CARDS];
  float horz_phase[CBPM_MAX_CARDS];
  float vert_amp[CBPM_MAX_CARDS];
  float vert_phase[CBPM_MAX_CARDS];
} CBPM_PHASE_CALCS;


//-----------------------------------------------------------------------+
// CBPM_TBT_DATA - For holding corrected turn-by-turn values that are
//     the result of performing pedestal subtraction and gain scaling on
//     raw ADC data values.  This also
//-----------------------------------------------------------------------+
typedef struct {
  char   location[10];
  float  corrected_0[CBPM_ADC_BUF_USE];
  float  corrected_1[CBPM_ADC_BUF_USE];
  float  corrected_2[CBPM_ADC_BUF_USE];
  float  corrected_3[CBPM_ADC_BUF_USE];
  float  X_positions[CBPM_ADC_BUF_USE];
  float  Y_positions[CBPM_ADC_BUF_USE];
  int    horz_phase_words[CBPM_ADC_BUF_USE];
  int    vert_phase_words[CBPM_ADC_BUF_USE];
  int    timing_blocks[CBI_MAX_BUNCHES];
} CBPM_TBT_DATA;


//-----------------------------------------------------------------------+
// CBPM_POS_DATA - For holding calculated horizontal and vertical 
//     position values on a turn-by-turn basis.
//-----------------------------------------------------------------------+
typedef struct {
   float X_positions[CBPM_ADC_BUF_USE];
   float Y_positions[CBPM_ADC_BUF_USE];
} CBPM_POS_DATA;


//-----------------------------------------------------------------------+
//  CBPM_TBT_ANALYSIS - Aggregation structure holding pointers to
//     substructures that in turn hold turn-by-turn (TBT) data buffers
//     useful for analysis work.
//-----------------------------------------------------------------------+
typedef struct {
  CBPM_TBT_DATA   *tbt_data;
  CBPM_POS_DATA   *pos_data;
} CBPM_TBT_ANALYSIS;


//----------------------------------------------------
// Structure to hold collection of data
// specification parameters. 
// These are used to properly format and
// organize data as it is being written to disk.
//----------------------------------------------------
struct data_spec {
  int species;
  int bunch;
   // generalize name and/or home structure
  // For enhanced plotting features: CC data and so forth
  int  cstart[4];
};


//-----------------------------------------------------------------------+
// CONFIGURATION_PARAMETERS:
//
// Cache of various parameters that can change depending on the timing 
// setup (14ns bunch spacing or 4ns bunch spacing).  The parameters can 
// then be copied over to the instrument's communication structures for 
// use.
//
// timing_setup : A software-level concept - Corresponds to a particular
//                combination of timing MODE (4ns or 14ns hardware-level
//                concept) and a specific set of operational timing 
//                parameters for use within that mode.
//                The configuration regimes are:
//                   4ns-spaced e+ bunches
//                   4ns-spaced e- bunches
//                   14ns-spaced single or dual species
//
// BP_offsets   : The bunch pattern offsets used when rotating an
//                on-board bunch pattern prior to an acquisition.
//
// cTMD         : Common turns-marker delay value.  This gets loaded into
//                a register on the timing board.
//
// bTMDs        : Block turns-marker delays.  Also correspond to registers
//                on the timing board.  One value per timing block.
//
// glob_delays  : Global delays.  These reflect a SUM of two delay chips
//                each with a maximum delay count of 1024 steps.  There
//                is one such summed global delay chain per timing block 
//                and per gain setting regime.
//
// chan_delays  : The channel delays used for fine tuning the sampling
//                point on the sampled waveform.  One channel delay for
//                each of four cards in each of two timing blocks and per
//                gain setting regime.
//
// gain_codes   : Collection of gain values that can be chosen from to
//                set the signal gain on each front-end card.  These are
//                the actual values that get written to the gain control
//                registers on the front-end cards.
//
// default_gain : Gain setting (index of gain code) to be used when
//                first starting up the instrument.
//
// ph_turn_offsets_4ns :  640 total slots for each species
//
// ph_turn_offsets_14ns:  45 total for each species
//-----------------------------------------------------------------------+
typedef struct {
  CBPM_TIMING_CONFIG timing_config;
  int ph_race_wait_flags[CBI_NUM_SPECIES];
  int ph_turn_offsets_4ns[CBI_NUM_SPECIES][32][20]; 
  int ph_turn_offsets_14ns[CBI_NUM_SPECIES][5][9];

  int phase_jump_boundaries[CBI_NUM_SPECIES][CBPM_MAX_PHASE_JUMPS];
  int phase_turn_offsets[CBI_NUM_SPECIES][CBPM_MAX_PHASE_JUMPS+1];
  int phase_wait_values[CBI_NUM_SPECIES][CBPM_MAX_PHASE_JUMPS+1];
} CONFIGURATION_PARAMETERS;



//-----------------------------------------------------------------------+
// CBPM_TSHARC_CONTROL :  Highest level control-system-only data         |
//    structure for the CBPM-TSHARC (PT) instrumentation platform.       |
//      All structs above get attached to this one.                      |
//                                                                       |
//-----------------------------------------------------------------------+
typedef struct {
  CBPM_DETECTOR             detector;
  CONFIGURATION_PARAMETERS  config_params;
  CBPM_PHASE_CALCS          phase_calcs;
} CBPM_CONTROL;


#endif // CBPM_CTL_TYPES_H
