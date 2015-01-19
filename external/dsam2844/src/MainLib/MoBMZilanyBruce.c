/**********************
 *
 * File:		MoBMZilanyBruce.c
 * Purpose:	This is the Zilany and Bruce BM filter module.
 *				It was revised from code provided by I. C. Bruce
 * 				(zbcatmodel2007v2.tar.gz).
 *				Zilany, M. S. A. and Bruce, I. C. (2006). "Modeling auditory-
 * 				nerve responses for high sound pressure levels in the normal
 * 				and impaired auditory periphery," Journal of the Acoustical
 * 				Society of America 120(3):1446-1466.
 * 				Zilany, M. S. A. and Bruce, I. C. (2007). "Representation of
 * 				the vowel /eh/ in normal and impaired auditory nerve fibers:
 * 				Model predictions of responses in cats," Journal of the
 * 				Acoustical Society of America 122(1):402-417.
 * Comments:	Written using ModuleProducer version 1.6.0 (Mar  4 2008).
 * Author:		Revised by L. P. O'Mard
 * Comments:	Written using ModuleProducer version 1.6.0 (Mar  4 2008).
 * Author:
 * Created:		04 Mar 2008
 * Updated:
 * Copyright:	(c) 2010 Lowel P. O'Mard
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtBandwidth.h"
#include "UtCmplxM.h"
#include "UtCFList.h"
#include "UtZilanyBruce.h"
#include "UtZhang.h"
#include "MoBMZilanyBruce.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BMZBPtr	bMZBPtr = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_BasilarM_ZilanyBruce(void)
{
	/* static const WChar	*funcName = wxT("Free_BasilarM_ZilanyBruce"); */

	if (bMZBPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_ZilanyBruce();
	Free_CFList(&bMZBPtr->cFList);
	if (bMZBPtr->parList)
		FreeList_UniParMgr(&bMZBPtr->parList);
	if (bMZBPtr->parSpec == GLOBAL) {
		free(bMZBPtr);
		bMZBPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitOutputModeList ****************************/

/*
 * This function initialises the 'outputMode' list array
 */

BOOLN
InitOutputModeList_BasilarM_ZilanyBruce(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("C1_FILTER"),	BM_ZILANYBRUCE_OUTPUTMODE_C1_FILTER },
			{ wxT("C2_FILTER"),	BM_ZILANYBRUCE_OUTPUTMODE_C2_FILTER },
			{ wxT("FILTER"),	BM_ZILANYBRUCE_OUTPUTMODE_FILTER },
			{ wxT("C1_VIHC"),	BM_ZILANYBRUCE_OUTPUTMODE_C1_VIHC },
			{ wxT("C2_VIHC"),	BM_ZILANYBRUCE_OUTPUTMODE_C2_VIHC },
			{ wxT("VIHC"),		BM_ZILANYBRUCE_OUTPUTMODE_VIHC },
			{ 0, BM_ZILANYBRUCE_OUTPUTMODE_NULL },
		};
	bMZBPtr->outputModeList = modeList;
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This function initialises the module by setting module's parameter
 * pointer structure.
 * The GLOBAL option is for hard programming - it sets the module's
 * pointer to the global parameter structure and initialises the
 * parameters. The LOCAL option is for generic programming - it
 * initialises the parameter structure currently pointed to by the
 * module's parameter pointer.
 */

BOOLN
Init_BasilarM_ZilanyBruce(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_BasilarM_ZilanyBruce");

	if (parSpec == GLOBAL) {
		if (bMZBPtr != NULL)
			Free_BasilarM_ZilanyBruce();
		if ((bMZBPtr = (BMZBPtr) malloc(sizeof(BMZB))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMZBPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bMZBPtr->parSpec = parSpec;
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->outputMode = BM_ZILANYBRUCE_OUTPUTMODE_VIHC;
	bMZBPtr->microPaInput = GENERAL_BOOLEAN_ON;
	bMZBPtr->wborder = 3;
	bMZBPtr->s0 = 12.0;
	bMZBPtr->x1 = 5.0;
	bMZBPtr->s1 = 5.0;
	bMZBPtr->shiftCP = 0.125;
	bMZBPtr->cutOffCP = 600.0;	/* Value for Zilany and Bruce (2006): 1e3 used for (2007). */
	bMZBPtr->lPOrder = 2;
	bMZBPtr->dc = 0.37;
	bMZBPtr->rC1 = 0.05;
	bMZBPtr->cOHC = 1.0;
	bMZBPtr->ohcasym = 7.0;
	bMZBPtr->ihcasym = 3.0;
	bMZBPtr->nBorder = 10;
	bMZBPtr->aIHC0 = 0.1;
	bMZBPtr->bIHC = 2000.0;
	bMZBPtr->cIHC = 1.0;
	bMZBPtr->cutOffIHCLP = 3800.0;
	bMZBPtr->iHCLPOrder = 7;
	if ((bMZBPtr->cFList = GenerateDefault_CFList(

	  CFLIST_DEFAULT_MODE_NAME, CFLIST_DEFAULT_CHANNELS,
	  CFLIST_DEFAULT_LOW_FREQ, CFLIST_DEFAULT_HIGH_FREQ,
	  CFLIST_DEFAULT_BW_MODE_NAME, CFLIST_DEFAULT_BW_MODE_FUNC)) == NULL) {
		NotifyError(wxT("%s: Could not set default CFList."), funcName);
		return(FALSE);
	}

	InitOutputModeList_BasilarM_ZilanyBruce();
	if (!SetUniParList_BasilarM_ZilanyBruce()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_BasilarM_ZilanyBruce();
		return(FALSE);
	}
	bMZBPtr->numChannels = 0;
	bMZBPtr->tempSamples = NULL;
	bMZBPtr->wbgt = NULL;
	bMZBPtr->c1Filter = NULL;
	bMZBPtr->c2Filter = NULL;
	bMZBPtr->ohcLowPass = NULL;
	bMZBPtr->ihcLowPass = NULL;
	bMZBPtr->tmpgain = NULL;
	bMZBPtr->delayedSamples = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_ZilanyBruce(void)
{
	static const WChar	*funcName = wxT("SetUniParList_BasilarM_ZilanyBruce");
	UniParPtr	pars;

	if ((bMZBPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BM_ZILANYBRUCE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = bMZBPtr->parList->pars;
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Output mode ('c1_filter', 'c2_filter', 'filter', 'c1_vihc', ")
	    wxT("'c2_vihc' or 'vihc'."),
	  UNIPAR_NAME_SPEC,
	  &bMZBPtr->outputMode, bMZBPtr->outputModeList,
	  (void * (*)) SetOutputMode_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_MICROPAINPUT], wxT("MICRO_PASCALS"),
	  wxT("Input expected in micro pascals instead of pascals ('on' or 'off'")
	    wxT(")."),
	  UNIPAR_BOOL,
	  &bMZBPtr->microPaInput, NULL,
	  (void * (*)) SetMicroPaInput_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_WBORDER], wxT("WB_ORDER"),
	  wxT("Order of the wide-band Gammatone filter."),
	  UNIPAR_INT,
	  &bMZBPtr->wborder, NULL,
	  (void * (*)) SetWborder_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_S0], wxT("S0"),
	  wxT("S0 Parameter in Boltzmann function."),
	  UNIPAR_REAL,
	  &bMZBPtr->s0, NULL,
	  (void * (*)) SetS0_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_X1], wxT("X1"),
	  wxT("X1 Parameter in Boltzmann function."),
	  UNIPAR_REAL,
	  &bMZBPtr->x1, NULL,
	  (void * (*)) SetX1_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_S1], wxT("S1"),
	  wxT("S1 Parameter in Boltzmann function."),
	  UNIPAR_REAL,
	  &bMZBPtr->s1, NULL,
	  (void * (*)) SetS1_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_SHIFTCP], wxT("SHIFT_CP"),
	  wxT("Shift parameter in Boltzmann function."),
	  UNIPAR_REAL,
	  &bMZBPtr->shiftCP, NULL,
	  (void * (*)) SetShiftCP_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_CUTOFFCP], wxT("CUTOFF_CP"),
	  wxT("Cut-off frequency of control path low-pass filter (Hz)."),
	  UNIPAR_REAL,
	  &bMZBPtr->cutOffCP, NULL,
	  (void * (*)) SetCutOffCP_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_LPORDER], wxT("LP_ORDER"),
	  wxT("Oder of the control path low-pass filter."),
	  UNIPAR_INT,
	  &bMZBPtr->lPOrder, NULL,
	  (void * (*)) SetLPOrder_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_DC], wxT("DC"),
	  wxT("Estimated dc shift of low-pass filter output at high-level."),
	  UNIPAR_REAL,
	  &bMZBPtr->dc, NULL,
	  (void * (*)) SetDc_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_RC1], wxT("R_C1"),
	  wxT("Ratio of lower bound ot tau_C1 to tau_C1max."),
	  UNIPAR_REAL,
	  &bMZBPtr->rC1, NULL,
	  (void * (*)) SetRC1_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_COHC], wxT("C_OHC"),
	  wxT("Scaling constant for impairment in the OHC."),
	  UNIPAR_REAL,
	  &bMZBPtr->cOHC, NULL,
	  (void * (*)) SetCOHC_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_OHCASYM], wxT("OHC_ASYM"),
	  wxT("Nonlinear asymmetry of OHC function."),
	  UNIPAR_REAL,
	  &bMZBPtr->ohcasym, NULL,
	  (void * (*)) SetOhcasym_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_IHCASYM], wxT("IHC_ASYM"),
	  wxT("Nonlinear asymmetry of IHC C1 transduction function"),
	  UNIPAR_REAL,
	  &bMZBPtr->ihcasym, NULL,
	  (void * (*)) SetIhcasym_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_NBORDER], wxT("NB_ORDER_C1"),
	  wxT("Order of the narrow-band C1 filter (chirp)."),
	  UNIPAR_INT,
	  &bMZBPtr->nBorder, NULL,
	  (void * (*)) SetNBorder_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_AIHC0], wxT("A_IHC0"),
	  wxT("Constant in C1/C2 transduction function."),
	  UNIPAR_REAL,
	  &bMZBPtr->aIHC0, NULL,
	  (void * (*)) SetAIHC0_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_BIHC], wxT("B_IHC"),
	  wxT("Constant in C1/C2 transduction function."),
	  UNIPAR_REAL,
	  &bMZBPtr->bIHC, NULL,
	  (void * (*)) SetBIHC_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_CIHC], wxT("C_IHC"),
	  wxT("Scaling constant for impairment in the IHC (not C_ihc in paper)."),
	  UNIPAR_REAL,
	  &bMZBPtr->cIHC, NULL,
	  (void * (*)) SetCIHC_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_CUTOFFIHCLP], wxT("CUTOFF_IHCLP"),
	  wxT("Low-pass filter cut off in the IHC (Hz)."),
	  UNIPAR_REAL,
	  &bMZBPtr->cutOffIHCLP, NULL,
	  (void * (*)) SetCutOffIHCLP_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_IHCLPORDER], wxT("LP_ORDER_IHC"),
	  wxT("Low-pass filter order in the IHC."),
	  UNIPAR_INT,
	  &bMZBPtr->iHCLPOrder, NULL,
	  (void * (*)) SetIHCLPOrder_BasilarM_ZilanyBruce);
	SetPar_UniParMgr(&pars[BM_ZILANYBRUCE_CFLIST], wxT("CFLIST"),
	  wxT("Centre frequency list."),
	  UNIPAR_CFLIST,
	  &bMZBPtr->cFList, NULL,
	  (void * (*)) SetCFList_BasilarM_ZilanyBruce);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_ZilanyBruce(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMZBPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(bMZBPtr->parList);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_BasilarM_ZilanyBruce(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_BasilarM_ZilanyBruce");
	int		specifier;

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
		bMZBPtr->outputModeList)) == BM_ZILANYBRUCE_OUTPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->outputMode = specifier;
	return(TRUE);

}

/****************************** SetMicroPaInput *******************************/

/*
 * This function sets the module's microPaInput parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMicroPaInput_BasilarM_ZilanyBruce(WChar * theMicroPaInput)
{
	static const WChar	*funcName = wxT("SetMicroPaInput_BasilarM_ZilanyBruce");
	int		specifier;

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMicroPaInput,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName, theMicroPaInput);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->microPaInput = specifier;
	return(TRUE);

}

/****************************** SetWborder ************************************/

/*
 * This function sets the module's wborder parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetWborder_BasilarM_ZilanyBruce(int theWborder)
{
	static const WChar	*funcName = wxT("SetWborder_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->wborder = theWborder;
	return(TRUE);

}

/****************************** SetS0 *****************************************/

/*
 * This function sets the module's s0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetS0_BasilarM_ZilanyBruce(Float theS0)
{
	static const WChar	*funcName = wxT("SetS0_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->s0 = theS0;
	return(TRUE);

}

/****************************** SetX1 *****************************************/

/*
 * This function sets the module's x1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetX1_BasilarM_ZilanyBruce(Float theX1)
{
	static const WChar	*funcName = wxT("SetX1_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->x1 = theX1;
	return(TRUE);

}

/****************************** SetS1 *****************************************/

/*
 * This function sets the module's s1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetS1_BasilarM_ZilanyBruce(Float theS1)
{
	static const WChar	*funcName = wxT("SetS1_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->s1 = theS1;
	return(TRUE);

}

/****************************** SetShiftCP ************************************/

/*
 * This function sets the module's shiftCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetShiftCP_BasilarM_ZilanyBruce(Float theShiftCP)
{
	static const WChar	*funcName = wxT("SetShiftCP_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->shiftCP = theShiftCP;
	return(TRUE);

}

/****************************** SetCutOffCP ***********************************/

/*
 * This function sets the module's cutOffCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCutOffCP_BasilarM_ZilanyBruce(Float theCutOffCP)
{
	static const WChar	*funcName = wxT("SetCutOffCP_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->cutOffCP = theCutOffCP;
	return(TRUE);

}

/****************************** SetLPOrder ************************************/

/*
 * This function sets the module's lPOrder parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLPOrder_BasilarM_ZilanyBruce(int theLPOrder)
{
	static const WChar	*funcName = wxT("SetLPOrder_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theLPOrder >= MAX_ORDER) {
		NotifyError(wxT("%s: Order (%d) exceeds maximum (%d"), funcName,
		  theLPOrder, MAX_ORDER);
		return(FALSE);
	}
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->lPOrder = theLPOrder;
	return(TRUE);

}

/****************************** SetDc *****************************************/

/*
 * This function sets the module's dc parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDc_BasilarM_ZilanyBruce(Float theDc)
{
	static const WChar	*funcName = wxT("SetDc_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->dc = theDc;
	return(TRUE);

}

/****************************** SetRC1 ****************************************/

/*
 * This function sets the module's rC1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRC1_BasilarM_ZilanyBruce(Float theRC1)
{
	static const WChar	*funcName = wxT("SetRC1_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->rC1 = theRC1;
	return(TRUE);

}

/****************************** SetCOHC ***************************************/

/*
 * This function sets the module's cOHC parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCOHC_BasilarM_ZilanyBruce(Float theCOHC)
{
	static const WChar	*funcName = wxT("SetCOHC_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->cOHC = theCOHC;
	return(TRUE);

}

/****************************** SetOhcasym *************************************/

/*
 * This function sets the module's ohcasym parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOhcasym_BasilarM_ZilanyBruce(Float theOhcasym)
{
	static const WChar	*funcName = wxT("SetOhcasym_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->ohcasym = theOhcasym;
	return(TRUE);

}

/****************************** SetIhcasym ************************************/

/*
 * This function sets the module's ihcasym parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIhcasym_BasilarM_ZilanyBruce(Float theIhcasym)
{
	static const WChar	*funcName = wxT("SetIhcasym_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->ihcasym = theIhcasym;
	return(TRUE);

}

/****************************** SetNBorder ************************************/

/*
 * This function sets the module's nBorder parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNBorder_BasilarM_ZilanyBruce(int theNBorder)
{
	static const WChar	*funcName = wxT("SetNBorder_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->nBorder = theNBorder;
	return(TRUE);

}

/****************************** SetAIHC0 **************************************/

/*
 * This function sets the module's aIHC0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAIHC0_BasilarM_ZilanyBruce(Float theAIHC0)
{
	static const WChar	*funcName = wxT("SetAIHC0_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->aIHC0 = theAIHC0;
	return(TRUE);

}

/****************************** SetBIHC ***************************************/

/*
 * This function sets the module's bIHC parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBIHC_BasilarM_ZilanyBruce(Float theBIHC)
{
	static const WChar	*funcName = wxT("SetBIHC_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->bIHC = theBIHC;
	return(TRUE);

}

/****************************** SetCIHC ***************************************/

/*
 * This function sets the module's cIHC parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCIHC_BasilarM_ZilanyBruce(Float theCIHC)
{
	static const WChar	*funcName = wxT("SetCIHC_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->cIHC = theCIHC;
	return(TRUE);

}

/****************************** SetCutOffIHCLP ********************************/

/*
 * This function sets the module's cutOffIHCLP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCutOffIHCLP_BasilarM_ZilanyBruce(Float theCutOffIHCLP)
{
	static const WChar	*funcName = wxT("SetCutOffIHCLP_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->cutOffIHCLP = theCutOffIHCLP;
	return(TRUE);

}

/****************************** SetIHCLPOrder *********************************/

/*
 * This function sets the module's iHCLPOrder parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIHCLPOrder_BasilarM_ZilanyBruce(int theIHCLPOrder)
{
	static const WChar	*funcName = wxT("SetIHCLPOrder_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->iHCLPOrder = theIHCLPOrder;
	return(TRUE);

}

/****************************** SetCFList *************************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_ZilanyBruce(CFListPtr theCFList)
{
	static const WChar	*funcName = wxT("SetCFList_BasilarM_ZilanyBruce");

	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckPars_CFList(theCFList)) {
		NotifyError(wxT("%s: Centre frequency structure not correctly set."),
		  funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(theCFList, wxT("internal_dynamic"), NULL)) {
		NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
		return(FALSE);
	}
	theCFList->bParList->pars[BANDWIDTH_PAR_MODE].enabled = FALSE;
	if (bMZBPtr->cFList != NULL)
		Free_CFList(&bMZBPtr->cFList);
	bMZBPtr->updateProcessVariablesFlag = TRUE;
	bMZBPtr->cFList = theCFList;
	return(TRUE);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 */

CFListPtr
GetCFListPtr_BasilarM_ZilanyBruce(void)
{
	static const WChar	*funcName = wxT("GetCFListPtr_BasilarM_ZilanyBruce");
	if (bMZBPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMZBPtr->cFList == NULL) {
		NotifyError(wxT("%s: CFList data structure has not been correctly set.  ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(bMZBPtr->cFList);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_BasilarM_ZilanyBruce(void)
{

	DPrint(wxT("Zilany & Bruce (JASA 2006, 2007) BM Filter ")
			wxT("Module Parameters:-\n"));
	DPrint(wxT("\tOutput mode = %s,\n"), bMZBPtr->outputModeList[bMZBPtr->
	  outputMode].name);
	DPrint(wxT("\tMicro Pascal Input = %s,\n"), BooleanList_NSpecLists(bMZBPtr->
	  microPaInput)->name);
	DPrint(wxT("\tControl path parameters:\n"));
	DPrint(wxT("\ts0 = %g,"), bMZBPtr->s0);
	DPrint(wxT("\tx1 = %g,"), bMZBPtr->x1);
	DPrint(wxT("\ts1 = %g\n"), bMZBPtr->s1);
	DPrint(wxT("\tshiftCP = %g,"), bMZBPtr->shiftCP);
	DPrint(wxT("\tcutoffCP = %g,"), bMZBPtr->cutOffCP);
	DPrint(wxT("\tLPOrder = %d\n"), bMZBPtr->lPOrder);
	DPrint(wxT("\tdc = %g,"), bMZBPtr->dc);
	DPrint(wxT("\tR_C1 = %g,"), bMZBPtr->rC1);
	DPrint(wxT("\tC_OHC = %g\n"), bMZBPtr->cOHC);
	DPrint(wxT("\tSignal path C1 filter parameters:\n"));
	DPrint(wxT("\tnBorder = %d\n"), bMZBPtr->nBorder);
	DPrint(wxT("\tInner hair cell (IHC) parameters:\n"));
	DPrint(wxT("\tohcasym = %g,"), bMZBPtr->ohcasym);
	DPrint(wxT("\tihcasym = %g,\n"), bMZBPtr->ihcasym);
	DPrint(wxT("\tA_IHC0 = %g,"), bMZBPtr->aIHC0);
	DPrint(wxT("\tB_IHC = %g,\n"), bMZBPtr->bIHC);
	DPrint(wxT("\tC_IHC = %g,\n"), bMZBPtr->cIHC);
	DPrint(wxT("\tcutOff_IHCLP = %g,"), bMZBPtr->cutOffIHCLP);
	DPrint(wxT("\tIHCLPorder = %d.\n"), bMZBPtr->iHCLPOrder);
	PrintPars_CFList(bMZBPtr->cFList);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_ZilanyBruce(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_BasilarM_ZilanyBruce");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bMZBPtr = (BMZBPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_BasilarM_ZilanyBruce(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_BasilarM_ZilanyBruce");

	if (!SetParsPointer_BasilarM_ZilanyBruce(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_ZilanyBruce(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMZBPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_BasilarM_ZilanyBruce;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_ZilanyBruce;
	theModule->PrintPars = PrintPars_BasilarM_ZilanyBruce;
	theModule->RunProcess = RunModel_BasilarM_ZilanyBruce;
	theModule->SetParsPointer = SetParsPointer_BasilarM_ZilanyBruce;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 * The 'CheckRamp_SignalData()' can be used instead of the
 * 'CheckInit_SignalData()' routine if the signal must be ramped for
 * the process.
 */

BOOLN
CheckData_BasilarM_ZilanyBruce(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_BasilarM_ZilanyBruce");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_BasilarM_ZilanyBruce(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_BasilarM_ZilanyBruce");
	int		i, cFIndex;
	ChanLen	maxDelayedSamples;
	Float	centreFreq, CAgain, dt;
	BMZBPtr	p = bMZBPtr;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);

	dt = outSignal->dt;
	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_BasilarM_ZilanyBruce();
		p->numChannels = outSignal->numChannels;
		if ((p->wbgt = (ZBWBGTCoeffsPtr *) calloc(p->numChannels,
		  sizeof(ZBWBGTCoeffsPtr))) == NULL) {
			NotifyError(wxT("%s: Out of memory (wbgt)."), funcName);
			return(FALSE);
		}
		if ((p->c1Filter = (ZBGCCoeffs *) calloc(p->numChannels,
		  sizeof(ZBGCCoeffs))) == NULL) {
			NotifyError(wxT("%s: Out of memory (c1Filter)."), funcName);
			return(FALSE);
		}
		if ((p->c2Filter = (ZBGCCoeffs *) calloc(p->numChannels,
		  sizeof(ZBGCCoeffs))) == NULL) {
			NotifyError(wxT("%s: Out of memory (c2Filter)."), funcName);
			return(FALSE);
		}
		if ((p->ohcLowPass = (TLowPass *) calloc(p->numChannels,
		  sizeof(TLowPass))) == NULL) {
			NotifyError(wxT("%s: Out of memory (ohcLowPass)."), funcName);
			return(FALSE);
		}
		if ((p->ihcLowPass = (TLowPass *) calloc(p->numChannels,
		  sizeof(TLowPass))) == NULL) {
			NotifyError(wxT("%s: Out of memory (ihcLowPass)."), funcName);
			return(FALSE);
		}
		p->tmpgain = Init_EarObject(wxT("NULL"));
		if (!InitOutSignal_EarObject(p->tmpgain, outSignal->numChannels,
		  outSignal->length, outSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise 'y' memory."), funcName);
			return(FALSE);
		}
		maxDelayedSamples = (ChanLen) ceil(delay_cat_Utility_ZilanyBruce(
		  p->cFList->frequency[0]) / outSignal->dt);
		if ((p->tempSamples = (ChanData *) calloc(maxDelayedSamples, sizeof(ChanData))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for temporary samples (%d)"), funcName,
			  maxDelayedSamples);
			return(FALSE);
		}
		p->delayedSamples = Init_EarObject(wxT("NULL"));
		if (!InitOutSignal_EarObject(p->delayedSamples, outSignal->numChannels,
		  maxDelayedSamples, outSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise delayed samples memory."), funcName);
			return(FALSE);
		}
		for (i = 0; i < p->numChannels; i++) {
			cFIndex = i / _InSig_EarObject(data, 0)->interleaveLevel;
			centreFreq = p->cFList->frequency[cFIndex];
		    CAgain = 52.0 / 2.0 * (tanh(2.2 * log10(centreFreq / p->cutOffCP) +
		      0.15) + 1);
		    if(CAgain < 15.0)
		    	CAgain = 15.0;
			if ((p->wbgt[i] = InitZBWBGTCoeffs_Utility_ZilanyBruce(p->wborder,
			  dt, centreFreq, CAgain, p->cOHC)) == NULL) {
				NotifyError(wxT("%s: Could not set wbgt[%d]."), funcName, i);
				return(FALSE);
			}
			InitLowPass_Utility_Zhang(&p->ohcLowPass[i], dt, p->cutOffCP, 1.0,
			  p->lPOrder);
			InitLowPass_Utility_Zhang(&p->ihcLowPass[i], dt, p->cutOffIHCLP, 1.0,
			  p->iHCLPOrder);
		}
		SetLocalInfoFlag_SignalData(outSignal, TRUE);
		SetInfoChannelTitle_SignalData(outSignal, wxT("Frequency (Hz)"));
		SetInfoChannelLabels_SignalData(outSignal, p->cFList->frequency);
		SetInfoCFArray_SignalData(outSignal, p->cFList->frequency);
		p->updateProcessVariablesFlag = FALSE;
		p->cFList->updateFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		ResetOutSignal_EarObject(p->tmpgain);
		ResetOutSignal_EarObject(p->delayedSamples);
		for (i = 0; i < p->numChannels; i++) {
			ResetZBWBGTCoeffs_Utility_ZilanyBruce(p->wbgt[i]);
			cFIndex = i / _InSig_EarObject(data, 0)->interleaveLevel;
			centreFreq = p->cFList->frequency[cFIndex];
			ResetZBGCCoeffs_Utility_ZilanyBruce(&p->c1Filter[i], dt, centreFreq,
			  p->wbgt[i]->bmTaumax, ZB_GC_C1);
			ResetZBGCCoeffs_Utility_ZilanyBruce(&p->c2Filter[i], dt, centreFreq,
			  p->wbgt[i]->bmTaumax, ZB_GC_C2);
		}
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_BasilarM_ZilanyBruce(void)
{
	int		i;

	if (bMZBPtr->wbgt) {
		for (i = 0; i < bMZBPtr->numChannels; i++)
			FreeZBWBGTCoeffs_Utility_ZilanyBruce(&bMZBPtr->wbgt[i]);
		free(bMZBPtr->wbgt);
		bMZBPtr->wbgt = NULL;
	}
	if (bMZBPtr->c1Filter) {
		free(bMZBPtr->c1Filter);
		bMZBPtr->c1Filter = NULL;
	}
	if (bMZBPtr->c2Filter) {
		free(bMZBPtr->c2Filter);
		bMZBPtr->c2Filter = NULL;
	}
	if (bMZBPtr->tempSamples) {
		free(bMZBPtr->tempSamples);
		bMZBPtr->tempSamples = NULL;
	}
	if (bMZBPtr->ohcLowPass) {
		free(bMZBPtr->ohcLowPass);
		bMZBPtr->ohcLowPass = NULL;
	}
	if (bMZBPtr->ihcLowPass) {
		free(bMZBPtr->ihcLowPass);
		bMZBPtr->ihcLowPass = NULL;
	}
	Free_EarObject(&bMZBPtr->tmpgain);
	Free_EarObject(&bMZBPtr->delayedSamples);
	return(TRUE);

}

/****************************** RunModel **************************************/

/*
 * This routine allocates memory for the output signal, if necessary,
 * and generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by
 * calling the appropriate checking routines.
 * It can be called repeatedly with different parameter values if
 * required.
 * Stimulus generation only sets the output signal, the input signal
 * is not used.
 * With repeated calls the Signal memory is only allocated once, then
 * re-used.
 * The delay algorithm using the 'delaypoint' variable is carried out backwards so that
 * additional signal storages space is not required.
 */

#define C1_FILTER()		(ChirpFilt_Utility_ZilanyBruce(input, &p->c1Filter[chan], \
						  rsigma))
#define C1_VIHC(FILT)	(NLogarithm_Utility_ZilanyBruce(p->cIHC * (FILT), \
						  p->aIHC0, p->ihcasym))
#define C2_FILTER()		(ChirpFilt_Utility_ZilanyBruce(input, &p->c2Filter[chan], \
						  1.0 / wbgt->ratiobm))
#define C2_VIHC(FILT)	(-NLogarithm_Utility_ZilanyBruce((FILT) * fabs((FILT)) * \
						  cF / 10.0 * cF / p->bIHC, 2.0 * p->aIHC0, 1.0))

BOOLN
RunModel_BasilarM_ZilanyBruce(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_BasilarM_ZilanyBruce");
	register ChanData	 *inPtr, *outPtr = NULL, *tmpgain, *dSamples, *tSamples, input;
	uShort	totalChannels;
	int		chan, grd;
	Float	cF, wbout1, wbout, ohcnonlinout, ohcout, tmptauc1, dt, tauc1, rsigma;
	Float	wb_gain, c1filterouttmp, c2filterouttmp;
	ChanLen	n, delaypoint;
	SignalDataPtr	inSignal, outSignal;
	BMZBPtr	p = bMZBPtr;
	ZBWBGTCoeffsPtr	wbgt;

	inSignal = _InSig_EarObject(data, 0);
	if (!data->threadRunFlag) {
		if (!CheckData_BasilarM_ZilanyBruce(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Zilany & Bruce (JASA 2006, 2007) ")
				wxT("BM filter process"));
		totalChannels = p->cFList->numChannels * _InSig_EarObject(data, 0)->
		  numChannels;
		if (!InitOutTypeFromInSignal_EarObject(data, totalChannels)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_BasilarM_ZilanyBruce(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	dt = outSignal->dt;
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = inSignal->channel[chan % inSignal->interleaveLevel];
		outPtr = outSignal->channel[chan];
		tmpgain = _OutSig_EarObject(p->tmpgain)->channel[chan];
		wbgt = p->wbgt[chan];
		cF = p->cFList->frequency[chan];
		for (n = 0; n < data->outSignal->length; n++, inPtr++, tmpgain++) {
			input = (p->microPaInput)? *inPtr * 1e-6: *inPtr;
			/* Control-path filter */
			wbout1 = WbGammaTone_Utility_ZilanyBruce(input, wbgt, dt);
			wbout  = pow((wbgt->tau / wbgt->TauWBMax), wbgt->order) * wbout1 * 10e3 *
			  MAXIMUM(1.0, cF / 5e3);
			ohcnonlinout = Boltzman_Utility_ZilanyBruce(wbout, p->ohcasym, p->s0,
			  p->s1, p->x1); /* pass the control signal through OHC Nonlinear Function */
			 /* The next line uses a function From UtZhang */
			ohcout = p->ohcLowPass[chan].Run(&(p->ohcLowPass[chan]), ohcnonlinout);
			tmptauc1 = NLafterohc_Utility_ZilanyBruce(ohcout, wbgt->bmTaumin,
			  wbgt->bmTaumax, p->ohcasym); /* nonlinear function after OHC low-pass filter */
			tauc1    = p->cOHC * (tmptauc1 - wbgt->bmTaumin) + wbgt->bmTaumin;  /* time -constant for the signal-path C1 filter */
			rsigma   = 1.0 / tauc1 - 1 / wbgt->bmTaumax; /* shift of the location of poles of the C1 filter from the initial positions */

			if (1.0 / tauc1 < 0.0) {
				NotifyError(wxT("%s: The poles are in the right-half plane; system ")
				  wxT("is unstable.\n"), funcName);
				return(FALSE);
			}
		    Set_tau_Utility_ZilanyBruce(wbgt, tauc1);

		    wb_gain = gain_groupdelay_Utility_ZilanyBruce(dt, wbgt->cF, cF, wbgt->tau,
		    	  &wbgt->grdelay);
			grd = wbgt->grdelay;
	        if ((grd + n) < outSignal->length)
		         *(tmpgain + grd) = wb_gain;
	        if (*tmpgain == 0)
				*tmpgain = wbgt->lasttmpgain;
			wbgt->gain = *tmpgain;
			wbgt->lasttmpgain = wbgt->gain;

			switch (p->outputMode) {
			case BM_ZILANYBRUCE_OUTPUTMODE_C1_FILTER:
				*outPtr++ = C1_FILTER();
				break;
			case BM_ZILANYBRUCE_OUTPUTMODE_C2_FILTER:
				*outPtr++ = C2_FILTER();
				break;
			case BM_ZILANYBRUCE_OUTPUTMODE_FILTER:
				*outPtr++ = C1_FILTER() + C2_FILTER();
				break;
			case BM_ZILANYBRUCE_OUTPUTMODE_C1_VIHC:
				c1filterouttmp = C1_FILTER();
				*outPtr++ = C1_VIHC(c1filterouttmp);
				break;
			case BM_ZILANYBRUCE_OUTPUTMODE_C2_VIHC:
				c2filterouttmp = C2_FILTER();
				*outPtr++ = C2_VIHC(c2filterouttmp);
				break;
			case BM_ZILANYBRUCE_OUTPUTMODE_VIHC:
				c1filterouttmp = C1_FILTER();
				c2filterouttmp = C2_FILTER();
				*outPtr++ = p->ihcLowPass[chan].Run(&(p->ihcLowPass[chan]),
				  C1_VIHC(c1filterouttmp) + C2_VIHC(c2filterouttmp));
				break;
			}

		}
		delaypoint = MAXIMUM(0,(ChanLen) ceil(delay_cat_Utility_ZilanyBruce(cF) / dt));
		if (delaypoint) {
			outPtr = outSignal->channel[chan] + outSignal->length - 1;
			tSamples = p->tempSamples + delaypoint - 1;
			for (n = delaypoint; n; n--)
				*tSamples-- = *outPtr--;
			outPtr = outSignal->channel[chan] + outSignal->length - 1;
			for (n = data->outSignal->length - delaypoint; n; n--)
				*outPtr-- = *(outPtr - delaypoint);
			dSamples = _OutSig_EarObject(p->delayedSamples)->channel[chan] + delaypoint - 1;
			tSamples = p->tempSamples + delaypoint - 1;
			for (n = delaypoint; n ; n--) {
				*outPtr-- = *dSamples;
				*dSamples-- = *tSamples--;
			}
		}
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

#undef C1_FILTER
#undef C1_VIHC
#undef C2_FILTER
#undef C2_VIHC

