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

#include "gds_globals.h"
#include "gdsobject.h"
#include "gdsobjectlist.h"



#define HIERARCHY_LIMIT  30000

GDSObject::GDSObject(char *NewName)
{
    PointCount = 0;
    noHierarchy = false;
	collapsed = false;

	hasBoundary = false;
    
	Name = new char[strlen(NewName)+1];
	strcpy(Name, NewName); 

    PCell = false;
}

GDSObject::~GDSObject()
{
	for(unsigned int i=0;i<PolygonItems.size();i++)
		delete PolygonItems[i];

	for(unsigned int i=0;i<PathItems.size();i++)
		delete PathItems[i];

	for(unsigned int i=0;i<TextItems.size();i++)
		delete TextItems[i];

	for(unsigned int i=0;i<SRefItems.size();i++)
	{
		if(SRefItems[i]->Name)
			delete [] SRefItems[i]->Name;
		delete SRefItems[i];
	}

	for(unsigned int i=0;i<ARefItems.size();i++)
	{
		if(ARefItems[i]->Name)
			delete [] ARefItems[i]->Name;
		delete ARefItems[i];
	}

	for(unsigned int i=0;i<refs.size();i++)		
		delete refs[i];	
    
	delete [] Name;
}

void GDSObject::AddText(float newX, float newY, float newZ, bool newFlipped, float newMag, int newVJust, int newHJust, struct ProcessLayer *newlayer)
{
	TextItems.push_back(new class GDSText(newX, newY, newZ, newFlipped, newMag, newVJust, newHJust, newlayer));
}

class GDSText *GDSObject::GetCurrentText()
{
	if(TextItems.size()){
		return TextItems[TextItems.size()-1];
	}else{
		return NULL;
	}
}

char *GDSObject::GetName()
{
	return Name;
}

void GDSObject::AddPolygon(float Height, float Thickness, int Points, struct ProcessLayer *layer)
{
	PolygonItems.push_back(new class GDSPolygon(Height, Thickness, layer));

    PointCount += Points*2;
}

class GDSPolygon *GDSObject::GetCurrentPolygon()
{
    if(PolygonItems.size()>0)
        return PolygonItems[PolygonItems.size()-1];
    else
        return NULL;
}

void GDSObject::AddSRef(char *Name, float X, float Y, int Flipped, float Mag)
{
	SRefElement *NewSRef = new SRefElement;
      
	NewSRef->Name = new char[strlen(Name)+1];
	strcpy(NewSRef->Name, Name);
	NewSRef->X = X;
	NewSRef->Y = Y;
	NewSRef->Rotate.X = 0.0;
	NewSRef->Rotate.Y = 0.0;
	NewSRef->Rotate.Z = 0.0;
	NewSRef->Flipped = Flipped;
	NewSRef->Mag = Mag;
	NewSRef->object = NULL;
    NewSRef->collapsed = false;
    
	SRefItems.push_back(NewSRef);
}

void GDSObject::SetSRefRotation(float X, float Y, float Z)
{
	if(SRefItems.size() > 0)
	{
		SRefItems[SRefItems.size()-1]->Rotate.X = X;
		SRefItems[SRefItems.size()-1]->Rotate.Y = Y;
		SRefItems[SRefItems.size()-1]->Rotate.Z = Z;
	}
}

void GDSObject::AddARef(char *Name, float X1, float Y1, float X2, float Y2, float X3, float Y3, int Columns, int Rows, int Flipped, float Mag)
{
	ARefElement *NewARef = new ARefElement;
    
	NewARef->Name = new char[strlen(Name)+1];
	strcpy(NewARef->Name, Name);
	NewARef->X1 = X1;
	NewARef->Y1 = Y1;
	NewARef->X2 = X2;
	NewARef->Y2 = Y2;
	NewARef->X3 = X3;
	NewARef->Y3 = Y3;
	NewARef->Columns = Columns;
	NewARef->Rows = Rows;
	NewARef->Rotate.X = 0.0;
	NewARef->Rotate.Y = 0.0;
	NewARef->Rotate.Z = 0.0;
	NewARef->Flipped = Flipped;
	NewARef->Mag = Mag;
	NewARef->object = NULL;
    NewARef->collapsed = false;

	ARefItems.push_back(NewARef);
}

void GDSObject::SetARefRotation(float X, float Y, float Z)
{
	if(ARefItems.size() > 0)
	{
		ARefItems[ARefItems.size()-1]->Rotate.X = X;
		ARefItems[ARefItems.size()-1]->Rotate.Y = Y;
		ARefItems[ARefItems.size()-1]->Rotate.Z = Z;
	}
}

/*
struct _Boundary *GDSObject::GetTotalBoundary(class MATRIX4X4 *modelview)
{
	MATRIX4X4 trans;
	MATRIX4X4 temp;
	AA_BOUNDING_BOX bbox;
	VECTOR3D point;
	float dx1, dx2, dy1, dy2;
	int i,j;
    
	if(GotBoundary){
		return &Boundary;
	}
    
	class GDSPolygon *polygon;
	for(unsigned long i=0; i<PolygonItems.size(); i++){
		polygon = PolygonItems[i];
		if(!polygon->GetLayer()->Show)
			continue;
		for(unsigned int j=0; j<polygon->GetPoints(); j++)
		{
			point.Set(polygon->GetXCoords(j), polygon->GetYCoords(j), 0.0);

			if(point.GetX() > Boundary.XMax){
				Boundary.XMax = point.GetX();
			}
			if(point.GetX() < Boundary.XMin){
				Boundary.XMin = point.GetX();
			}
			if(point.GetY() > Boundary.YMax){
				Boundary.YMax = point.GetY();
			}
			if(point.GetY() < Boundary.YMin){
				Boundary.YMin = point.GetY();
			}
		}
	}
    
	// FIXME!!!!!!
	if(FirstSRef){
		SRefElement dummysref;
		dummysref.Next=FirstSRef;
		SRefElement *sref = &dummysref;
        
		struct ObjectList *object;
		struct _Boundary *NewBound;
        
		while(sref->Next){
			sref = sref->Next;
			if(strcmp(sref->Name, this->Name)!=0){
				object = &dummyobject;
                
				// Build transformation matrix
				trans.LoadIdentity();
				if(sref->Mag!=1.0)
				{
					temp.SetScale(VECTOR3D(sref->Mag, sref->Mag, 1.0f));
					trans = trans * temp;
				}
				temp.SetTranslation(VECTOR3D(sref->X, sref->Y, 0.0f));
				trans = trans * temp;
				if(sref->Rotate.Y)
				{
					temp.SetRotationAxis(-sref->Rotate.Y, VECTOR3D(0.0f, 0.0f, 1.0f));
					trans = trans * temp;
				}
				if(sref->Flipped)
				{
					temp.SetScale(VECTOR3D(1.0f, -1.0f, 1.0f));
					trans = trans * temp;
				}
                
				while(object->Next){
					object = object->Next;
					if(strcmp(object->Object->GetName(), sref->Name)==0){
						NewBound = object->Object->GetBoundary(objectlist, NULL);
						bbox.SetFromMinsMaxes(VECTOR3D(NewBound->XMin,NewBound->YMin,0.0), VECTOR3D(NewBound->XMax,NewBound->YMax,0.0));
						bbox.Mult(trans); // Transform bounding box
                        
						if(bbox.maxes.GetX() > Boundary.XMax){
							Boundary.XMax = bbox.maxes.GetX();
						}
						if(bbox.mins.GetX() < Boundary.XMin){
							Boundary.XMin = bbox.mins.GetX();
						}
						if(bbox.maxes.GetY() > Boundary.YMax){
							Boundary.YMax = bbox.maxes.GetY();
						}
						if(bbox.mins.GetY() < Boundary.YMin){
							Boundary.YMin = bbox.mins.GetY();
						}
						break;
					}
				}
			}
		}
	}
    
	if(FirstARef){
		ARefElement dummyaref;
		dummyaref.Next = FirstARef;
        
		ARefElement *aref = &dummyaref;
        
		struct ObjectList *object;
		dummyobject.Next = objectlist;
		
		struct _Boundary *NewBound;
		while(aref->Next){
			aref = aref->Next;
			if(strcmp(aref->Name, this->Name)!=0){
				object = &dummyobject;
				object = &dummyobject;
                
				while(object->Next){
					object = object->Next;
					if(strcmp(object->Object->GetName(), aref->Name)==0){
                        
						dx1 = (float)(aref->X2 - aref->X1) / (float)aref->Columns;
						dy1 = (float)(aref->Y2 - aref->Y1) / (float)aref->Columns;
						dx2 = (float)(aref->X3 - aref->X1) / (float)aref->Rows;
						dy2 = (float)(aref->Y3 - aref->Y1) / (float)aref->Rows;
                        
						for(i=0; i<aref->Rows; i++){
							for(j=0; j<aref->Columns; j++){
                                
                                // Build transformation matrix
                                trans.LoadIdentity();
                                if(aref->Mag!=1.0)
                                {
                                    temp.SetScale(VECTOR3D(aref->Mag, aref->Mag, 1.0f));
                                    trans = trans * temp;
                                }
                                temp.SetTranslation(VECTOR3D(aref->X1+dx1*(float)j+dx2*(float)i, aref->Y1+dy2*(float)i+dy1*(float)j, 0.0f));
                                trans = trans * temp;
                                if(aref->Rotate.Y)
                                {
                                    temp.SetRotationAxis(-aref->Rotate.Y, VECTOR3D(0.0f, 0.0f, 1.0f));
                                    trans = trans * temp;
                                }
                                if(aref->Flipped)
                                {
                                    temp.SetScale(VECTOR3D(1.0f, -1.0f, 1.0f));
                                    trans = trans * temp;
                                }
                                
                                NewBound = object->Object->GetBoundary(objectlist, NULL);
                                bbox.SetFromMinsMaxes(VECTOR3D(NewBound->XMin,NewBound->YMin,0.0), VECTOR3D(NewBound->XMax,NewBound->YMax,0.0));
                                bbox.Mult(trans); // Transform bounding box
                                
                                if(bbox.maxes.GetX() > Boundary.XMax){
                                    Boundary.XMax = bbox.maxes.GetX();
                                }
                                if(bbox.mins.GetX() < Boundary.XMin){
                                    Boundary.XMin = bbox.mins.GetX();
                                }
                                if(bbox.maxes.GetY() > Boundary.YMax){
                                    Boundary.YMax = bbox.maxes.GetY();
                                }
                                if(bbox.mins.GetY() < Boundary.YMin){
                                    Boundary.YMin = bbox.mins.GetY();
                                }
                                
							}
						}
					}
				}
			}
		}
	}
    
	if(PathItems.empty() && PolygonItems.empty() && !FirstSRef && !FirstARef){
		Boundary.XMax = Boundary.XMin = Boundary.YMax = Boundary.YMin = 0;
	}
    
	v_printf(2, "%-30s XMax=%8.2f \tXMin=%8.2f \tYMax= %8.2f \tYMin=%8.2f\n", Name, Boundary.XMax, Boundary.XMin, Boundary.YMax, Boundary.YMin);
	GotBoundary = true;
    
	return &Boundary;
}*/

GDSBB GDSObject::GetTotalBoundary()
{
	GDSBB BB, t;
	GDSPolygon *polygon;

	if(hasBoundary)
		return boundary;

	for(unsigned int i=0; i<PolygonItems.size(); i++)
	{
		polygon = PolygonItems[i];
		if(!polygon->GetLayer()->Show)
			continue;

		BB.merge(*polygon->GetBBox());
	}

	for(unsigned int i=0;i<refs.size();i++)
	{
		t = refs[i]->object->GetTotalBoundary();
		t.transform(refs[i]->mat);
		BB.merge(t);
	}

	boundary = BB;
	hasBoundary = true;
	return BB;
}

void GDSObject::AddPath(int PathType, float Height, float Thickness, int Points, float Width, float BgnExtn, float EndExtn, struct ProcessLayer *layer)
{
	PathItems.push_back(new class GDSPath(PathType, Height, Thickness, Points, Width, BgnExtn, EndExtn, layer));

    PointCount += Points*4;
}

class GDSPath *GDSObject::GetCurrentPath()
{
	if(PathItems.size() < 1) // No pathitems
		return NULL;

	return PathItems[PathItems.size()-1];
}



void GDSObject::ConnectReferences(class GDSObjectList *Objects)
{
	GDSMat M;
	float dx1, dx2, dy1, dy2;
	int i,j;

    //Find SRef objects
	for(unsigned int k=0;k<SRefItems.size();k++)
	{
		SRefElement *sref = SRefItems[k];
		sref->object = Objects->SearchObject(sref->Name);

		// If not found, remove from SRef list
		if(!sref->object)
		{
			SRefItems.erase(SRefItems.begin()+k);
			k--; // Check the new SRef on this index again
			continue;
		}

		// Decode 2D transformation matrix
		GDSRef *newRef = new GDSRef;
		newRef->object = sref->object;

		newRef->mat.loadIdentity();
		if(sref->Mag!=1.0)
		{
			M.setScaling(sref->Mag, sref->Mag);
			newRef->mat = newRef->mat * M;
		}
		M.setTranslation(sref->X, sref->Y);
		newRef->mat = newRef->mat * M;
		if(sref->Rotate.Y)
		{
			M.setRotation(-sref->Rotate.Y);
			newRef->mat = newRef->mat * M;
		}
		if(sref->Flipped)
		{
			M.setScaling(1.0f, -1.0f);
			newRef->mat = newRef->mat * M;
		}
		
		// Round matrix to avoid small errors
		newRef->mat.Round();

		// Add
		refs.push_back(newRef);
	}

	//Find ARefs objects
	for(unsigned int k=0;k<ARefItems.size();k++)
	{
		ARefElement *aref = ARefItems[k];
		aref->object = Objects->SearchObject(aref->Name);

		// If not found, remove from ARef list
		if(!aref->object)
		{
			ARefItems.erase(ARefItems.begin()+k);
			k--; // Check the new ARef on this index again
			continue;
		}

		dx1 = (float)(aref->X2 - aref->X1) / (float)aref->Columns;
		dy1 = (float)(aref->Y2 - aref->Y1) / (float)aref->Columns;
		dx2 = (float)(aref->X3 - aref->X1) / (float)aref->Rows;
		dy2 = (float)(aref->Y3 - aref->Y1) / (float)aref->Rows;

		for(i=0; i<aref->Rows; i++)
		{
			for(j=0; j<aref->Columns; j++)
			{
				// Decode 2D transformation matrix
				GDSRef *newRef = new GDSRef;
				newRef->object = aref->object;

				// Build transformation matrix      
				newRef->mat.loadIdentity();
				if(aref->Mag!=1.0)
				{
					M.setScaling(aref->Mag, aref->Mag);
					newRef->mat = newRef->mat * M;
				}
				M.setTranslation(aref->X1+dx1*(float)j+dx2*(float)i, aref->Y1+dy2*(float)i+dy1*(float)j);
				newRef->mat = newRef->mat * M;
				if(aref->Rotate.Y)
				{
					M.setRotation(-aref->Rotate.Y);
					newRef->mat = newRef->mat * M;
				}
				if(aref->Flipped)
				{
					M.setScaling(1.0f, -1.0f);
					newRef->mat = newRef->mat * M;
				}
				
				// Round matrix to avoid small errors
				newRef->mat.Round();

				// Add
				refs.push_back(newRef);
			}
		}
	}
}

unsigned int GDSObject::GetNumSRefs()
{
	return SRefItems.size();
}

SRefElement* GDSObject::GetSRef(unsigned int index)
{
	assert(index < SRefItems.size());
	return SRefItems[index];
}

unsigned int GDSObject::GetNumARefs()
{
	return ARefItems.size();
}

ARefElement* GDSObject::GetARef(unsigned int index)
{
	assert(index < ARefItems.size());
	return ARefItems[index];
}

bool GDSObject::isPCell()
{
	return PCell;
}

void GDSObject::printHierarchy(int depth)
{
    for(int i=0;i<depth;i++)
        v_printf(2, "  ");
    v_printf(2, "%s, %d total points\n", Name, AccumPointCount);
    
    if(noHierarchy)
        return;
    
    for(unsigned int i=0;i<SRefItems.size();i++)
	{
            GDSObject* obj = SRefItems[i]->object;
            if(obj && ! SRefItems[i]->collapsed)
                obj->printHierarchy(depth+1);
    }

	for(unsigned int i=0;i<ARefItems.size();i++)
	{
            GDSObject* obj = ARefItems[i]->object;
            if(obj && ! ARefItems[i]->collapsed)
                obj->printHierarchy(depth+1);
    }    
}

int GDSObject::countTotalPoints()
{
    AccumPointCount = PointCount;
    char * dummy2 = NULL;
    char *dummy3 = NULL;
    
    // PCell detection, try to find something like __1018272
    dummy2 = strstr(Name, "__");
    if(dummy2)
    {
        dummy2+=2;
        dummy3 = dummy2;
        while(dummy2)
        {   
            dummy2 = strstr(dummy2, "__");
            if(dummy2)
            {
                dummy2 += 2;
                dummy3 = dummy2;
            }
        }
    }
    if(dummy3)
    {
        if(atoi(dummy3)) 
            PCell = true;
    }
    // PCell detection, try to find something like ___1018272
    dummy2 = strstr(Name, "___");
    if(dummy2)
    {
        dummy2+=3;
        dummy3 = dummy2;
        while(dummy2)
        {   
            dummy2 = strstr(dummy2, "___");
            if(dummy2)
            {
                dummy2 += 3;
                dummy3 = dummy2;
            }
        }
    }
    if(dummy3)
    {
        if(atoi(dummy3)) 
            PCell = true;
    }
    
	/*
    // Count SRefs
   for(unsigned int i=0;i<SRefItems.size();i++)
	{
            GDSObject* obj = SRefItems[i]->object;
            if(obj  && !SRefItems[i]->collapsed && !noHierarchy)
                AccumPointCount += obj->countTotalPoints();            
    }
    
    // Count ARefs
   for(unsigned int i=0;i<ARefItems.size();i++)
	{
            GDSObject* obj = ARefItems[i]->object;
            if(obj && !ARefItems[i]->collapsed && !noHierarchy)
                AccumPointCount += obj->countTotalPoints();          
    }*/
	for(unsigned int i=0;i<refs.size();i++)
		AccumPointCount += refs[i]->object->countTotalPoints();
    
    return AccumPointCount;
}

void GDSObject::collapseHierachy()
{
    // Traverse hierarchy
	//v_printf(1, ".");
    for(unsigned int i=0;i<SRefItems.size();i++)
	{
		if(!SRefItems[i]->object->collapsed)
            SRefItems[i]->object->collapseHierachy();
	}
	for(unsigned int i=0;i<ARefItems.size();i++)
	{
		if(!ARefItems[i]->object->collapsed)
            ARefItems[i]->object->collapseHierachy();
	}
  
    // Collapse total cell?
    if(AccumPointCount < HIERARCHY_LIMIT)
        noHierarchy = true;
    
	GDSObject *obj;
	vector<int> remove;
	for(unsigned int i=0;i<refs.size();i++)
	{
		obj = refs[i]->object;
		if(noHierarchy || obj->AccumPointCount < HIERARCHY_LIMIT/10)
		{
			remove.push_back(i);
			TransformAddObject(obj, refs[i]->mat);
		}
	}
	// This really hurts performance
	if(remove.size() > 0)
	{
		for(int i=remove.size()-1;i>=0;i--)
        {
            delete refs[remove[i]]; // Remember refs are new
			refs.erase(refs.begin()+remove[i]);
        }
	}

	collapsed = true;
}

void GDSObject::TransformAddObject(GDSObject *obj, GDSMat mat)
{
    GDSPolygon *polygon;
    
    // Polygons
    if(!obj->PolygonItems.empty())
    {
		for(unsigned long i=0; i<obj->PolygonItems.size(); i++)
        {
			// Copy the polygon into the object
            AddPolygon(obj->PolygonItems[i]->GetHeight(), obj->PolygonItems[i]->GetThickness(),obj->PolygonItems[i]->GetPoints(), obj->PolygonItems[i]->GetLayer());
			polygon = GetCurrentPolygon();
			obj->PolygonItems[i]->CopyInto(polygon);
            
			// Transform polygon
            polygon->transformPoints(mat);            
            
            // Flipped by transformation?
            if(mat.NegativeTrace())
                polygon->Flip();
        }
    }
}

bool 
GDSObject::referencesToObject(char *name)
{
    //SRefs
   for(unsigned int i=0;i<SRefItems.size();i++)
	{
        SRefElement *sref = SRefItems[i];        
		
            if(!strcmp(sref->object->GetName(), name))
               return true;           
	}
    
    //ARefs
   for(unsigned int i=0;i<ARefItems.size();i++)
	{
		ARefElement *aref = ARefItems[i];
        
	
            if(!strcmp(aref->object->GetName(), name))
                return true;          
	}
    
    return false;
}
