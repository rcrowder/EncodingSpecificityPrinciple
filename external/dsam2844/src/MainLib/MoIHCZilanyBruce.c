/**********************
 *
 * File:		MoIHCZilanyBruce.c
 * Purpose:		This is the Zilany and Bruce BM filter module.
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
#include "FiParFile.h"
#include "MoIHCZilanyBruce.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

ZBHCPtr	zBHCPtr = NULL;

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
Free_IHC_ZilanyBruce(void)
{
	/* static const WChar	*funcName = wxT("Free_IHC_ZilanyBruce"); */

	if (zBHCPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IHC_ZilanyBruce();
	if (zBHCPtr->parList)
		FreeList_UniParMgr(&zBHCPtr->parList);
	if (zBHCPtr->parSpec == GLOBAL) {
		free(zBHCPtr);
		zBHCPtr = NULL;
	}
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
Init_IHC_ZilanyBruce(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_IHC_ZilanyBruce");

	if (parSpec == GLOBAL) {
		if (zBHCPtr != NULL)
			Free_IHC_ZilanyBruce();
		if ((zBHCPtr = (ZBHCPtr) malloc(sizeof(ZBHC))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (zBHCPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	zBHCPtr->parSpec = parSpec;
	zBHCPtr->updateProcessVariablesFlag = TRUE;
	zBHCPtr->spont = 50.0;
	zBHCPtr->Ass = 350.0;
	zBHCPtr->tauST = 60.0e-3;
	zBHCPtr->tauR = 2.0e-3;
	zBHCPtr->Ar_Ast = 6.0;
	zBHCPtr->PImax = 0.6;

	if (!SetUniParList_IHC_ZilanyBruce()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHC_ZilanyBruce();
		return(FALSE);
	}
	zBHCPtr->synapse = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHC_ZilanyBruce(void)
{
	static const WChar	*funcName = wxT("SetUniParList_IHC_ZilanyBruce");
	UniParPtr	pars;

	if ((zBHCPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHC_ZILANYBRUCE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = zBHCPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHC_ZILANYBRUCE_SPONT], wxT("SPONT"),
	  wxT("Spontaneous firing rate - eq. 10 (spikes/s)."),
	  UNIPAR_REAL,
	  &zBHCPtr->spont, NULL,
	  (void * (*)) SetSpont_IHC_ZilanyBruce);
	SetPar_UniParMgr(&pars[IHC_ZILANYBRUCE_ASS], wxT("A_SS"),
	  wxT("Steady-state firing rate - eq. 10 (spikes/s)."),
	  UNIPAR_REAL,
	  &zBHCPtr->Ass, NULL,
	  (void * (*)) SetAss_IHC_ZilanyBruce);
	SetPar_UniParMgr(&pars[IHC_ZILANYBRUCE_TAUST], wxT("TAU_ST"),
	  wxT("Short-term time constant - eq. 10 (s)."),
	  UNIPAR_REAL,
	  &zBHCPtr->tauST, NULL,
	  (void * (*)) SetTauST_IHC_ZilanyBruce);
	SetPar_UniParMgr(&pars[IHC_ZILANYBRUCE_TAUR], wxT("TAU_R"),
	  wxT("Rapid time constant - eq. 10 (s)."),
	  UNIPAR_REAL,
	  &zBHCPtr->tauR, NULL,
	  (void * (*)) SetTauR_IHC_ZilanyBruce);
	SetPar_UniParMgr(&pars[IHC_ZILANYBRUCE_AR_AST], wxT("AR_AST"),
	  wxT("Rapid response amplitude to short-term response amplitude ratio."),
	  UNIPAR_REAL,
	  &zBHCPtr->Ar_Ast, NULL,
	  (void * (*)) SetAr_Ast_IHC_ZilanyBruce);
	SetPar_UniParMgr(&pars[IHC_ZILANYBRUCE_PIMAX], wxT("PI_MAX"),
	  wxT("Permeability at high sound level."),
	  UNIPAR_REAL,
	  &zBHCPtr->PImax, NULL,
	  (void * (*)) SetPImax_IHC_ZilanyBruce);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHC_ZilanyBruce(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_IHC_ZilanyBruce");

	if (zBHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (zBHCPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(zBHCPtr->parList);

}

/****************************** SetSpont **************************************/

/*
 * This function sets the module's spont parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSpont_IHC_ZilanyBruce(Float theSpont)
{
	static const WChar	*funcName = wxT("SetSpont_IHC_ZilanyBruce");

	if (zBHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zBHCPtr->updateProcessVariablesFlag = TRUE;
	zBHCPtr->spont = theSpont;
	return(TRUE);

}

/****************************** SetAss ***************************************/

/*
 * This function sets the module's Ass parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAss_IHC_ZilanyBruce(Float theAss)
{
	static const WChar	*funcName = wxT("SetAss_IHC_ZilanyBruce");

	if (zBHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zBHCPtr->updateProcessVariablesFlag = TRUE;
	zBHCPtr->Ass = theAss;
	return(TRUE);

}

/****************************** SetTauST **************************************/

/*
 * This function sets the module's tauST parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTauST_IHC_ZilanyBruce(Float theTauST)
{
	static const WChar	*funcName = wxT("SetTauST_IHC_ZilanyBruce");

	if (zBHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zBHCPtr->updateProcessVariablesFlag = TRUE;
	zBHCPtr->tauST = theTauST;
	return(TRUE);

}

/****************************** SetTauR ***************************************/

/*
 * This function sets the module's tauR parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTauR_IHC_ZilanyBruce(Float theTauR)
{
	static const WChar	*funcName = wxT("SetTauR_IHC_ZilanyBruce");

	if (zBHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zBHCPtr->updateProcessVariablesFlag = TRUE;
	zBHCPtr->tauR = theTauR;
	return(TRUE);

}

/****************************** SetAr_Ast *********************************/

/*
 * This function sets the module's Ar_Ast parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAr_Ast_IHC_ZilanyBruce(Float theAr_Ast)
{
	static const WChar	*funcName = wxT("SetAr_Ast_IHC_ZilanyBruce");

	if (zBHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zBHCPtr->updateProcessVariablesFlag = TRUE;
	zBHCPtr->Ar_Ast = theAr_Ast;
	return(TRUE);

}

/****************************** SetPImax **************************************/

/*
 * This function sets the module's PImax parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPImax_IHC_ZilanyBruce(Float thePImax)
{
	static const WChar	*funcName = wxT("SetPImax_IHC_ZilanyBruce");

	if (zBHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zBHCPtr->updateProcessVariablesFlag = TRUE;
	zBHCPtr->PImax = thePImax;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_IHC_ZilanyBruce(void)
{
	/*static const WChar	*funcName = wxT("PrintPars_IHC_ZilanyBruce");*/

	DPrint(wxT("Zilany and Bruce IHC Module Parameters:-\n"));
	DPrint(wxT("\tSpontaneous rate of fiber, spont = %g (spikes/s)\n"),
	  zBHCPtr->spont);
	DPrint(wxT("\tSteady-state rate, Ass = %g (spikes/s),\n"), zBHCPtr->Ass);
	DPrint(wxT("\tShort-term time constant, tauST = %g (ms),\n"), zBHCPtr->tauST);
	DPrint(wxT("\tRapid time constant tauR = %g (ms),\n"), zBHCPtr->tauR);
	DPrint(wxT("\tResponse ratio, A_R / A_ST = %g,\n"), zBHCPtr->Ar_Ast);
	DPrint(wxT("\tPermeability at high sound level, Pi_Max = %g.\n"), zBHCPtr->PImax);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHC_ZilanyBruce(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHC_ZilanyBruce");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	zBHCPtr = (ZBHCPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_IHC_ZilanyBruce(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHC_ZilanyBruce");

	if (!SetParsPointer_IHC_ZilanyBruce(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHC_ZilanyBruce(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = zBHCPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_IHC_ZilanyBruce;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_ZilanyBruce;
	theModule->PrintPars = PrintPars_IHC_ZilanyBruce;
	theModule->RunProcess = RunModel_IHC_ZilanyBruce;
	theModule->SetParsPointer = SetParsPointer_IHC_ZilanyBruce;
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
CheckData_IHC_ZilanyBruce(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_IHC_ZilanyBruce");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** InitSynapse ***********************************/

/*
 * Original name: initSynapse
 * Original file: synapse.c
 * The 'error' variable that awas in the original code was removed as it was,
 * not set anywhere in the routine.
 */

void
InitSynapse_IHC_ZilanyBruce(ZBSynapsePtr syn, Float cf)
{
	Float cf_factor,kslope,Asp,PTS,Aon,AR,AST,Prest,gamma1,gamma2,k1,k2;
	Float VI0,VI1,alpha,beta,theta1,theta2,theta3,vsat = 0.0;
    Float cfsat,cfslope,cfconst, tmpst;
	ZBHCPtr	p = zBHCPtr;

    /*        cf_factor = __min(1e3,pow(10,0.29*cf/1e3 + 0.4));
	*/
	if (p->spont >= 50)
		cf_factor = MINIMUM(1e3,pow(10,0.29*cf/1e3 + 0.4));
	else {
		cfslope = pow(p->spont,0.19)*pow(10,-0.87);
		cfconst = 0.1*pow(log10(p->spont),2)+0.56*log10(p->spont)-0.84;
		cfsat = pow(10,cfslope*8965.5/1e3 + cfconst);    /*find saturation at saturation freq: 8965.5 Hz*/
		cf_factor = MINIMUM(cfsat,pow(10,cfslope*cf/1e3 + cfconst));
	};                                       /*added by Tim Zeyl June 14 2006*/

	kslope = (1+50.0)/(5+50.0)*cf_factor*20.0*p->PImax;

	Asp    = p->spont;              /* Spontaneous Firing Rate  eq.10 */
	PTS    = 1.0+9.0*50.0/(9.0+50.0);    /* Peak to Steady State Ratio, characteristic of PSTH */

	/* now get the other parameters */
	Aon    = PTS*p->Ass;                          /* Onset rate = Ass+Ar+Ast eq.10 */
	AR     = (Aon-p->Ass)*p->Ar_Ast/(1+p->Ar_Ast);      /* Rapid component magnitude: eq.10 */
	AST    = Aon-p->Ass-AR;                       /* Short time component: eq.10 */
	Prest  = p->PImax/Aon*Asp;                    /* eq.A15 */
	syn->CG  = (Asp*(Aon-Asp))/(Aon*Prest*(1-Asp/p->Ass));    /* eq.A16 */
	gamma1 = syn->CG/Asp;							/* eq.A19 */
	gamma2 = syn->CG/p->Ass;						/* eq.A20 */
	k1     = -1/p->tauR;							/* eq.8 & eq.10 */
	k2     = -1/p->tauST;							/* eq.8 & eq.10 */
	/* eq.A21 & eq.A22 */
	VI0    = (1-p->PImax/Prest)/(gamma1*(AR*(k1-k2)/syn->CG/p->PImax+k2/Prest/gamma1-k2/p->PImax/gamma2));
	VI1    = (1-p->PImax/Prest)/(gamma1*(AST*(k2-k1)/syn->CG/p->PImax+k1/Prest/gamma1-k1/p->PImax/gamma2));
	syn->VI  = (VI0+VI1)/2;
	alpha  = gamma2/k1/k2;       /* eq.A23,eq.A24 or eq.7 */
	beta   = -(k1+k2)*alpha;     /* eq.A23 or eq.7 */
	theta1 = alpha*p->PImax/syn->VI;
	theta2 = syn->VI/p->PImax;
	theta3 = gamma2-1/p->PImax;

	syn->PL  = ((beta-theta2*theta3)/theta1-1)*p->PImax;	/* eq.4' */
	syn->PG  = 1/(theta3-1/syn->PL);						/* eq.5' */
	syn->VL  = theta1*syn->PL*syn->PG;						/* eq.3' */
	syn->CI  = Asp/Prest;									/* CI at rest, from eq.A3,eq.A12 */
	syn->CL  = syn->CI*(Prest+syn->PL)/syn->PL;				/* CL at rest, from eq.1 */

	if (kslope>=0)
		vsat = kslope+Prest;
	tmpst  = log(2)*vsat/Prest;
	if(tmpst<400)
		syn->synstrength = log(exp(tmpst)-1);
	else
		syn->synstrength = tmpst;
	syn->synslope = Prest/log(2)*syn->synstrength;
	syn->slopeOverStrength = syn->synslope / syn->synstrength;
	/*if (spont<0)
		spont = 50; LPO:  what was this for ??*/

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_IHC_ZilanyBruce(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_IHC_ZilanyBruce");
	int		i, cFIndex;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);
	ZBHCPtr	p = zBHCPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_IHC_ZilanyBruce();
		if ((p->synapse = (ZBSynapse *) calloc(outSignal->numChannels,
		  sizeof(ZBSynapse))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory for synapse coefficients ")
			  wxT("array."), funcName);
		 	return(FALSE);
		}
		SetLocalInfoFlag_SignalData(outSignal, TRUE);
		CopyInfo_SignalData(outSignal, _InSig_EarObject(data, 0));
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < outSignal->numChannels; i++) {
			cFIndex = i / outSignal->interleaveLevel;
			InitSynapse_IHC_ZilanyBruce(&p->synapse[i], _OutSig_EarObject(
			  data)->info.cFArray[cFIndex]);
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
FreeProcessVariables_IHC_ZilanyBruce(void)
{
	/*static const WChar	*funcName = wxT("FreeProcessVariables_IHC_ZilanyBruce");*/

	if (zBHCPtr->synapse) {
		free(zBHCPtr->synapse);
		zBHCPtr->synapse = NULL;
	}
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
 */

BOOLN
RunModel_IHC_ZilanyBruce(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_IHC_ZilanyBruce");
	register ChanData	 *inPtr, *outPtr, tmp, PPI, CIlast;
	ChanLen	i;
	int		chan;
	SignalDataPtr	inSignal, outSignal;
	ZBSynapsePtr	syn;
	ZBHCPtr	p = zBHCPtr;

	inSignal = _InSig_EarObject(data, 0);
	if (!data->threadRunFlag) {
		if (!CheckData_IHC_ZilanyBruce(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Zilany Bruce IHC module process"));
		if (!InitOutSignal_EarObject(data, inSignal->numChannels, inSignal->length,
		  inSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_IHC_ZilanyBruce(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = inSignal->channel[chan];
		outPtr = outSignal->channel[chan];
		syn = &p->synapse[chan];
		for (i = 0; i < data->outSignal->length; i++) {
			tmp = syn->synstrength * *inPtr++;
			if(tmp < 400)
				tmp = log(1.0 + exp(tmp));
			PPI = syn->slopeOverStrength * tmp;

			CIlast = syn->CI;
			syn->CI = syn->CI + (outSignal->dt / syn->VI) * (-PPI*syn->CI +
			  syn->PL * (syn->CL - syn->CI));
			syn->CL = syn->CL + (outSignal->dt / syn->VL) * (-syn->PL *
			  (syn->CL - CIlast) + syn->PG * (syn->CG - syn->CL));

			if (syn->CI < 0) {
				tmp = 1.0 / syn->PG + 1.0 /syn->PL + 1.0 / PPI;
				syn->CI = syn->CG / (PPI * tmp);
				syn->CL = syn->CI * (PPI + syn->PL) / syn->PL;
			};
			*outPtr++ = syn->CI * PPI;
		}

	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

