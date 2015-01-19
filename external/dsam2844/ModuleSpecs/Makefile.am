#######################
##
## File:		Makefile.am
## Purpose:		ModuleSpecs directory makefile.
## Comments:	Process this file with automake to produce Makefile.in
## Author:		L. P. O'Mard
## Created:		
## Updated:
## Copyright:	(c) 2010 Lowel P. O'Mard
##
##********************
##
##  This file is part of DSAM.
##
##  DSAM is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  DSAM is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
##
#######################

AUTOMAKE_OPTIONS = foreign

An_Modules	= AnAutoCorr.mod AnAverages.mod AnConvolve.mod AnCrossCorr.mod \
				 AnFindBin.mod AnFindNextIndex.mod AnHistogram.mod \
				 AnIntensity.mod AnInterSIH.mod AnSpikeReg.mod AnFourierT.mod \
				 AnSynchIndex.mod AnSAImage.mod AnALSR.mod AnSAC.mod

IO_Modules	= FiDataFile.mod IOAudioIn.mod

Fl_Modules	= FlBandPass.mod FlMultiBPass.mod FlFIR.mod FlZBMiddleEar.mod \
				FlDistLowPass.mod

Mo_Modules	= MoANSGBinom.mod MoANSGCarney.mod MoANSGSimple.mod \
				MoBMCarney.mod MoBMCooke.mod MoBMGammaT.mod MoBMDRNL.mod \
				FlLowPass.mod MoHCRPCarney.mod MoHCRPMeddis.mod \
				MoHCRPShamma.mod MoIHCCarney.mod MoIHCCooke.mod \
				MoIHC86Meddis.mod MoIHC86aMeddis.mod MoIHCMeddis2000.mod \
				MoNCArleKim.mod MoNCHHuxley.mod MoNCMcGregor.mod \
				MoBMGammaC.mod MoHCRPSham3StVIn.mod MoBMZhang.mod \
				MoHCRPZhang.mod MoIHCZhang.mod MoANSGMeddis02.mod \
				MoHCRPLopezPoveda.mod MoBMSRNL.mod

Pa_Modules	= PaMaster1.mod

St_Modules	= StPTone.mod StFMTone.mod StHarmonic.mod StPulseTrain.mod \
				StEGatedTone.mod StPTone2.mod StAMTone.mod StBPTone.mod \
				StMPTone.mod StMPPTone.mod StStepFun.mod StWhiteNoise.mod \
				StClick.mod StFSHarmonic.mod StCMRNoiseMask.mod \
				StCMRHarmMask.mod

Ut_Modules	= UtAccumulate.mod UtAmpMod.mod UtBinSignal.mod UtTranspose.mod \
				UtCBinauralITD.mod UtCompress.mod UtConvMonaural.mod \
				UtCreateBinaural.mod UtDelay.mod UtIterRipple.mod \
				UtRedceChans.mod UtReduceDt.mod UtRefractory.mod UtSample.mod \
				UtSelectChans.mod UtShapePulse.mod UtSimScript.mod \
				UtStddise.mod UtStrobe.mod UtPause.mod UtHalfWRectify.mod \
				UtLocalChans.mod UtSwapLR.mod UtPadSignal.mod UtMathOp.mod \
				UtCollateSignals.mod UtNoiseMod.mod

Tr_Modules	= TrGate.mod TrSetDBSPL.mod TrCollectSignals.mod

EXTRA_DIST	= $(An_Modules) $(IO_Modules) $(Fl_Modules) $(Mo_Modules) \
				$(Pa_Modules) $(St_Modules) $(Ut_Modules) $(Tr_Modules)
