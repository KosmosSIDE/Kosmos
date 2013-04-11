//see cpp for documentation

#ifndef VIRTUALDIRECTORY_H
#define VIRTUALDIRECTORY_H

#include "main.h"
#include "CallFunction.h"
#include "Constants.h"

// VirtualDirectory class
// Purpose:
//		- used to browse directories
//		- 
//		- 
// Notes:
//		used to browse directories
class VirtualDirectory
{

	public:
		bool findingFile;
		bool foundFile;
		std::string filename;
		std::string callback; // function name to execute when finished finding file
		CallFunction cf;
		vector<string> currentDir;
		string dirName;
		int currentIndex;
		string title;
	
		VirtualDirectory()
		{
			findingFile = false;
			foundFile = false;
			filename = "";
			currentIndex = 0;
			dirName = ROOTPATH;
			title = "";
		}
		
		void startBrowse(const string& cb,void(*funPtr)(vector<string>), const string &titley = "", const string &startDir = "");
		void upPressed();
		void downPressed();
		void selectFile();
		void selectDirectory();
		void draw() const;
		void drawText(float distance, float ypos, string text, bool selected) const;
	
	private:
		void openDir(string directory);
};

#endif
