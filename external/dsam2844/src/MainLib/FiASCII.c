/******************
 *
 * File:		FiASCII.c
 * Purpose:		This Filing module deals with the writing reading of raw binary
 *				data files.
 * Comments:	17-04-98 LPO: This module has now been separated from the main
 *				FiDataFile module.
 * Authors:		L. P. O'Mard
 * Created:		17 Apr 1998
 * Updated:
 * Copyright:	(c) 1998, 2010 Lowel P. O'Mard
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
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtString.h"
#include "FiParFile.h"
#include "FiDataFile.h"
#include "FiASCII.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/**************************** ReadFile ****************************************/

/*
 * This function reads a file in ASCII stream format.
 * The data is stored in the output signal of an EarObject.
 * This is the default file, and is assumed if no suffix is given
 * If a dash, '-.suffix', is given as the file name, then the data will be
 * read from the standard input.
 * It returns TRUE if successful.
 * In segmented mode, this routine returns FALSE when it gets to the end of
 * the signal.
 */

BOOLN
ReadFile_ASCII(WChar *fileName, EarObjectPtr data)
{
	static const WChar *funcName = wxT("ReadFile_ASCII");
	BOOLN	endOfSignal;
	WChar	line[MAXLINE_LARGE];
	uShort	numColumns;
	int		chan;
	Float	dt;
	ChanLen	i, length, numSamples;
	FILE	*fp;

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((fp = OpenFile_DataFile(fileName, FOR_ASCII_READING)) == NULL) {
		NotifyError(wxT("%s: Couldn't open file."), funcName);
		return(FALSE);
	}
	if ((numColumns = NumberOfColumns_DataFile(fp)) == 0) {
		NotifyError(wxT("%s: Illegal no. of columns: '%s'."), funcName,
		  fileName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, wxT("'%s' ASCII (dat) file"),
	  GetFileNameFPath_Utility_String(fileName));
	dataFilePtr->subFormatType = ASCII_DATA_FILE;
	numSamples = (int32) floor(dataFilePtr->duration * dataFilePtr->
	  defaultSampleRate + 0.5);
	if (!InitProcessVariables_DataFile(data, numSamples,
	  dataFilePtr->defaultSampleRate)) {
		NotifyError(wxT("%s: Could not initialise process variables."),
		  funcName);
		return(FALSE);
	}
	dt = 1.0 / dataFilePtr->defaultSampleRate;
	length = (ChanLen) dataFilePtr->maxSamples;
	if (!InitOutSignal_EarObject(data, numColumns, length, dt) ) {
		NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
		return(FALSE);
	}
	if (numColumns == 2)
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), 2);
	for (i = 0; i < data->timeIndex; i++)
		if (DSAM_fgets(line, MAXLINE_LARGE, fp) == NULL)
			return(FALSE);
	for (i = 0, endOfSignal = FALSE; !endOfSignal && (i < length); i++)
		for (chan = 0; (chan < _OutSig_EarObject(data)->numChannels); chan++)
			if (DSAM_fscanf(fp, wxT("%lf"), &_OutSig_EarObject(data)->channel[
			  chan][i]) == EOF) {
				endOfSignal = TRUE;
				break;
			}
	if (endOfSignal) {
		if ((_OutSig_EarObject(data)->length = i - 1) == 0) {
			if (!GetDSAMPtr_Common()->segmentedMode)
				NotifyError(wxT("%s: Couldn't read samples from the file ")
				  wxT("'%s'."), funcName ,fileName);
			return(FALSE);
		}
	}
	if (fp != stdin)
		fclose(fp);
	return(TRUE);

}

/**************************** GetDuration *************************************/

/*
 * This function is upposed to return the duration of a file.
 * It will return the error value -1 for this file type as the end of the
 * file cannot be calculated without reading the file.
 */

Float
GetDuration_ASCII(WChar *fileName)
{
	static const WChar *funcName = wxT("GetDuration_ASCII");

	NotifyError(wxT("%s: Signal duration cannot be calculated for this file ")
	  wxT("type."), funcName);
	return(-1.0);

}

/**************************** WriteFile ***************************************/

/*
 * This routine sends the output signal of an EarObject to file.
 * It assumes that the GeEarObject.has been initialised.
 */

BOOLN
WriteFile_ASCII(WChar *fileName, EarObjectPtr data)
{
	static const WChar *funcName = wxT("WriteFile_ASCII");

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	SetTimeIndex_SignalData(_OutSig_EarObject(data), _WorldTime_EarObject(data));
	dataFilePtr->subFormatType = ASCII_DATA_FILE;
	if (!OutputToFile_SignalData(fileName, _OutSig_EarObject(data))) {
		if (data->processName != NULL)
			NotifyWarning(wxT("%s: Data from EarObject: %s, has not been ")
			  wxT("output to file."), funcName, data->processName);
		else
			NotifyWarning(wxT("%s: Data from EarObject: <no name>, has not ")
			  wxT("been output to file."), funcName);
		return(FALSE);
	} else {
		return(TRUE);
	}

}


