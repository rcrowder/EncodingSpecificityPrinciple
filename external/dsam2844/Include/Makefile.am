#######################
##
## File:		Makefile.am
## Purpose:		Include directory makefile.
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

An_Headers	= AnAutoCorr.h AnAverages.h AnConvolve.h AnCrossCorr.h \
				AnExperiment.h AnFindBin.h AnFindNextIndex.h AnFourierT.h \
				AnGeneral.h AnHistogram.h AnIntensity.h AnInterSIH.h \
				AnSpikeReg.h AnSynchIndex.h AnSAImage.h AnALSR.h AnSAC.h
  
Di_Headers	= DiSignalDisp.h

Fl_Headers	=  FlBandPass.h FlLowPass.h FlFIR.h FlZBMiddleEar.h \
				FlDistLowPass.h

Ge_Headers	= GeEarObject.h GeCommon.h GeModuleMgr.h GeSignalData.h \
				GeNSpecLists.h GeUniParMgr.h GeModuleReg.h

Gr_Headers	= GrBrushes.h GrCanvas.h GrLines.h GrDisplayS.h \
				GrSimMgr.h GrUtils.h GrModParDialog.h GrParListInfo.h \
				GrFonts.h GrDialogInfo.h GrParListInfoList.h GrPrintDisp.h \
				GrParControl.h ExtCommon.h GrAxisScale.h dsam.rc \
				GrSDIPalette.h GrMainApp.h GrSDIView.h GrSDIFrame.h \
				GrSDICanvas.h GrSDIDiagram.h GrSDIEvtHandler.h GrSDICommand.h \
				GrSDIShapes.h GrDiagFrame.h GrSDIDoc.h GrIPCServer.h \
				GrSDIDocManager.h GrSDIXMLDoc.h GrSDIBaseShapes.h

IO_Headers	= FiDataFile.h FiASCII.h FiSndFile.h FiParFile.h IOAudioIn.h

Mo_Headers	= MoANSGBinom.h MoANSGCarney.h MoANSGSimple.h \
				MoBMCarney.h MoBMCooke.h MoBMDRNL.h MoBMGammaT.h MoBMZilanyBruce.h \
				MoHCRPCarney.h MoHCRPMeddis.h MoHCRPShamma.h MoIHC86Meddis.h \
				MoIHC86aMeddis.h MoIHCMeddis2000.h MoIHCCarney.h MoIHCCooke.h \
				MoNCArleKim.h MoNCHHuxley.h MoNCMcGregor.h FlMultiBPass.h \
				MoBMGammaC.h MoHCRPSham3StVIn.h MoBMZhang.h MoHCRPZhang.h \
				MoIHCZhang.h MoANSGMeddis02.h MoBMDGammaC.h MoHCRPLopezPoveda.h \
				MoIHCZilanyBruce.h

Pa_Headers	= PaComUtils.h PaGeneral.h PaMaster1.h PaUtilities.h PaWorker1.h

St_Headers	= StAMTone.h StBPTone.h StClick.h StFMTone.h StHarmonic.h \
				StMPPTone.h StMPTone.h StPTone.h StPTone2.h StPulseTrain.h \
				StStepFun.h StWhiteNoise.h StEGatedTone.h StFSHarmonic.h \
				StCMRNoiseMask.h StCMRHarmMask.h 

Ext_Headers	= ExtMainApp.h ExtIPCServer.h ExtSimThread.h ExtIPCUtils.h \
				ExtIPCClient.h ExtSocket.h ExtSocketServer.h ExtProcThread.h \
				ExtRunThreadedProc.h ExtXMLDocument.h ExtSingleProcThread.h \
				ExtProcChainThread.h ExtXMLNode.h

Ut_Headers	= UtAccumulate.h UtAmpMod.h UtBandwidth.h UtBinSignal.h \
				UtCFList.h UtCmplxM.h UtCompress.h \
				UtConvMonaural.h UtCreateBinaural.h UtDatum.h UtDebug.h \
				UtDelay.h UtFilters.h UtIonChanList.h UtIterRipple.h \
				UtNameSpecs.h UtRamp.h UtRandom.h UtRedceChans.h UtReduceDt.h \
				UtRefractory.h UtSample.h UtSelectChans.h UtShapePulse.h \
				UtSSSymbols.h UtSimScript.h UtSpikeList.h UtStddise.h \
				UtCreateJoined.h UtStrobe.h UtOptions.h UtString.h UtPause.h \
				UtHalfWRectify.h UtDynaList.h UtAppInterface.h UtFFT.h \
				UtParArray.h UtGCFilters.h UtFIRFilters.h UtRemez.h \
				UtDynaBList.h UtLocalChans.h UtSwapLR.h UtPadSignal.h \
				UtMathOp.h UtZhang.h UtTranspose.h UtCollateSignals.h \
				UtZilanyBruce.h UtNoiseMod.h UtANSGUtils.h

Tr_Headers	= TrGate.h TrSetDBSPL.h TrCollectSignals.h

pkginclude_HEADERS	= DSAM.h $(St_Headers) $(Mo_Headers) $(Fl_Headers) \
					$(IO_Headers) $(An_Headers) $(Ge_Headers) $(Ut_Headers) \
					$(Tr_Headers) $(Pa_Headers) $(Gr_Headers) $(Std_Headers) \
					$(Ext_Headers) $(Di_Headers)

DSAM.h:	$(St_Headers) $(Mo_Headers) $(Fl_Headers) $(IO_Headers) $(An_Headers) \
					$(Ge_Headers) $(Ut_Headers) $(Tr_Headers) $(Pa_Headers) \
					$(Gr_Headers) $(Std_Headers) $(Ext_Headers) $(Di_Headers)
		touch $(top_srcdir)/Include/DSAM.h
