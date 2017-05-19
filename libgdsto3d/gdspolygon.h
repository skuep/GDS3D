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

#ifndef __GDSPOLYGON_H__
#define __GDSPOLYGON_H__

#include "gds_globals.h"
#include "process_cfg.h"
#include <math.h>

// All these types are 2D
class GDSMat
{
private:
	// 3x3 matrix, in XYW form, assumes bottom row is always [0,0,1]
	// | 0 2 4 |
	// | 1 3 5 |
	float entries[6]; 
public:
	GDSMat();
	GDSMat(float e0, float e1, float e2, float e3, float e4, float e5);

	// Operators
	Point2D operator*(const Point2D& P) const;
	GDSMat operator*(const GDSMat& M) const;
	float operator[](const int index) const {return entries[index];};
	bool operator<(const GDSMat& B) const; // Comparison for ordered lists

	// Manipulation
	void loadIdentity();
	void setScaling(const float& X, const float& Y);
	void setTranslation(const float& X, const float& Y);
	void setRotation(const float& angle);

	bool NegativeTrace() const;
	GDSMat Inverse() const;
	void Round();
};

inline
bool GDSMat::NegativeTrace() const
{
	return ((entries[0] < 0) != (entries[3] < 0));
}

// | 0 2 4 |   | X |
// | 1 3 5 | X | Y | 
inline Point2D GDSMat::operator*(const Point2D& P) const
{
	return Point2D(entries[0]*P.X+entries[2]*P.Y+entries[4], entries[1]*P.X+entries[3]*P.Y+entries[5]);
}

// | 0 2 4 |   | 0 2 4 |
// | 1 3 5 | X | 1 3 5 | 
inline GDSMat GDSMat::operator*(const GDSMat& M) const
{
	return GDSMat(
		entries[0]*M.entries[0]+entries[2]*M.entries[1],
		entries[1]*M.entries[0]+entries[3]*M.entries[1],
		entries[0]*M.entries[2]+entries[2]*M.entries[3],
		entries[1]*M.entries[2]+entries[3]*M.entries[3],
		entries[0]*M.entries[4]+entries[2]*M.entries[5]+entries[4],
		entries[1]*M.entries[4]+entries[3]*M.entries[5]+entries[5]
		);
}

inline bool GDSMat::operator<(const GDSMat& A) const
{
	for(unsigned int i=0;i<6;i++)
	{
		if(this->entries[i] < A.entries[i])
			return true;
		if(this->entries[i] > A.entries[i])
			return false;

		// Jump to next entry
	}
	 
	//We should never reach this point, means matrices are equal
	return false;
}

inline
void GDSMat::Round()
{
	// Rounds the matrix translation on 1nm grid and locks rotation to 90 degrees
	for(unsigned int i=0;i<=3;i++)
		entries[i] = floor(entries[i] + 0.5f); // Make either 0 or 1
	for(unsigned int i=4;i<=5;i++)
		entries[i] = floor(entries[i] *1000.0f + 0.5f) / 1000.0f;
}

class GDSBB
{
public:

	Point2D min, max;

	GDSBB();

	void clear();
	bool isEmpty();
	void addPoint(const Point2D& P);
	void merge(const GDSBB& BB);
	void transform(const GDSMat& M);
	bool isPointInside(const Point2D& P);

	static bool intersect(const GDSBB& BB1, const GDSBB& BB2);
};

class GDSTriangle
{
private:
	Point2D	coords[3];
	GDSBB	bbox;

	void project(const Point2D& axis, float& min, float& max) const;
	static void normal(const Point2D& P1, const Point2D& P2, Point2D& N);
public:
	GDSTriangle() {};

	void set(const Point2D& P1, const Point2D& P2, const Point2D& P3);
	static bool intersect(const GDSTriangle& T1, const GDSTriangle& T2);	
};

class GDSPolygon
{
private:
	float			_Height;
	float			_Thickness;
	vector<Point2D>	_Coords;
	vector<int>		indices;
	struct ProcessLayer	*_Layer;
	GDSBB			bbox;

	double epsilon;
	double area(const Point2D& A, const Point2D& B, const Point2D& C);
	bool onLine(const Point2D& A, const Point2D& B, const Point2D& P);
    bool insideTriangle(const Point2D& A, const Point2D& B, const Point2D& C,const Point2D& P);

public:
	GDSPolygon() {_Layer = NULL;};
	GDSPolygon(float Height, float Thickness, struct ProcessLayer *Layer);
	~GDSPolygon();

    void Clear();
	void CopyInto(GDSPolygon *p); // Remove? nothing really different from default copy..
	void AddPoint(float X, float Y);
	void Tesselate(); // Build a triangle index list

	GDSBB* GetBBox();
	float GetHeight();
	float GetThickness();
	unsigned int GetPoints();
	vector<int>* GetIndices();
	float GetXCoords(unsigned int Index);
	float GetYCoords(unsigned int Index);
	float GetAngleCoords(unsigned int Index);
	void SetAngleCoords(unsigned int Index, float value);
    void Flip(); // Flip the winding order
    void Orientate(); // Make sure normal points upwards
	struct ProcessLayer *GetLayer();
    bool isSimple();
	bool isPointInside(const Point2D& P);

	void transformPoints(const GDSMat& M);
	static bool intersect(GDSPolygon *P1, GDSPolygon *P2);
};

inline GDSBB* GDSPolygon::GetBBox()
{
	return &bbox;
}

inline struct ProcessLayer *GDSPolygon::GetLayer()
{
	return _Layer;
}

inline float GDSPolygon::GetHeight()
{
	return _Height;
}

inline float GDSPolygon::GetThickness()
{
	return _Thickness;
}

#endif // __GDSPOLYGON_H__
