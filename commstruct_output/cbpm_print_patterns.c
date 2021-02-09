///-----------------------------------------------------------------------+
/// File         :  cbpm_print_patterns.c                                 |
///                                                                       |
/// Description  :  Prints side-by-side pre-rotation and post-rotation    |
///                 bit patterns for visual comparison.                   |
///                                                                       |
///                                                                       |
/// Arguments    :  Pointer to BP word (bit patterns) array               |
///                 Pointer to rotated bunch pattern (ints) array         | 
///                                                                       |
/// Author       :  M. Rendina                                            |
///-----------------------------------------------------------------------+

#include "cbpm_includes.h"


int cbpm_print_patterns( int BP[], int RBP[], FILE *fp ) {

  int i, j, reg, bit;

  fprintf(fp, "  Pre-Rotation        Post-Rotation \n");
  fprintf(fp, "---------------------------------------\n");
  fprintf(fp, "0  |   |   |   |     0  |   |   |   |\n");

  for( reg = 0; reg < 40; reg++ ) {

    for( bit = 0; bit < 16; bit++ ) { // Print raw bunch pattern
      if( cbi_bitval(BP[reg], bit) == 1 ) {
	fprintf(fp, "X");
      } else {
	fprintf(fp, ".");
      }

      if( bit == 15 ) {  // Print rotated bunch pattern
	fprintf(fp, "     ");
	for(j=0; j < 16; j++) {
	  if( RBP[(reg*16)+j] == 1) {
	    fprintf(fp, "R");
	  } else {
	    fprintf(fp, ".");
	  }
	}
	fprintf(fp, "\n");
      }
    }
  }

  return F_SUCCESS;
}

