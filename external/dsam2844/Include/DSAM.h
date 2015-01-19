/**********************
 *
 * File:		DSAM.h
 * Purpose:		This file includes all of the headers required for using the
 *				DSAM Core Routines Library
 * Comments:	It can be used to avoid missing vital header files, though
 *				header files may still be included individually.
 *				16-04-99 LPO: Introduced the 'myApp' declaration to solve some
 *				linking problems with shared libraries on IRIX 6.2.
 * Author:		Lowel P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		16 Apr 1999
 * Copyright:	(c) 1999, 2010 Lowel P. O'Mard
 *
 *********************
 *
 *  This file is part of DSAM.
 *
 *  DSAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DSAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
 *
 *********************/

#ifndef _DSAMHEADERS_H
#define	_DSAMHEADERS_H 1

#include <stdio.h>

#ifdef HAVE_CONFIG_H
#	include <DSAMSetup.h>
#endif /* HAVE_CONFIG_H */

#ifdef _MSC_VER
#	ifdef HAVE_STRINGS_H
#		undef HAVE_STRINGS_H
#	endif
#endif

#include <GeCommon.h>
#include <GeSignalData.h>
#include <GeEarObject.h>
#include <GeNSpecLists.h>
#include <GeUniParMgr.h>
#include <GeModuleMgr.h>
#include <GeModuleReg.h>

#include <UtAccumulate.h>
#include <UtAmpMod.h>
#include <UtAppInterface.h>
#include <UtBandwidth.h>
#include <UtBinSignal.h>
#include <UtDelay.h>
#include <UtDynaBList.h>
#include <UtDynaList.h>
#include <UtCFList.h>
#include <UtCmplxM.h>
#include <UtCompress.h>
#include <UtConvMonaural.h>
#include <UtCreateBinaural.h>
#include <UtCreateJoined.h>
#include <UtFFT.h>
#include <UtFilters.h>
#include <UtGCFilters.h>
#include <UtFIRFilters.h>
#include <UtHalfWRectify.h>
#include <UtIonChanList.h>
#include <UtIterRipple.h>
#include <UtLocalChans.h>
#include <UtMathOp.h>
#include <UtNameSpecs.h>
#include <UtNoiseMod.h>
#include <UtOptions.h>
#include <UtPadSignal.h>
#include <UtPause.h>
#include <UtRamp.h>
#include <UtRandom.h>
#include <UtRedceChans.h>
#include <UtReduceDt.h>
#include <UtRefractory.h>
#include <UtRemez.h>
#include <UtSample.h>
#include <UtSelectChans.h>
#include <UtSSSymbols.h>
#include <UtSimScript.h>
#include <UtShapePulse.h>
#include <UtSpikeList.h>
#include <UtStddise.h>
#include <UtStrobe.h>
#include <UtString.h>
#include <UtSwapLR.h>
#include <UtParArray.h>
#include <UtZhang.h>
#include <UtZilanyBruce.h>
#include <UtTranspose.h>
#include <UtCollateSignals.h>
#include <UtANSGUtils.h>

#include <AnALSR.h>
#include <AnAutoCorr.h>
#include <AnAverages.h>
#include <AnConvolve.h>
#include <AnCrossCorr.h>
#include <AnExperiment.h>
#include <AnFourierT.h>
#include <AnFindNextIndex.h>
#include <AnFindBin.h>
#include <AnGeneral.h>
#include <AnHistogram.h>
#include <AnIntensity.h>
#include <AnInterSIH.h>
#include <AnSAImage.h>
#include <AnSpikeReg.h>
#include <AnSynchIndex.h>
#include <AnSAC.h>

#include <FiParFile.h>
#include <FiDataFile.h>
#include <FiASCII.h>
#include <FlBandPass.h>
#include <FlDistLowPass.h>
#include <FlFIR.h>
#include <FlLowPass.h>
#include <FlMultiBPass.h>
#include <FlZBMiddleEar.h>

#include <MoANSGBinom.h>
#include <MoANSGCarney.h>
#include <MoANSGMeddis02.h>
#include <MoANSGSimple.h>
#include <MoBMDGammaC.h>
#include <MoBMDRNL.h>
#include <MoBMGammaC.h>
#include <MoBMGammaT.h>
#include <MoBMCooke.h>
#include <MoBMCarney.h>
#include <MoBMZhang.h>
#include <MoBMZilanyBruce.h>
#include <MoHCRPCarney.h>
#include <MoHCRPMeddis.h>
#include <MoHCRPLopezPoveda.h>
#include <MoHCRPSham3StVIn.h>
#include <MoHCRPShamma.h>
#include <MoHCRPZhang.h>
#include <MoIHC86Meddis.h>
#include <MoIHC86aMeddis.h>
#include <MoIHCMeddis2000.h>
#include <MoIHCCarney.h>
#include <MoIHCCooke.h>
#include <MoIHCZhang.h>
#include <MoIHCZilanyBruce.h>
#include <MoNCArleKim.h>
#include <MoNCHHuxley.h>
#include <MoNCMcGregor.h>

#include <StAMTone.h>
#include <StBPTone.h>
#include <StClick.h>
#include <StCMRHarmMask.h>
#include <StCMRNoiseMask.h>
#include <StFMTone.h>
#include <StFSHarmonic.h>
#include <StEGatedTone.h>
#include <StHarmonic.h>
#include <StMPTone.h>
#include <StMPPTone.h>
#include <StPTone.h>
#include <StPulseTrain.h>
#include <StPTone2.h>
#include <StStepFun.h>
#include <StWhiteNoise.h>

#include <TrCollectSignals.h>
#include <TrGate.h>
#include <TrSetDBSPL.h>

#include <DiSignalDisp.h>

#if defined(__cplusplus)
#/* --cplusplus ensures only included with c++ compile */
#	if defined(GRAPHICS_SUPPORT) || defined(USE_EXTENSIONS_LIBRARY)
#		undef TRUE			/* required because WxWin doesn't check. */
#		undef FALSE

		/* For compilers that support precompilation, includes >wx.h>.*/
#		undef	wxT
#		include <wx/wxprec.h>

#		ifdef __BORLANDC__
	   		#pragma hdrstop
#		endif

		/* Any files you want to include if not precompiling by including
		 * the whole of <wx/wx.h>
		 */
#		ifndef WX_PRECOMP
#			include <wx/wx.h>
#		endif
#	endif /* __WXWIN__ */

#	if USE_EXTENSIONS_LIBRARY
		/* Any files included regardless of precompiled headers */
#		include <wx/socket.h>
#		include <wx/filesys.h>
#		include <ExtIPCUtils.h>
#		include <ExtIPCClient.h>
#		include <ExtIPCServer.h>
#		include <ExtMainApp.h>
#		include <ExtSimThread.h>
#	endif /* USE_EXTENSIONS_LIBRARY */

#	if defined(GRAPHICS_SUPPORT) && defined(USE_GUI)
		/* Any files included regardless of precompiled headers */
#		include <wx/notebook.h>
#		include <wx/print.h>
#		include <wx/printdlg.h>
#		include <wx/socket.h>
#		include <wx/cmdproc.h>

#		if wxTEST_POSTSCRIPT_IN_MSW
#			include <wx/generic/printps.h>
#			include <wx/generic/prntdlgg.h>
#		endif

#		include <GrUtils.h>
#		include <GrFonts.h>
#		include <GrDialogInfo.h>
#		include <GrParControl.h>
#		include <GrParListInfo.h>
#		include <GrParListInfoList.h>
#		include <GrModParDialog.h>
#		include <GrIPCServer.h>
#		include <GrSimMgr.h>
#		include <GrLines.h>
#		include <GrAxisScale.h>
#		include <GrCanvas.h>
#		include <GrPrintDisp.h>
#		include <GrBrushes.h>
#		include <GrDisplayS.h>
#		include <GrDiagFrame.h>
#		include <GrSDICanvas.h>
#		include <GrSDIFrame.h>
#		include <GrSDIEvtHandler.h>
#		include <GrSDIDiagram.h>
#		include <GrSDIDoc.h>
#		include <GrSDICommand.h>
#		include <GrSDIView.h>
#		include <GrSDIPalette.h>

#	endif /* GRAPHICS_SUPPORT */
#endif /* __cplusplus */

#if USE_PORTAUDIO
#	include <IOAudioIn.h>
#endif

#ifdef MPI_SUPPORT
#	include <mpi.h>
#	include	>PaGeneral.h>
#	include	>PaMaster1.h>
#	include	>PaUtilities.h>
#endif /* MPI_SUPPORT */

#endif /* _DSAMHEADERS_H */

