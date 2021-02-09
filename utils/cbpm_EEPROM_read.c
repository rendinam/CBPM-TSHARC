/*!
*-------------------------------------------------------------------------+
* File         :  cbpm_EEPROM_read.c                                      |
*                                                                         |
* Description  :  Reads a single 8-bit value from a slot in the EEPROM    |
*                 on a single FPGA-equipped BPM front end board.          |
*                                                                         |
*                 NOTE: The EEPROM is 4 kbits (512 bytes) on each card.   |
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


int cbpm_EEPROM_read( int BPM_IDX, int card, int address ) {
  
  int card_offset[CBPM_MAX_CARDS] = { 0x9000000, 0x9100000, 0x9200000, 0x9300000 };

  // Bit 9 is LOW to command a WRITE, bit 11 is high to set up STROBE
  int control_reg = EEPROM_STROBE_MASK;  

  // Address definitions that change per card
  int EEREADDAT   = card_offset[card] + EEREADDAT_OFFSET;
  int EE_ADDR_CTL = card_offset[card] + EEPROM_ADDR_CTL_OFFSET;

  int writestat = 0; 
  int value     = 0;
  int status = CBI_F_SUCCESS;


  // Bounds check on EEPROM address
  if ( (address > 511) || (address < 0) ) {
    printf("Address out of bounds for remote write to EEPROM!\n");
    return 0xFFFFFFFF;
  }

  control_reg = control_reg + address;

  // Write the destination slot and the control bits to the EEPROM control register
  // to start the write
  //printf("read_from_EEPROM: Control reg write: 0x%x\n", control_reg);
  cbi_xbus_poke( BPM_IDX, EE_ADDR_CTL, control_reg );

  // Wait on the done bit to signal completion of the write
  int val;
  status = cbi_xbus_peek( BPM_IDX, EE_ADDR_CTL, &val );
  writestat = (EEPROM_DONE_MASK & val) >> EEPROM_DONEBIT_SHIFT;
  while ( writestat != 1 ) {
    sleep(0.1);
    status = cbi_xbus_peek( BPM_IDX, EE_ADDR_CTL, &val );
    writestat = (EEPROM_DONE_MASK & val) >> EEPROM_DONEBIT_SHIFT;
  }
  
  // Final step is to read back the data waiting in the EEREADDAT register
  status = cbi_xbus_peek( BPM_IDX, EE_ADDR_CTL, &value );
  //printf("read_from_EEPROM: ctl reg value = 0x%x\n", value);

  status = cbi_xbus_peek( BPM_IDX, EEREADDAT, &value );
  //printf("read_from_EEPROM:         value = %d\n", value);

  // Set the strobe low to reset for next operation 
  // (actually set the whole control register to 0 for now)
  status = cbi_xbus_poke( BPM_IDX, EE_ADDR_CTL, 0 );
  //value = xbus_peek( BPM_IDX, EE_ADDR_CTL );
  //printf("read_from_EEPROM: ctl reg value = 0x%x\n", value);

  //printf("value = 0x%x,   short value = 0x%x\n", value, (short)value); 
  return value;

}
