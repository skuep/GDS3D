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

#include "windowmanager.h"
#include "gdsparse_ogl.h"
#include "ui_highlight.h"

UIHighlight::UIHighlight()
{
	Disable();
	render_object = NULL;
	cur_instance = NULL;

	time = wm->new_timer();
}

void
UIHighlight::Disable()
{
	state = 0;
	cur_layer = NULL;
	triangles.clear();
	color_scale = 1.0f; // in renderer
}

void
UIHighlight::Reset()
{
	Disable();
	
	// Cleanup previously traced geometry
	if(render_object)
	{
		delete render_object;
		render_object = NULL;
	}
	cur_instance = NULL;
	instances.clear();
}

void
UIHighlight::Draw()
{
	float layerz = 0.0f;
	if(cur_layer)
		layerz = (cur_layer->Height+cur_layer->Height*1.5f*exploded_fraction + cur_layer->Thickness) / 1000.0f;

	if(state<1)
		return; // disabled

	// Render geometry of trace
	if(render_object)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-1.0,-1.0); // Bring closer
		
		color_scale = 1.0f;
		render_object->PrepareRender(projection, worldview);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDepthFunc(GL_LEQUAL);
		render_object->RenderList(worldview, false);
		render_object->EndRender();		
		glDepthFunc(GL_LESS);

		glDisable(GL_POLYGON_OFFSET_FILL);		
	}
	color_scale = 0.0f;

	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	// What about the modelview matrix??

	// Colored plane
	{
        /*
		float _xmin = wm->getWorld()->_xmin;
		float _ymin = wm->getWorld()->_ymin;
		float _xmax = wm->getWorld()->_xmax;
		float _ymax = wm->getWorld()->_ymax;*/

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);

		// Grid for debugging		
		/*
		int grid_z = 1;

		glBegin(GL_LINES);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glVertex3f(0.5,0.5,grid_z); glVertex3f(0.5,-0.5,grid_z);
		glVertex3f(0.5,-0.5,grid_z); glVertex3f(-0.5,-0.5,grid_z);
		glVertex3f(-.5,-0.5,grid_z); glVertex3f(-0.5,0.5,grid_z);
		glVertex3f(-0.5,0.5,grid_z); glVertex3f(0.5,0.5,grid_z);

		glEnd();

		glLineWidth(1.0);

		
		glBegin(GL_LINES);
		for(int i=(int)_xmin;i<=(int)_xmax;i++)
		{
			glVertex3i(i,(int)_ymin,grid_z); glVertex3i(i,(int)_ymax,grid_z);	
		}
		for(int i=(int)_ymin;i<=(int)_ymax;i++)
		{
			glVertex3i((int)_xmin,i,grid_z); glVertex3i((int)_xmax,i,grid_z);	
		}
		glEnd();

		glLineWidth(4.0);

		glBegin(GL_LINES);
		for(int i=((int)_xmin)/10*10;i<=((int)_xmax)/10*10;i+=10)
		{
			glVertex3i(i,(int)_ymin,grid_z); glVertex3i(i,(int)_ymax,grid_z);	
		}
		for(int i=((int)_ymin)/10*10;i<=((int)_ymax)/10*10;i+=10)
		{
			glVertex3i((int)_xmin,i,grid_z); glVertex3i((int)_xmax,i,grid_z);	
		}
		glEnd();

		glLineWidth(2.0);

		glBegin(GL_LINES);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(0.0,0.0,grid_z); glVertex3f(0.5,0.0,grid_z);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(0.0,0.0,grid_z); glVertex3f(0.0,0.5,grid_z);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glEnd();*/
		/*
		glPolygonOffset(-1,-1); // Bring closer
		glColor4f(1.0f, 0.0f, 0.0f, 0.33f);
		float extra = 0.05f;
        glEnable(GL_BLEND);*/
        /*
		glBegin(GL_TRIANGLES);		
        
		glVertex3f(_xmin-(_xmax-_xmin)*extra, _ymin-(_ymax-_ymin)*extra, layerz);
		glVertex3f(_xmin-(_xmax-_xmin)*extra, _ymax+(_ymax-_ymin)*extra, layerz);
		glVertex3f(_xmax+(_xmax-_xmin)*extra, _ymax+(_ymax-_ymin)*extra, layerz);
        
		glVertex3f(_xmax+(_xmax-_xmin)*extra, _ymax+(_ymax-_ymin)*extra, layerz);
		glVertex3f(_xmax+(_xmax-_xmin)*extra, _ymin-(_ymax-_ymin)*extra, layerz);
		glVertex3f(_xmin-(_xmax-_xmin)*extra, _ymin-(_ymax-_ymin)*extra, layerz);
        
		glEnd();*/
        
        glDisable(GL_BLEND);
        
		glDisable(GL_POLYGON_OFFSET_FILL);		
        
	}

	// Some points
	//if(triangles.size()>2)
	//{
	//	glEnable(GL_POLYGON_OFFSET_FILL);
	//	glPolygonOffset(-1,-2); // Bring even closer		

	//	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	//	glBegin(GL_TRIANGLES);
	//	for(unsigned int i=0;i<triangles.size()/3;i++)
	//	{
	//		glVertex3f(triangles[i*3+0].x, triangles[i*3+0].y, triangles[i*3+0].z);
	//		glVertex3f(triangles[i*3+1].x, triangles[i*3+1].y, triangles[i*3+1].z);
	//		glVertex3f(triangles[i*3+2].x, triangles[i*3+2].y, triangles[i*3+2].z);
	//	}
	//	glEnd();

	//	glDisable(GL_POLYGON_OFFSET_FILL);

	//}

	// Text
	{
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		//glPushMatrix();
        
		glLoadIdentity();
		glOrtho(0.0, (GLdouble) wm->screenWidth, 0.0, (GLdouble) wm->screenHeight, -1.0f, 1.0f);
        
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
        
		// Draw border
		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
		wm->getWorld()->gl_square(wm->screenWidth - 300.0f,wm->screenHeight - 90.0f, wm->screenWidth - 20.0f, wm->screenHeight - 140.0f, 1);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		wm->getWorld()->gl_square(wm->screenWidth - 300.0f, wm->screenHeight - 90.0f, wm->screenWidth - 20.0f, wm->screenHeight - 140.0f, 0);
        
		// Text
		wm->getWorld()->gl_printf(1.0f, 1.0f, 1.0f, 0.4f, wm->screenWidth - 290, wm->screenHeight - 110, "Net Highlight (Beta):");
		wm->getWorld()->gl_printf(1.0f, 1.0f, 1.0f, 0.4f, wm->screenWidth - 290, wm->screenHeight - 130, "Left-Click on Top of Metal");
		//wm->getWorld()->gl_printf(1.0f, 1.0f, 1.0f, 0.4f, wm->screenWidth - 290, wm->screenHeight - 150, "Esc to abort");
	}

	// Continue building the list if there's work to be done
	if(cur_instance)
	{
		processList();
		buildRenderObject();
		drawTracing(false);
	}	
}

bool
UIHighlight::Event(int event, int data, int xpos, int ypos , bool shift, bool control, bool alt)
{
	float layerz = 0.0f;
	//if(cur_layer)
		//layerz = (cur_layer->Height+cur_layer->Height*1.5f*exploded_fraction + cur_layer->Thickness) / 1000.0f;

	if( event == EVENT_KEY_UP ) // Key up
	{
		switch(data)
		{
		case KEY_H:
			if(state == 0)
			{
				ProcessLayer *layer = wm->getProcess()->GetLayer(255, 0); // This will reset the ruler to top metal every time..
				while(layer->Next != NULL)
					layer = layer->Next;
				cur_layer = layer;	
				state = 1;
				wm->getWorld()->LockOnUIElement(this);
			}
			else
			{
				state = 0;
				color_scale = 1.0f;
			}
			return true;	

		case KEY_ESC:
			if(cur_instance)
			{
				cur_instance = NULL;
				return true;
			}
			break;
		}
     }

	if(state < 1)
		return false; // disabled

	float ray_x = 0.0f;
	float ray_y = 0.0f;
	float ray_z = 0.0f;
	if(cur_layer)
	{
		VECTOR3D ray = VECTOR3D(-((float)wm->screenWidth)/((float)wm->screenHeight)*(((float)xpos)/((float)wm->screenWidth)-0.5f), 1.0f*(((float)ypos)/((float)wm->screenHeight)-0.5f), 1.07f); // HACK!!
		ray.Normalize(); // Is dit nodig?
		MATRIX4X4 view = worldview.GetInverse();
		VECTOR3D dir = view.GetRotatedVector3D(ray);
        
		ray_x = wm->getWorld()->_x-(wm->getWorld()->_z-layerz)/dir.z*dir.x;
		ray_y = wm->getWorld()->_y-(wm->getWorld()->_z-layerz)/dir.z*dir.y;
		ray_z = wm->getWorld()->_z-layerz;
	}
	/*
	if( event == EVENT_BUTTON_DOWN ) // mouse button down 
	{
        if ( data == 2 ) {// Mouse wheel down
			if (control && !alt) 
			{ 
				ProcessLayer *layer = cur_layer ;
				if(layer->Next)
					cur_layer = layer->Next;
				return true;
			}
		}
		if ( data == 3 ) {// Mouse wheel up
			if (control && !alt) 
			{ 
				ProcessLayer *layer = wm->getProcess()->GetLayer(255, 0);
				while(layer->Next)
				{
					if(layer->Next == cur_layer)
					{
						cur_layer = layer;
						break;
					}
					layer = layer->Next;
				}
				return true;
			}
		}
	}*/
	if( event == EVENT_BUTTON_UP ) // mouse button up 
	{
        if(data == 0 && wm->getWorld()->_first_move)
		{
			// Clear database
			triangles.clear();
			instances.clear();

			// Trace and highlight
			GDSMat identity; // For the world root 
			cur_poly = NULL;
			cur_layer = NULL;
			cur_mat = identity;

			// Step through all layers, calculate an intersect point and try to find intersections
			// We have to replace this with a true 3D ray tracing algorithm...
			ProcessLayer *layer = wm->getProcess()->GetLayer();
			while(layer)
			{
				if(!layer->Show || !layer->Metal)
				{
					layer = layer->Next;
					continue;
				}

				layerz = (layer->Height+layer->Height*1.5f*exploded_fraction + layer->Thickness) / 1000.0f;
				VECTOR3D ray = VECTOR3D(-((float)wm->screenWidth)/((float)wm->screenHeight)*(((float)xpos)/((float)wm->screenWidth)-0.5f), 1.0f*(((float)ypos)/((float)wm->screenHeight)-0.5f), 1.07f); // HACK!!
				ray.Normalize(); // Is dit nodig?
				MATRIX4X4 view = worldview.GetInverse();
				VECTOR3D dir = view.GetRotatedVector3D(ray);

				ray_x = wm->getWorld()->_x-(wm->getWorld()->_z-layerz)/dir.z*dir.x;
				ray_y = wm->getWorld()->_y-(wm->getWorld()->_z-layerz)/dir.z*dir.y;
				ray_z = wm->getWorld()->_z-layerz;

				if(ray_z > 0.0f)
					tracePoint(ray_x, ray_y,  wm->getWorld()->_topcell, identity, layer);
				layer = layer->Next;
			}

			if(cur_poly)
			{
				drawTracing(true);

				// Add the root polygon to the list
				ObjectInstance new_instance;
				instances[cur_mat] = new_instance;
				cur_instance = &instances[cur_mat];
				cur_instance->unchecked_poly.insert(cur_poly);

				// Trace path
				processList();

				// Build 3D polygons			
				buildRenderObject();
			}			
		}
	}

	return false;
}

// This is where the trace magic happens..

void 
UIHighlight::tracePoint(float x, float y, GDSObject *obj, GDSMat object_mat, ProcessLayer *layer)
{
	GDSPolygon poly;	

	// Is it within the boundary of this object?
	GDSBB boundary = obj->GetTotalBoundary();
	boundary.transform(object_mat);
	if(!boundary.isPointInside(Point2D(x, y)))
		return;

	// Check object polygons
	for(unsigned int i=0;i<obj->PolygonItems.size();i++)
	{
		poly = *obj->PolygonItems[i];

		//
		if(!poly.GetLayer()->Show || poly.GetLayer() != layer)
			continue;

		// Transform polygon
		poly.transformPoints(object_mat);

		// Raytraced point in polygon?
		// Goes wrong when flipped transformation matrix!!!
		poly.Orientate();
		if(!poly.isPointInside(Point2D(x, y)))
			continue;

		// Same as current layer?
		if((poly.GetLayer() != cur_layer) && cur_layer)
		{
			if(poly.GetLayer()->Height+poly.GetLayer()->Thickness < cur_layer->Height + cur_layer->Thickness)
				continue;
		}

		// Start tracing the path
		cur_layer = poly.GetLayer();
		cur_poly = obj->PolygonItems[i];
		cur_mat = object_mat;	
		cur_object = obj;
	}	

	// Propagate through hierarchy
	for(unsigned int i=0;i<obj->refs.size();i++)
		tracePoint(x, y, obj->refs[i]->object, object_mat * obj->refs[i]->mat, layer);
}

void 
UIHighlight::processList()
{
	GDSMat identity; // For the world root

	// Reset timer
	wm->timer(time, 1);

	// Work though the list
	while(cur_instance)
	{
		// Work though current instance
		while(cur_instance->unchecked_poly.size()>0)
		{
			GDSPolygon *poly = *cur_instance->unchecked_poly.begin();

			// Move to checked list
			cur_instance->unchecked_poly.erase(poly);
			cur_instance->checked_poly.insert(poly);			

			// Trace against world
			intersectTraverse(poly, cur_mat, wm->getWorld()->_topcell, identity);

			// Check timer?
			if(wm->timer(time, 0) > 1.0f)
				break;
		}

		// Check timer?
		if(wm->timer(time, 0) > 1.0f)
			break;

		// Find a new instance with work to do
		cur_instance = NULL;
		for(map<GDSMat, ObjectInstance>::iterator it=instances.begin(); it!=instances.end(); ++it)
		{
			if(it->second.unchecked_poly.size()>0)
			{
				cur_instance = &it->second;
				cur_mat = it->first;
				break;
			}
		}
	}
}

void 
UIHighlight::intersectTraverse(GDSPolygon *poly, GDSMat poly_mat, GDSObject *object, GDSMat object_mat)
{
	// Is it within the boundary of this object?
	GDSBB boundary = object->GetTotalBoundary();
	GDSBB bb = *poly->GetBBox();
	boundary.transform(object_mat);
	bb.transform(poly_mat);
	if(!GDSBB::intersect(bb, boundary))
		return;

	// Intersect with this object
	intersectPolyOnObject(poly, poly_mat, object, object_mat);

	// Go to sub cells
	for(unsigned int i=0;i<object->refs.size();i++)
		intersectTraverse(poly, poly_mat, object->refs[i]->object, object_mat * object->refs[i]->mat);
}

void 
UIHighlight::intersectPolyOnObject(GDSPolygon *poly, GDSMat poly_mat, GDSObject *object, GDSMat object_mat)
{
	GDSPolygon *target_poly;

	// Transform poly into worldspace -> do this on root level
	GDSPolygon transformed_poly = *poly;
	transformed_poly.transformPoints(poly_mat);	

	// Transform poly into object space
	GDSMat invMat = object_mat.Inverse();
	transformed_poly.transformPoints(invMat);

	// Cache instances who already have polygons in the check list

	for(unsigned int i=0;i<object->PolygonItems.size();i++)
	{
		target_poly = object->PolygonItems[i];

		// Possible reject on layers
		if(!target_poly->GetLayer()->Show)
			continue;
		if(target_poly->GetLayer() != poly->GetLayer())
		{			
			if(target_poly->GetLayer()->Height > poly->GetLayer()->Height+poly->GetLayer()->Thickness + 1.0f)
				continue; // Too high
			if(target_poly->GetLayer()->Height + target_poly->GetLayer()->Thickness + 1.0f < poly->GetLayer()->Height)
				continue; // Too low
			if(target_poly->GetLayer()->Metal == poly->GetLayer()->Metal)
				continue; // Only jump between VIA -> METAL or METAL -> VIA
		}
		else
		{
			if(!target_poly->GetLayer()->Metal)
				continue; // Do not intersect within VIA layers
		}

		// Do bounds overlap?
		if(!GDSBB::intersect(*transformed_poly.GetBBox(), *target_poly->GetBBox()))
			continue;		

		// Do we already have this polygon?
		map<GDSMat, ObjectInstance>::iterator cur_instance = instances.find(object_mat);
		if(cur_instance != instances.end())
		{
			if( cur_instance->second.checked_poly.find(target_poly) != cur_instance->second.checked_poly.end())
				continue; // Found it
		}					

		// Intersects with polygon?
		if(!GDSPolygon::intersect(&transformed_poly, target_poly))
			continue;

		// Add to unchecked list
		if(cur_instance == instances.end())
		{
			ObjectInstance new_instance;
			instances[object_mat] = new_instance;
			cur_instance = instances.find(object_mat);
		}
		cur_instance->second.unchecked_poly.insert(object->PolygonItems[i]);
	}		
}

void 
UIHighlight::buildRenderObject()
{
	GDSPolygon poly;

	if(render_object)
		delete render_object;

	render_object = new GDSObject_ogl((char*)"_ui_highlight");

	// Iterate over all object instances
	for(map<GDSMat, ObjectInstance>::iterator inst=instances.begin(); inst != instances.end(); ++inst)
	{
		// Iterate over all polygons (unchecked should be empty by now)
		for(set<GDSPolygon*>::iterator it=inst->second.checked_poly.begin(); it!=inst->second.checked_poly.end(); ++it)
		{
			poly = **it;
			poly.transformPoints(inst->first);
			poly.Orientate();

			render_object->AddPolygon(poly.GetHeight(), poly.GetThickness(), poly.GetPoints(), poly.GetLayer());
			*render_object->GetCurrentPolygon() = poly; // Perform a copy
		}
	}

}

void
UIHighlight::drawTracing(bool finish)
{
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glMatrixMode(GL_PROJECTION);
	//glPushMatrix();

	glLoadIdentity();
	glOrtho(0.0, (GLdouble) wm->screenWidth, 0.0, (GLdouble) wm->screenHeight, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	wm->getWorld()->gl_square(wm->screenWidth/2-70.0f, wm->screenHeight/2-36.0f, wm->screenWidth/2+70.0f , wm->screenHeight/2+16.0f, 1);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	wm->getWorld()->gl_square(wm->screenWidth/2-70.0f, wm->screenHeight/2-36.0f, wm->screenWidth/2+70.0f , wm->screenHeight/2+16.0f, 0);

	// Text
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	wm->getWorld()->gl_printf(1.0f, 1.0f, 1.0f, 0.4f, wm->screenWidth/2 - 40, wm->screenHeight/2-6, "Tracing..");
	wm->getWorld()->gl_printf(1.0f, 1.0f, 1.0f, 0.4f, wm->screenWidth/2 - 60, wm->screenHeight/2-26, " Esc to abort");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glEnable(GL_FOG);

    if(finish)
        wm->gl_finish();
}
