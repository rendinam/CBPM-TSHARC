#ifndef CBPM_REGISTERS_H
#define CBPM_REGISTERS_H

#include "cbi_constants.h"
#include "cbi_types_a.h"

#include "cbpm_types_a.h"

//-----------------------------------------------------------------------+
// File         :  cbpm_registers.h      [TigerSHARC DSP-based BPM]      |
//                                                                       |
// Description  :  This header file defines the name to address mappping |
//                 of registers and memory locations for BPM hardware.   |
//                                                                       |
// Author(s)    :  M. Rendina / M. Palmer / C. Strohman                  |
//-----------------------------------------------------------------------+

//-----------------------------------------------------------------------+
//  The following provides for C++ compatibility (ie, C++ routines can   |
//  explicitly use this include file).                                   |
//-----------------------------------------------------------------------+
#if defined (__cplusplus)
extern "C" {
#endif


//-----------------------------------------------------------------------+
// Indexing aids
//   REG_LOW -  The low 16-bits of a split 32-bit register (as seen on
//              various front-end FPGA register map specifications.)
//   REG_HIGH - The high 16-bits of a split 32-bit register.
//-----------------------------------------------------------------------+
#define REG_SPLIT_COUNT        2
#define REG_LOW                0
#define REG_HIGH               1

//-----------------------------------------------------------------------+
// Various acquisition control bitmasks
//-----------------------------------------------------------------------+
#define ACQ_SKIP_TURN_BIT      0x01
#define ACQ_CONT_BIT           0x02
#define ACQ_DONE_BIT           0x04
#define ACQ_ENAB_BIT           0x08
#define ACQ_MODE_BIT           0x01


//-----------------------------------------------------------------------+
// Digital board, DSP, and other system registers,                       
// addresses and constants.                                              
//-----------------------------------------------------------------------+

// Hardware addresses in DSP address space
#define SYSCON_ADR             0x180480


//-----------------------------------------------------------------------+
// DSP reset register and bitmask
//-----------------------------------------------------------------------+
#define DSP_RESET              0x10040001


//-----------------------------------------------------------------------+
// DIMM reset register for resetting Coldfire communications card on
// the Communications board
//-----------------------------------------------------------------------+
#define DIMM_RESET             0x10040008


//-----------------------------------------------------------------------+
// This is the memory offset between the register spaces of
// individual front-end cards.   "Card-to-card offset"
//-----------------------------------------------------------------------+
#define CTC_OFFSET             0x00100000        


//-----------------------------------------------------------------------+
// Turns counter and turns data registers
//-----------------------------------------------------------------------+
#define GLOBAL_TURN_CNT        0x10040004   // 20-bit turn counter  //Move to hardware header
#define GLOBAL_TURN_DAT        0x10040005   // Global turn data from CESR Timing signal //Move

#define TOD_SECONDS            0x10040006   // 17-bit TimeOfDay counter for time stamps //Move
#define ADC_MEM_FIRST          0x10040016   // Move


//-----------------------------------------------------------------------+
// Digital board acquisition control register
//-----------------------------------------------------------------------+
#define DATA_ACQ               0x10040002


//-----------------------------------------------------------------------+
// Front-end card primary acquisition control registers
//-----------------------------------------------------------------------+
#define ACQ_CONTROL_0          0x09000000
#define ACQ_CONTROL_1          0x09100000
#define ACQ_CONTROL_2          0x09200000
#define ACQ_CONTROL_3          0x09300000


//-----------------------------------------------------------------------+
// Acquisition controller ID value registers
//-----------------------------------------------------------------------+
#define ACQ_CONTROL_ID_0       0x09000002


//-----------------------------------------------------------------------+
// Other acquisition control registers
//-----------------------------------------------------------------------+
#define ACQ_SKIP_TURNS_0       0x09000039
#define ACQ_SKIP_TURNS_1       0x09100039
#define ACQ_SKIP_TURNS_2       0x09200039
#define ACQ_SKIP_TURNS_3       0x09300039


//-----------------------------------------------------------------------+
// Address in which to load the number of turns to be acquired
//-----------------------------------------------------------------------+
#define ACQ_TURN_REQ_0_L       0x09000007
#define ACQ_TURN_REQ_0_H       0x09000008

#define ACQ_TURN_REQ_1_L       0x09100007
#define ACQ_TURN_REQ_1_H       0x09100008

#define ACQ_TURN_REQ_2_L       0x09200007
#define ACQ_TURN_REQ_2_H       0x09200008

#define ACQ_TURN_REQ_3_L       0x09300007
#define ACQ_TURN_REQ_3_H       0x09300008


//-----------------------------------------------------------------------+
// Front-end registers that hold the number of completed turns during an
// acquisition.
//-----------------------------------------------------------------------+
#define ACQ_TURNS_COUNT_0_L    0x09000009
#define ACQ_TURNS_COUNT_0_H    0x0900000A

#define ACQ_TURNS_COUNT_1_L    0x09100009
#define ACQ_TURNS_COUNT_1_H    0x0910000A

#define ACQ_TURNS_COUNT_2_L    0x09200009
#define ACQ_TURNS_COUNT_2_H    0x0920000A

#define ACQ_TURNS_COUNT_3_L    0x09300009
#define ACQ_TURNS_COUNT_3_H    0x0930000A


//-----------------------------------------------------------------------+
// Front-end registers that hold the initial address in ADC buffer space
// to use for the next acquisition.
//-----------------------------------------------------------------------+
#define ACQ_MEM_FIRST_0_L      0x09000005
#define ACQ_MEM_FIRST_0_H      0x09000006

#define ACQ_MEM_FIRST_1_L      0x09100005
#define ACQ_MEM_FIRST_1_H      0x09100006

#define ACQ_MEM_FIRST_2_L      0x09200005
#define ACQ_MEM_FIRST_2_H      0x09200006

#define ACQ_MEM_FIRST_3_L      0x09300005
#define ACQ_MEM_FIRST_3_H      0x09300006


//-----------------------------------------------------------------------+
// Front-end Card Bunch Pattern Registers.  One base address for each    |
//  acquisition controller.                                              |
//-----------------------------------------------------------------------+
#define ACQ_BUNCH_PAT_0        0x0900000E
#define ACQ_BUNCH_PAT_1        0x0910000E
#define ACQ_BUNCH_PAT_2        0x0920000E
#define ACQ_BUNCH_PAT_3        0x0930000E


//-----------------------------------------------------------------------+
// Front-end card ADC buffer ('SRAM') control and data registers by      |
//   Card, Timing Block, and Channel                                     |
//-----------------------------------------------------------------------+
#define ACQ_SRAM_CTRL_0        0x0900003B
#define ACQ_SRAM_DATA_0        0x09001000

#define ACQ_SRAM_CTRL_1        0x0910003B
#define ACQ_SRAM_DATA_1        0x09101000

#define ACQ_SRAM_CTRL_2        0x0920003B
#define ACQ_SRAM_DATA_2        0x09201000

#define ACQ_SRAM_CTRL_3        0x0930003B
#define ACQ_SRAM_DATA_3        0x09301000


//-----------------------------------------------------------------------+
// Front-end Card Buffers by Card, Timing Block, and Channel             |
//-----------------------------------------------------------------------+
#define BUF_C0_A_0             0x08000000
#define BUF_C0_B_0             0x08080000

#define BUF_C1_A_0             0x08100000
#define BUF_C1_B_0             0x08180000

#define BUF_C2_A_0             0x08200000
#define BUF_C2_B_0             0x08280000

#define BUF_C3_A_0             0x08300000
#define BUF_C3_B_0             0x08380000


//-----------------------------------------------------------------------+
// Timing Board control registers:                                       |
//     10-bit                                                            |
//     write-only                                                        |
//     10 ps/bit                                                         |
//-----------------------------------------------------------------------+
#define ADC_CLOCK_CONTROL      0x1004001A
#define TURN_MARK_DELAY        0x10040019

#define TIM_REG_BASE_ADR       0x10020000    // Base address of timing register chunk

#define A_FREQUENCY            0x10020011
#define A_PULSEWIDTH           0x10020012
#define B_FREQUENCY            0x10020019
#define B_PULSEWIDTH           0x1002001A
  
#define COMMON_TMD             0x1002001F
#define BLOCK_TMD_A            0x10020010
#define BLOCK_TMD_B            0x10020018

#define CBPM_GDEL_A0           0x10020001 // Block-A global delay 0
#define CBPM_GDEL_A1           0x10020000 // Block-A global delay 1
#define CBPM_GDEL_B0           0x10020009 // Block-B global delay 0
#define CBPM_GDEL_B1           0x10020008 // Block-B global delay 1

#define CBPM_CDEL_C0_A         0x10020005
#define CBPM_CDEL_C0_B         0x1002000D

#define CBPM_CDEL_C1_A         0x10020004
#define CBPM_CDEL_C1_B         0x1002000C

#define CBPM_CDEL_C2_A         0x10020003
#define CBPM_CDEL_C2_B         0x1002000B

#define CBPM_CDEL_C3_A         0x10020002
#define CBPM_CDEL_C3_B         0x1002000A


//-----------------------------------------------------------------------+
// Gain control registers by Card and Timing Block                       |
//-----------------------------------------------------------------------+
#define GAIN_C0_A              0x09000003
#define GAIN_C0_B              0x09000004

#define GAIN_C1_A              0x09100003
#define GAIN_C1_B              0x09100004

#define GAIN_C2_A              0x09200003
#define GAIN_C2_B              0x09200004

#define GAIN_C3_A              0x09300003
#define GAIN_C3_B              0x09300004


//-----------------------------------------------------------------------+
// Gain control
//-----------------------------------------------------------------------+
#define MIN_GAIN               0
#define MAX_GAIN               256


#define ACQ_DEBUG_IN_0         0x0900003E
#define ACQ_DEBUG_IN_1         0x0910003E
#define ACQ_DEBUG_IN_2         0x0920003E
#define ACQ_DEBUG_IN_3         0x0930003E


//-----------------------------------------------------------------------+
// Digital Board FPGA ID Registers                                       |
//-----------------------------------------------------------------------+
#define MODULE_TYPE            0x10040027  // 1=BSM, 2=BPM, 3=FLM, 4=FLMA, 5=ERL_BPM
#define MAJOR_REV              0x10040028  //  (See digital board reference document
#define MINOR_REV              0x10040029  //   for value assignments.)


//-----------------------------------------------------------------------+
// Temperature sensor registers
//-----------------------------------------------------------------------+
#define TEMPERATURE_CSR        0x10040020  // For digital board temp refresh.

#define TIMING_TEMP            0x10040025
#define DIGITAL_TEMP           0x10040026

#define ACQ_TEMPERATURE_0      0x0900000D
#define ACQ_TEMPERATURE_1      0x0910000D
#define ACQ_TEMPERATURE_2      0x0920000D
#define ACQ_TEMPERATURE_3      0x0930000D




//-----------------------------------------------------------------------+
// Other memory locations
//-----------------------------------------------------------------------+
#define SRAM_FIRST             0x0C080000
#define SRAM_LAST              0x0C0FFFFF // 0x80000 = 524288 words total

#define CF_IP_ADDR             0x0C0FF000 // NULL-terminated, 15-char string
                                          // (Begins 4095 words from end of SRAM)
#define CF_HOSTNAME            0x0C0FF010 // NULL-terminated string, (~8-chars)
                                          // (Begins 4079 words from end of SRAM)
                                          // 
                                          // 520193 words available before CBInet
                                          // server area.
                                          

// Memory Locations used for debugging DSP code                                                                                    
#define DEBUG_HEARTBEAT    0x0C0FF020
#define DEBUG_RUN          0x0C0FF021
#define DEBUG_STEP         0x0C0FF022
#define DEBUG_LOCATION     0x0C0FF023
#define DEBUG_BREAKPOINT         0x0C0FF024
#define DEBUG_INPUT              0x0C0FF025
#define DEBUG_OUTPUT             0x0C0FF026 
#define DEBUG_VALUE1             0x0C0FF027  
#define DEBUG_VALUE2             0x0C0FF028 
#define DEBUG_VALUE3             0x0C0FF029
#define DEBUG_VALUE4             0x0C0FF030 
#define DEBUG_VALUE5             0x0C0FF031 
#define DEBUG_STATUS             0x0C0FF032 


#if defined (__cplusplus)
}
#endif



#endif // CBPM_REGISTERS_H 

