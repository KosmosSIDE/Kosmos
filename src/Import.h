// see cpp for documentation


#ifndef IMPORT_H
#define IMPORT_H

#include "main.h"
#include "ProjectManager.h"
#include "Object.h"

class Import
{
	public:
		
		static void copyTo(const string &frompath, const string &topath); //used to copy file to proper folder in current project directory
		static void import(const string &filename, const string &path); //used to import into our environment
		static void import(const string &filename, int x, int y, int z, int h, int p, int r, int scale, const string &path = "", int type = 2); //used to import into our environment
		static void importCallback(vector<string> args);
		
	private:
		static int inline findAndReplace(string& source, const string& find, const string& replace);
		static rapidxml::xml_document<> importdoc;
		static vector<char> importstr;
};


#endif
