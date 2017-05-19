//  GDS3D, a program for viewing GDSII files in 3D.
//  Created by Jasper Velner and Michiel Soer, http://icd.el.utwente.nl
//  Based on code by Roger Light, http://atchoo.org/gds2pov/
//  
//  Copyright (C) 2013 IC-Design Group, University of Twente.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA

#ifndef __GDSOBJECT_H__
#define __GDSOBJECT_H__

#include "gds_globals.h"
#include "process_cfg.h"
#include "gdselements.h"
#include "gdspath.h"
#include "gdstext.h"
#include "gdspolygon.h"

typedef struct GDSRef
{
	GDSObject	*object;
	GDSMat		mat;
}GDSRef;

class GDSObject
{
protected:
	// Temporary data for parsing	
	vector<GDSPath*> PathItems;
	vector<GDSText*> TextItems;
	vector<SRefElement*> SRefItems;
	vector<ARefElement*> ARefItems;	
	
    int PointCount;
    int AccumPointCount;
    bool noHierarchy;

	bool hasBoundary;
	GDSBB boundary;

	char *Name;
	bool PCell; // After PCell detection
	bool collapsed;

public:
	// Please move to private...
	vector<GDSPolygon*> PolygonItems; 	
	vector<GDSRef*> refs; // Use these references for rendering

	GDSObject(char *Name);
	virtual ~GDSObject();

	// Adding of new elements
	void AddText(float newX, float newY, float newZ, bool newFlipped, float newMag, int newVJust, int newHJust, struct ProcessLayer *newlayer);
	class GDSText *GetCurrentText();
	void AddPolygon(float Height, float Thickness, int Points, struct ProcessLayer *layer);
	class GDSPolygon *GetCurrentPolygon();
	void AddSRef(char *Name, float X, float Y, int Flipped, float Mag);
	void SetSRefRotation(float X, float Y, float Z);
	void AddARef(char *Name, float X1, float Y1, float X2, float Y2, float X3, float Y3, int Columns, int Rows, int Flipped, float Mag);
	void SetARefRotation(float X, float Y, float Z);
	void AddPath(int PathType, float Height, float Thickness, int Points, float Width, float BgnExtn, float EndExtn, struct ProcessLayer *layer);
	class GDSPath *GetCurrentPath();

	void ConnectReferences(class GDSObjectList *Objects);
	void TransformAddObject(GDSObject *obj, GDSMat mat);

	// Get stuff
	char *GetName();    
	bool referencesToObject(char *name);
	GDSBB GetTotalBoundary();
	bool isPCell();
	unsigned int GetNumSRefs();
	SRefElement* GetSRef(unsigned int index);
	unsigned int GetNumARefs();
	ARefElement* GetARef(unsigned int index);
    
    // Flatten lower part of hierarchy
    void printHierarchy(int);
    int countTotalPoints();
    void collapseHierachy();   
};

#endif // __GDSOBJECT_H__

