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

#include "gdsobjectlist.h"

// ObjectTree Class
ObjectTree::ObjectTree(GDSObject *object, const GDSMat& mat)
{
	this->object = object;
	this->mat = mat;

	// Go through all ARefs and SRefs
}

ObjectTree::~ObjectTree()
{
	for(unsigned int i=0;i<leaves.size();i++)
		delete leaves[i];
	leaves.clear();
}

// GDSObjectList Class
GDSObjectList::GDSObjectList()
{
	tree = NULL;
}

GDSObjectList::~GDSObjectList()
{
	for(unsigned int i=0;i<objects.size();i++)
		delete objects[i];

	if(tree)
		delete tree;
}

GDSObject *GDSObjectList::AddObject(class GDSObject *newobject)
{
	objects.push_back(newobject);

	return newobject;
}

GDSObject *GDSObjectList::SearchObject(const char *Name)
{
	for(unsigned int i=0;i<objects.size();i++)
	{
		if(strcmp(Name, objects[i]->GetName())==0)
			return objects[i];
	}

	return NULL;
}

void GDSObjectList::ConnectReferences()
{
	for(unsigned int i=0;i<objects.size();i++)
		objects[i]->ConnectReferences(this);
}


GDSObject *
GDSObjectList::GetTopObject()
{
    // Loop through objects
    for(unsigned int i=0;i<objects.size();i++)
	{
        bool found = false;
        for(unsigned int j=0;j<objects.size();j++)
		{
            GDSObject *obj = objects[j];
            
            if(obj->referencesToObject(objects[i]->GetName()))
            {
                found = true;
                break;
            }
        }
        if(!found) // Object is not referenced by any other objects
            return objects[i];
    }
    return NULL;
}

unsigned int	GDSObjectList::getNumObjects()
{
	return objects.size();
}

GDSObject* GDSObjectList::getObject(unsigned int index)
{
	assert(index < objects.size());
	return objects[index];
}

void GDSObjectList::buildObjectTree()
{
	if(tree)
		delete tree;

	// This will recursively build itself
	tree = new ObjectTree(GetTopObject(), GDSMat());
}