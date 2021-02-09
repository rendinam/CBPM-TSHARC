/*!-----------------------------------------------------------------------+
* File         :  cbpm_find_delay_plot_peak                               |
*                                                                         |
* Description  :  Plots the data from the fine peak scan from find_delays |
*                 Assumes gnuplot is available in path.                   |
*                                                                         |
*                                                                         |
* Arguments    :  datfile - Pointer to string holding datafile name       |
*                 col_start - First column of data (start numbering at 1) |
*                 col_end - Last column of data                           |
*                 plot_title - String holding the desired plot title      |
*                 titles - Array of strings holding titles for each data  |
*                  series to plot.  (FIXME: add some error checking here) |
*                 plot_fit - is there fit to plot or just the peaks?      |
*                 fcoeffs - fitter parameters                             |
*                 offsets - new channel delays                            |
*                                                                         |
* Author       :  M. Stedinger                                            |
*                                                                         |
*-------------------------------------------------------------------------+*/
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>

#include "cbpm_includes.h"
#include "cbi_gp_plotting.h"

#define EXISTS(x) (!((x) - (x)))

int cbpm_find_delay_plot_peak( char *full_fname, int col_start, int col_end, PLOT_PARAMS pparams, char *plot_title, char titles[][CBI_MAX_STRING_LENGTH], int plot_fit, float fcoeffs[CBPM_MAX_FIT_COEFFS], int offsets[CBPM_MAX_ACQ_CONTROLLERS]) {

  int  i=0;
  int  col;
  FILE *plotscript;  
  char temp_name[CBI_MAX_STRING_LENGTH];
  char command[100];
  int card, addr;
  
  // Write gnuplot command file
  if ( CTL_System.RunningMode == CBI_BACKEND_MODE) {
    sprintf( temp_name, "plot_fit.gp-%d", CTL_System.instance_ID );
    plotscript = fopen(temp_name, "w");
  } else {
    sprintf( temp_name, "plot_fit.gp");
    plotscript = fopen(temp_name, "w");
  }
  
  // Compose plot appearance commands
  fprintf(plotscript, "set xtics 5\n");
  fprintf(plotscript, "set mxtics 5\n");
  fprintf(plotscript, "### set grid\n");
  fprintf(plotscript, "set style data points\n");
  fprintf(plotscript, "\n");

  // Compose actual plot command
  for (card = 0; card< CBPM_MAX_ACQ_CONTROLLERS; card++) {
    addr = card;
    if ((offsets[addr] < 0) || !EXISTS( fcoeffs[(addr*3)]) ||  !EXISTS( fcoeffs[(addr*3 +1)]) || !EXISTS( fcoeffs[(addr*3 +2)]) )
      continue; 
    
    fprintf(plotscript, "\n# Card %i \n\n",addr);
    
    if (plot_fit) {
      fprintf(plotscript, "## For plotting the instrument code fits\n");
      fprintf(plotscript, "F%i(x) = a%i*(x-%i)**2 + b%i*(x-%i) + c%i\n", addr, addr, offsets[addr], addr, offsets[addr], addr);
      fprintf(plotscript, "\n");
      
      fprintf(plotscript, "a%i = %f\n", addr, fcoeffs[(addr*3)    ]);
      fprintf(plotscript, "b%i = %f\n", addr, fcoeffs[(addr*3) + 1]);
      fprintf(plotscript, "c%i = %f\n", addr, fcoeffs[(addr*3) + 2]);
      fprintf(plotscript, "\n");
    }
    
    fprintf(plotscript, "## For GNUPLOT fitting\n");
    fprintf(plotscript, "fg%i(x) = ag%i*x**2 + bg%i*x + cg%i\n", addr, addr, addr, addr);
    fprintf(plotscript, "\n");
    fprintf(plotscript, "fit fg%i(x) \"%s\" using 1:%i via ag%i, bg%i, cg%i\n", addr, full_fname, 2+2*addr, addr, addr, addr);
    fprintf(plotscript, "\n");
    fprintf(plotscript, "set pointsize 2\n");
    fprintf(plotscript, "set term x11 %i\n",addr); 
    fprintf(plotscript, "set title \"%s\"  \n", plot_title);
    fprintf(plotscript, "set xlabel \"Channel delay\"\n"); 
    fprintf(plotscript, "set ylabel \"ADC value\"\n"); 
    fprintf(plotscript, "plot '%s' ", full_fname);
    fprintf(plotscript, "using 1:%i:%i with yerrorbars title \"%s : 128-turn avg\" lt 1,", 2+addr*2, 2+addr*2+1, titles[addr]);
    
    if (plot_fit) {
      fprintf(plotscript, "F%i(x) title \"Fit\",", addr);
    }
    
    fprintf(plotscript, "fg%i(x) title \"NLLS-LM\"\n", addr);
    fprintf(plotscript, "\n");
  }
  
  fprintf(plotscript, "pause -1\n");
  
  fclose(plotscript);
  

  // Generate the plot
  sprintf(command," xterm -e gnuplot %s &", temp_name);// Allows multiple plots at the same time.
  system( command );                                   // Creates two windows 1 term/1 graphic for each, though.
  
  
  return F_SUCCESS;
  
}
