import os, sys, time
from copy import deepcopy

import numpy as np

from classes import *
from file_io_sets import *


std_pos_coeffs = [0.028800, 0.022300]


def rd_file_version( filename ):
    """ Fast check of file version.
    
    Open file
    Read first 2 lines
    Compare and report
    
    File versions available:
        0      Unrecognized/unsupported file format
    ver 1    = Original               (RD  prior to 17-Dec-2009  ID:000864 to 002215  )
    ver 1.01 = Original (w/ num bunches)  (RD  from  1-Jul-2010  ID:002216 to Present )
    ver 2    = Extended                   (RDe from 13-May-2010  ID:001650 to Present )
                                             1595-1649 are trash
                                             2216       is trash
                                            1650-2218 are missing number of bunches in file header.
                                            2219 has first additional file header line for bunches.
    ver 3    = New canonical              (RD  from  8-Mar-2011  ID:004080+)
    """
    version = 0
    
    file = open(filename, 'r')
    line = file.readline()
    line = file.readline()
    if line.find("Command_ID") != -1:
        version = 3
        return version
    
    if line.find("RAW DATA - EXTENDED") != -1:
        version = 2
        return version
    
    if line.find("RAW DATA") != -1:
        for lnum in range(1, 14):
            line = file.readline()
        if line.find("#----") != -1:
            version = 1
        else:
            version = 1.01
        # Close and reopen file to reset input
        file.close()
        file = open(filename, 'r')
        line = file.readline()
        line = file.readline()
        return version
    
    return version





def read_rd_file( filename, version ):
    """ Reads the RD_V2 file with name 'filename' and extracts all
    supported file-level header fields, instrument-level header fields,
    and instrument data blocks, storing them in efficient numpy arrays
    where appropriate.
    """
    global data_parcels

    bunpat_bits_per_word = 16

    parse_begin = time.time()

    file = open(filename, 'r')
    
    fdata = file_data()
    fdata.name = os.path.split( filename )[1]

    fheader       = file_header()
    fdata.fheader = deepcopy( fheader )
    del fheader

    cheader       = cesr_header()
    fdata.cheader = deepcopy( cheader )
    del cheader

    EOF_found = False;

    # Read and store supported version-specific file header values
    (line, EOF_found) = populate_attribs_from_file(
        header_field_sets[version],
        custom_field_reader_sets[version],
        fdata.fheader,
        file)


    # Process the hex-word representation of th ebunch pattern
    # and store it as a list of the integers 0 & 1.
    if version >= 3:
        fdata.fheader.bunch_pattern_hex.reverse()
    for word in fdata.fheader.bunch_pattern_hex:
        unpadded = str(bin(word)[2:])
        padding = bunpat_bits_per_word - (len(unpadded))
        value = ''
        for padval in range(0,padding):
            value = value + '0'
        value = value + unpadded
        # Reverse each word's bit string for proper ordering of
        # bunch bits the same as list indexing.
        value = value[::-1]
        for char in value:
            fdata.fheader.bunch_pattern.append( int(char) )


    #print fdata.fheader.bunch_pattern_hex
    #print ""
    #print fdata.fheader.bunch_pattern
    

##     # Read and store supported CESR header values
##      FIXME (Breaks reading of files lower than V3) FIXME
    if version >= 3:
        (line, EOF_found) = populate_attribs_from_file(
            cesr_field_sets[version],
            custom_field_writer_sets[version],
            fdata.cheader,
            file)

    
    # Process each instrument header TBT data (sub)section
    inst_idx = 0

    while line != "":

        #sys.stdout.write("read_rd_file LINE: " + line)
        
        iheader = inst_header()

        #print "read_rd_file INST HEADER READ"
        # Store supported instrument header values
        (line, EOF_found) = populate_attribs_from_file(
            inst_field_sets[version],
            custom_field_reader_sets[version],
            iheader,
            file)

        # If attribute populator returned EOF, parsing is done; break out.
        if EOF_found == True:
            #print "read_rd_file BREAKING!"
            break


        idata = inst_data()
        fdata.insts.append( deepcopy(idata) )
        del idata

        fdata.insts[inst_idx].iheader = deepcopy( iheader )
        del iheader

        if version != 1 and version != 1.01:
            num_turns = fdata.insts[inst_idx].iheader.num_turns
        else:
            num_turns = -1

        ##print "DATABLOCK read"
        for method in datablock_read_methods[version]:
            method( fdata, inst_idx, file)

        inst_idx = inst_idx + 1    

    parse_end = time.time()

    fdata.read_time_s = (parse_end - parse_begin)

    data_parcels.append( deepcopy(fdata) )
    
    del fdata
    elapsed = (parse_end - parse_begin)
    return elapsed








def populate_attribs_from_file( field_labels, custom_parsers, object, open_file ):
    """Read and store all values associated with a field label from an opened data file.

    Loop over a collection of field_labels with attached
    characteristics as seen in list definitions above, and
    scan through the file passed in looking for those labels.
    
    When a label is found, extract the value associated with it
    and store it once casted into the appropriate type in the
    attribute named along with the label collection entry.

    If the type associated with the value expected for a given
    label is of the type 'CUS', a custom parser will be invoked
    to handle various unusual file formatting.

    Returns:
      The line on which parsing has stopped in the file so that
      incremental parsing may be performed on the rest of it.
    """
    line = ""
    EOF_found = False

    # Type conversion functions
    t_convs = {
        "INT" : int,
        "FLT" : float
        }

    for field in field_labels:

        ##print "popu_attribs       " + field[0]

        # If field is disabled, skip processing it.
        if field[1] == "DIS":
            continue
        
        line = ""
        try:
            line = open_file.next()
            ##sys.stdout.write("popu_attribs LINE: " + line)
        except StopIteration:
            #print "DEPLETED HEADERS IN LIST"
            EOF_found = True
            return (line, EOF_found)
        while line.find( field[0] ) == -1:
            try:
                line = open_file.next()
            except StopIteration:
                #print "DEPLETED HEADERS IN LIST 2"
                EOF_found = True
                return (line, EOF_found)
            
        datatype = field[1]
        try:
            values = line.split(":",1)[1].strip()
        except IndexError: # This is a bare label, with no ':' char.
            pass
        attribute = field[3]
        if datatype == "STR":
            setattr( object, attribute, values )

        if datatype == "INT" or datatype == "FLT":
            values = values.split()
            # Convert all values in-place in list
            for idx in range(0,len(values)):
                values[idx] = t_convs[datatype](values[idx])

            if type(field[2]) is list:
                store = []
                sublen = field[2][1]
                for lidx in range(0, field[2][0]):
                    store.append( values[lidx*sublen:(lidx+1)*sublen] )
                setattr( object, attribute, store )
                    
            elif len(values) > 1:
                setattr( object, attribute, values )
                
            else:
                setattr( object, attribute, values[0] )

        elif datatype == "CUS":
            custom_parsers[field[0]]( object, attribute, line, open_file )

        #print field[3] + "\n   ",
        #print getattr( object, attribute )
    #print "RETURNING WITH NO EOF"
    return (line, EOF_found)




# TODO: allow parameter specifying the width to use before ':' character,
#       as this varies between file versions.
def write_attribs( version, fields, custom_dumpers, object ):

    comment_char_field_versions = [1, 1.01, 2]
    widths = { 1   :23,
               1.01:23,
               2   :23,
               3   :18  }

    cmnt = ''
    if version in comment_char_field_versions:
        cmnt = '# '

    for field in fields:
        
        datatype = field[1]
        attribute = field[3]
        if datatype == "CUS":
            custom_dumpers[field[0]]( object, attribute )
        else:
            print (cmnt + field[0]).ljust(widths[version]) + ':',
            if type(field[2]) is list:
                values = getattr(object, attribute)
                sublen = field[2][1]
                for sublist in values:
                    for value in sublist:
                        print value,
                        sys.stdout.write('  ')
                    print "   ",
                print ""

            else:
                if type( getattr(object, attribute) ) is list:
                    sys.stdout.write(' ')
                    for item in getattr(object, attribute):
                        sys.stdout.write( str(item) + ' ')
                    print ""
                else:
                    print str(getattr(object, attribute))




def calc_position( TI, BI, BO, TO, x_coeff, y_coeff):
    sum   = TI + BI + BO + TO
    x_num = TO + BO - BI - TI
    y_num = TO - BO - BI + TI
    x_pos = x_coeff * (x_num/sum)
    y_pos = y_coeff * (y_num/sum)

    return (x_pos, y_pos)
