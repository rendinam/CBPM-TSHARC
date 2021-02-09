/*!
*-------------------------------------------------------------------------+
* File         :  cbpm_EEPROM_write.c                                     |
*                                                                         |
* Description  :  Writes a single 8-bit value to a slot in the EEPROM     |
*                 on a single FPGA-equipped BPM front end board.          |
*                                                                         |
*                 NOTE: The EEPROM is 1 kbit (256 bytes) on each card.    |
*                 See BPM front-end board FPGA documentation for details  |
*                 of the write sequence.                                  |
*                                                                         |
* Arguments    :  int  card    - Which card to write to                   |
*                 int  address - 0-512 (address within EEPROM)            |
*                 int  value   - 8-bit value to write                     |
*                                                                         |
* Author       :  M. Rendina                                              |
*-------------------------------------------------------------------------+*/

#include "cbpm_includes.h"


#define EEREADDAT_OFFSET           55
#define EEWRITEDAT_OFFSET          56

#define EEPROM_ADDR_CTL_OFFSET     54
#define EEPROM_STROBE_MASK         0x800
#define EEPROM_DONE_MASK           0x400
#define EEPROM_DONEBIT_SHIFT       10


int cbpm_EEPROM_write( int BPM_IDX, int card, int address, int value ) {
  
  int card_offset[CBPM_MAX_CARDS] = { 0x9000000, 0x9100000, 0x9200000, 0x9300000 };

  // Bit 9 is high to command a WRITE, bit 11 is high to set up STROBE
  int control_reg = 0xA00;  

  // Address definitions that change per card
  int EEWRITEDAT  = card_offset[card] + EEWRITEDAT_OFFSET;
  int EE_ADDR_CTL = card_offset[card] + EEPROM_ADDR_CTL_OFFSET;

  int writestat = 0;  

  // Write data to EEPROM's incoming data register
  //printf("write_to_EEPROM: Control reg write: 0x%x\n", control_reg);
  cbi_xbus_poke( BPM_IDX, EEWRITEDAT, value );

  // Bounds check on EEPROM address
  //printf("write_to_EEPROM: checking address bounds...\n");
  if ( (address > 511) || (address < 0) ) {
    printf("Address out of bounds for remote write to EEPROM!\n");
    return F_FAILURE;
  }

  control_reg = control_reg + address;


  // Write the destination slot and the control bits to the EEPROM control register
  // to start the write
  //printf("write_to_EEPROM: sending control register value...\n");
  cbi_xbus_poke( BPM_IDX, EE_ADDR_CTL, control_reg );

  // Wait on the done bit to signal completion of the write

  int val;
  int status = cbi_xbus_peek( BPM_IDX, EE_ADDR_CTL, &val );
  writestat = (EEPROM_DONE_MASK & val) >> EEPROM_DONEBIT_SHIFT;
  //writestat = cbi_xbus_peek( BPM_IDX, EE_ADDR_CTL );
  //printf("write_to_EEPROM: writestat = %d\n", writestat);
  while ( writestat != 1 ) {
    sleep(0.25);
    status = cbi_xbus_peek( BPM_IDX, EE_ADDR_CTL, &val );
    writestat = (EEPROM_DONE_MASK & val) >> EEPROM_DONEBIT_SHIFT;
    //printf("write_to_EEPROM: writestat = %d\n", writestat);
  }
  
  // Set the strobe low to reset for next operation 
  // (actually set the whole control register to 0 for now)
  cbi_xbus_poke( BPM_IDX, EE_ADDR_CTL, 0 );
  //value = cbi_xbus_peek( BPM_IDX, EE_ADDR_CTL );
  //printf("write_to_EEPROM: ctl reg value = 0x%x\n", value);

  return F_SUCCESS;

}
