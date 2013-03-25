/*#include <iostream>
#include <fstream>
#include <vector>

//include for mkdir
#include <direct.h>

//rapid xml library
#include "../lib/rapidxml/rapidxml.hpp"
#include "../lib/rapidxml/rapidxml_print.hpp"*/
#include "ProjectManager.h"

using namespace std;
using namespace rapidxml;

string projectName = "kosmosGenerated";
xml_document<> codeTree;

//removes beginning and end whitespace from a char*
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

//removes end whitespace from a char*
char *trimtrailingwhitespace(char *str)
{
  char *end;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

//find a string in a string and replace with a string
int inline findAndReplace(string& source, const string& find, const string& replace)
{
    int num=0;
    int fLen = find.size();
    int rLen = replace.size();
    for (int pos=0; (pos=source.find(find, pos))!=string::npos; pos+=rLen)
    {
        num++;
        source.replace(pos, fLen, replace);
    }
    return num;
}

//remove a node (and all children) with atrribute name = attributeName
void removeNodesWithAttribute(rapidxml::xml_node<> *node, char* attributeName)
{
	rapidxml::xml_node<> *sib = node->next_sibling();
	bool removed = false;
	if (node->first_attribute() != 0)
	{
		rapidxml::xml_attribute<> *attr = node->first_attribute();
		cout << "Node has attribute: " << attr->name() << ", with value: " << attr->value() << "\n";
		if (strcmp(attributeName, attr->name()) == 0)
		{
			rapidxml::xml_node<> *parent = node->parent();
			parent->remove_node(node);
			cout << "node has been removed" << "\n";
			removed = true;
		}
	}
	
	if (removed == false && node->first_node() != 0)
	{
		removeNodesWithAttribute(node->first_node(), attributeName);
	}
	if (sib != 0)
	{
		removeNodesWithAttribute(sib, attributeName);
	}
}

/*
void searchForParent(rapidxml::xml_node<> *node)
{
	if (node->first_attribute() != 0)
	{
		rapidxml::xml_attribute<> *attr = node->first_attribute();
		cout << "Node has attribute: " << attr->name() << ", with value: " << attr->value() << "\n";
	}
	
	if (node->first_node() != 0)
	{
		searchForParent(node->first_node());
	}
	if (node->next_sibling() != 0)
	{
		searchForParent(node->next_sibling());
	}
}
*/

/// this function writes to a header file
void generateHeader(xml_node<> *node, ofstream &out)
{
	//out << node value . replace (&quot; with ") . replace (&projName, projectName) etc.
	//cout << "create header" << "\n";
	while(node != 0)
	{
		if ((strcmp(node->first_node("type")->value(),"include") == 0) || (strcmp(node->first_node("type")->value(),"code") == 0))
		{
			//cout << node->first_node("code")->value() << "\n"; //TODO make sure you replace &projName; with the real projName, " and & already happen
			out << trimwhitespace(node->first_node("code")->value()) << "\n" << flush;
		}
		//out << node value . replace (&projName, projectName) etc.   //. replace (&quot; with ") already happens???
		//cout << "create code" << "\n";
		
		node = node->next_sibling();
	}
}

/// this function writes to a codefile
/// param node: first codeblock in the file ie file->codeblocks->first codeblock
/// param out: ofstream connecting to the file to output
void generateCode(xml_node<> *node, ofstream &out)
{
	while(node != 0)
	{
		//types function, include, code
		if (strcmp(node->first_node("type")->value(),"function") == 0)
		{
			xml_node<> *codenode = node->first_node("functioncode")->first_node();
			
			//cout << node->first_node("code")->value() << "\n"; //TODO make sure you replace &projName; with the real projName, " and & already happen
			out << trimwhitespace(node->first_node("documentation")->value()) << "\n" << flush;
			out << trimwhitespace(node->first_node("declaration")->value()) << "\n" << flush;
			out << "{" << flush;
			while (codenode != 0)
			{
				out << trimtrailingwhitespace(codenode->value()) << "\n" << flush;
				codenode = codenode->next_sibling(); //TODO i really wanna trim the end whitespace here, see the trimwhitespace function and modify
			}
			out << "\n";
			out << "}" << "\n\n" << flush;
			//documentation
			//declaration
			//functioncode
				//code*
		}
		else if ((strcmp(node->first_node("type")->value(),"include") == 0) || (strcmp(node->first_node("type")->value(),"code") == 0))
		{
			//cout << node->first_node("code")->value() << "\n"; //TODO make sure you replace &projName; with the real projName, " and & already happen
			out << trimwhitespace(node->first_node("code")->value()) << "\n" << flush;
		}
		//out << node value . replace (&projName, projectName) etc.   //. replace (&quot; with ") already happens???
		//cout << "create code" << "\n";
		
		node = node->next_sibling();
	}
}

/// This function recursively travels the tree writing out to filesystem
void generateRecur(xml_node<> *node, string &path)
{
	if (strcmp(node->name(),"file") == 0)
	{
		if (strcmp(node->first_node("type")->value(),"external") == 0)
		{
			//do nothing
			cout << "found external file: " << node->first_node("name")->value() << "\n";
		}
		else if (strcmp(node->first_node("type")->value(),"plaintext") == 0)
		{
			//make new stream out to path+filename
			//out << node value . replace (&quot; with ") . replace (&projName, projectName) etc.
			string newpath = path+string("/")+string(node->first_node("name")->value());
			ofstream myfile;
			myfile.open(newpath.c_str());
			string output = string(node->first_node("text")->value());
			// TODO implement replace findAndReplace(output, "&projName;","kosmos");
			findAndReplace(output, "&projName;",projectName);
			myfile << output;
			myfile.close();
			cout << "write to plaintext file: " << newpath << "\n";
		}
		else if (strcmp(node->first_node("type")->value(),"makefile") == 0)
		{
			//make new stream out to path+filename
			//out << node value . replace (&quot; with ") . replace (&projName, projectName) etc.
			string newpath = path+string("/")+string(node->first_node("name")->value());
			ofstream myfile;
			myfile.open(newpath.c_str());
			string output = string(node->first_node("text")->value());
			// TODO implement replace findAndReplace(output, "&projName;","kosmos");
			findAndReplace(output, "&projName;",projectName);
			myfile << output;
			myfile.close();
			cout << "write to plaintext file: " << newpath << "\n";
		}
		else if (strcmp(node->first_node("type")->value(),"codefile") == 0)
		{
			//make new stream out to path+filename
			string newpath = path+"/"+node->first_node("name")->value();
			ofstream myfile;
			myfile.open(newpath.c_str());
			generateCode(node->first_node("codeblocks")->first_node("codeblock"), myfile);
			myfile.close();
			
			cout << "write to codefile file: " << newpath << "\n";
			
			
		}
		else if (strcmp(node->first_node("type")->value(),"headerfile") == 0)
		{
			//make new stream out to path+filename
			string newpath = path+"/"+node->first_node("name")->value();
			ofstream myfile;
			myfile.open(newpath.c_str());
			generateHeader(node->first_node("codeblocks")->first_node("codeblock"), myfile);
			myfile.close();
			
			cout << "write to header file: " << newpath << "\n";
		}
		
		if(node->next_sibling() != 0)
		{
			generateRecur(node->next_sibling(), path);
		}
	}
	else if (strcmp(node->name(),"directory") == 0)
	{
		//make new directory at path+dirname
		string newpath = path+"/"+node->first_node("name")->value();
		
		//make directory newpath
		#if defined(_WIN32)
		_mkdir(newpath.c_str());
		cout << "create windows dir: " << newpath << "\n";
		#else 
		mkdir(newpath.c_str(), 0777); // notice that 777 is different than 0777
		cout << "create linux dir: " << newpath << "\n";
		#endif
		
		if(node->first_node() != 0)
		{
			generateRecur(node->first_node(), newpath);
		}
		if(node->next_sibling() != 0)
		{
			generateRecur(node->next_sibling(), path);
		}
	}
	else 
	{
		if(node->first_node() != 0)
		{
			generateRecur(node->first_node(), path);
		}
		if(node->next_sibling() != 0)
		{
			generateRecur(node->next_sibling(), path);
		}
	}
}

/// this function initializes doc by populating it with the contents from filename
void initialize(string &filename, xml_document<> &doc)
{
	ifstream myfile(filename.c_str());
	vector<char> buffer((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>( ));
	buffer.push_back('\0');
	
	doc.parse<0>(&buffer[0]);
	
	myfile.close();
	
	/*
	//searchForParent(doc.first_node());
	//cout << doc;
	//print(cout, doc, 0);
	//removeNodesWithAttribute(doc.first_node(), "parent");
	//searchForParent(doc.first_node());
	//cout << doc;
	*/
}

/// load a previously created file for editing
void loadProject(string &filename, xml_document<> &doc)
{
	initialize(filename, doc);
	//load into environment
}

/// this function will create a new project at the specified location
void createNewProject(string &projectFolder, xml_document<> &doc) //, int|string template //TODO
{
	//cp template
	//unzip template
	//loadProject(projectFolder/xmlFile, doc)
	//initialize
	//load into environment
}

/// this function saves a project to filepath from doc
void saveProject(string &filepath, xml_document<> &doc)
{
	//no save as for now...later maybe, needs to cp all external files
	cout << "saving project file to: " << filepath << "\n";
	ofstream myfile;
	myfile.open(filepath.c_str());
	myfile << doc;
	myfile.close();
}
/*
int main()
{
    string filename = "kosmos example proj.xml.bak";
	string path = "C:/Users/Aaron/Desktop/rapidxml-1.13/rapidxml-1.13";
	string filepath = path+string("/")+filename;
	
	
	//load
	initialize(filepath, codeTree); //create nodes
	
	//save
	saveProject(filepath, codeTree);
	
	//remove something
	//removeNodesWithAttribute(doc.first_node(), "parent");
	
	//export/generate code
	generateRecur(codeTree.first_node()->first_node()->first_node(), path); //output code, folder, etc	
}
*/
