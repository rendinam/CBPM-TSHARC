#======================================================================
# File header label definitions and read/write methods for
# the CBPM RD_V2 data file format.
#======================================================================
import os, sys, string
from copy import deepcopy
import numpy as np
from classes import *
from file_io import *
from rdv1_file_info import *
from file_io_sets import *


#-----------------------------------------------------------------
# RD_V2 file fields
#-----------------------------------------------------------------
rdv2_header_fields = deepcopy(rdv1b_header_fields)

rdv2_cesr_fields = deepcopy(rdv1a_cesr_fields)

rdv2_inst_fields = [
    [  "BPM ID",              "STR", 1,  "hostname"           ],
    [  "Location",            "STR", 1,  "location"           ],
    [  "Detector Type",       "STR", 1,  "det_type"           ],
    [  "EXE Name",            "STR", 1,  "exe_name"           ],
    [  "EXE Build ID",        "STR", 1,  "build_id"           ],
    [  "Dig. board FPGA",     "CUS", 1,  "dig_board_fpga"     ],
    [  "Front-End FPGAs",     "CUS", 4,  "front_end_fpgas"    ],
    [  "Number of turns",     "INT", 1,  "num_turns"          ],
    [  "Turn counter",        "INT", 1,  "turn_sync_counter"  ],
    [  "Turn spacing",        "INT", 1,  "turn_spacing"       ],
    [  "Trigger",             "STR", 1,  "trigger"            ],
    [  "Data Correction",     "STR", 1,  "data_correction"    ],
    [  "Delay Summary",       "CUS", 1,  ""                   ],
    [  "Gain Table",          "CUS", 24, ""                   ],
    [  "Pedestal Table",      "CUS", 4,  "pedestals"          ],
    [  "Temperatures",        "CUS", 6,  ""                   ],
    [  "ADC SATURATION",      "CUS", 8,  "adc_saturation"     ],
    [  "ADC    High",         "CUS", 8,  "adc_high"           ],
    [  "ADC    Low",          "CUS", 8,  "adc_low"            ]
    ]

header_field_sets[2] = rdv2_header_fields
cesr_field_sets[2]   = rdv2_cesr_fields
inst_field_sets[2]   = rdv2_inst_fields


# ---Custom field readers---


rdv2_read_bunch_pattern   = rdv1a_read_bunch_pattern
rdv2_read_dig_board_fpga  = rdv1a_read_dig_board_fpga
rdv2_read_front_end_fpgas = rdv1a_read_front_end_fpgas
rdv2_read_delays          = rdv1a_read_delays


def rdv2_read_gains( obj, attr, line, file ):
    for block in [0,1]:
        line = file.next().strip('#').replace('[',' ').replace(']',' ').replace(':',' ')
        llist = line.split()
        obj.gain_settings[block][0] = int(llist[2])
        obj.gain_settings[block][1] = int(llist[5])
        obj.gain_settings[block][2] = int(llist[8])
        obj.gain_settings[block][3] = int(llist[11])
        obj.gain_codes[block][0] = int(llist[3])
        obj.gain_codes[block][1] = int(llist[6])
        obj.gain_codes[block][2] = int(llist[9])
        obj.gain_codes[block][3] = int(llist[12])
        obj.gain_coeffs[block][0] = float(llist[4])
        obj.gain_coeffs[block][1] = float(llist[7])
        obj.gain_coeffs[block][2] = float(llist[10])
        obj.gain_coeffs[block][3] = float(llist[13])



def rdv2_read_pedestals( obj, attr, line, file):
    for block in [0,1]:
        line = file.next().strip('#')
        llist = line.split()
        obj.pedestals[block][0] = float(llist[2])
        obj.pedestals[block][1] = float(llist[3])
        obj.pedestals[block][2] = float(llist[4])
        obj.pedestals[block][3] = float(llist[5])


rdv2_read_temperatures    = rdv1a_read_temperatures


def rdv2_read_signal_flags( obj, attr, line, file):
    llist = line.split('--')[1]
    values = llist.split()
    
    flags = [[0,0,0,0],[0,0,0,0]]

    for card in [0,1,2,3]:
        flags[0][card] = int(values[card][0])
        flags[1][card] = int(values[card][2])
    setattr( obj, attr, flags )
    


def rdv2_read_phase_data( fdata_obj, iidx, open_file ):
    """ Read and store the turn-by-turn phase word data from a single
    instrument in the provided opened data file.
    """
    line = open_file.next()
    while line.find( "# Phase Data" ) == -1:
        line = open_file.next()
    line = open_file.next()
    num_turns = fdata_obj.insts[iidx].iheader.num_turns
    fdata_obj.insts[iidx].encoded_phase = np.empty( [num_turns] )
    for turn in range(0,num_turns):
       line = open_file.next()
       fdata_obj.insts[iidx].encoded_phase[turn] = int( line.strip().split()[0] )



def rdv2_read_bunches_data( fdata_obj, iidx, open_file ):
    """ Read and store the turn-by-turn button data for all bunches
    from a single instrument in the provided opened data file.
    """
    line = open_file.next()
    num_turns = fdata_obj.insts[iidx].iheader.num_turns
    num_bunches = fdata_obj.fheader.num_bunches
    for bidx in range(0,num_bunches):
        while line.find( "# --- Bunch") == -1:
            line = open_file.next()
        line = open_file.next()            
        bdata = bunch_data()
        bdata.buttons   = []
        bdata.corrected = []
        bdata.x_pos = np.empty( [num_turns] )
        bdata.y_pos = np.empty( [num_turns] )
        for button in range(0,4):
            bdata.buttons.append( np.empty( [num_turns] ) )
            bdata.corrected.append( np.empty( [num_turns] ) )
        for turn in range(0,num_turns):
            fields = line.split()
            #bdata.buttons[0][turn] = float( fields[0] )
            #bdata.buttons[1][turn] = float( fields[1] )
            #bdata.buttons[2][turn] = float( fields[2] )
            #bdata.buttons[3][turn] = float( fields[3] )
            bdata.corrected[0][turn] = float( fields[0] )
            bdata.corrected[1][turn] = float( fields[1] )
            bdata.corrected[2][turn] = float( fields[2] )
            bdata.corrected[3][turn] = float( fields[3] )
            # Convert corrected values into raw ADC counts using gain coefficient and pedestal information.
            #bdata.buttons[0][turn] = (bdata.corrected[0][turn] * fdata_obj.insts[iidx].iheader.gain_coeffs[tblock][0]) + fdata_obj.insts[iidx].iheader.pedestals[tblock][0]
            #bdata.buttons[1][turn] = (bdata.corrected[1][turn] * fdata_obj.insts[iidx].iheader.gain_coeffs[tblock][1]) + fdata_obj.insts[iidx].iheader.pedestals[tblock][1]
            #bdata.buttons[2][turn] = (bdata.corrected[2][turn] * fdata_obj.insts[iidx].iheader.gain_coeffs[tblock][2]) + fdata_obj.insts[iidx].iheader.pedestals[tblock][2]
            #bdata.buttons[3][turn] = (bdata.corrected[3][turn] * fdata_obj.insts[iidx].iheader.gain_coeffs[tblock][3]) + fdata_obj.insts[iidx].iheader.pedestals[tblock][3]
            (x_pos, y_pos) = calc_position(
                bdata.corrected[0][turn],
                bdata.corrected[1][turn],
                bdata.corrected[2][turn],
                bdata.corrected[3][turn],
                std_pos_coeffs[0],
                std_pos_coeffs[1]  )
            bdata.x_pos = np.append( bdata.x_pos, x_pos )
            bdata.y_pos = np.append( bdata.y_pos, y_pos )
            try:
                line = open_file.next()
            except StopIteration:
                fdata_obj.insts[iidx].bunches.append( deepcopy(bdata) )
                return
        fdata_obj.insts[iidx].bunches.append( deepcopy(bdata) )
        del bdata


rdv2_custom_field_readers = {
    "Bunch Pattern (hex)" : rdv2_read_bunch_pattern,
    "Dig. board FPGA"     : rdv2_read_dig_board_fpga,
    "Front-End FPGAs"     : rdv2_read_front_end_fpgas,
    "Delay Summary"       : rdv2_read_delays,
    "Gain Table"          : rdv2_read_gains,
    "Pedestal Table"      : rdv2_read_pedestals,
    "Temperatures"        : rdv2_read_temperatures,
    "ADC SATURATION"      : rdv2_read_signal_flags,
    "ADC    High"         : rdv2_read_signal_flags,
    "ADC    Low"          : rdv2_read_signal_flags
    }


datablock_read_methods[2]   = [rdv2_read_phase_data, rdv2_read_bunches_data]
custom_field_reader_sets[2] = rdv2_custom_field_readers



# ---Custom field writers---


rdv2_write_bunch_pattern   = rdv1a_write_bunch_pattern
rdv2_write_dig_board_fpga  = rdv1a_write_dig_board_fpga
rdv2_write_front_end_fpgas = rdv1a_write_front_end_fpgas


def write_rdv2_file_header( header_field_sets, custom_field_writer_sets, parcel ):
    version = 2
    fhdr = parcel.fheader
    chdr = parcel.cheader
    print '#---------------------------------------------------'
    print '#\tRAW DATA - EXTENDED'
    print '#'
    write_attribs( version,
                   header_field_sets[version],
                   custom_field_writer_sets[version],
                   fhdr )
    print '#---------------------------------------------------'
    print '# CERN Current Reading:     Raw        Corrected'
    # Print rdv2 current data
    print '#                          {:6d}       {:3.3f} mA'.format(
        chdr.CERN_raw,
        chdr.CERN_scaled
        )
    print '#---------------------------------------------------'



def write_rdv2_inst_header( inst ):
    ihdr = inst.iheader
    print '#---------------------------------------------------'
    print '# BPM ID            : '+ str(ihdr.hostname)
    print '# Location          : '+ str(ihdr.location)
    print '# Detector Type     : '+ str(ihdr.det_type)
    print '# EXE Name          : '+ str(ihdr.exe_name)
    print '# EXE Build ID      : '+ str(ihdr.build_id)
    print '# Dig. board FPGA   : v'+ str(ihdr.dig_board_fpga)
    print '# Front-End FPGAs   : {} / {} / {} / {} '.format(
        ihdr.front_end_fpgas[0],
        ihdr.front_end_fpgas[1],
        ihdr.front_end_fpgas[2],
        ihdr.front_end_fpgas[3]
        )
    print '#'
    print '# Number of turns   : '+ str(ihdr.num_turns)
    print '# Turn counter      : '+ str(ihdr.turn_sync_counter)
    print '# Turn spacing      : '+ str(ihdr.turn_spacing)
    print '# Trigger           : '+ ihdr.trigger +' '
    print '# Data Correction   : PED SUB & SCALED' ##+ ihdr.data_correction
    blocks = ['A', 'B']
    print '#'
    print '#    Delay Summary: '
    print '#         Offset Block                           Channel         '
    
    for block in range(0,2):
        print '# Block  {} {:>4}  {:>4}   {:4}               {:4}               {:4}               {:4}    '.format(
            blocknames[block],
            ihdr.bunch_pat_offsets[block],
            ihdr.block_delays[block],
            ihdr.channel_delays[block][0],
            ihdr.channel_delays[block][1],
            ihdr.channel_delays[block][2],
            ihdr.channel_delays[block][3]
            )
    print '#'
    print '#    Gain Table:                [Gain_Setting Gain_Code]:Gain Coeff. '
    for block in range(0,2):
        idx = 0
        print '# Block {}         '.format(blocknames[block]),
        for value in ihdr.gain_settings[block]:
            print '[{}   {}]:{:1.6f}  '.format(
                ihdr.gain_settings[block][idx],
                ihdr.gain_codes[block][idx],
                ihdr.gain_coeffs[block][idx]
                ),
            idx = idx + 1
        sys.stdout.write('\n')
        
    print '#'
    print '#    Pedestal Table:'
    for block in range(0,2):
        idx = 0
        print '# Block {}               '.format(blocknames[block]),
        for value in ihdr.gain_settings[block]:
            print '{:3.6f}        '.format(
                ihdr.pedestals[block][idx]
                ),
            idx = idx + 1
        print ''
    print '#'
    print '#    Temperatures:'
    print '# Digital  Timing'
    print '# {:2.1f} C   {:2.1f} C'.format(
        ihdr.digital_temp_c,    
        ihdr.timing_temp_c),
    for temperature in ihdr.card_temps_c:
        print '          {:2.1f} C'.format(temperature),
    print ''
    print '#'
    print '#  ADC SATURATION  --      {}:{}                {}:{}                {}:{}                {}:{}                '.format(
        ihdr.adc_saturation[0][0],
        ihdr.adc_saturation[1][0],
        ihdr.adc_saturation[0][1],
        ihdr.adc_saturation[1][1],
        ihdr.adc_saturation[0][2],
        ihdr.adc_saturation[1][2],
        ihdr.adc_saturation[0][3],
        ihdr.adc_saturation[1][3]  )        
    print '#  ADC    High     --      {}:{}                {}:{}                {}:{}                {}:{}                '.format(
        ihdr.adc_high[0][0],
        ihdr.adc_high[1][0],
        ihdr.adc_high[0][1],
        ihdr.adc_high[1][1],
        ihdr.adc_high[0][2],
        ihdr.adc_high[1][2],
        ihdr.adc_high[0][3],
        ihdr.adc_high[1][3]  )     
    print '#  ADC    Low      --      {}:{}                {}:{}                {}:{}                {}:{}                '.format(
        ihdr.adc_low[0][0],
        ihdr.adc_low[1][0],
        ihdr.adc_low[0][1],
        ihdr.adc_low[1][1],
        ihdr.adc_low[0][2],
        ihdr.adc_low[1][2],
        ihdr.adc_low[0][3],
        ihdr.adc_low[1][3]  )     



rdv2_custom_field_writers = {
    "Bunch Pattern (hex)" : rdv2_write_bunch_pattern,
    "Dig. board FPGA"     : rdv2_write_dig_board_fpga,
    "Front-End FPGAs"     : rdv2_write_front_end_fpgas
    }


custom_field_writer_sets[2] = rdv2_custom_field_writers



#-- Top-level writer


def rdv2_write_file( header_field_sets, custom_field_writer_sets, parcel, file ):
    # Redirect output to file
    file = open(file, 'w')
    sys.stdout = file
    
    write_rdv2_file_header( header_field_sets,custom_field_writer_sets, parcel )
    for inst in parcel.insts:
        write_rdv2_inst_header( inst )
        print '#---------------------------------------------------------'
        print '# Encoded     Ph_Word  Ph_Word  H ph_diff    V ph_diff'
        print '# Phase Data     (H)     (V)      (deg)        (deg)'
        print '#---------------------------------------------------------'
        h_diff = 0.0
        v_diff = 0.0
        for phase in inst.encoded_phase:
            h_val = (int(phase) >> 19) & 0x000001FF
            v_val = (int(phase) >> 10) & 0x000001FF
            print '{:>10}  {:>6}   {:>6}   000.000000   000.000000'.format(
                int(phase),
                h_val,
                v_val,
                h_diff,
                v_diff
                )
        bcount = 1
        print '#----------------------------------------------------------------------------'
        print '#   Card-0       Card-1       Card-2       Card-3       Pos X      Pos Y     '
        print '#   b3(TI)       b1(BI)       b2(BO)       b4(TO)        (m)        (m)      '
        print '#----------------------------------------------------------------------------'
        for bunch in inst.bunches:
            print '# --- Bunch ' + str(bcount) + ' ---'
            tcount = 0
            for turn in bunch.buttons[0]:
                TI = bunch.corrected[0][tcount]
                BI = bunch.corrected[1][tcount]
                BO = bunch.corrected[2][tcount]
                TO = bunch.corrected[3][tcount]
                sum   = TI + BI + BO + TO
                x_num = TO + BO - BI - TI
                y_num = TO - BO - BI + TI
                x_pos = std_pos_coeffs[0] * (x_num/sum)
                y_pos = std_pos_coeffs[1] * (y_num/sum)
                print ' {: >11.4f}  {: >11.4f}  {: >11.4f}  {: >11.4f}    {: > .6f}  {: > .6f}'.format(
                    TI,
                    BI,
                    BO,
                    TO,
                    x_pos,
                    y_pos
                    ) 
                tcount = tcount + 1
            bcount = bcount + 1
            print ''

    # Restore stdout
    sys.stdout = sys.__stdout__
