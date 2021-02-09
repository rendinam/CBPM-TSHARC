#======================================================================
# File header label definitions and read/write methods for
# the CBPM RD_V1a/b data file formats.
#======================================================================
import os, sys, string
from copy import deepcopy
import numpy as np
from classes import *
from file_io import *
from file_io_sets import *

#-------------------------------------------------------------------------
# RD_V1a file fields
#
# Nested list describing characteristics of general data file header
# fields in a "RD_V1" data file.
#
# Characteristics include the data type, number of values, and class
# attribute destination name.
#
#  Types:  STR = string
#          INT = integer
#          FLT = floating point
#          CUS = Custom subparsing routine for uniquely-formatted values
#-------------------------------------------------------------------------
rdv1a_header_fields = [
    [  "Measurement ID",       "INT", 1,  "file_id"           ],
    [  "Timestamp",            "STR", 1,  "timestamp"         ],
    [  "CORE Struct Rev.",     "STR", 1,  "core_comm_v"       ],
    [  "PLATFORM Struct Rev.", "STR", 1,  "plat_comm_v"       ],
    [  "Species",              "STR", 1,  "species"           ],
    [  "Bunch Pattern Name",   "STR", 1,  "bunch_pat_name"    ],
    [  "Bunch Pattern (hex)",  "CUS", 40, "bunch_pattern_hex" ]
    ]

rdv1a_inst_fields = [
    [  "BPM ID",               "STR", 1,  "hostname"           ],
    [  "Location",             "STR", 1,  "location"           ],
    [  "Detector Type",        "STR", 1,  "det_type"           ],
    [  "EXE Name",             "STR", 1,  "exe_name"           ],
    [  "EXE Build ID",         "STR", 1,  "build_id"           ],
    [  "Dig. board FPGA",      "CUS", 1,  "dig_board_fpga"     ],
    [  "Front-End FPGAs",      "CUS", 4,  "front_end_fpgas"    ],
    [  "Temperature Sensor",   "CUS", 6,  ""                   ],
    [  "BP Offset",            "CUS", 12, ""                   ],
    [  "Triggering Summary",   "STR", 1,  "trigger"            ],
    [  "Data Correction",      "STR", 1,  "data_correction"    ],
    [  "Gain Table",           "CUS", 24, ""                   ],
    [  "Pedestal Table",       "CUS", 4,  "pedestals"          ]
    ]

rdv1a_cesr_fields = [
    ["CERN Current Reading",   "DIS", 2,  ""                   ]
    ]



header_field_sets[1]        = rdv1a_header_fields
cesr_field_sets[1]          = rdv1a_cesr_fields
inst_field_sets[1]          = rdv1a_inst_fields


#-- Custom field readers
def rdv1a_read_bunch_pattern( obj, attr, line, file ):
    """Custom reader for bunch pattern spread over multiple lines"""
    pattern = []
    for lidx in range(0,4):
        line = file.next().strip('#')
        words = line.split()
        for word in words:
            pattern.append( int(word) )
    setattr( obj, attr, pattern )



def rdv1a_read_dig_board_fpga( obj, attr, line, file ):
    value = line.split(':')[1]
    value = value[2:]
    value = float( value )
    setattr( obj, attr, value )



def rdv1a_read_front_end_fpgas( obj, attr, line, file ):
    subline = line.strip('\n/').split(':')[1]
    vals = subline.split('/')
    values = []
    for val in vals:
        values.append( int(val) )
    setattr( obj, attr, values )



def rdv1a_read_temperatures( obj, attr, line, file ):
    line = file.next()
    line = file.next().strip('#').replace('C', '')
    llist = line.split()
    obj.digital_temp_c  = float(llist[0])
    obj.timing_temp_c   = float(llist[1])
    obj.card_temps_c[0] = float(llist[2])
    obj.card_temps_c[1] = float(llist[3])
    obj.card_temps_c[2] = float(llist[4])
    obj.card_temps_c[3] = float(llist[5])


def rdv1a_read_delays( obj, attr, line, file):
    line = file.next()
    for block in [0,1]:
        line = file.next().strip('#')
        llist = line.split()
        obj.bunch_pat_offsets[block] = int(llist[2])
        obj.block_delays[block]      = int(llist[3])
        obj.channel_delays[block][0] = int(llist[4])
        obj.channel_delays[block][1] = int(llist[5])
        obj.channel_delays[block][2] = int(llist[6])
        obj.channel_delays[block][3] = int(llist[7])


def rdv1a_read_gains( obj, attr, line, file ):
    line = file.next()
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
        

def rdv1a_read_pedestals( obj, attr, line, file):
    line = file.next()
    for block in [0,1]:
        line = file.next().strip('#')
        llist = line.split()
        obj.pedestals[block][0] = float(llist[2])
        obj.pedestals[block][1] = float(llist[3])
        obj.pedestals[block][2] = float(llist[4])
        obj.pedestals[block][3] = float(llist[5])


def rdv1a_read_bunches_data( fdata_obj, iidx, open_file ):
    """ Read and store the RD_V1a turn-by-turn button/phase data for
    all bunches from a single instrument in the provided opened data file.
    """
    EOF_found = False
    line = open_file.next()
    bunch_open = False
    bunch_idx = 0
    turn_idx  = 0
    while line.find( '# --- Bunch' ) == -1:
        #sys.stdout.write( "++ " + line )
        try:
            line = open_file.next()
        except StopIteration:
            EOF_found = True
            break
    #print "Bunch found"
    bdata = bunch_data()
    bdata.buttons = []
    bdata.x_pos   = np.empty([0])
    bdata.y_pos   = np.empty([0])
    for button in range(0,4):
        bdata.buttons.append( np.empty([0]) )
    fdata_obj.insts[iidx].encoded_phase = np.empty([0])
    turn_idx = 0

    while True:
        ##sys.stdout.write( "++ " + line )
        try:
            line = open_file.next()
            #sys.stdout.write( "Line = " + line )
        except StopIteration:
            EOF_found = True
            break

        if line.find( '#-------------' ) != -1:
            break

        if line.find( '# --- Bunch' ) != -1:
            fdata_obj.insts[iidx].bunches.append( deepcopy(bdata) )
            del bdata
            bdata = bunch_data()
            bdata.buttons = []
            bdata.x_pos   = np.empty([0])
            bdata.y_pos   = np.empty([0])
            for button in range(0,4):
                bdata.buttons.append( np.empty([0]) )
            bunch_idx = bunch_idx + 1
            turn_idx = 0
            continue

        if line.strip() == '':
            continue
        else:
            #print "Bunch: " + str(bunch_idx) + "  Turn: " + str(turn_idx)
            fields = line.split()
            #print fields
            turn_idx = turn_idx + 1
            bdata.corrected[0] = np.append( bdata.corrected[0], float(fields[0]) )
            bdata.corrected[1] = np.append( bdata.corrected[1], float(fields[1]) )
            bdata.corrected[2] = np.append( bdata.corrected[2], float(fields[2]) )
            bdata.corrected[3] = np.append( bdata.corrected[3], float(fields[3]) )
            if bunch_idx == 0: # Take phase field from file unmodified on first turn (it's 0)
                fdata_obj.insts[iidx].encoded_phase = np.append(
                    fdata_obj.insts[iidx].encoded_phase,
                    int(fields[6]) )
            (x_pos, y_pos) = calc_position(
                bdata.buttons[0],
                bdata.buttons[1],
                bdata.buttons[2],
                bdata.buttons[3],
                std_pos_coeffs[0],
                std_pos_coeffs[1]  )
            bdata.x_pos = np.append( bdata.x_pos, x_pos )
            bdata.y_pos = np.append( bdata.y_pos, y_pos )

    fdata_obj.insts[iidx].bunches.append( deepcopy(bdata) )
    del bdata

    fdata_obj.fheader.num_bunches = (bunch_idx+1)
    fdata_obj.insts[iidx].iheader.trigger = ''

    fdata_obj.insts[iidx].iheader.num_turns = (turn_idx)



datablock_read_methods[1] = [rdv1a_read_bunches_data]


rdv1a_custom_field_readers = {
    "Bunch Pattern (hex)" : rdv1a_read_bunch_pattern,
    "Dig. board FPGA"     : rdv1a_read_dig_board_fpga,
    "Front-End FPGAs"     : rdv1a_read_front_end_fpgas,
    "Temperature Sensor"  : rdv1a_read_temperatures,
    "BP Offset"           : rdv1a_read_delays,
    "Gain Table"          : rdv1a_read_gains,
    "Pedestal Table"      : rdv1a_read_pedestals
    }



custom_field_reader_sets[1] = rdv1a_custom_field_readers


#-- Custom field writers


def rdv1a_write_bunch_pattern( obj, attr ):
    """Custom reader for bunch pattern spread over multiple lines"""
    pattern = getattr( obj, attr )
    wcount = 1
    print '# Bunch Pattern (hex)  : '
    sys.stdout.write( '# ' )
    for word in pattern:
        print "{:04}".format( word ),
        if wcount % 10 == 0:
            if wcount != 40:
                sys.stdout.write( '\n# ' )
            else:
                sys.stdout.write( '\n' )
        wcount = wcount + 1
        

def rdv1a_write_dig_board_fpga( obj, attr ):
    pass


def rdv1a_write_front_end_fpgas( obj, attr ):
    values = getattr( obj, attr )
    for val in values:
        print val + " / "


def write_rdv1a_inst_header( inst ):
    ihdr = inst.iheader
    print '#---------------------------------------------------'
    print '# BPM ID            : '+ ihdr.hostname
    print '# Location          : '+ ihdr.location
    print '# Detector Type     : '+ ihdr.det_type
    print '# EXE Name          : '+ ihdr.exe_name
    print '# EXE Build ID      : '+ str(ihdr.build_id)
    print '# Dig. board FPGA   : v'+ str(ihdr.dig_board_fpga)
    print '# Front-End FPGAs   : {} / {} / {} / {}'.format(
        ihdr.front_end_fpgas[0],
        ihdr.front_end_fpgas[1],
        ihdr.front_end_fpgas[2],
        ihdr.front_end_fpgas[3]
        )
    print '#---------------------------------------------------'
    print '#    Temperature Sensor Summary:'
    print '# Digital_Board   Timing_Board   Front-End_0   Front-End_1   Front-End_2   Front-End_3'
    print '# {} C  {} C'.format(
        ihdr.digital_temp_c,    
        ihdr.timing_temp_c),
    for temperature in ihdr.card_temps_c:
        print '            {} C'.format(temperature),
    print ''
    print '#-----------------------------------------------------------------------------------------'
    print '#         BP Offset        Block  Delay                          Channel Delays'
    print '#                     0               1                2                3'
    for block in [0,1]:
        print '# Block  {}  {:>4}   {:>4}   {}              {}                {}               {}'.format(
            blocknames[block],
            ihdr.bunch_pat_offsets[block],
            ihdr.block_delays[block],
            ihdr.channel_delays[block][0],
            ihdr.channel_delays[block][1],
            ihdr.channel_delays[block][2],
            ihdr.channel_delays[block][3]
            )
    print '#-----------------------------------------------------------------------------------------'
    print '# Triggering Summary: '+ ihdr.trigger
    print '#-----------------------------------------------------------------------------------------'
    print '# Data Correction Applied : '+ ihdr.data_correction
    print '#-----------------------------------------------------------------------------------------'
    print '#    Gain Table:                [Gain_Setting Gain_Code]:Gain Coeff.'
    print '#                  0                  1                  2                  3'
    for block in [0,1]:
        idx = 0
        print '# Block {}          '.format(blocknames[block]),
        for value in ihdr.gain_settings[block]:
            print '[{}   {}]:{}   '.format(
                ihdr.gain_settings[block][idx],
                ihdr.gain_codes[block][idx],
                ihdr.gain_coeffs[block][idx]
                ),
            idx = idx + 1
        print ''
    print '#-----------------------------------------------------------------------------------------'
    print '#    Pedestal Table:'
    print '#                  0                  1                  2                  3       '
    for block in range(0,2):
        idx = 0
        print '# Block {}          '.format(blocknames[block]),
        for value in ihdr.gain_settings[block]:
            print '{}    '.format(
                ihdr.pedestals[block][idx]
                ),
            idx = idx + 1
        print ''
    print '#'
    print '#-------------------------------------------------------------------------------------------------------------------------------'
    print '#   Card-0       Card-1       Card-2       Card-3       Pos X      Pos Y      Turn      Ph_Word  Ph_Word  H ph_diff    V ph_diff'
    print '#   b3(TI)       b1(BI)       b2(BO)       b4(TO)        (m)        (m)        Data        (H)     (V)      (deg)        (deg)'
    print '#-------------------------------------------------------------------------------------------------------------------------------'


rdv1a_custom_field_writers = {
    "Bunch Pattern (hex)" : rdv1a_write_bunch_pattern,
    "Dig. board FPGA"     : rdv1a_write_dig_board_fpga,
    "Front-End FPGAs"     : rdv1a_write_front_end_fpgas
    }


custom_field_writer_sets[1] = rdv1a_custom_field_writers

#-- General writers
def write_rdv1a_file_header( header_field_set, custom_field_writer_sets, parcel ):
    version = 1
    hdr = parcel.fheader
    chdr = parcel.cheader
    print '#---------------------------------------------------'
    print '#       RAW DATA'
    print '#'
    write_attribs( version,
                   header_field_set,
                   custom_field_writer_sets[version],
                   hdr ) # ,file?
    print '#---------------------------------------------------'
    print '# CERN Current Reading:     Raw        Corrected'
    # Print rdv1 current data
    print '#                          {}       {} mA'.format(
        chdr.CERN_raw,
        chdr.CERN_scaled  )
    print '#---------------------------------------------------'


def write_rdv1_data_block( parcel ):
    for inst in parcel.insts:
        write_rdv1a_inst_header( inst )
        bcount = 0
        for bunch in inst.bunches:
            print '# --- Bunch ' + str(bcount+1) + ' ---'
            tcount = 0
            for turn in bunch.buttons[0]:
                TI = bunch.corrected[0][tcount]
                BI = bunch.corrected[1][tcount]
                BO = bunch.corrected[2][tcount]
                TO = bunch.corrected[3][tcount]
                phase = int(inst.encoded_phase[tcount])
                h_val = (int(phase) >> 19) & 0x000001FF
                v_val = (int(phase) >> 10) & 0x000001FF
                x_pos = bunch.x_pos[tcount]
                y_pos = bunch.y_pos[tcount]
                print ' {: >11.4f}  {: >11.4f}  {: >11.4f}  {: >11.4f}    {: > .6f}  {: > .6f}  {:>10}   {:>6}  {:>6}   000.000000   000.000000'.format(
                    TI,
                    BI,
                    BO,
                    TO,
                    x_pos,
                    y_pos,
                    phase,
                    h_val,
                    v_val
                    )                 
                tcount = tcount + 1
            bcount = bcount + 1
            print ''



#-- Top-level writer
def rdv1a_write_file( all_field_sets, custom_writers, parcel, file ):

    version = 1

    header_field_set = all_field_sets[0][version]
    cesr_field_set   = all_field_sets[1][version]
    instr_field_set  = all_field_sets[2][version]
    
    # Redirect output to file
    file = open(file, 'w')
    sys.stdout = file
    
    write_rdv1a_file_header( header_field_set, custom_writers, parcel )
    write_rdv1_data_block( parcel )

    # Restore stdout
    sys.stdout = sys.__stdout__




#-------------------------------------------------------------------------
# RD_V1b file fields
#-------------------------------------------------------------------------



#-- File fields
# Add the one extra field that distinguishes RD_V1a from RD_V1b.
rdv1b_header_fields        = deepcopy(rdv1a_header_fields)
rdv1b_header_fields.append( [  "Number of bunches",    "INT", 1, "num_bunches"  ] )

rdv1b_cesr_fields          = deepcopy(rdv1a_cesr_fields)

rdv1b_inst_fields          = deepcopy(rdv1a_inst_fields)

header_field_sets[1.01]        = rdv1b_header_fields
cesr_field_sets[1.01]          = rdv1b_cesr_fields
inst_field_sets[1.01]          = rdv1b_inst_fields



#-- Custom field readers
rdv1b_custom_field_readers = deepcopy(rdv1a_custom_field_readers)

custom_field_reader_sets[1.01] = rdv1b_custom_field_readers



#-- datablock read methods
def rdv1b_read_bunches_data( fdata_obj, iidx, open_file ):
    rdv1a_read_bunches_data( fdata_obj, iidx, open_file )


datablock_read_methods[1.01] = [rdv1b_read_bunches_data]


#-- Custom field writers
rdv1b_custom_field_writers = deepcopy(rdv1a_custom_field_writers)

custom_field_writer_sets[1.01] = rdv1b_custom_field_writers



#-- General writers    
def write_rdv1b_file_header( header_field_set, custom_field_writer_sets, parcel ):
    version = 1.01
    hdr  = parcel.fheader
    chdr = parcel.cheader
    print '#---------------------------------------------------'
    print '#       RAW DATA'
    print '#'
    write_attribs( version,
                   header_field_set,
                   custom_field_writer_sets[version],
                   hdr )
    print '#---------------------------------------------------'
    print '# CERN Current Reading:     Raw        Corrected'
    # Print rdv1 current data
    print '#                          {}       {} mA'.format(
        chdr.CERN_raw,
        chdr.CERN_scaled  )
    print '#---------------------------------------------------'

def write_rdv1b_inst_header( inst ):
    """
    """
    write_rdv1a_inst_header( inst )



#-- Top-level writer
def rdv1b_write_file( all_field_sets, custom_field_writer_sets, parcel, file ):

    version = 1.01

    header_field_set = all_field_sets[0][version]

    cesr_field_set   = all_field_sets[1][version]
    instr_field_set  = all_field_sets[2][version]
    
    # Redirect output to file
    file = open(file, 'w')
    sys.stdout = file
    
    write_rdv1b_file_header( header_field_set, custom_field_writer_sets, parcel )
    write_rdv1_data_block( parcel )

    # Restore stdout
    sys.stdout = sys.__stdout__



