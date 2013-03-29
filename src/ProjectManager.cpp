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
			xml_node<> *codenode = node->first_node("code");
			
			while (codenode != 0)
			{
				out << trimtrailingwhitespace(codenode->value()) << "\n" << flush;
				codenode = codenode->next_sibling(); //TODO i really wanna trim the end whitespace here, see the trimwhitespace function and modify
			}
			
			//out << trimwhitespace(node->first_node("code")->value()) << "\n" << flush;
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
	loadEnvironment(doc);
	cout << "finished load environment" << "\n" << flush;
}

/// <summary>
/// unzips a KIDE file to allow for edits to be made
/// </summary>
/// <param name="chosenFile">the filename to be unzipped</param>
/// <param name="workingPath">path to place the KIDE contents</param>
void unzipKIDE(string &chosenFile, string &workingPath)
{
	STARTUPINFO startinfo; //structure that allows you to, for example, run the program minimized (if it is a window program)
    ZeroMemory(&startinfo, sizeof(STARTUPINFO)); //initialize the memory and all the members
    startinfo.cb = sizeof(STARTUPINFO);
    startinfo.lpReserved = NULL;
    startinfo.lpDesktop = NULL;
    startinfo.lpTitle = NULL;
    startinfo.dwFlags = 0;
    startinfo.cbReserved2 = 0;
    startinfo.lpReserved2 = NULL;
    PROCESS_INFORMATION procinfo; //CreateProcess fills this structure with stuff you can pass to other winapi functions to control the child process
    ZeroMemory(&procinfo, sizeof(PROCESS_INFORMATION));
	cout << "starting 7zip process" << "\n" << flush;
	//TODO would like the path to be relative
	string process = "C:/aszgard5/szg/projects/Kosmos/lib/7zipCMD/7za.exe x -tzip \""+chosenFile+"\" -y -o\""+workingPath+"\" -mx=9";
	char* proc = strdup(process.c_str());
    CreateProcess(NULL, proc, NULL, NULL, false, 0, NULL, NULL, &startinfo, &procinfo); //this is the most important line in the program. it runs the program specified in the command-line argument (argv[1])
	WaitForSingleObject( procinfo.hProcess, INFINITE );
	CloseHandle(procinfo.hProcess); //and, clean up after Windows because I don't need the process handles it gives me
    CloseHandle(procinfo.hThread);
	free(proc);
	cout << "finished 7zip process" << "\n" << flush;
}

/// this function will create a new project at the specified location
void createNewProject(string &projectFolder, xml_document<> &doc, string &templateLocation)
{
	//cp template
	//unzip template
	unzipKIDE(templateLocation, projectFolder);
	string projectFile = projectFolder + "\\" + projectName + ".kproj";
	string templateKproj = projectFolder + "\\newProjectTemplate.kproj";
	rename(templateKproj.c_str(),projectFile.c_str());
	loadProject(projectFile, doc);
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

void findTemplateCallback(vector<string> args)
{
	templateName = args[0];
	cout << "template: " << templateName << "\n" << flush;
	cout << "findingfile: " << virtualdirectory.findingFile << "\n" << flush;
	virtualdirectory.startBrowse("findprojdir", &findProjectCallback,"Select project directory: ", "C:\\aszgard5\\szg\\projects\\");
	cout << "findingfile: " << virtualdirectory.findingFile << "\n" << flush;
}

void findProjectCallback(vector<string> args)
{
	projectDir = args[0];
	unsigned found = projectDir.find_last_of("/\\");
	string pname = projectDir.substr(found+1);
	setProjectName(pname);
	createNewProject(projectDir, codeTree, templateName);
	
	generateRecur(codeTree.first_node()->first_node()->first_node(), projectDir); //output code, folder, etc
}

void loadEnvironment(xml_document<> &doc)
{
	cout << "loading environment..." << "\n" << flush;
	xml_node<> *profile = doc.first_node()->first_node("profile");
	xml_node<> *user = profile->first_node("user");
	xml_node<> *objecties = profile->first_node("object");
	//read x y z h p r from user block and place object there
	
	cout << "obtained variables..." << "\n" << flush;
	//look at obj nodes, then check the doc for the location, then import and position at the correct location
	
	string path = projectDir + "\\data\\obj";
	//string filename = objecties->first_node("resourceName")->value();
	cout << "projdir: " << projectDir << "\n" << flush;
	vector<string> pathv;
	vector<string> filenamev;
	vector<int> objx;
	vector<int> objy;
	vector<int> objz;
	vector<int> objh;
	vector<int> objp;
	vector<int> objr;
	//filenamev.push_back(filename);
	
	xml_node<> *userlocation = user->first_node("startingLocation");
	if (userlocation != 0)
	{
		int x = atoi(userlocation->first_node("x")->value());
		int y = atoi(userlocation->first_node("y")->value());
		int z = atoi(userlocation->first_node("z")->value());
		int h = atoi(userlocation->first_node("heading")->value());
		int p = atoi(userlocation->first_node("pitch")->value());
		int r = atoi(userlocation->first_node("roll")->value());
		
		cout << "user set x..." << x << "\n" << flush;
		cout << "user set y..." << y << "\n" << flush;
		cout << "user set z..." << z << "\n" << flush;
		cout << "user set h..." << h << "\n" << flush;
		cout << "user set p..." << p << "\n" << flush;
		
		objx.push_back(x);
		objy.push_back(y);
		objz.push_back(z);
		objh.push_back(h);
		objp.push_back(p);
		objr.push_back(r);
		
		string filename = "MrBodyWithHands.obj";
		filenamev.push_back(filename);
		string pathy = "C:\\aszgard5\\szg\\projects\\Kosmos\\data\\obj";
		pathv.push_back(pathy);
		//Import::import("cello.obj", x, y, z, h, p, r, "C:\\aszgard5\\szg\\projects\\Kosmos\\data\\obj");
	}
	
	
	
	
	while(objecties != 0)
	{
		cout << "loading object..." << "\n" << flush;
		//filename = "";
		// get xyzhpr and import object
		cout << "wth: " << objecties->first_node()->value() << "\n" << flush;
		cout << "wth: " << objecties->first_node("name") << "\n" << flush;
		cout << "wth: " << objecties->first_node("name")->value() << "\n" << flush;
		cout << "wth: " << objecties->first_node("resourceName") << "\n" << flush;
		cout << "wth: " << objecties->first_node("resourceName")->value() << "\n" << flush;
		
		string filename = objecties->first_node("resourceName")->value();
		filenamev.push_back(filename);
		pathv.push_back(path);
		/*int length = filename.length();
		if (filename[length-1] == 'j' && filename[length-2] == 'b' && filename[length-3] == 'o')
		{
			cout << "wth file is obj " << "\n" << flush;
		}*/
		cout << "set filename..." << filename << "\n" << flush;
		cout << "setting x..." << objecties->first_node("x")->value() << "\n" << flush;
		int x = atoi(objecties->first_node("x")->value());
		cout << "set x..." << x << "\n" << flush;
		int y = atoi(objecties->first_node("y")->value());
		cout << "set y..." << y << "\n" << flush;
		int z = atoi(objecties->first_node("z")->value());
		cout << "set z..." << z << "\n" << flush;
		int h = atoi(objecties->first_node("heading")->value());
		cout << "set h..." << h << "\n" << flush;
		int p = atoi(objecties->first_node("pitch")->value());
		cout << "set p..." << p << "\n" << flush;
		int r = atoi(objecties->first_node("roll")->value());
		
		objx.push_back(x);
		objy.push_back(y);
		objz.push_back(z);
		objh.push_back(h);
		objp.push_back(p);
		objr.push_back(r);
		
		cout << "set xyzhpr..." << x << y << z << h << p << r << "\n" << flush;
		/*
			<resourceName>cello.obj</resourceName>
			<x>0</x>
			<y>4</y>
			<z>-4</z>
			<heading>0</heading>
			<pitch>0</pitch>
			<roll>0</roll>
		*/
		cout << "importing object..." << "\n" << flush;
		//Import::import(filename, x, y, z, h, p, r, path);
		//Import::import(filenamev.back(), path);
		cout << "object imported..." << "\n" << flush;
		objecties = objecties->next_sibling();
		cout << "obtained next object..." << objecties << "\n" << flush;
		if (objecties == 0)
		{
			cout << "break?..." << "\n" << flush;
			break;
		}
	}
	/*vector<string>::iterator it;
	for (it=filenamev.begin(); it!=filenamev.end(); ++it)
	{
		cout << ' ' << *it << '\n';
		Import::import(*it, path);
	}*/
	for( int i=0; i<filenamev.size(); ++i)
	{
		//Import::import(filenamev[i], path);
		Import::import(filenamev[i], objx[i], objy[i], objz[i], objh[i], objp[i], objr[i], pathv[i]);
	}
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
