/**********************
 *
 * File:		GrSDICommand.h
 * Purpose: 	Command class for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 *				Most user interface commands are routed through this, to give
 *				us the Undo/Redo mechanism. If you add more commands, such as
 *				changing the shape colour, you will need to add members to
 *				'remember' what the user applied (for 'Do') and what the
 *				previous state was (for 'Undo').
 *				You can have one member for each property to be changed.
 *				Assume we also have a pointer member wxShape *shape, which is
 *				set to the shape being changed. Let's assume we're changing
 *				the shape colour. Our member for this is shapeColour.
 *				- In 'Do':
 *					o Set a temporary variable 'temp' to the current colour
 *						for 'shape'.
 *					o Change the colour to the new colour.
 *					o Set shapeColour to the _old_ colour, 'temp'.
 *				- In 'Undo':
 *					o Set a temporary variable 'temp' to the current colour
 *						for 'shape'.
 *					o Change the colour to shapeColour (the old colour).
 *					o Set shapeColour to 'temp'.
 *
 *				So, as long as we have a pointer to the shape being changed, we
 *				only need one member variable for each property.
 *
 *				PROBLEM: when an Add shape command is redone, the 'shape'
 *				pointer changes.
 *				Assume, as here, that we keep a pointer to the old shape so we
 *				reuse it when we recreate.
 * Author:		L.P.O'Mard
 * Created:		13 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, 2010 Lowel P. O'Mard
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

#ifndef _GRSDICOMMAND_H
#define _GRSDICOMMAND_H 1

#ifdef __GNUG__
// #pragma interface
#endif

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDICommand ***************************************/
 
class SDICommand: public wxCommand
{
  protected:
	SDIDocument *doc;
	int		cmd, processType;
	wxShape *shape; // Pointer to the shape we're acting on
	wxShape *fromShape;
	wxShape *toShape;
	wxClassInfo *shapeInfo;
	double x;
	double y;
	bool selected;
	bool deleteShape;

	// Storage for property commands
	wxBrush	*shapeBrush;
	wxPen	*shapePen;
	wxString	shapeLabel;
	void	*savedInfo;

  public:
	// Multi-purpose constructor for creating, deleting shapes
	SDICommand(const wxString& name, int cmd, SDIDocument *doc, wxClassInfo 
	  *shapeInfo = NULL, int theProcessType = -1, double x = 0.0, double y =
	  0.0, bool sel = FALSE, wxShape *theShape = NULL, wxShape *fs = NULL,
	  wxShape *ts = NULL);

	// Property-changing command constructors
	SDICommand(const wxString& name, int cmd, SDIDocument *ddoc,
	  wxBrush *backgroundColour, wxShape *theShape);
	SDICommand(const wxString& name, int cmd, SDIDocument *ddoc, const 
	  wxString& lab, wxShape *theShape);

	~SDICommand(void);

	bool	Do(void);
	bool	Undo(void);

	inline void		SetShape(wxShape *s) { shape = s; }
	inline wxShape	*GetShape(void) { return shape; }
	inline wxShape	*GetFromShape(void) { return fromShape; }
	inline wxShape	*GetToShape(void) { return toShape; }
	inline wxClassInfo	*GetShapeInfo(void) { return shapeInfo; }
	inline bool		GetSelected(void) { return selected; }

	bool	AddLineShape(int lineType);
	bool	ConnectInstructions(wxShape *fromShape, wxShape *toShape);
	void	DisconnectProcessInsts(wxShape *fromShape, wxShape *toShape);
	void	RedrawShapeLabel(wxShape *shape);
	void	RemoveLines(wxShape *shape);
	void	SetBasic(int command, SDIDocument *ddoc, wxShape *theShape);

};


#endif
  // _OGLSAMPLE_DOC_H_
