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

#include "gdsobject.h"
#include "gdspath.h"

GDSPath::GDSPath(int Type, float Height, float Thickness, unsigned int Points, float Width, float BgnExtn, float EndExtn, struct ProcessLayer *Layer)
{
	_Type = Type;
	_Coords = new Point2D[Points];
	_Height = Height;
	_Thickness = Thickness;
	_Points = Points;
	_Width = Width;
	_BgnExtn = BgnExtn;
	_EndExtn = EndExtn;
	_Layer = Layer;
}

GDSPath::~GDSPath()
{
	if(_Coords) delete [] _Coords;
}

void GDSPath::AddPoint(unsigned int Index, float X, float Y)
{
	if(_Points >= Index){
		_Coords[Index].X = X;
		_Coords[Index].Y = Y;
	}
}


void GDSPath::SetRotation(float X, float Y, float Z)
{
	_Rotate.X = X;
	_Rotate.Y = Y;
	_Rotate.Z = Z;
}

float GDSPath::GetXCoords(unsigned int Index)
{
	return _Coords[Index].X;
}

float GDSPath::GetYCoords(unsigned int Index)
{
	return _Coords[Index].Y;
}

unsigned int GDSPath::GetPoints()
{
	return _Points;
}

float GDSPath::GetHeight()
{
	return _Height;
}

float GDSPath::GetThickness()
{
	return _Thickness;
}

float GDSPath::GetWidth()
{
	return _Width;
}

float GDSPath::GetBgnExtn()
{
	return _BgnExtn;
}

float GDSPath::GetEndExtn()
{
	return _EndExtn;
}

int GDSPath::GetType()
{
	return _Type;
}

struct ProcessLayer *GDSPath::GetLayer()
{
	return _Layer;
}

