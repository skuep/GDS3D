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
#include "gdspolygon.h"
#include "../math/Maths.h"

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

// GDSMat Class
GDSMat::GDSMat()
{
	loadIdentity();
}

GDSMat::GDSMat(float e0, float e1, float e2, float e3, float e4, float e5)
{
	entries[0] = e0;
	entries[1] = e1;
	entries[2] = e2;
	entries[3] = e3;
	entries[4] = e4;
	entries[5] = e5;
}

void GDSMat::loadIdentity()
{
	memset(entries, 0, 6*sizeof(float));
	entries[0]=1.0f;
	entries[3]=1.0f;
}

void GDSMat::setScaling(const float& X, const float& Y)
{
	memset(entries, 0, 6*sizeof(float));
	entries[0] = X;
	entries[3] = Y;
}

void GDSMat::setTranslation(const float& X, const float& Y)
{
	memset(entries, 0, 6*sizeof(float));
	entries[0]=1.0f;
	entries[3]=1.0f;
	entries[4] = X;
	entries[5] = Y;
}

void GDSMat::setRotation(const float& angle)
{
	memset(entries, 0, 6*sizeof(float));

	entries[0]=(float)cos(M_PI*angle/180);
	entries[1]=(float)sin(M_PI*angle/180);

	entries[2]=-entries[1];
	entries[3]=entries[0];
}

// This code is untested...
// http://www.wikihow.com/Inverse-a-3X3-Matrix
//
GDSMat GDSMat::Inverse()const
{
	GDSMat result;
	float det;
	float tmp[9];

	det = entries[0]*entries[3]-entries[2]*entries[1];

	tmp[0] = entries[3];
	tmp[1] = -entries[2];
	tmp[2] = (entries[2]*entries[5] - entries[4]*entries[3]);

	tmp[3] = -entries[1];
	tmp[4] = entries[0];
	tmp[5] = -(entries[0]*entries[5] - entries[4]*entries[1]);

	tmp[6] = 0.0;
	tmp[7] = 0.0;
	tmp[8] = (entries[0]*entries[3] - entries[2]*entries[1]);

	return GDSMat(tmp[0]/det, tmp[3]/det, tmp[1]/det, tmp[4]/det, tmp[2]/det, tmp[5]/det);
}

// GDSBB Class
GDSBB::GDSBB()
{
	clear();
}

void GDSBB::clear()
{
	min.X = min.Y = 10000.0f;
	max.X = max.Y = -10000.0f;
}

bool GDSBB::isEmpty()
{
	if(min.X > max.X || min.Y > max.Y)
		return true;

	return false;
}

void GDSBB::addPoint(const Point2D& P)
{
	min.X = std::min(min.X, P.X);
	min.Y = std::min(min.Y, P.Y);
	max.X = std::max(max.X, P.X);
	max.Y = std::max(max.Y, P.Y);
}

void GDSBB::merge(const GDSBB& BB)
{
	min.X = std::min(min.X, BB.min.X);
	min.Y = std::min(min.Y, BB.min.Y);
	max.X = std::max(max.X, BB.max.X);
	max.Y = std::max(max.Y, BB.max.Y);
}

void GDSBB::transform(const GDSMat& M)
{
	Point2D P0(min.X, min.Y);
	Point2D P1(min.X, max.Y);
	Point2D P2(max.X, min.Y);
	Point2D P3(max.X, max.Y);

	clear();
	addPoint(M * P0);
	addPoint(M * P1);
	addPoint(M * P2);
	addPoint(M * P3);
}

bool GDSBB::isPointInside(const Point2D& P)
{
	if( (P.X - max.X) > 0.001f || (min.X-P.X) > 0.001f )
			return false;
	if( (P.Y - max.Y) > 0.001f || (min.Y-P.Y) > 0.001f )
			return false;

	return true;
}

bool GDSBB::intersect(const GDSBB& BB1, const GDSBB& BB2)
{
	// How much of a margin??
	if( (BB1.min.X - BB2.max.X) > 0.001f || (BB2.min.X-BB1.max.X) > 0.001f )
			return false;
	if( (BB1.min.Y - BB2.max.Y) > 0.001f || (BB2.min.Y-BB1.max.Y) > 0.001f )
			return false;

	return true;
}

// GDSTriangle Class
void GDSTriangle::set(const Point2D& P1, const Point2D& P2, const Point2D& P3)
{
	coords[0] = P1;
	coords[1] = P2;
	coords[2] = P3;

	bbox.addPoint(P1);
	bbox.addPoint(P2);
	bbox.addPoint(P3);
}

void GDSTriangle::project(const Point2D& axis, float& min, float& max) const
{
	float d;

	min = axis.X*coords[0].X+axis.Y*coords[0].Y;
	max = min;

	d = axis.X*coords[1].X+axis.Y*coords[1].Y;
	min = std::min(min, d);
	max = std::max(max, d);

	d = axis.X*coords[2].X+axis.Y*coords[2].Y;
	min = std::min(min, d);
	max = std::max(max, d);
}

void GDSTriangle::normal(const Point2D& P1, const Point2D& P2, Point2D& N)
{
	N.X = P1.Y - P2.Y;
	N.Y = P2.X - P1.X;
}

bool GDSTriangle::intersect(const GDSTriangle& T1, const GDSTriangle& T2)
{
	Point2D axis;
	float min1 = 0, max1 = 0, min2 = 0, max2 = 0;

	// Bounding box Test
	if(!GDSBB::intersect(T1.bbox, T2.bbox))
		return false;

	// Test T1 edges
	for(unsigned int i=0;i<3;i++)
	{
		GDSTriangle::normal(T1.coords[i], T1.coords[(i+1)%3], axis);
		T1.project(axis, min1, max1);
		T2.project(axis, min2, max2);
		if( (min1 - max2) > 0.001f || (min2-max1) > 0.001f )
			return false;
	}

	// Test T2 edges
	for(unsigned int i=0;i<3;i++)
	{
		GDSTriangle::normal(T2.coords[i], T2.coords[(i+1)%3], axis);
		T1.project(axis, min1, max1);
		T2.project(axis, min2, max2);
		if( (min1 - max2) > 0.001f || (min2-max1) > 0.001f )
			return false;
	}

	// No seperating axis, so intersection must have taken place
	return true;
}

// GDSPolygon Class
GDSPolygon::GDSPolygon(float Height, float Thickness, struct ProcessLayer *Layer)
{
	_Height = Height;
	_Thickness = Thickness;
	_Layer = Layer;

	epsilon = 0.001; // Default precision of 1nm
}

GDSPolygon::~GDSPolygon()
{
}

void
GDSPolygon::Clear()
{
    _Coords.clear();
    indices.clear();
	bbox.clear();
}

void 
GDSPolygon::CopyInto(GDSPolygon *p)
{
	// Nothing special happens here
	p->_Height = _Height;
	p->_Thickness = _Thickness;
	p->_Layer = _Layer;
    p->indices = indices;
    p->_Coords = _Coords;
	p->bbox = bbox;
}

void 
GDSPolygon::AddPoint(float X, float Y)
{
    _Coords.push_back(Point2D(X,Y));
	bbox.addPoint(Point2D(X,Y));
}

void
GDSPolygon::Tesselate()
{
	if(indices.size() > 0 || _Coords.size() < 3)
		return;

	// Fast path for simple polygons
	if(isSimple())
	{
		for(unsigned int j=0;j<_Coords.size()-2;j++)
        {
                indices.push_back(0);
                indices.push_back(j+1);
                indices.push_back(j+2);
        }
		return;
	}
    
    // Build double linked list, with N indices
    vector<int> V;
    for (unsigned int i = 0; i <_Coords.size(); i++)
        V.push_back(i);
    
    // Prepare to build N-2 triangles
    int a,b,c;
    for (unsigned int i = 0; i < _Coords.size()-2; i++)
    {
        // Go through the double linked list
        for(unsigned int j=0;j<V.size();j++)
        {
            a = j; b = (j+1)%V.size(); c = (j+2)%V.size();
            bool flagged = false;     
            
            // Orientation or degenerate?
            if(area(_Coords[V[a]], _Coords[V[b]], _Coords[V[c]]) < (epsilon))
                continue;
            
            // Go through all of the points
            for(unsigned int k=0;k<V.size();k++) 
            {
				// Same point as triangle
                if(k==a || k==b || k==c) 
                    continue;

				// Check if on polygon edge				
                if( abs(V[a]-V[b])==1 ||  abs(V[a]-V[b])==_Coords.size()-1)
				{
					if(onLine(_Coords[V[a]], _Coords[V[b]], _Coords[V[k]]))
						continue;
				}
				if( abs(V[b]-V[c])==1 ||  abs(V[b]-V[c])==_Coords.size()-1)
				{
					if(onLine(_Coords[V[b]], _Coords[V[c]], _Coords[V[k]]))
						continue;
				}
				if( abs(V[c]-V[a])==1 ||  abs(V[c]-V[a])==_Coords.size()-1)
				{
					if(onLine(_Coords[V[c]], _Coords[V[a]], _Coords[V[k]]))
						continue;
				}

				// Check if in triangle
                if(insideTriangle(_Coords[V[a]], _Coords[V[b]], _Coords[V[c]], _Coords[V[k]]))
                {
                    flagged = true;
                    break;
                }
            }
            
            // We have an ear
            if(!flagged)
            {
                indices.push_back(V[a]);
                indices.push_back(V[b]);
                indices.push_back(V[c]);
                V.erase(V.begin()+b);
                break;
            }
        }
    }
    
    
    //Clean up
    V.clear();
}

float GDSPolygon::GetXCoords(unsigned int Index)
{
	return _Coords[Index].X;
}

float GDSPolygon::GetYCoords(unsigned int Index)
{
	return _Coords[Index].Y;
}

unsigned int GDSPolygon::GetPoints()
{
	return _Coords.size();
}

vector<int>* GDSPolygon::GetIndices()
{
	// Tesselate if not done before
	if(indices.size() == 0)
		Tesselate();

	return &indices;
}

void GDSPolygon::Flip()
{
	// Flip points for boundary
    vector<Point2D> TCoords = _Coords;
    for(unsigned int i=0;i<TCoords.size();i++)
        _Coords[i] = TCoords[TCoords.size()-1-i];

	// Adjust indices?
    int a,b,c;

    for(unsigned int i=0;i<indices.size()/3;i++)
    {
        // New index numbers
        a = _Coords.size()-1-indices[i*3+0];
        b = _Coords.size()-1-indices[i*3+1];
        c = _Coords.size()-1-indices[i*3+2];

        // Swap Order
        indices[i*3+0] = a;
        indices[i*3+1] = c;
        indices[i*3+2] = b;
    }
}

void GDSPolygon::Orientate()
{
    float x0,y0,x1,y1,x2,y2,nz;
    float dx1,dy1,dx2,dy2;
    nz = 0.0f;
    
    // Do we have to flip?    
    for(unsigned int j=1; j<_Coords.size(); j++){
		if(j==1)
		{
			x0 = _Coords[0].X;
			y0 = _Coords[0].Y;
		}
        x1 = _Coords[j].X;// + offx;
        y1 = _Coords[j].Y;// + offy;
        
        x2 = _Coords[(j+1)%_Coords.size()].X;// + offx;
        y2 = _Coords[(j+1)%_Coords.size()].Y;// + offy;
        
        dx1 = x2 - x0;
        dy1 = y2 - y0;
        
        dx2 = x2 - x1;
        dy2 = y2 - y1;
        nz += dx1*dy2 - dy1*dx2;
    }
    
    if(nz < 0.0f)
        Flip();
}

bool
GDSPolygon::isSimple()
{
    int numPos, numNeg;
    numPos = numNeg = 0;
    
    float dx1, dy1, dx2, dy2, nz;
    
    for(unsigned int j=0; j<_Coords.size(); j++){ // Iterate over edges
        dx1 = _Coords[(j+1)%_Coords.size()].X - _Coords[j].X;
        dy1 = _Coords[(j+1)%_Coords.size()].Y - _Coords[j].Y;
        
        dx2 = _Coords[(j+2)%_Coords.size()].X - _Coords[(j+1)%_Coords.size()].X;
        dy2 = _Coords[(j+2)%_Coords.size()].Y - _Coords[(j+1)%_Coords.size()].Y;
        
        nz = dx1*dy2 - dy1*dx2;
        
        if(nz > 0.0000001f)
            numPos+=1;
        if(nz < -0.0000001f)
            numNeg+=1;
    }
    
    if(numPos>0 && numNeg>0)
        return false;
    
    return true;
}

bool 
GDSPolygon::isPointInside(const Point2D& P)
{
	//GDSTriangle T;

	//We are doing this brute force
	for(unsigned int i=0;i<indices.size()/3;i++)
	{
		if( insideTriangle(_Coords[indices[i*3+0]], _Coords[indices[i*3+1]], _Coords[indices[i*3+2]], P))
			return true;
	}
	return false;
}

// Private functions

double 
GDSPolygon::area(const Point2D& A, const Point2D& B, const Point2D& C)
{
    return ((B.X - A.X) * (C.Y - A.Y)) - ((B.Y - A.Y) * (C.X - A.X));
}

bool 
GDSPolygon::onLine(const Point2D& A, const Point2D& B,const Point2D& P)
{
	// Check if a point lies on a line segment


	// Inside bounding box?
	if(P.X < A.X && P.X < B.X)
		return false;
	if(P.X > A.X && P.X > B.X)
		return false;
	if(P.Y < A.Y && P.Y < B.Y)
		return false;
	if(P.Y > A.Y && P.Y > B.Y)
		return false;
	
	// On line?
	double a,b;  
	
	if( B.X - A.X != 0.0f)
	{
		a = (B.Y - A.Y) / (B.X - A.X);
		b = A.Y - a * A.X;
		if ( fabs(P.Y - (a*P.X+b)) < epsilon)
			return true;
	}
	else
	{
		a = (B.X - A.X) / (B.Y - A.Y);
		b = A.X - a * A.Y;
		if ( fabs(P.X - (a*P.Y+b)) < epsilon)
			return true;
	}

	return false;
}

bool 
GDSPolygon::insideTriangle(const Point2D& A, const Point2D& B, const Point2D& C,const Point2D& P)
{
    double ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
    double cCROSSap, bCROSScp, aCROSSbp;

	// Exclude points on vertices
	if(A.X == P.X && A.Y == P.Y)
		return false;
	if(B.X == P.X && B.Y == P.Y)
		return false;
	if(C.X == P.X && C.Y == P.Y)
		return false;
    
    ax = C.X - B.X;  ay = C.Y - B.Y;
    bx = A.X - C.X;  by = A.Y - C.Y;
    cx = B.X - A.X;  cy = B.Y - A.Y;
    apx = P.X - A.X;  apy = P.Y - A.Y;
    bpx = P.X - B.X;  bpy = P.Y - B.Y;
    cpx = P.X - C.X;  cpy = P.Y - C.Y;
    
    aCROSSbp = ax * bpy - ay * bpx;
    cCROSSap = cx * apy - cy * apx;
    bCROSScp = bx * cpy - by * cpx;
    
    return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
    
    //return ((aCROSSbp >= epsilon) && (bCROSScp >= epsilon) && (cCROSSap >= epsilon));
}

void GDSPolygon::transformPoints(const GDSMat& M)
{
	// Clear bounding box
	bbox.clear();

	for(unsigned int i=0;i<_Coords.size();i++)
	{
		_Coords[i] = M * _Coords[i];
		bbox.addPoint(_Coords[i]);
	}
}

bool GDSPolygon::intersect(GDSPolygon *P1, GDSPolygon *P2)
{
	GDSTriangle		T1, T2;

	// Bounding box intersection
	if(!GDSBB::intersect(P1->bbox, P2->bbox))
		return false;

	//We are doing this brute force
	for(unsigned int i=0;i<P1->indices.size()/3;i++)
	{
		for(unsigned int j=0;j<P2->indices.size()/3;j++)
		{
			T1.set(P1->_Coords[P1->indices[i*3+0]], P1->_Coords[P1->indices[i*3+1]], P1->_Coords[P1->indices[i*3+2]]);
			T2.set(P2->_Coords[P2->indices[j*3+0]], P2->_Coords[P2->indices[j*3+1]], P2->_Coords[P2->indices[j*3+2]]);

			if(GDSTriangle::intersect(T1, T2))
				return true;
		}
	}

	return false;
}

