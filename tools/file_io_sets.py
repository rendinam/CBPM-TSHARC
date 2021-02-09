#-----------------------------------------------
# file_io_sets.py
#
# Defines dictionaries used to hold
# various header and reader/writer method
# sets for each supported file format to
# support, in a very modular fasion, requests
# within external code to read and write files
# of different versions.
#
# The files that contain file version-specific
# machinery must contain the line
#
#   from file_io_sets import *
#
# before they set about populating the
# dictionaries below.
#-----------------------------------------------

header_field_sets        = {}
cesr_field_sets          = {}
inst_field_sets          = {}

datablock_read_methods   = {}

custom_field_reader_sets = {}
custom_field_writer_sets = {}


