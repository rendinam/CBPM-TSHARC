#ifndef CBPM_VERSIONS_H
#define CBPM_VERSIONS_H

//-----------------------------------------------------------------------+
// File         :  cbpm_versions.h                                       |
//                                                                       |
// Description  :  This header file maintains the code version ID        |
//                 numbers for TigerSHARC BPM DSP and control system     |
//                 code.                                                 |       
//                                                                       |
// Author       :  M. Palmer / M. Rendina                                |
//-----------------------------------------------------------------------+

//-----------------------------------------------------------------------+
//  The following provides for C++ compatibility (ie, C++ routines can   |
//  explicitly use this include file).                                   |
//-----------------------------------------------------------------------+
#if defined (__cplusplus)
extern "C" {
#endif


//-----------------------------------------------------------------------+
// CBPM DSP code version                                                 |
// CBPM STD (position monitoring) executable version                     |
// CBPM FFT (bunch tune monitoring) executable version                   |
// Value should be of the form xx.yz or x.yz (ie, only 2 decimal places) | 
//-----------------------------------------------------------------------+
#define TBPM_DSP_CODE_VERSION 1.00

#define TBPM_DSP_STD_VERSION 1.00

#define TBPM_DSP_FFT_VERSION 0.00


//-----------------------------------------------------------------------+
// CBPM CTL code version                                                 |
// Value should be of the form xx.yz or x.yz (ie, only 2 decimal places) |
//-----------------------------------------------------------------------+
#define CTL_CODE_VERSION 1.0


//========================================================================
// AUTOMATICALLY Inserted Information, DO NOT EDIT
//========================================================================
#define LDR_NAME "CBPM_II.ldr"

#define COMPILER_VERSION 1

#define BUILD_TIMESTAMP 1209141408

#define DSP_CODE_TYPE 2


//========================================
// END AUTOMATICALLY Inserted Information
//========================================
#if defined (__cplusplus)
}
#endif

#endif // CBPM_VERSIONS_H
