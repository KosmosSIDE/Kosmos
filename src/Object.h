#ifndef OBJECT_H
#define OBJECT_H

#include "main.h"
#include <arMath.h>

// Object class
// Purpose:
//		- Creates an interactable object based on provided dimensions.
class Object:public arInteractableThing {
	
	//extern double currentTimeGlobal;
	
	public:
		
		bool _selected;
		
		// Object's loaded OBJ file
		arOBJRenderer loadedOBJ;
		arMatrix4 matrix;
		string name;
		string filenm;
	
		// Default constructor. 
		// Parameters are:
		//		type - 0 for cube, 1 for sphere, 2 for loaded OBJ file
		//		length - length (X-axis) of object
		//		height - height (Y-axis) of object
		//		width - width (Z-axis) of object
		Object(int type = 0, float length = 1.0, float height = 1.0, float width = 1.0, const string& filename = "", const string& path ="data/obj", const char* namey = ""):arInteractableThing() 
		{
			++numObjects;
			// Track values.
			_type = type;
			_length = length;
			_height = height;
			_width = width;
			filenm = filename;
			_selected = false;
			// Read OBJ file if provided.
			if(filename != "") 
			{
				cout << "loading " << filename << "..." << '\n';
				cout.flush();
				if(!loadedOBJ.readOBJ(filename,path)) 
				{ 
					cout << "Cound not load OBJ file: " << filename << '\n';
					_type = -1;
				}
				else if(_type == 0) 
				{
					// Set type to loaded file
					_type = 2;
				}
				else
				{
					cout << "loaded " << filename << '\n' << flush;
				}
				
				if(_type == 5)
				{
					//arInteractableThing::disable();
				}
				if(strcmp(namey,"") == 0 )
				{
					//make sure it doesn't exist, we may have something from previous obj
					ostringstream eek;
					unsigned found = filename.find_last_of("/\\");
					string namy = filename.substr(found+1);
					eek << namy.substr(0,namy.size()-4);
					eek << numObjects;
					vector<arInteractable*>::iterator i;
					for(i=objects.begin(); i != objects.end(); ++i) 
					{
						Object* obj = ((Object*)(*i));
						if(strcmp(eek.str().c_str(), obj->name.c_str())==0)
						{
							eek << "new";
						}
					}
					name = eek.str();
				}
				else //you passed a name
				{
					name = namey;
				}
			}
			// invalid type and file combination
			else if(_type >= 2) { _type = -1; }
		}
		
		// Draw the object's representation.
		void draw();
		void normalize() { loadedOBJ.normalizeModelSize(); }
		
		// Get object's type.
		int getType() { return _type; }
		
		// Get object's length, height, or width.
		float getLength() { return _length; }
		float getHeight() { return _height; }
		float getWidth() { return _width; }
		arOBJRenderer* getOBJ() { return &loadedOBJ; }
		void snapMatrix();
		void setHPR(float h, float p, float r);
		void setMatrix(const arMatrix4& matrix);
		bool touch( arEffector& effector );
		bool _touch( arEffector& /*effector*/ );
		
		void updateProjectFile();
		void setScale(float scaleBy) {_length = _length*scaleBy; _height = _height*scaleBy; _width = _width*scaleBy; updateProjectFile();}
		
		void deleteObject();
		void insertObject();
		
	private:
		
		// Object's type (0 for cube, 1 for sphere, 2 for loaded object)
		int _type;
		// Object's length, height, and width.
		float _length;
		float _height;
		float _width;
		bool processInteraction( arEffector& effector );
		
		
		//bool _selected;
};

#endif
