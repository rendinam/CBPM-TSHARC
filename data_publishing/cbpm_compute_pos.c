//-----------------------------------------------------------------------+
// File         :  cbpm_compute_pos.c                                    |
//                                                                       |
// Description  :  Computes either a horizontal or vertical position     |
//                 from four BPM button values and the provided          |
//                 geometric detector coefficients.                      |
//                                                                       |
// Arguments    :  iidx  - instrument index                              |
//                 plane - flag determining the dimension for which a    |
//                         position shall be calculated.                 |
//                 TI, BI, BO, TO - Floats corresponding to the four     |
//                         BPM button values                             |        
//                                                                       |
// Return Value :  Status - F_SUCCESS/F_FAILURE                          |
//                                                                       |
// Author       :  M. Rendina                                            |
//-----------------------------------------------------------------------+

#include "cbpm_includes.h"


float cbpm_compute_pos( int iidx, int plane, float TI, float BI, float BO, float TO ) {

  CBPM_CONTROL *cp;
  cp = CTL_System.p_Module[iidx]->control;

  float Xcoeff, Ycoeff, Xnum, Ynum, Xpos, Ypos, sum;
  
  Xcoeff = cp->detector.horz_coeff;
  Ycoeff = cp->detector.vert_coeff;

  sum  = TI + BI + BO + TO;
  Xnum = TO + BO - BI - TI;
  Ynum = TO - BO - BI + TI;

  if (plane == CBPM_HORZ_DIM) {
    Xpos = Xcoeff * (Xnum/sum);
    return Xpos;
  } else {
    Ypos = Ycoeff * (Ynum/sum);
    return Ypos;
  }


}
