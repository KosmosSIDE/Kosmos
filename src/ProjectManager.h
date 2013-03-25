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


using namespace std;
using namespace rapidxml;

//removes beginning and end whitespace from a char*
char *trimwhitespace(char *str);
//removes end whitespace from a char*
char *trimtrailingwhitespace(char *str);//find a string in a string and replace with a string
int inline findAndReplace(string& source, const string& find, const string& replace);
//remove a node (and all children) with atrribute name = attributeName
void removeNodesWithAttribute(rapidxml::xml_node<> *node, char* attributeName);
/// this function writes to a header file
void generateHeader(xml_node<> *node, ofstream &out);
/// this function writes to a codefile
/// param node: first codeblock in the file ie file->codeblocks->first codeblock
/// param out: ofstream connecting to the file to output
void generateCode(xml_node<> *node, ofstream &out);
/// This function recursively travels the tree writing out to filesystem
void generateRecur(xml_node<> *node, string &path);
/// this function initializes doc by populating it with the contents from filename
void initialize(string &filename, xml_document<> &doc);
/// load a previously created file for editing
void loadProject(string &filename, xml_document<> &doc);
/// this function will create a new project at the specified location
void createNewProject(string &projectFolder, xml_document<> &doc);
/// this function saves a project to filepath from doc
void saveProject(string &filepath, xml_document<> &doc);

#endif
