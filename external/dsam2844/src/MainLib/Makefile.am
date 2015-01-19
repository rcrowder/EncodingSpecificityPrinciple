#######################
##
## File:		Makefile.am
## Purpose:		src/MainLib directory makefile.
## Comments:	Process this file with automake to produce Makefile.in
## Author:		L. P. O'Mard
## Created:		
## Updated:
## Copyright:	(c) 2010 Lowel P. O'Mard
##
#######################
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

include $(top_srcdir)/src/makeunix.env

if MPI
MPI_INC = -I$(CHIMPHOME)/include
endif

bin_PROGRAMS	= @PAWORKER1M@
EXTRA_PROGRAMS	= PaWorker1m.exe

lib_LTLIBRARIES = libdsam.la
EXTRA_DIST		= makefile.b32

YFLAGS			= -dl
libdsam_la_CPPFLAGS= -DLIBRARY_COMPILE $(MAIN_INC)

An_Sources	= AnAutoCorr.c AnAverages.c AnConvolve.c AnCrossCorr.c \
				AnExperiment.c AnFindBin.c AnFindNextIndex.c AnFourierT.c \
				AnGeneral.c AnHistogram.c AnIntensity.c AnInterSIH.c \
				AnSpikeReg.c AnSynchIndex.c AnSAImage.c AnALSR.c AnSAC.c

Di_Sources	= DiSignalDisp.c

  
Fl_Sources	= FlBandPass.c  FlDistLowPass.c FlMultiBPass.c FlFIR.c FlZBMiddleEar.c \
				FlLowPass.c

Ge_Sources	= GeEarObject.c GeCommon.c GeModuleMgr.c GeSignalData.c \
				GeNSpecLists.c GeUniParMgr.c GeModuleReg.c

IO_Sources	= FiDataFile.c FiASCII.c FiSndFile.c FiParFile.c IOAudioIn.c

Mo_Sources	= MoANSGBinom.c MoANSGCarney.c MoANSGSimple.c \
				MoBMCarney.c MoBMCooke.c MoBMDRNL.c MoBMGammaT.c \
				MoHCRPCarney.c MoHCRPMeddis.c MoHCRPShamma.c MoIHC86Meddis.c \
				MoIHC86aMeddis.c MoIHCMeddis2000.c MoIHCCarney.c MoIHCCooke.c \
				MoNCArleKim.c MoNCHHuxley.c MoNCMcGregor.c MoBMGammaC.c \
				MoHCRPSham3StVIn.c MoBMZhang.c MoHCRPZhang.c MoIHCZhang.c \
				MoANSGMeddis02.c MoBMDGammaC.c MoBMZilanyBruce.c MoHCRPLopezPoveda.c \
				MoIHCZilanyBruce.c

Pa_Sources	= PaComUtils.c PaGeneral.c PaMaster1.c PaUtilities.c

St_Sources	= StAMTone.c StBPTone.c StClick.c StFMTone.c StHarmonic.c \
				StMPPTone.c StMPTone.c StPTone.c StPTone2.c StPulseTrain.c \
				StStepFun.c StWhiteNoise.c StEGatedTone.c StFSHarmonic.c \
				StCMRNoiseMask.c StCMRHarmMask.c

Ut_Sources	= UtSSParser.y UtAccumulate.c UtAmpMod.c UtBandwidth.c \
				UtBinSignal.c UtCFList.c UtCmplxM.c UtCompress.c \
				UtConvMonaural.c UtCreateBinaural.c UtDatum.c UtDebug.c \
				UtDelay.c UtFilters.c UtIonChanList.c UtIterRipple.c \
				UtNameSpecs.c UtRamp.c UtRandom.c UtRedceChans.c UtReduceDt.c \
				UtRefractory.c UtSample.c UtSelectChans.c UtShapePulse.c \
				UtSSSymbols.c UtSimScript.c UtSpikeList.c UtStddise.c \
				UtCreateJoined.c UtStrobe.c UtOptions.c UtString.c UtPause.c \
				UtHalfWRectify.c UtDynaList.c UtAppInterface.c UtFFT.c \
				UtParArray.c UtGCFilters.c UtFIRFilters.c UtRemez.c \
				UtDynaBList.c UtLocalChans.c UtSwapLR.c UtPadSignal.c \
				UtMathOp.c UtZhang.c UtTranspose.c UtCollateSignals.c \
				UtZilanyBruce.c UtNoiseMod.c UtANSGUtils.c

Tr_Sources	= TrGate.c TrSetDBSPL.c TrCollectSignals.c

pkginclude_HEADERS	= UtSSParser.h
BUILT_SOURCES		= UtSSParser.c

libdsam_la_SOURCES= $(Ut_Sources) $(Ge_Sources) $(An_Sources) $(IO_Sources) \
					$(Mo_Sources) $(Fl_Sources) $(St_Sources) $(Tr_Sources) \
					$(Di_Sources)
libdsam_la_LDFLAGS=  $(GenLaLDFLAGS)

PaWorker1m_exe_SOURCES	= PaWorker1.c
PaWorker1m_exe_LDADD	= libDSAMm.a -L$(CHIMP_LIB) -lmpi -lchimp -lm

