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

#ifndef _PROCESS_CFG2_H
#define _PROCESS_CFG2_H

struct ProcessLayer{
	struct ProcessLayer *Next;
	char *Name;
	int Layer;
	int Datatype;
	float Height;
	float Thickness;
	int Show;
	float Red;
	float Green;
	float Blue;
	float Filter;
	int Metal;
	int Index;
	int LegendIndex;
	bool Alt;
	bool Ctrl;
	bool Shift;
	int ShortKey;
};

typedef struct ProcessLayer layers;

class GDSProcess
{
private:
	struct ProcessLayer	*_FirstLayer;
	int _Count;		/* Number of layers found */

	bool _Valid;		/* Is the process file valid? */
public:
	GDSProcess ();
	~GDSProcess ();

	void Parse(char *processfile);
	//bool Parse(char *processfile);

	void AddLayer(struct ProcessLayer *NewLayer);
	void AddLayer(int Layer, int Datatype);
	void ChangeVisibility(struct ProcessLayer *Layer, bool Show);
	void ChangeLegendIndex(struct ProcessLayer *Layer, int LegendIndex);
	struct ProcessLayer *GetLayer(int Number, int Datatype);
	struct ProcessLayer *GetLayer(int Index);
	struct ProcessLayer *GetLayer();
	struct ProcessLayer *GetLayer(const char *Name);
	int LayerCount();
	bool IsValid();
	float GetHighest();
	float GetLowest();
	bool Save(const char *filename);
};

#endif // _PROCESS_CFG_H

