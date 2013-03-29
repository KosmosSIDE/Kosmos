/*
Summary: this function is used to import files into the environment
TODO: some functions may need to be moved from main to here for modularity
		copyTo function needs to be implemented

Author: Aaron Hardin

*/

#include "Import.h"


void Import::import(const string &filename,const string &path = "data/obj")
{
	cout << "attempting to import..." << filename << " from: " << path << "\n" << flush;
	int length = filename.length();
	if (filename[length-1] == 'j' && filename[length-2] == 'b' && filename[length-3] == 'o')
	{
		Object* newObj = new Object(2,1,1,1,filename,path);
		newObj->normalize();
		newObj->setMatrix(ar_translationMatrix(0, 4, -8)); // initial position
		objects.push_back(newObj);
	}
}

void Import::import(const string &filename, int &x, int &y, int &z, int &h, int &p, int &r, const string &path)
{
	cout << "attempting to import..." << filename << " from: " << path << "\n" << flush;
	int length = filename.length();
	if (filename[length-1] == 'j' && filename[length-2] == 'b' && filename[length-3] == 'o')
	{
		Object* newObj = (path != "")?new Object(2,1,1,1,filename,path):new Object(2,1,1,1,filename);
		cout << "created object..." << "\n" << flush;
		newObj->normalize();
		cout << "normalized object..." << "\n" << flush;
		newObj->setMatrix(ar_translationMatrix(x, y, z)); // initial position
		cout << "set object position..." << "\n" << flush;
		//TODO hpr
		objects.push_back(newObj);
	}
}

void Import::importCallback(vector<string> args)
{
	if(args.size() > 1)
	{
		import(args[0],args[1]);
	}
	else
	{
		import(args[0]);
	}
}

