/**********************
 *
 * File:		GrPrintDisp.cpp
 * Purpose: 	Routines for printing the display window.
 * Comments:	Created to use the WxWin 2 printing routines.
 *				The global 'printData' is initialised in 'MyApp::OnInit'.
 * Created:		20 Sep 1999
 * Updated:		
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

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
 	   #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/wx.h>
#	endif

// Any files included regardless of precompiled headers
#	include "wx/metafile.h"
#	include "wx/print.h"
#	include "wx/printdlg.h"

#	if wxTEST_POSTSCRIPT_IN_MSW
#		include "wx/generic/printps.h"
#		include "wx/generic/prntdlgg.h"
#	endif
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GrUtils.h"
#include "GrBrushes.h"
#include "DiSignalDisp.h"
#include "GrDisplayS.h"
#include "GrCanvas.h"
#include "GrPrintDisp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

wxPrintData *printData = (wxPrintData*) NULL ;

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * Define a constructor for my canvas.
 */

PrintDisp::PrintDisp(MyCanvas *theCanvas, const wxChar *title): wxPrintout(title)
{
	canvas = theCanvas;

}

/****************************** OnPrintPage ***********************************/

bool
PrintDisp::OnPrintPage(int page)
{
	wxDC *dc = GetDC();

	if (dc) {
		wxChar	buf[LONG_STRING];
		int		w, h, ppiScreenX, ppiScreenY, ppiPrinterX, ppiPrinterY;
		int		pageWidth, pageHeight;
		float	scale, overAllScale;

		GetPPIScreen(&ppiScreenX, &ppiScreenY);
		GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
		GetPageSizePixels(&pageWidth, &pageHeight);
		scale = (float) ppiPrinterX / ppiScreenX;
		//canvas->SetUseTextAdjust(TRUE); - not needed at present.
		dc->GetSize(&w, &h);
		overAllScale = scale * (float) w / pageWidth;
		dc->SetUserScale(PRINTDISP_X_SCALE * overAllScale, PRINTDISP_Y_SCALE *
		  overAllScale);

		DSAM_snprintf(buf, LONG_STRING, wxT("%s: %s"), canvas->GetSignalDispPtr(
		  )->title, wxNow().c_str());
		dc->DrawText(buf, (int) PRINTDISP_PS_X_OFFSET, (int)
		  PRINTDISP_PS_Y_OFFSET);

		canvas->DrawGraph(*dc, (int) PRINTDISP_PS_X_OFFSET, (int) (
		  PRINTDISP_PS_Y_OFFSET + PRINTDISP_HEADER_OFFSET));
		//canvas->SetUseTextAdjust(FALSE);

		return(TRUE);
	}
	return(FALSE);

}

/****************************** OnBeginDocument *******************************/

bool
PrintDisp::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return FALSE;
    
    return TRUE;

}

/****************************** GetPageInfo ***********************************/

void
PrintDisp::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom,
  int *selPageTo)
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;

}

/****************************** HasPage ***************************************/

bool
PrintDisp::HasPage(int pageNum)
{
    return (pageNum == 1);

}

