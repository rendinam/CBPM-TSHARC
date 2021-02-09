//-----------------------------------------------------------------------
// File         :  dsp_init.c
//
// Description  :  - Initialize the DSP TS101 chip 
//                 - Perform other variable and data structure 
//                   initializations.  
//                 - Obtains the hostname and IP address published by the
//                   Coldfire communications module during its boot 
//                   sequence.
//                 - Global Interrupt Enable (GIE) bit setting for
//                   interrupt handling.
//                 - Hardware flag pin setup and enabling for diagnostics
//
// Contributors :  M. Rendina
//-----------------------------------------------------------------------

#define  INST_NO_EXTERN

// Instrumentation Core
#include "cbi_core_inst_includes.h"

// Platform-specific
#include "cbpm_inst_includes.h"


void dsp_init( void )
{
	
   //--------------------------------
   // Initialize DEBUG struct
   //--------------------------------
   clear_debug();	
	
   //--------------------------------------------------------------------
   // Set up the SYSCON register 
   // Configure 1 wait state for MS0 and MS1, 3 wait states for MSH 
   // Configure slow protocol with idle cycles for all segments. 
   //  [ <OR> of all the bits yields 0x000278E3 ]
   //--------------------------------------------------------------------
   volatile int *syscon_ptr    = (int *)SYSCON_ADR;
   *syscon_ptr =
   SYSCON_MSH_SLOW | SYSCON_MSH_PIPE1 | SYSCON_MSH_WT3 | SYSCON_MSH_IDLE |
   SYSCON_MS1_SLOW | SYSCON_MS1_PIPE1 | SYSCON_MS1_WT1 | SYSCON_MS1_IDLE |
   SYSCON_MS0_SLOW | SYSCON_MS0_PIPE1 | SYSCON_MS0_WT1 | SYSCON_MS0_IDLE | SYSCON_MSH_WT2;
	

   // Add function ID to the trace
   int FID = INIT_ID;
   INST_stat.trace[INST_stat.num_levels++] = FID;
      
   int   i, j, k, card, tblock;
   float angle;
   
   
   int rtemp;
   //------------------------------------------
   // Enable handling of all interrupts by
   // setting Global Interrupt Enable (GIE)
   //------------------------------------------
   rtemp = __builtin_sysreg_read( __IMASKH );
   rtemp = rtemp | INT_GIE;
   __builtin_sysreg_write( __IMASKH, rtemp );
   
   
   turncount = 0;
   
 
   // Initialize Time-of-day counter
   *p_tod_seconds = 0;
   
   
   //-----------------------------------------
   // Create pointers to important registers 
   // that are only used in init.
   //-----------------------------------------
   
   // The clock control register.
   volatile int *clock_control = (int *)ADC_CLOCK_CONTROL;

   // Turns marker delay to ensure ADC setup and hold times.
   //  (See digital board documentation for details)
   volatile int *turn_mark_delay = (int *)TURN_MARK_DELAY;
   
   
   // Check the floating point error bits and clear as necessary
   if (float_err_check() == CBI_F_FAILURE) {
      float_err_clear();
   }
      

   //Clear any errors
   volatile int *p_error                    = INST_cmd.error;   
   cbpm_set_error(CBI_NO_ERR,p_error);
   
   //--------------------------------------------------------------
   // Set the recommended value of the ADC_CLOCK_CONTROL register
   // on the digital board to set up relative set-up and hold 
   // times for data clocking purposes.
   //--------------------------------------------------------------
   *clock_control      = 0x0F;
   *p_ts_adc_mem_first = 0;  // First address ADC buffers will start at when filling.
   memfirsts(0);             // Set first address for acquisitions on all front-end cards.
   *turn_mark_delay    = 0;  // INST_find_delay_work.turns_mark_del;
   
   
   //==== REGISTER INITIALIZATIONS =================================
   
   //-----------------------------------
   // Put ADCs in non-acquiring state
   //-----------------------------------
   for (i = 0; i < CBPM_MAX_CARDS; i++) {
      *p_ts_data_acq = 0;
   }

      
   //--------------------------------------------
   // Initialize gain mode and value
   // Starts up in lowest VARIABLE GAIN setting.  
   // Setting 1.
   //--------------------------------------------
   for (card = 0; card < CBPM_MAX_CARDS; card++ ) {
     for (tblock = 0; tblock < CBPM_MAX_TIMING_BLOCKS; tblock++) {
       *p_gain_regs[card][tblock] = CBPM_DEFAULT_GAIN_CODE;
       *p_gain_regs[card][tblock] = CBPM_DEFAULT_GAIN_CODE;
     }
   }


   //---------------------------------------------------------------
   // Call automatically-generated code to initialize packet tables
   //---------------------------------------------------------------
   init_core_packet_tables();
   init_packet_tables();


   //---------------------------------------------
   // For diagnostics
   // Set up DSP hardware flag pin 0 for output.
   //--------------------------------------------
   // FLAG0 output enable is bit 20 in SQCTL
   //---------------------------------------------
   int f0_en_bitmask  = MAKE_BITMASK_(SQCTL_FLAG0_EN_P);
   int f0_out_bitmask = MAKE_BITMASK_(SQCTL_FLAG0_OUT_P);
   
   int temp = 0;
   // Step 1) Enable flag pin for output.
   temp = __builtin_sysreg_read( __SQCTL );
   temp = (temp | f0_en_bitmask);
   __builtin_sysreg_write( __SQCTL, temp );
   
   // Step 2) Set flag pin output LOW
   temp = __builtin_sysreg_read( __SQCTL );
   temp = (temp & (~f0_out_bitmask) );
   __builtin_sysreg_write( __SQCTL, temp );
   
   
   
   //=======================================================================+
   // Communication Data Structure Initializations                          |
   //=======================================================================+
   
   
   //--------------------------------------------------------------
   // Populate MODULE_CONFIG
   //--------------------------------------------------------------
   //INST_module_config.exe_type        = INST_CODE_TYPE; // placeholder
   //INST_module_config.exe_version     = 99;             // placeholder
   
   
   // Publish intrument's EXE (.ldr) name string
   ////int *destptr  = (int *)&INST_module_config.ldr_name;
   char *destptr  = (char *)&INST_module_config.ldr_name;
   char *ldrname = LDR_NAME;   
   for (i = 0; i < 40; i++) {
   	  *(destptr + i) = *(ldrname + i);
   }
   
   // Load the core communication structure set revision mumber
   INST_module_config.core_comm_struct_rev     = CBI_COMM_STRUCT_REV;
   
   // Load the platform-specific communication structure set revision number
   INST_module_config.platform_comm_struct_rev = CBPM_COMM_STRUCT_REV;
   
   INST_module_config.build_timestamp          = BUILD_TIMESTAMP;
   INST_module_config.compiler_ver             = COMPILER_VERSION;
   
   
   
   //---------------------------------------------------------------------
   // Populate the platform-specific identification structure (IDENT)
   //---------------------------------------------------------------------
   // Gather IP address information from the SRAM mailbox filled by
   // Coldfire module upon booting.
   //
   // The ColdFire takes much longer to boot than the DSP.  
   // Thus, the DSP will need to repeatedly check these locations
   // for changes from 0 to determine when to proceed with initialization.
   for (i = 0; i < 4; i++) {   // Init all IP address drop locations to 0.
      INST_ident.ipaddr[i] = 0;   
      *(volatile unsigned int *)(CF_IP_ADDR + i) = 0;
   }                             
   
                          
                                 
   // Reset the Coldfire module to refresh the IP address and hostname areas of SRAM
   *(volatile unsigned int *)(DIMM_RESET) = 0;
   *(volatile unsigned int *)(DIMM_RESET) = 1;
   
                             
   // Repeatedly check the last byte of the IP address to make sure the
   // CBInet server has written ALL the bytes of IP address string.
   j=0;
   if ( *sram_id_ptr_last == 0 ) {
     while ( *sram_id_ptr_last == 0 ) {
   	   j++;
     }
   }

   
   int packed;
   int ch = 0;
   char tch;
   // Now, retrieve the IP address values from CBInet server drop locations.
   // Reverse the order of the bytes in each word to produce a proper
   // character string.
   for (i = 0; i < 4; i++ ) {
   	  packed = *(sram_id_ptr + i);
   	  tch = (char)unpack(packed,3);
      INST_ident.ipaddr[ch++] = tch;
   	  tch = (char)unpack(packed,2);
      INST_ident.ipaddr[ch++] = tch;
   	  tch = (char)unpack(packed,1);
      INST_ident.ipaddr[ch++] = tch;
   	  tch = (char)unpack(packed,0);
      INST_ident.ipaddr[ch++] = tch;
   }
   //INST_ident.ipaddr[ch] = '\0';

  
   
   // Retrieve the hostname string from the CBInet server drop locations,
   // again reversing the order of bytes to account for the way they were
   // stored by the CBInet server.
   ch = 0;
   for (i = 0; i < 3; i++ ) {
      packed = *(sram_hostname + i);
   	  tch = (char)unpack(packed,3);
      INST_ident.hostname[ch++] = tch;
      tch = (char)unpack(packed,2);
      INST_ident.hostname[ch++] = tch;
      tch = (char)unpack(packed,1);
      INST_ident.hostname[ch++] = tch;
      tch = (char)unpack(packed,0);
      INST_ident.hostname[ch++] = tch;
   }
   
   //Publish the hardware type and central (digital board) FPGA version numbers
   INST_ident.module_type = *(volatile unsigned int *)(MODULE_TYPE);
   INST_ident.fpga_maj    = *(volatile unsigned int *)(MAJOR_REV);
   INST_ident.fpga_min    = *(volatile unsigned int *)(MINOR_REV);
   
   
   //Publish each card's FPGA ID value
   for (i = 0; i < CBPM_MAX_CARDS; i++ ) {
     INST_ident.fe_fpga_id[i] = sign_extend( *(volatile unsigned int *)(ACQ_CONTROL_ID_0 + i*(CTC_OFFSET)) );
   }
   
   
   
   //-----------------------------------------------
   // Communications structure initializations
   //-----------------------------------------------
   
   // Debug is initialized at beginning of this function, in case you want to USE add_debug before this point in the function
         
   //--------------------------------
   // Initialize CMD_PARAMS struct
   //--------------------------------
   INST_cmd_params.delay_cal  = CBI_FALSE;
   INST_cmd_params.gain_cal   = CBI_FALSE;
   

   //--------------------------------
   // Initialize DIAGNOSTIC struct
   //--------------------------------
   
     
   
   //-----------------------------------
   // Initialize raw_data_header struct
   //-----------------------------------
   for (card = 0; card < CBPM_MAX_CARDS; card++) {
      INST_raw_data_header.ADC_zero_val_bitfields[card]   = 0;
      INST_raw_data_header.ADC_lower_thresh_bitfields[card]   = 0;
      INST_raw_data_header.ADC_low_thresh_bitfields[card]   = 0;
      INST_raw_data_header.ADC_high_thresh_bitfields[card]  = 0;
      INST_raw_data_header.ADC_saturation_bitfields[card]   = 0;
   }
   
   //-----------------------------------
   // Initialize PROC_BUF_HEADER struct
   //-----------------------------------   
   for (i = 0; i < CBPM_MAX_PROC_BUFS; i++) {
      INST_proc_buf_header.write_ptr[i] = 0;
   }   
   INST_proc_buf_header.active_buf   = 0;

   
   
   //--------------------------------------------------------------
   // Initialize the sin and cos tables for betatron measurements
   //  (Uses center value of each bin)
   //--------------------------------------------------------------
   for (i = 0; i < CBPM_SIN_TABLE_SIZE; i++) {
      angle = ((2*i) + 1) * (2*PI_) / (2*CBPM_SIN_TABLE_SIZE);
      sin_table[i] = sinf(angle);
      cos_table[i] = cosf(angle);
   }
   

  //=======================================================================+
  // Other variable initializations                                        |
  //=======================================================================+

  // Global
  
  // Bunch pattern offsets
  bun_pat_offset[0] = 0;
  bun_pat_offset[1] = 0;
  
  
  
  // Decrement the trace level on exit
  INST_stat.num_levels--;
  return;
  
}



