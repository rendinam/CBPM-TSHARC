
#------------------------------------------------
# Container classes for header values and data
#------------------------------------------------
class file_data:
    """Holds all header and measurement data from a single file"""
    def __init__(self):
        self.insts = []
        self.read_time_s = 0.0


class file_header:
    """Holds file header fields"""
    def __init__(self):
        self.instrument_type   = ""
        self.command_id        = ""
        self.file_type         = ""
        self.file_id           = 0
        self.file_version      = 0
        self.timestamp         = ""
        self.core_comm_v       = 0
        self.plat_comm_v       = 0
        self.bunch_pat_name    = ""
        self.bunch_pattern_hex = []
        self.bunch_pattern     = []
        self.species           = ""
        self.num_instruments   = 0
        self.num_bunches       = 0
        self.num_turns         = 0
        self.averaging_mode    = ""
        self.data_correction   = ""
        self.turn_spacing      = 0
        self.timing_setup      = ""
        self.trigger           = ""


class cesr_header:
    """Holds CESR machine header fields"""
    def __init__(self):
        self.condx       = 0
        self.CERN_raw    = 0
        self.CERN_scaled = 0.0


class inst_header:
    """Holds instrument header fields"""
    def __init__(self):
        self.location           = ""
        self.hostname           = ""
        self.IP_address         = ""
        self.det_type           = ""
        self.det_coeffs         = []
        self.exe_name           = ""
        self.build_id           = 0
        self.dig_board_fpga     = 0.0
        self.front_end_fpgas    = []
        self.timing_setup       = ""
        self.num_turns          = 0
        self.averaging          = ""
        self.data_correction    = ""
        self.turn_sync_counter  = -1
        self.turn_spacing       = -1
        self.trigger            = ""
        self.bunch_pat_offsets  = [0,0]
        self.com_tmark_delay    = 0
        self.block_tmark_delays = [-1,-1]
        self.block_delays       = [0,0]
        self.channel_delays     = [[-1,-1,-1,-1],[-1,-1,-1,-1]]
        self.gain_settings      = [[-1,-1,-1,-1],[-1,-1,-1,-1]]
        self.gain_codes         = [[-1,-1,-1,-1],[-1,-1,-1,-1]]
        self.gain_coeffs        = [[-1,-1,-1,-1],[-1,-1,-1,-1]]
        self.pedestals          = [[-1,-1,-1,-1],[-1,-1,-1,-1]]
        self.digital_temp_c     = 0.0
        self.timing_temp_c      = 0.0
        self.card_temps_c       = [0,0,0,0]
        self.adc_saturation     = [[-1,-1,-1,-1],[-1,-1,-1,-1]]
        self.adc_high           = [[-1,-1,-1,-1],[-1,-1,-1,-1]]
        self.adc_low            = [[-1,-1,-1,-1],[-1,-1,-1,-1]]


class inst_data:
    """Holds header and data objects from a single instrument."""
    def __init__(self):
        self.bunches      = []


class bunch_data:
    """Holds array of bunch data objects for a single instrument."""
    def __init__(self):
        self.tblock      = -1
        #self.buttons     = []
        #self.corrected   = []
        #self.x_pos       = []
        #self.y_pos       = []

    


#------------------------------------------------
# Global array for holding all measurement data
# from multiple measurements, files, instruments,
# etc.
#------------------------------------------------
data_parcels = []


blocknames = { 0:'A', 1:'B' }
