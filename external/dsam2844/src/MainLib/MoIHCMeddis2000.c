/**********************
 *
 * File:		MoIHCMeddis2000.c
 * Purpose:		This module contains the routines for synaptic adaptation of
 *				inner hair cells. It is a stochastic model, and with the
 *				exception of a probabilistic output option, is the same as the
 *				Meddis 95 module, but without the transmitter release stage.
 *				It is intended to be used with the Calcium transmitter release
 *				stage.
 * Comments:	Written using ModuleProducer version 1.2.5 (May  7 1999).
 * Authors:		L.P. O'Mard, revised Chris Sumner (7th May 2000).
 * Created:		16 Mar 2000
 * Updated:
 * Copyright:	(c) 2000, 2010 Lowel P. O'Mard
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
#include <string.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtString.h"
#include "UtRandom.h"
#include "GeNSpecLists.h"
#include "MoIHCMeddis2000.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

HairCell2Ptr	hairCell2Ptr = NULL;

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
Free_IHC_Meddis2000(void)
{
	if (hairCell2Ptr == NULL)
		return(FALSE);
	FreeProcessVariables_IHC_Meddis2000();
	if (hairCell2Ptr->parList)
		FreeList_UniParMgr(&hairCell2Ptr->parList);
	if (hairCell2Ptr->diagModeList)
		free(hairCell2Ptr->diagModeList);
	if (hairCell2Ptr->parSpec == GLOBAL) {
		free(hairCell2Ptr);
		hairCell2Ptr = NULL;
	}
	return(TRUE);

}

/****************************** InitOpModeList ********************************/

/*
 * This function initialises the 'opMode' list array
 */

BOOLN
InitOpModeList_IHC_Meddis2000(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("SPIKE"),	IHC_MEDDIS2000_OPMODE_SPIKE },
			{ wxT("PROB"),	IHC_MEDDIS2000_OPMODE_PROB },
			{ NULL,			IHC_MEDDIS2000_OPMODE_NULL },
		};
	hairCell2Ptr->opModeList = modeList;
	return(TRUE);

}

/****************************** InitCaCondModeList ****************************/

/*
 * This function initialises the 'caCondMode' list array
 */

BOOLN
InitCaCondModeList_IHC_Meddis2000(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("ORIGINAL"),		IHC_MEDDIS2000_CACONDMODE_ORIGINAL },
			{ wxT("REVISION_1"),	IHC_MEDDIS2000_CACONDMODE_REVISION1 },
			{ NULL,					IHC_MEDDIS2000_CACONDMODE_NULL },
		};
	hairCell2Ptr->caCondModeList = modeList;
	return(TRUE);

}

/****************************** InitCleftReplenishModeList ********************/

/*
 * This function initialises the 'cleftReplenishMode' list array
 */

BOOLN
InitCleftReplenishModeList_IHC_Meddis2000(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("ORIGINAL"),	IHC_MEDDIS2000_CLEFTREPLENISHMODE_ORIGINAL },
			{ wxT("UNITY"),		IHC_MEDDIS2000_CLEFTREPLENISHMODE_UNITY },
			{ NULL,				IHC_MEDDIS2000_CLEFTREPLENISHMODE_NULL },
		};
	hairCell2Ptr->cleftReplenishModeList = modeList;
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
Init_IHC_Meddis2000(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_IHC_Meddis2000");

	if (parSpec == GLOBAL) {
		if (hairCell2Ptr != NULL)
			Free_IHC_Meddis2000();
		if ((hairCell2Ptr = (HairCell2Ptr) malloc(sizeof(HairCell2))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (hairCell2Ptr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->parSpec = parSpec;
	hairCell2Ptr->opMode = IHC_MEDDIS2000_OPMODE_PROB;
	hairCell2Ptr->diagMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	hairCell2Ptr->caCondMode = IHC_MEDDIS2000_CACONDMODE_ORIGINAL;
	hairCell2Ptr->cleftReplenishMode =
	  IHC_MEDDIS2000_CLEFTREPLENISHMODE_ORIGINAL;
	hairCell2Ptr->ranSeed = -1;
	hairCell2Ptr->CaVrev = 0.066;
	hairCell2Ptr->betaCa = 400.0;
	hairCell2Ptr->gammaCa = 130.0;
	hairCell2Ptr->pCa = 3.0;
	hairCell2Ptr->GCaMax = 8e-09;
	hairCell2Ptr->perm_Ca0 = 4.48e-11;
	hairCell2Ptr->perm_z = 2e+32;
	hairCell2Ptr->tauCaChan = 1e-4;
	hairCell2Ptr->tauConcCa = 1e-4;
	hairCell2Ptr->maxFreePool_M = 10;
	hairCell2Ptr->replenishRate_y = 10.0;
	hairCell2Ptr->lossRate_l = 2580.0;
	hairCell2Ptr->reprocessRate_x = 66.31;
	hairCell2Ptr->recoveryRate_r = 6580.0;

	InitOpModeList_IHC_Meddis2000();
	InitCaCondModeList_IHC_Meddis2000();
	InitCleftReplenishModeList_IHC_Meddis2000();

	if ((hairCell2Ptr->diagModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), hairCell2Ptr->diagFileName)) == NULL)
		return(FALSE);

	if (!SetUniParList_IHC_Meddis2000()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHC_Meddis2000();
		return(FALSE);
	}
	SetEnabledPars_IHC_Meddis2000();
	DSAM_strcpy(hairCell2Ptr->diagFileName, DEFAULT_FILE_NAME);
	hairCell2Ptr->hCChannels = NULL;
	hairCell2Ptr->fp = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHC_Meddis2000(void)
{
	static const WChar *funcName = wxT("SetUniParList_IHC_Meddis2000");
	UniParPtr	pars;

	if ((hairCell2Ptr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHC_MEDDIS2000_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = hairCell2Ptr->parList->pars;
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_OPMODE], wxT("OP_MODE"),
	  wxT("Output mode: stochastic ('spike') or probability ('prob')."),
	  UNIPAR_NAME_SPEC,
	  &hairCell2Ptr->opMode, hairCell2Ptr->opModeList,
	  (void * (*)) SetOpMode_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_DIAGMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostic mode. Outputs internal states of running model in non-")
	    wxT("threaded mode('off', 'screen' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &hairCell2Ptr->diagMode, hairCell2Ptr->diagModeList,
	  (void * (*)) SetDiagMode_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_CACONDMODE], wxT("CA_COND_MODE"),
	  wxT("Calcium conductance mode ('original' or 'revision_1')."),
	  UNIPAR_NAME_SPEC,
	  &hairCell2Ptr->caCondMode, hairCell2Ptr->caCondModeList,
	  (void * (*)) SetCaCondMode_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_CLEFTREPLENISHMODE],
	  wxT("CLEFT_REPLENISH_MODE"),
	  wxT("Cleft replenishment mode ('original' or 'unity')."),
	  UNIPAR_NAME_SPEC,
	  &hairCell2Ptr->cleftReplenishMode, hairCell2Ptr->cleftReplenishModeList,
	  (void * (*)) SetCleftReplenishMode_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 for different seed for each run)."),
	  UNIPAR_LONG,
	  &hairCell2Ptr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_CAVREV], wxT("REV_POT_ECA"),
	  wxT("Calcium reversal potential, E_Ca (Volts)."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->CaVrev, NULL,
	  (void * (*)) SetCaVrev_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_BETACA], wxT("BETA_CA"),
	  wxT("Calcium channel Boltzmann function parameter, beta."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->betaCa, NULL,
	  (void * (*)) SetBetaCa_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_GAMMACA], wxT("GAMMA_CA"),
	  wxT("Calcium channel Boltzmann function parameter, gamma."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->gammaCa, NULL,
	  (void * (*)) SetGammaCa_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_PCA], wxT("POWER_CA"),
	  wxT("Calcium channel transmitter release exponent (power)."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->pCa, NULL,
	  (void * (*)) SetPCa_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_GCAMAX], wxT("GMAX_CA"),
	  wxT("Maximum calcium conductance (Siemens)."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->GCaMax, NULL,
	  (void * (*)) SetGCaMax_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_PERM_CA0], wxT("CONC_THRESH_CA"),
	  wxT("Calcium threshold Concentration."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->perm_Ca0, NULL,
	  (void * (*)) SetPerm_Ca0_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_PERM_Z], wxT("PERM_Z"),
	  wxT("Transmitter release permeability, Z (unitless gain)"),
	  UNIPAR_REAL,
	  &hairCell2Ptr->perm_z, NULL,
	  (void * (*)) SetPerm_z_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_TAUCACHAN], wxT("TAU_M"),
	  wxT("Calcium current time constant (s)."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->tauCaChan, NULL,
	  (void * (*)) SetTauCaChan_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_TAUCONCCA], wxT("TAU_CA"),
	  wxT("Calcium ion diffusion (accumulation) time constant (s)."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->tauConcCa, NULL,
	  (void * (*)) SetTauConcCa_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_MAXFREEPOOL_M], wxT(
	  "MAX_FREE_POOL_M"),
	  wxT("Max. no. of transmitter packets in free pool (integer)."),
	  UNIPAR_INT,
	  &hairCell2Ptr->maxFreePool_M, NULL,
	  (void * (*)) SetMaxFreePool_M_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_REPLENISHRATE_Y], wxT("REPLENISH_Y"),
	  wxT("Replenishment rate (units per second)."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->replenishRate_y, NULL,
	  (void * (*)) SetReplenishRate_y_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_LOSSRATE_L], wxT("LOSS_L"),
	  wxT("Loss rate (units per second)."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->lossRate_l, NULL,
	  (void * (*)) SetLossRate_l_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_REPROCESSRATE_X], wxT("REPROCESS_X"),
	  wxT("Reprocessing rate (units per second)."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->reprocessRate_x, NULL,
	  (void * (*)) SetReprocessRate_x_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_RECOVERYRATE_R], wxT("RECOVERY_R"),
	  wxT("Recovery rate (units per second)."),
	  UNIPAR_REAL,
	  &hairCell2Ptr->recoveryRate_r, NULL,
	  (void * (*)) SetRecoveryRate_r_IHC_Meddis2000);
	return(TRUE);

}

/****************************** SetEnabledPars ********************************/

/*
 * This routine sets the enabled parameters.
 */

void
SetEnabledPars_IHC_Meddis2000(void)
{
	hairCell2Ptr->parList->pars[IHC_MEDDIS2000_MAXFREEPOOL_M].enabled =
	  (hairCell2Ptr->cleftReplenishMode ==
	  IHC_MEDDIS2000_CLEFTREPLENISHMODE_ORIGINAL);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHC_Meddis2000(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (hairCell2Ptr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(hairCell2Ptr->parList);

}

/****************************** SetOpMode *************************************/

/*
 * This function sets the module's opMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOpMode_IHC_Meddis2000(WChar * theOpMode)
{
	static const WChar	*funcName = wxT("SetOpMode_IHC_Meddis2000");
	int		specifier;

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOpMode,
		hairCell2Ptr->opModeList)) == IHC_MEDDIS2000_OPMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOpMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->opMode = specifier;
	return(TRUE);

}

/****************************** SetDiagMode ***********************************/

/*
 * This function sets the module's diagMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagMode_IHC_Meddis2000(WChar * theDiagMode)
{
	static const WChar	*funcName = wxT("SetDiagMode_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}

	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->diagMode = IdentifyDiag_NSpecLists(theDiagMode,
	  hairCell2Ptr->diagModeList);
	return(TRUE);

}

/****************************** SetCaCondMode *********************************/

/*
 * This function sets the module's caCondMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCaCondMode_IHC_Meddis2000(WChar * theCaCondMode)
{
	static const WChar	*funcName = wxT("SetCaCondMode_IHC_Meddis2000");
	int		specifier;

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theCaCondMode,
		hairCell2Ptr->caCondModeList)) == IHC_MEDDIS2000_CACONDMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theCaCondMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->caCondMode = specifier;
	SetEnabledPars_IHC_Meddis2000();
	return(TRUE);

}

/****************************** SetCleftReplenishMode *************************/

/*
 * This function sets the module's cleftReplenishMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCleftReplenishMode_IHC_Meddis2000(WChar * theCleftReplenishMode)
{
	static const WChar	*funcName = wxT("SetCleftReplenishMode_IHC_Meddis2000");
	int		specifier;

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theCleftReplenishMode,
		hairCell2Ptr->cleftReplenishModeList)) ==
		  IHC_MEDDIS2000_CLEFTREPLENISHMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName,
		  theCleftReplenishMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->cleftReplenishMode = specifier;
	SetEnabledPars_IHC_Meddis2000();
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_IHC_Meddis2000(long theRanSeed)
{
	static const WChar	*funcName = wxT("SetRanSeed_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** SetCaVrev *************************************/

/*
 * This function sets the module's CaVrev parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCaVrev_IHC_Meddis2000(Float theCaVrev)
{
	static const WChar	*funcName = wxT("SetCaVrev_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->CaVrev = theCaVrev;
	return(TRUE);

}

/****************************** SetBetaCa *************************************/

/*
 * This function sets the module's betaCa parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBetaCa_IHC_Meddis2000(Float theBetaCa)
{
	static const WChar	*funcName = wxT("SetBetaCa_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->betaCa = theBetaCa;
	return(TRUE);

}

/****************************** SetGammaCa ************************************/

/*
 * This function sets the module's gammaCa parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGammaCa_IHC_Meddis2000(Float theGammaCa)
{
	static const WChar	*funcName = wxT("SetGammaCa_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->gammaCa = theGammaCa;
	return(TRUE);

}

/****************************** SetPCa ****************************************/

/*
 * This function sets the module's pCa parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPCa_IHC_Meddis2000(Float thePCa)
{
	static const WChar	*funcName = wxT("SetPCa_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->pCa = thePCa;
	return(TRUE);

}

/****************************** SetGCaMax *************************************/

/*
 * This function sets the module's GCaMax parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGCaMax_IHC_Meddis2000(Float theGCaMax)
{
	static const WChar	*funcName = wxT("SetGCaMax_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->GCaMax = theGCaMax;
	return(TRUE);

}

/****************************** SetPerm_Ca0 ***********************************/

/*
 * This function sets the module's perm_Ca0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPerm_Ca0_IHC_Meddis2000(Float thePerm_Ca0)
{
	static const WChar	*funcName = wxT("SetPerm_Ca0_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->perm_Ca0 = thePerm_Ca0;
	return(TRUE);

}

/****************************** SetPerm_z *************************************/

/*
 * This function sets the module's Perm_z parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPerm_z_IHC_Meddis2000(Float thePerm_z)
{
	static const WChar	*funcName = wxT("SetPerm_z_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->perm_z = thePerm_z;
	return(TRUE);

}

/****************************** SetTauCaChan **********************************/

/*
 * This function sets the module's tauCaChan parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTauCaChan_IHC_Meddis2000(Float theTauCaChan)
{
	static const WChar	*funcName = wxT("SetTauCaChan_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->tauCaChan = theTauCaChan;
	return(TRUE);

}

/****************************** SetTauConcCa **********************************/

/*
 * This function sets the module's tauConcCa parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTauConcCa_IHC_Meddis2000(Float theTauConcCa)
{
	static const WChar	*funcName = wxT("SetTauConcCa_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->tauConcCa = theTauConcCa;
	return(TRUE);

}

/****************************** SetMaxFreePool_M ******************************/

/*
 * This function sets the module's maxFreePool_M parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxFreePool_M_IHC_Meddis2000(int theMaxFreePool_M)
{
	static const WChar	*funcName = wxT("SetMaxFreePool_M_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->maxFreePool_M = theMaxFreePool_M;
	return(TRUE);

}

/****************************** SetReplenishRate_y ****************************/

/*
 * This function sets the module's replenishRate_y parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetReplenishRate_y_IHC_Meddis2000(Float theReplenishRate_y)
{
	static const WChar	*funcName = wxT("SetReplenishRate_y_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->replenishRate_y = theReplenishRate_y;
	return(TRUE);

}

/****************************** SetLossRate_l *********************************/

/*
 * This function sets the module's lossRate_l parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLossRate_l_IHC_Meddis2000(Float theLossRate_l)
{
	static const WChar	*funcName = wxT("SetLossRate_l_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->lossRate_l = theLossRate_l;
	return(TRUE);

}

/****************************** SetReprocessRate_x ****************************/

/*
 * This function sets the module's reprocessRate_x parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetReprocessRate_x_IHC_Meddis2000(Float theReprocessRate_x)
{
	static const WChar	*funcName = wxT("SetReprocessRate_x_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->reprocessRate_x = theReprocessRate_x;
	return(TRUE);

}

/****************************** SetRecoveryRate_r *****************************/

/*
 * This function sets the module's recoveryRate_r parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRecoveryRate_r_IHC_Meddis2000(Float theRecoveryRate_r)
{
	static const WChar	*funcName = wxT("SetRecoveryRate_r_IHC_Meddis2000");

	if (hairCell2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->recoveryRate_r = theRecoveryRate_r;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_IHC_Meddis2000(void)
{
	DPrint(wxT("Meddis 2000 IHC Module Parameters:-\n"));
	DPrint(wxT("\tOperational mode = %s \n"),
	  hairCell2Ptr->opModeList[hairCell2Ptr->opMode].name);
	DPrint(wxT("\tDiagnostic mode = %s \n"),
	  hairCell2Ptr->diagModeList[hairCell2Ptr->diagMode].name);
	DPrint(wxT("\tCalcium conductance mode = %s\n"), hairCell2Ptr->
	  caCondModeList[hairCell2Ptr->caCondMode].name);
	DPrint(wxT("\tCleft replenishment mode = %s \n"), hairCell2Ptr->
	  cleftReplenishModeList[hairCell2Ptr->cleftReplenishMode].name);
	DPrint(wxT("\tRandom Seed = %ld \n"), hairCell2Ptr->ranSeed);
	DPrint(wxT("\tCalcium reversal potential = %g (V)\n"), hairCell2Ptr->
	  CaVrev);
	DPrint(wxT("\tBeta = %g \n"), hairCell2Ptr->betaCa);
	DPrint(wxT("\tGamma = %g \n"), hairCell2Ptr->gammaCa);
	DPrint(wxT("\tcalcium stoichiometry = %g \n"), hairCell2Ptr->pCa);
	DPrint(wxT("\tMax calcium conductance = %g (S)\n"), hairCell2Ptr->GCaMax);
	DPrint(wxT("\tZero calcium transmitter permeability = %g \n"),
	  hairCell2Ptr->perm_Ca0);
	DPrint(wxT("\tTransmitter release permeability constant, Z = %g \n"),
	  hairCell2Ptr->perm_z);
	DPrint(wxT("\tCalcium channel time constant = %g (s)\n"),
	  hairCell2Ptr->tauCaChan);
	DPrint(wxT("\tCalcium accumulation time constant = %g (s)\n"),
	  hairCell2Ptr->tauConcCa);
	DPrint(wxT("\tMaximum transmitter packets in free pool, M  = %d \n"),
	  hairCell2Ptr->maxFreePool_M);
	DPrint(wxT("\tReplenishment rate, y = %g /s\n"), hairCell2Ptr->
	  replenishRate_y);
	DPrint(wxT("\tLoss rate, l = %g /s,\tReprocessing rate, x = %g /s\n"),
	  hairCell2Ptr->lossRate_l, hairCell2Ptr->reprocessRate_x);
	DPrint(wxT("\tRecovery rate, r = %g /s\n"), hairCell2Ptr->recoveryRate_r);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHC_Meddis2000(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHC_Meddis2000");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	hairCell2Ptr = (HairCell2Ptr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHC_Meddis2000(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHC_Meddis2000");

	if (!SetParsPointer_IHC_Meddis2000(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHC_Meddis2000(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = hairCell2Ptr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_IHC_Meddis2000;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_Meddis2000;
	theModule->PrintPars = PrintPars_IHC_Meddis2000;
	theModule->ResetProcess = ResetProcess_IHC_Meddis2000;
	theModule->RunProcess = RunModel_IHC_Meddis2000;
	theModule->SetParsPointer = SetParsPointer_IHC_Meddis2000;
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
CheckData_IHC_Meddis2000(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_IHC_Meddis2000");
	Float	dt;
	BOOLN		ok;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);

	/*** Put additional checks here. ***/

	/* Checks taken and modified from MoIHC95Meddis.c (CJS 1/2/00) */
	dt = _InSig_EarObject(data, 0)->dt;
	if (dt > MEDDIS2000_MAX_DT) {
		NotifyError(wxT("%s: Maximum sampling interval exceeded."), funcName);
		return(FALSE);
	}
	ok = TRUE;
	if (hairCell2Ptr->replenishRate_y * dt >= 1.0) {
		NotifyError(wxT("%s: Replenishment rate, y = %g /s is too high for ")
		  wxT("the sampling interval."), funcName, hairCell2Ptr->replenishRate_y);
		ok = FALSE;
	}
	if (hairCell2Ptr->lossRate_l * dt >= 1.0) {
		NotifyError(wxT("%s: Loss rate, l = %g /s is too high for the sampling ")
		  wxT("interval."), funcName, hairCell2Ptr->lossRate_l);
		ok = FALSE;
	}
	if (hairCell2Ptr->recoveryRate_r * dt >= 1.0) {
		NotifyError(wxT("%s: Recovery rate, r = %g /s is too high for the ")
		  wxT("sampling interval."), funcName, hairCell2Ptr->recoveryRate_r);
		ok = FALSE;
	}
	if (hairCell2Ptr->reprocessRate_x * dt >= 1.0) {
		NotifyError(wxT("%s: Reprocess rate, x = %g /s is too high for the ")
		  wxT("sampling interval."), funcName, hairCell2Ptr->reprocessRate_x);
		ok = FALSE;
	}

	/* Additional checks added for the new code */
	if ( dt/hairCell2Ptr->tauCaChan  >= 1.0) {
		NotifyError(wxT("%s: tauCaChan = %g /s is too high for the ")
		  wxT("sampling interval."), funcName, hairCell2Ptr->tauCaChan);
		ok = FALSE;
	}
	if ( dt/hairCell2Ptr->tauConcCa  >= 1.0) {
		NotifyError(wxT("%s: tauConcCa = %g /s is too high for the ")
		  wxT("sampling interval."), funcName, hairCell2Ptr->tauConcCa);
		ok = FALSE;
	}

	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 * In this module it always needs to be run after the first input sample as
 * it uses this sample to set the resting condition.
 * The ICa variable is actually the calcuim conductance. This is reflected by
 * the "IHC_MEDDIS2000_CACONDMODE_REVISION1" multiplication by the calcium
 * conductance time constant.
 */

void
ResetProcess_IHC_Meddis2000(EarObjectPtr data)
{
	int		i;
	Float	spontPerm_k0, spontCleft_c0, spontFreePool_q0, spontReprocess_w0;
	HairCell2Ptr	p = hairCell2Ptr; /* Shorter variable for long formulae. */

	Float	  	ICa;		/* Calcium conductance not current! */
	Float	  	ssactCa;	/* steady state Calcium activation */

	ssactCa = 1.0 / ( 1.0 + (exp(- (p->gammaCa*(_InSig_EarObject(data, 0)->channel[
	  0][0]))) / p->betaCa));
	ICa = p->GCaMax * pow(ssactCa, 3) * (_InSig_EarObject(data, 0)->channel[0][0] -
	  p->CaVrev);
	if (p->caCondMode == IHC_MEDDIS2000_CACONDMODE_REVISION1)
		ICa *= p->tauConcCa;
	spontPerm_k0 = ( -ICa > p->perm_Ca0 ) ? (p->perm_z * (pow(-ICa, p->pCa) -
	  pow(p->perm_Ca0, p->pCa))) : 0;
	spontCleft_c0 = (p->cleftReplenishMode ==
	  IHC_MEDDIS2000_CLEFTREPLENISHMODE_ORIGINAL)? p->maxFreePool_M *
	  p->replenishRate_y * spontPerm_k0 / (p->replenishRate_y * (p->lossRate_l +
	  p->recoveryRate_r) + spontPerm_k0 * p->lossRate_l): p->replenishRate_y /
	  p->lossRate_l;
	if (spontCleft_c0 > 0.0) {
	   if (p->opMode == IHC_MEDDIS2000_OPMODE_PROB)
		  spontFreePool_q0 = spontCleft_c0 * (p->lossRate_l +
		    p->recoveryRate_r) / spontPerm_k0;
		else
			spontFreePool_q0 = floor( (spontCleft_c0 * (p->lossRate_l +
		      p->recoveryRate_r) / spontPerm_k0) + 0.5);
	} else
	   spontFreePool_q0 = p->maxFreePool_M;

	spontReprocess_w0 = spontCleft_c0 * p->recoveryRate_r /
	  p->reprocessRate_x;

	for (i = _OutSig_EarObject(data)->offset; i < _OutSig_EarObject(data)->numChannels; i++) {
		p->hCChannels[i].actCa = ssactCa;
		p->hCChannels[i].concCa = -ICa;
		p->hCChannels[i].reservoirQ = spontFreePool_q0;
		p->hCChannels[i].cleftC = spontCleft_c0;
		p->hCChannels[i].reprocessedW = spontReprocess_w0;
	}

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the hair cells to the spontaneous firing rate.
 * The Spontaneous firing rate is determined from the equilbrium state of the
 * system with no input: the reservoir contents do not change in time.
 * For the p'(0) value it uses the first value from the input signal, which it
 * assumes to contain the appropriate value.  The is assumed to be the same
 * for all channels.
 */

/* N.B. Pretty much lifted from the Meddis 95 synapse, but modifed for the new
 * transmitter relese function (CJS 1-2-2000).
 */

BOOLN
InitProcessVariables_IHC_Meddis2000(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_IHC_Meddis2000");
	HairCell2Ptr	p = hairCell2Ptr; /* Shorter variable for long formulae. */

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
			return(FALSE);
		FreeProcessVariables_IHC_Meddis2000();
		OpenDiagnostics_NSpecLists(&p->fp, p->diagModeList, p->diagMode);

		if ((p->hCChannels = (HairCellVars2Ptr) calloc(
		  _OutSig_EarObject(data)->numChannels, sizeof (HairCellVars2))) == NULL) {
			NotifyError(wxT("%s: Out of memory."), funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}


/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_IHC_Meddis2000(void)
{
	if (hairCell2Ptr->hCChannels == NULL)
		return;
	if (hairCell2Ptr->fp) {
		fclose(hairCell2Ptr->fp);
		hairCell2Ptr->fp = NULL;
	}
	free(hairCell2Ptr->hCChannels);
	hairCell2Ptr->hCChannels = NULL;
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;

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
 * ssactCa:	steady state Calcium activation.
 * ICa:		Calcium current.
 */

BOOLN
RunModel_IHC_Meddis2000(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_IHC_Meddis2000");
	register	ChanData	 *inPtr, *outPtr;
	register Float kdt, replenish, reprocessed, ejected;
	BOOLN	debug;
	int		i;
	ChanLen	j;
	Float	dt, reUptake, reUptakeAndLost, timer, ssactCa, ICa, Vin;
	RandParsPtr		randParsPtr;
	SignalDataPtr	outSignal;
	HairCell2Ptr	p = hairCell2Ptr;

	if (!data->threadRunFlag) {
		if (!CheckData_IHC_Meddis2000(data)) {
	 		NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Meddis 2000 IHC. Calcium ")
		  wxT("transmitter release and quantal synapse"));

		/*** Put your code here: Initialise output signal. ***/

		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_IHC_Meddis2000(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}

		dt = _OutSig_EarObject(data)->dt;
		p->ydt = p->replenishRate_y * dt;
		p->rdt = p->recoveryRate_r * dt;
		p->xdt = p->reprocessRate_x * dt;
		p->zdt = p->perm_z * dt;
		p->k0pow = pow( p->perm_Ca0, p->pCa );
		p->l_Plus_rdt = (p->lossRate_l + p->recoveryRate_r) * dt;
		p->dt_Over_tauConcCa = dt / p->tauConcCa;
		p->dtOverTauCaChan = dt / p->tauCaChan;
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	outSignal = _OutSig_EarObject(data);
	if (data->timeIndex == PROCESS_START_TIME) {
		ResetProcess_IHC_Meddis2000(data);
	}

	/* Set up debug/diagnostic mode */
	debug = (!data->threadRunFlag && (hairCell2Ptr->diagMode !=
	  GENERAL_DIAGNOSTIC_OFF_MODE));
	if (debug) {
		DSAM_fprintf(hairCell2Ptr->fp, wxT("Time(s)\tVm (V)\tactCa (-)\tICa ")
		  wxT("(A)\tconcCa (-)\tkdt (P)\tQ \tC \tW \tEjected"));
		DSAM_fprintf(hairCell2Ptr->fp, wxT("\n"));
	}

	/*** Put your code here: process output signal. ***/
	/*** (using 'inPtr' and 'outPtr' for each channel?) ***/

	for (i = outSignal->offset, timer = DBL_MAX; i < outSignal->numChannels;
	  i++) {
		inPtr = _InSig_EarObject(data, 0)->channel[i];
		randParsPtr = &data->randPars[i];
		for (j = 0, outPtr = outSignal->channel[i]; j < outSignal->length; j++,
		  outPtr++) {

			/*** Calcium controlled transmitter release function ***/
			Vin = *inPtr;

			/* Ca current */
			ssactCa = 	1/( 1 + exp(-p->gammaCa*Vin)/p->betaCa );

			p->hCChannels[i].actCa += (ssactCa - p->hCChannels[i].actCa) *
			  p->dtOverTauCaChan;
			ICa = p->GCaMax * pow(p->hCChannels[i].actCa, 3) * (Vin -
			  p->CaVrev);

			/* Calcium Ion accumulation and diffusion */
			p->hCChannels[i].concCa += (hairCell2Ptr->caCondMode ==
			  IHC_MEDDIS2000_CACONDMODE_ORIGINAL)?
			  (-ICa - p->hCChannels[i].concCa) * p->dt_Over_tauConcCa:
			  (-ICa - p->hCChannels[i].concCa / p->tauConcCa) * outSignal->dt;

			/* power law release function */
			kdt = ( p->hCChannels[i].concCa > p->perm_Ca0 ) ? (p->zdt * (pow(
			  p->hCChannels[i].concCa, p->pCa) - p->k0pow)): 0;

			/* Increment input pointer */
			inPtr++;

 			/* end of new transmitter release function */

			/*** Synapse ***/

			switch(hairCell2Ptr->opMode) {
				/* spike output mode */
				case IHC_MEDDIS2000_OPMODE_SPIKE:
					replenish = (p->cleftReplenishMode ==
					  IHC_MEDDIS2000_CLEFTREPLENISHMODE_UNITY)? GeomDist_Random(
					  p->ydt, 1,randParsPtr): (p->hCChannels[i].reservoirQ <
					  p->maxFreePool_M)? GeomDist_Random(p->ydt, (int) (
					  p->maxFreePool_M - p->hCChannels[i].reservoirQ),
					  randParsPtr): 0;

					ejected = GeomDist_Random(kdt, (int) p->hCChannels[
					  i].reservoirQ, randParsPtr);

					reUptakeAndLost = p->l_Plus_rdt * p->hCChannels[i].cleftC;
					reUptake = p->rdt * p->hCChannels[i].cleftC;
					reprocessed = (p->hCChannels[i].reprocessedW < 1.0)? 0:
					GeomDist_Random(p->xdt, (int) floor(p->hCChannels[
					  i].reprocessedW), randParsPtr);

					p->hCChannels[i].reservoirQ += replenish - ejected +
					  reprocessed;
					*outPtr = p->hCChannels[i].cleftC += ejected -
					  reUptakeAndLost;

					if (ejected > 0)
						*outPtr = ejected;
					else
						*outPtr = 0.0;

					p->hCChannels[i].reprocessedW += reUptake - reprocessed;
					break;

				/* probability output mode */
				case IHC_MEDDIS2000_OPMODE_PROB:
					replenish = (p->cleftReplenishMode ==
					  IHC_MEDDIS2000_CLEFTREPLENISHMODE_UNITY)? p->ydt:
					  (p->hCChannels[i].reservoirQ < p->maxFreePool_M)? p->ydt *
					  (p->maxFreePool_M - p->hCChannels[i].reservoirQ): 0.0;

					ejected = kdt * p->hCChannels[i].reservoirQ;

					reUptakeAndLost = p->l_Plus_rdt * p->hCChannels[i].cleftC;
					reUptake = p->rdt * p->hCChannels[i].cleftC;

					reprocessed = p->xdt * p->hCChannels[i].reprocessedW;
					p->hCChannels[i].reservoirQ += replenish - ejected +
					  reprocessed;
					p->hCChannels[i].cleftC += ejected - reUptakeAndLost;

					*outPtr = ejected;

					p->hCChannels[i].reprocessedW += reUptake - reprocessed;
					break;

				/* neither mode set. error and exit */
				default:
					NotifyError(wxT("%s: Illegal output mode."), funcName);
					exit(0);
				}

			/* diagnostic mode output */
			if (debug) {
				DSAM_fprintf(hairCell2Ptr->fp,
				  wxT("%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g"), j * outSignal->
				  dt, Vin, p->hCChannels[i].actCa, ICa, p->hCChannels[i].concCa,
				  kdt, p->hCChannels[i].reservoirQ, p->hCChannels[i].cleftC, p->
				  hCChannels[i].reprocessedW, ejected);
				DSAM_fprintf(hairCell2Ptr->fp, wxT("\n"));
			}

		}
	}
	if (debug && p->fp)
		CloseDiagnostics_NSpecLists(&p->fp);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
