
#include "cbpm_includes.h"


void CoeffMatrix(int iidx, int card);
void BackUpMatrix();
void LUDecomp(void);
void LUSolve(void);
void SaveSol(void);
void Iterate(void);
void MatMult(void);
void UpdateSol(void);
int CalLocOffset(int species, int card);

//-----------------------------------------------------------
// Global arrays needed for the curve fitter function chain
//-----------------------------------------------------------
float alpha[3][3];
float cAlpha[3][3];
float beta[3];
float cBeta[3];
float polyV[3];
float vSol[3];
float loc_offset[CBPM_MAX_CARDS];
//------------------------------------------------------------

int   offset[CBPM_MAX_ACQ_CONTROLLERS];
int   CalRadius;


int fitter_test(int iidx) {
  int card;
  int status;
  int changed_gain;
  int func_status;
  int i;

  float *data;
  float *error;

  CBPM_DATA *dp;
  dp        = CTL_System.p_Module[iidx]->dsp_data;


  int species = dp->cbpm_cmd_params.species;

  for (card = 0; card < CBPM_MAX_CARDS; card++) {

    CalRadius = CBPM_CAL_RADIUS;
    data      = &(dp->cbpm_chan_cal_work.chan_cal_data[0].data[0]);
    error     = &(dp->cbpm_chan_cal_work.chan_cal_data[0].error[0]);

    offset[card] = dp->cbpm_diagnostic.pre_pf_offset_chan_delays[card];
    printf("Card %d offset = %d\n", card, offset[card] );

    CoeffMatrix(iidx, card);
    BackUpMatrix();
    LUDecomp();
    LUSolve();
    SaveSol();
    Iterate();
    UpdateSol();
    CalLocOffset(species, card);

  }
  
  return CBI_F_SUCCESS;
  
}





//-----------------------------------------------------------------------
void CoeffMatrix(int iidx, int card)
{
  float x, y, sig2i, sig;
  int i,j,k;
  
  CBPM_DATA *dp;
  dp        = CTL_System.p_Module[iidx]->dsp_data;

  float *data  = &(dp->cbpm_chan_cal_work.chan_cal_data[card].data[0]);
  float *error = &(dp->cbpm_chan_cal_work.chan_cal_data[card].error[0]);

  for(j=0; j<3; j++) {
    beta[j] = 0.0;
    for(k=0; k<3; k++) {
      alpha[j][k] = 0.0;
    }
  }


  printf("Card: %d\n", card);  
  for(i=0; i<(2*CalRadius+1)/CBPM_CHAN_SCAN_STEP; i++) {
    x = (i*CBPM_CHAN_SCAN_STEP);
    y = *(data + i);
    printf(" %f , %f\n", x, y);
    //sig = *(error + i);  //RMS^2
    sig = 1.0; // TESTING
    sig2i = 1.0/(sig);
    
    polyV[0] = x*x;
    polyV[1] = x;
    polyV[2] = 1;
    
    if (card == 0) {
      printf("polyV[0] = %f\n", polyV[0]);
      printf("polyV[1] = %f\n", polyV[1]);
      printf("polyV[2] = %f\n", polyV[2]);
    }

    for(j=0; j<3; j++)
      {
	beta[j] += sig2i*polyV[j]*y;
	for(k=0; k<3; k++) {
	  alpha[j][k] += sig2i*polyV[j]*polyV[k];
	}
      }
  }

  if (card == 0) {
    printf("EXTRA VALUE:\n");
    i = ((2*CalRadius+1)/CBPM_CHAN_SCAN_STEP);
    x = (i*CBPM_CHAN_SCAN_STEP); 
    y = *(data + i);
    printf("i = %d) %f , %f\n", i, x, y);
  }
  
  return;
}


//-------------------------------------------------------------------
void BackUpMatrix()
{
  int i, j;
    
  for(i=0; i<3; i++) {
    cBeta[i] = beta[i];
    for(j=0; j<3; j++) {
      cAlpha[i][j] = alpha[i][j];
    }
  }
  
  return;
}



//----------------------------------------------------------------
void LUDecomp(void)
{
  int i, j, k;
  
  for(j=0; j<3; j++)
    {
      for(i=1; i<j+1; i++) {
	for(k=0; k<i; k++) {
	  alpha[i][j] -= alpha[i][k]*alpha[k][j];
	}
      }
      
      for(i=j+1; i<3; i++) {
	for(k=0; k<j; k++) {
	  alpha[i][j] -= alpha[i][k]*alpha[k][j];
	}
	alpha[i][j] /=alpha[j][j];
      }
    }
  
  return;
}


//---------------------------------------------------------------------
void LUSolve(void)
{
  int i, j;
  
  
  for(i=1; i<3; i++){
    for(j=0; j<i; j++){
      beta[i] -= alpha[i][j]*beta[j];
    }
  }
   
  for(i=2; i>=0; i--) {
    for(j=i+1; j<3; j++) {
      beta[i] -= alpha[i][j]*beta[j];
    }
    beta[i] /= alpha[i][i];
  }
  
  
  return;
}



//----------------------------------------------------------------
void SaveSol(void)
{
  int i;
  
  
  for(i=0; i<3; i++) {
    vSol[i] = beta[i];
  }
  
  return;
}



//----------------------------------------------------------------
void Iterate(void)
{
  int i;
  
  MatMult();
  for(i=0; i<3; i++) {
    beta[i] -= cBeta[i];
  }
  LUSolve();
  
  return;
}



//----------------------------------------------------------------
void MatMult(void)
{
  int i, j;
  float temp[3];
  
  
  for(i=0; i<3; i++) {
    temp[i] = beta[i];
    beta[i] = 0.0;
  }
  
  for(i=0; i<3; i++){
    for(j=0; j<3; j++) {
      beta[i] += cAlpha[i][j]*temp[j];
    }
  }
  
  return;
}



//---------------------------------------------------------------
void UpdateSol(void)
{
  int i;
  
  for(i=0; i<3; i++) {
    vSol[i] -= beta[i];
    beta[i] = vSol[i];
  }
  
  return;
}




const char coeff_names[CBPM_MAX_CARDS][2] = {"A\0", "B\0", "C\0", "D\0"};
//----------------------------------------------------------------
// Final stage of the curve-fitter
//----------------------------------------------------------------
int CalLocOffset(int species, int card)
{

  int status = CBI_F_SUCCESS;
  
  //loc_offset[contr] = -vSol[1]/(2*vSol[0]);
  
  
  printf("%sl0 = %f\n", coeff_names[card], vSol[0]);
  printf("%sl1 = %f\n", coeff_names[card], vSol[1]);
  printf("%sl2 = %f\n", coeff_names[card], vSol[2]);
  printf("\n");
  
  
  
  return status;
} 


