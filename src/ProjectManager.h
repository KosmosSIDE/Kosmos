#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H



#include <iostream>
#include <fstream>
#include <vector>

//include for mkdir
#include <direct.h>

//rapid xml library
#include "../lib/rapidxml/rapidxml.hpp"
#include "../lib/rapidxml/rapidxml_print.hpp"
#include <windows.h>

#include "main.h"
#include "selectedObjects.h"
#include "Import.h"

using namespace std;
using namespace rapidxml;

class ProjectManager
{
	public:
		//removes beginning and end whitespace from a char*
		static char *trimwhitespace(char *str);
		//removes end whitespace from a char*
		static char *trimtrailingwhitespace(char *str);//find a string in a string and replace with a string
		
		//////////////////////////////////// the inplace functions do not create a new string, they modify the current one
		//removes beginning and end whitespace from a char*
		static char *trimwhitespaceinplace(char *str);
		//removes end whitespace from a char*
		static char *trimtrailingwhitespaceinplace(char *str);//find a string in a string and replace with a string
		
		//remove a node (and all children) with atrribute name = attributeName
		static void removeNodesWithAttribute(rapidxml::xml_node<> *node, char* attributeName, char* attributeValue);
		/// this function writes to a header file
		static void generateHeader(xml_node<> *node, ofstream &out);
		/// this function writes to a codefile
		/// param node: first codeblock in the file ie file->codeblocks->first codeblock
		/// param out: ofstream connecting to the file to output
		static void generateCode(xml_node<> *node, ofstream &out);
		/// This function recursively travels the tree writing out to filesystem
		static void generateRecur(xml_node<> *node, string &path);
		/// this function initializes doc by populating it with the contents from filename
		static void initialize(string &filename);
		/// load a previously created file for editing
		static void loadProject(string &filename);
		static void unzipKIDE(string &chosenFile, string &workingPath);
		/// this function will create a new project at the specified location
		static void createNewProject(string &projectFolder, string &templateLocation);
		/// this function saves a project to filepath from doc
		static void saveProject(string &filepath);

		static bool deleteDirectory(std::string& directoryname, int flags);
		
		static void findTemplateCallback(vector<string> args);
		static void findProjectCallback(vector<string> args);
		static void loadProjectCallback(vector<string> args);

		static void loadEnvironment(xml_document<> &doc);
		
	private:
		static int inline findAndReplace(string& source, const string& find, const string& replace);
};

#endif
