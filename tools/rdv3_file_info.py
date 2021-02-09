#======================================================================
# File header label definitions and read/write methods for
# the CBPM RD_V3 data file format.
#======================================================================
import os, sys, string
from copy import deepcopy
import numpy as np
from classes import *
from file_io import *
from file_io_sets import *

#-------------------------------------------------------------------------
# RD_V3 file fields
#-------------------------------------------------------------------------
rdv3_header_fields = [
    [  "Instrument_Type",    "STR",   1,   "instrument_type"     ],
    [  "Command_ID",         "STR",   1,   "command_id"          ],
    [  "File_type",          "STR",   1,   "file_type"           ],
    [  "File_ID",            "INT",   1,   "file_id"             ],
    [  "File_Version",       "INT",   1,   "file_version"        ],
    [  "Timestamp",          "STR",   1,   "timestamp"           ],
    [  "Core_commstruct_v",  "INT",   1,   "core_comm_v"         ],
    [  "Plat_commstruct_v",  "INT",   1,   "plat_comm_v"         ],
    [  "Bunch_Patt_Name",    "STR",   1,   "bunch_pat_name"      ],
    [  "Bunch_Patt_(hex)",   "INT",   40,  "bunch_pattern_hex"   ],
    [  "Species",            "STR",   1,   "species"             ],
    [  "Num_Instruments",    "INT",   1,   "num_instruments"     ],
    [  "Number_of_Bunches",  "INT",   1,   "num_bunches"         ],
    [  "Number_of_Turns",    "INT",   1,   "num_turns"           ],
    [  "Turn_Spacing",       "INT",   1,   "turn_spacing"        ],
    [  "Timing_Setup",       "STR",   1,   "timing_setup"        ],
    [  "Trigger",            "STR",   1,   "trigger"             ]
    ]

#    [  "Turn_sync_counter",  "INT",   1,   "turn_sync_counter"   ],
#    [  "Averaging_Mode",     "STR",   1,   "averaging_mode"      ],
#    [  "Data_Correction",    "STR",   1,   "data_correction"     ],

rdv3_cesr_fields = [
    [  "CESR CONDX",         "INT",   1,   "condx"               ],
    [  "CERN Current Raw",   "INT",   1,   "CERN_raw"            ],
    [  "CERN Current mA",    "FLT",   1,   "CERN_scaled"         ]
    ]

rdv3_inst_fields = [
    [  "Location",           "STR",   1,   "location"            ],
    [  "BPM_hostname",       "STR",   1,   "hostname"            ],
    [  "BPM_IP_Address",     "STR",   1,   "IP_address"          ],
    [  "Detector_Type",      "STR",   1,   "det_type"            ],
    [  "Detector_Coeffs",    "FLT",   2,   "det_coeffs"          ],
    [  "EXE_Name",           "STR",   1,   "exe_name"            ],
    [  "EXE_Build_ID",       "INT",   1,   "build_id"            ],
    [  "Dig.Board_FPGA",     "FLT",   1,   "dig_board_fpga"      ],
    [  "Front-End_FPGAs",    "INT",   4,   "front_end_fpgas"     ],
    [  "Timing_Setup",       "STR",   1,   "timing_setup"        ],
    [  "Number_of_Turns",    "INT",   1,   "num_turns"           ],
    [  "Turn_sync_counter",  "INT",   1,   "turn_sync_counter"   ],
    [  "Turn_spacing",       "INT",   1,   "turn_spacing"        ],
    [  "Trigger",            "STR",   1,   "trigger"             ],
    [  "Bunch_Pat_offsets",  "INT",   2,   "bunch_pat_offsets"   ],
    [  "Com_Turnmrk_Dly",    "INT",   1,   "com_tmark_delay"     ],
    [  "Blk_Turnmrk_Dlys",   "INT",   2,   "block_tmark_delays"  ],
    [  "Block_Delays",       "INT",   2,   "block_delays"        ],
    [  "Channel_Delays",     "INT", [2,4], "channel_delays"      ],
    [  "Gain_Settings",      "INT", [2,4], "gain_settings"       ],
    [  "Gain_Codes",         "INT", [2,4], "gain_codes"          ],
    [  "Gain_Coeffs",        "FLT", [2,4], "gain_coeffs"         ],
    [  "Pedestals",          "FLT", [2,4], "pedestals"           ],
    [  "Digital_Temp_C",     "FLT",   1,   "digital_temp_c"      ],
    [  "Card_Temps_C",       "FLT",   4,   "card_temps_c"        ],
#    [  "ADC_saturation",     "INT", [2,4], "adc_saturation"      ],
#    [  "ADC_High",           "INT", [2,4], "adc_high"            ],
#    [  "ADC_Low ",           "INT", [2,4], "adc_low"             ]
    ]

#    [  "Averaging_Mode",     "STR",   1,   "averaging"           ],
#    [  "Data_Correction",    "STR",   1,   "data_correction"     ],


header_field_sets[3] = rdv3_header_fields
cesr_field_sets[3]   = rdv3_cesr_fields
inst_field_sets[3]   = rdv3_inst_fields


#-- Custom field readers

rdv3_custom_field_readers = []
custom_field_reader_sets[3] = rdv3_custom_field_readers



 
def rdv3_read_bunches_data( fdata_obj, iidx, open_file ):
    """ Read and store the turn-by-turn button data for all bunches
    from a single instrument in the provided opened data file.
    """

    # Temporary post-processing of bunch pattern to make it match
    # string-storing way of the other file format readers.
    #  TODO: Bring non-V3 file format parsing of bunch pattern
    #        up to speed by storing and int for each work, or
    #        better yet, each bit as an int in a list.
    #for widx in range(0, len(fdata_obj.fheader.bunch_pattern)):
    #    print fdata_obj.fheader.bunch_pattern[widx]


    # Reverse the bunch pattern array to conform to earlier
    # formatting (LSB rightmost).
    # Only do this once per data parcel, hence the first-instrument
    # check.
    ##if iidx == 0:
    ##    fdata_obj.fheader.bunch_pattern.reverse()

    line = open_file.next()
    num_bunches = fdata_obj.fheader.num_bunches
    num_turns = fdata_obj.insts[iidx].iheader.num_turns
    while line.find( "--BEGIN DATA--") == -1:
        line = open_file.next()
    fdata_obj.insts[iidx].encoded_phase = np.empty( [num_turns] )
    for bidx in range(0,num_bunches):
        bdata = bunch_data()
        bdata.tblock    = -1
        bdata.buttons   = []
        bdata.corrected = []
        bdata.x_pos = np.empty( [num_turns] )
        bdata.y_pos = np.empty( [num_turns] )
        for button in range(0,4):
            bdata.buttons.append( np.empty( [num_turns] ) )
            bdata.corrected.append( np.empty( [num_turns] ) )
        for turn in range(0,num_turns):
            try:
                line = open_file.next()
            except StopIteration:
                fdata_obj.insts[iidx].bunches.append( deepcopy(bdata) )
                return
            fields = line.split()
            # Use the first turn's worth of data for the given bunch to get
            # the timing block used for acquisition.
            if turn == 0:
                tblock = int( fields[0] )
                #tblock = bdata.tblock
            bdata.buttons[0][turn] = float( fields[2] )
            bdata.buttons[1][turn] = float( fields[3] )
            bdata.buttons[2][turn] = float( fields[4] )
            bdata.buttons[3][turn] = float( fields[5] )
            bdata.corrected[0][turn] = (bdata.buttons[0][turn] - fdata_obj.insts[iidx].iheader.pedestals[tblock][0]) * (1/fdata_obj.insts[iidx].iheader.gain_coeffs[tblock][0])
            bdata.corrected[1][turn] = (bdata.buttons[1][turn] - fdata_obj.insts[iidx].iheader.pedestals[tblock][1]) * (1/fdata_obj.insts[iidx].iheader.gain_coeffs[tblock][1])
            bdata.corrected[2][turn] = (bdata.buttons[2][turn] - fdata_obj.insts[iidx].iheader.pedestals[tblock][2]) * (1/fdata_obj.insts[iidx].iheader.gain_coeffs[tblock][2])
            bdata.corrected[3][turn] = (bdata.buttons[3][turn] - fdata_obj.insts[iidx].iheader.pedestals[tblock][3]) * (1/fdata_obj.insts[iidx].iheader.gain_coeffs[tblock][3])
            (x_pos, y_pos) = calc_position(
                bdata.corrected[0][turn],
                bdata.corrected[1][turn],
                bdata.corrected[2][turn],
                bdata.corrected[3][turn],
                std_pos_coeffs[0],
                std_pos_coeffs[1]  )
            bdata.x_pos[turn] = x_pos
            bdata.y_pos[turn] = y_pos
            # Save phase word only for first bunch encountered
            if bidx == 0:
                fdata_obj.insts[iidx].encoded_phase[turn] = int( fields[1], 16 )
        fdata_obj.insts[iidx].bunches.append( deepcopy(bdata) )
        del bdata

datablock_read_methods[3] = [rdv3_read_bunches_data]



rdv3_custom_field_writers = []
custom_field_writer_sets[3] = rdv3_custom_field_writers



def rdv3_write_file( all_field_sets, custom_writers, parcel, file ):

    version = 3
    # Redirect output to file
    file = open(file, 'w')
    sys.stdout = file

    hdr  = parcel.fheader
    chdr = parcel.cheader
    header_field_set = all_field_sets[0][version]
    cesr_field_set   = all_field_sets[1][version]
    instr_field_set  = all_field_sets[2][version]

    write_attribs( version,
                   header_field_set,
                   custom_writers,
                   hdr )
    print ""

    ## FIXME: enable 
    write_attribs( version,
                   cesr_field_set,
                   custom_writers,
                   chdr )

    print ""

    for inst in parcel.insts:
        write_attribs( version,
                       instr_field_set,
                       custom_writers,
                       inst.iheader )
        print "# Timing  Encoded     Card-0   Card-1   Card-2   Card-3 "
        print "# Block  Phase word   b3(TI)   b1(BI)   b2(BO)   b4(TO) "
        print "--BEGIN DATA--"
        
        bcount = 0
        for bunch in inst.bunches:
            tcount = 0
            for turn in bunch.buttons[0]:
                TI = bunch.buttons[0][tcount]
                BI = bunch.buttons[1][tcount]
                BO = bunch.buttons[2][tcount] 
                TO = bunch.buttons[3][tcount]
                sum   = TI + BI + BO + TO
                x_num = TO + BO - BI - TI
                y_num = TO - BO - BI + TI
                x_pos = std_pos_coeffs[0] * (x_num/sum)
                y_pos = std_pos_coeffs[1] * (y_num/sum)
                phase = int(inst.encoded_phase[tcount])
                h_val = (int(phase) >> 19) & 0x000001FF
                v_val = (int(phase) >> 10) & 0x000001FF
                print '{:d}    {:>10}  {: >7d}  {: >7d}  {: >7d}  {: >7d}'.format(
                    bunch.tblock,
                    hex(phase)[2:],
                    int(TI),
                    int(BI),
                    int(BO),
                    int(TO),
                    )                
                tcount = tcount + 1
            bcount = bcount + 1

    # Direct output back to stdout.
    sys.stdout = sys.__stdout__
