#include "ProjectManager.h"
#include "ExtendBlock.h"

using namespace std;
using namespace rapidxml;

//removes beginning and end whitespace from a char*
char *ProjectManager::trimwhitespace(char *c)
{
	char *str = new char[strlen(c)+1];
	strcpy(str, c);

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
char *ProjectManager::trimtrailingwhitespace(char *c)
{
	char *str = new char[strlen(c)+1];
	strcpy(str, c);

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

//removes beginning and end whitespace from a char*
char *ProjectManager::trimwhitespaceinplace(char *str)
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
char *ProjectManager::trimtrailingwhitespaceinplace(char *str)
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
int inline ProjectManager::findAndReplace(string& source, const string& find, const string& replace)
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

//remove a node (and all children) with atrribute name = attributeName and attribute value = attributeValue
void ProjectManager::removeNodesWithAttribute(rapidxml::xml_node<> *node, char* attributeName, char* attributeValue)
{
	rapidxml::xml_node<> *sib = node->next_sibling();
	bool removed = false;
	if (node->first_attribute() != 0)
	{
		rapidxml::xml_attribute<> *attr = node->first_attribute(attributeName);
		cout << "Node has attribute: " << attr->name() << ", with value: " << attr->value() << "\n";
		if (strcmp(attributeValue, attr->value()) == 0)
		{
			rapidxml::xml_node<> *parent = node->parent();
			parent->remove_node(node);
			cout << "node has been removed" << "\n";
			removed = true;
		}
	}
	
	if (removed == false && node->first_node() != 0)
	{
		removeNodesWithAttribute(node->first_node(), attributeName, attributeValue);
	}
	if (sib != 0)
	{
		removeNodesWithAttribute(sib, attributeName, attributeValue);
	}
}

/// this function writes to a header file
void ProjectManager::generateHeader(xml_node<> *node, ofstream &out)
{
	while(node != 0)
	{
		if ((strcmp(node->first_node("type")->value(),"include") == 0) || (strcmp(node->first_node("type")->value(),"code") == 0))
		{
			char* c = trimwhitespace(node->first_node("code")->value());
			out << c << "\n" << flush;
			delete [] c;
		}
		else if (strcmp(node->first_node("type")->value(),"classDefinition") == 0)
		{
			xml_node<> *publiccode = node->first_node("public")->first_node();
			xml_node<> *constructor = node->first_node("constructor");
			xml_node<> *privatecode = node->first_node("private")->first_node();
			
			char *c = trimwhitespace(node->first_node("documentation")->value());
			out << c << "\n" << flush;
			delete [] c;
			char *d = trimwhitespace(node->first_node("declaration")->value());
			out << d << "\n" << flush;
			delete [] d;
			out << "{" << flush;
			while (publiccode != 0)
			{
				char *e = trimtrailingwhitespace(publiccode->value());
				out << e << "\n" << flush;
				delete [] e;
				publiccode = publiccode->next_sibling(); 
			}
			
			//handle constructor
			xml_node<> *codenode = constructor->first_node("functioncode")->first_node();
			
			char *f = trimwhitespace(constructor->first_node("documentation")->value());
			out << f << "\n" << flush;
			delete []f;
			char *g = trimwhitespace(constructor->first_node("declaration")->value());
			out << g << "\n" << flush;
			delete []g;
			out << "{" << flush;
			while (codenode != 0)
			{
				char *h = trimtrailingwhitespace(codenode->value());
				out << h << "\n" << flush;
				delete [] h;
				codenode = codenode->next_sibling();
			}
			out << "\n";
			out << "}" << "\n\n" << flush;
			
			while (privatecode != 0)
			{
				char *j = trimtrailingwhitespace(privatecode->value());
				out << j << "\n" << flush;
				delete []j;
				privatecode = privatecode->next_sibling(); 
			}
			out << "\n";
			out << "};" << "\n\n" << flush;
			//documentation
			//declaration
			//public
				//code*
			//private
				//code*
		}
		
		node = node->next_sibling();
	}
}

/// this function writes to a codefile
/// param node: first codeblock in the file ie file->codeblocks->first codeblock
/// param out: ofstream connecting to the file to output
void ProjectManager::generateCode(xml_node<> *node, ofstream &out)
{
	while(node != 0)
	{
		//types function, include, code
		if (strcmp(node->first_node("type")->value(),"function") == 0)
		{
			xml_node<> *codenode = node->first_node("functioncode")->first_node();
			
			//TODO make sure you replace &projName; with the real projName, " and & already happen
			char * c = trimwhitespace(node->first_node("documentation")->value());
			out << c << "\n" << flush;
			delete [] c;
			char *b = trimwhitespace(node->first_node("declaration")->value());
			out << b << "\n" << flush;
			delete [] b;
			out << "{" << flush;
			while (codenode != 0)
			{
				char *a = trimtrailingwhitespace(codenode->value());
				out << a << "\n" << flush;
				delete [] a;
				codenode = codenode->next_sibling();
			}
			out << "\n";
			out << "}" << "\n\n" << flush;
		}
		else if ((strcmp(node->first_node("type")->value(),"include") == 0) || (strcmp(node->first_node("type")->value(),"code") == 0))
		{
			//TODO make sure you replace &projName; with the real projName, " and & already happen
			xml_node<> *codenode = node->first_node("code");
			
			while (codenode != 0)
			{
				char *a = trimtrailingwhitespace(codenode->value());
				out << a << "\n" << flush;
				delete [] a;
				codenode = codenode->next_sibling();
			}
		}
		
		node = node->next_sibling();
	}
}

/// This function recursively travels the tree writing out to filesystem
void ProjectManager::generateRecur(xml_node<> *node, string &path)
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
			string newpath = path+string("/")+string(node->first_node("name")->value());
			ofstream myfile;
			myfile.open(newpath.c_str());
			string output = string(node->first_node("text")->value());
			ProjectManager::findAndReplace(output, "&projName;",projectName);
			myfile << output;
			myfile.close();
			cout << "write to plaintext file: " << newpath << "\n";
		}
		else if (strcmp(node->first_node("type")->value(),"makefile") == 0)
		{
			string newpath = path+string("/")+string(node->first_node("name")->value());
			ofstream myfile;
			myfile.open(newpath.c_str());
			string output = string(node->first_node("text")->value());
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

// this function initializes doc by populating it with the contents from filename
void ProjectManager::initialize(string &filename)
{
	//codeTree.clear();
	ifstream myfile(filename.c_str());
	vector<char> documenty((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>( ));
	documenty.push_back('\0');
	
	document = documenty;
	
	//std::ifstream myfile(filename.c_str(), std::ios::binary);
	//std::vector<char> fileContents;
	//document.reserve(fileSize);
	//document.assign(std::istreambuf_iterator<char>(myfile),
		//				std::istreambuf_iterator<char>());
		
		//ifstreambuf_iterator iter(myfile);
		//std::copy(iter.begin(), iter.end(), std::back_inserter(document));
	/*	typedef std::istream_iterator<char> istream_iterator;

			myfile >> std::noskipws;
		std::copy(istream_iterator(myfile), istream_iterator(),
				  std::back_inserter(document));*/
	
	codeTree.parse<0>(&document[0]);
	//codeTree.parse<parse_full>(&document[0]);
	
	myfile.close();
}

void ProjectManager::loadEnvironment(xml_document<> &doc)
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
	vector<int> objscale;
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
		objscale.push_back(7);
		
		string filename = "MrBodyWithHands.obj";
		filenamev.push_back(filename);
		string pathy = PATH+"Kosmos\\data\\obj";
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
		int x = atoi(objecties->first_node("x")->value());
		int y = atoi(objecties->first_node("y")->value());
		int z = atoi(objecties->first_node("z")->value());
		int h = atoi(objecties->first_node("heading")->value());
		int p = atoi(objecties->first_node("pitch")->value());
		int r = atoi(objecties->first_node("roll")->value());
		
		objx.push_back(x);
		objy.push_back(y);
		objz.push_back(z);
		objh.push_back(h);
		objp.push_back(p);
		objr.push_back(r);
		objscale.push_back(1);
		
		/*
			<resourceName>cello.obj</resourceName>
			<x>0</x>
			<y>4</y>
			<z>-4</z>
			<heading>0</heading>
			<pitch>0</pitch>
			<roll>0</roll>
		*/
		objecties = objecties->next_sibling();
		if (objecties == 0)
		{
			break;
		}
	}
	
	for( int i=0; i<filenamev.size(); ++i)
	{
		if (i==0)
		{
			Import::import(filenamev[i], objx[i], objy[i], objz[i], objh[i], objp[i], objr[i], objscale[i], pathv[i],5);
		}
		else
		{
			Import::import(filenamev[i], objx[i], objy[i], objz[i], objh[i], objp[i], objr[i], objscale[i], pathv[i]);
		}
	}
	
	filenamev.clear();
	objx.clear();
	objy.clear();
	objz.clear();
	objh.clear();
	objp.clear();
	objr.clear();
	objscale.clear();
	pathv.clear();
	
	
	rightWiimote = new Object(4, 0.5, 0.5, 0.5, "MrWiimote.obj");
	rightWiimote->normalize();
	rightWiimote->setMatrix(ar_translationMatrix(3, 2, -1.6)); // initial position
	rightWiimote->setHPR(135,0,0);
	//rightWiimote->disable();
	//rightWiimote->_selected = true;
	//rightWiimote->setHighlight(true);
	objects.push_back(rightWiimote);
	
	leftWiimote = new Object(4, 0.5, 0.5, 0.5, "MrWiimote.obj");
	leftWiimote->normalize();
	leftWiimote->setMatrix(ar_translationMatrix(-3, 2, -1.6)); // initial position
	leftWiimote->setHPR(135,0,0);
	//leftWiimote->disable();
	//leftWiimote->_selected = false;
	objects.push_back(leftWiimote);
}

// load a previously created file for editing
void ProjectManager::loadProject(string &filename)
{
	initialize(filename);
	//cout << "\n\n\n pname doc: " << doc.first_node()->name() << "\n\n\n" << flush;
	cout << "\n\n\n pname ctr: " << codeTree.first_node()->name() << "\n\n\n" << flush;
	//load into environment
	loadEnvironment(codeTree);
	cout << "finished load environment" << "\n" << flush;
}

/// <summary>
/// unzips a KIDE file to allow for edits to be made
/// </summary>
/// <param name="chosenFile">the filename to be unzipped</param>
/// <param name="workingPath">path to place the KIDE contents</param>
void ProjectManager::unzipKIDE(string &chosenFile, string &workingPath)
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
	string process = PATH+"Kosmos/lib/7zipCMD/7za.exe x -tzip \""+chosenFile+"\" -y -o\""+workingPath+"\" -mx=9";
	char* proc = strdup(process.c_str());
    CreateProcess(NULL, proc, NULL, NULL, false, 0, NULL, NULL, &startinfo, &procinfo); //this is the most important line in the program. it runs the program specified in the command-line argument (argv[1])
	WaitForSingleObject( procinfo.hProcess, INFINITE ); //wait until it's done
	CloseHandle(procinfo.hProcess); //and, clean up after Windows because I don't need the process handles it gives me
    CloseHandle(procinfo.hThread);
	free(proc);
}
enum DirectoryDeletion
 {
    CONTENTS = 0x1,
    DIRECTORY = 0x2,
    DIRECTORY_AND_CONTENTS = (0x1 | 0x2)
 };
 
///code taken from Jay Kramer as a solution to a problem posted by Charles on stackoverflow, along with enum above
///http://stackoverflow.com/questions/1468774/why-am-i-having-problems-recursively-deleting-directories
bool ProjectManager::deleteDirectory(std::string& directoryname, int flags)
{
	if(directoryname.at(directoryname.size()-1) !=  '\\') directoryname += '\\';

	if ((flags & CONTENTS) == CONTENTS)
	{
		WIN32_FIND_DATAA fdata;
		HANDLE dhandle;
		//BUG 1: Adding a extra \ to the directory name..
		directoryname += "*";
		dhandle = FindFirstFileA(directoryname.c_str(), &fdata);
		//BUG 2: Not checking for invalid file handle return from FindFirstFileA
		if( dhandle != INVALID_HANDLE_VALUE )
		{
			// Loop through all the files in the main directory and delete files & make a list of directories
			while(true)
			{
				if(FindNextFileA(dhandle, &fdata))
				{
					std::string     filename = fdata.cFileName;
					if(filename.compare("..") != 0)
					{
						//BUG 3: caused by BUG 1 - Removing too many characters from string.. removing 1 instead of 2
						std::string filelocation = directoryname.substr(0, directoryname.size()-1) + filename;

						// If we've encountered a directory then recall this function for that specific folder.

						//BUG 4: not really a bug, but spurious function call - we know its a directory from FindData already, use it.
						if( (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)  
						{
							DeleteFileA(filelocation.c_str());
						}
						else 
						{
							ProjectManager::deleteDirectory(filelocation, DIRECTORY_AND_CONTENTS);
						}
					}
				} 
				else if(GetLastError() == ERROR_NO_MORE_FILES)    
				{
					break;
				}
			}
			directoryname = directoryname.substr(0, directoryname.size()-2);
			//BUG 5: Not closing the FileFind with FindClose - OS keeps handles to directory open.  MAIN BUG
			FindClose( dhandle );
		}
	}
	if ((flags & DIRECTORY) == DIRECTORY)
	{
		HANDLE DirectoryHandle;
		DirectoryHandle = CreateFileA(directoryname.c_str(),
				FILE_LIST_DIRECTORY,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				NULL);
		
		//BUG 6: Not checking CreateFileA for invalid handle return.
		if( DirectoryHandle != INVALID_HANDLE_VALUE )
		{
			bool DeletionResult = (RemoveDirectoryA(directoryname.c_str()) != 0)?true:false;
			CloseHandle(DirectoryHandle);
			return DeletionResult;
		}
		else
		{
			return true;
		}
	}

	return true;
}

/// this function will create a new project at the specified location
void ProjectManager::createNewProject(string &projectFolder, string &templateLocation)
{
	//cp template
	//unzip template
	ProjectManager::deleteDirectory(projectFolder, CONTENTS);
	unzipKIDE(templateLocation, projectFolder);
	
	string projectFile = projectFolder + "\\" + projectName + ".kproj";
	string templateKproj = projectFolder + "\\newProjectTemplate.kproj";
	rename(templateKproj.c_str(),projectFile.c_str());
    
	loadProject(projectFile);
	//initialize
	//load into environment
}

void ProjectManager::generate()
{
	generateRecur(codeTree.first_node()->first_node()->first_node(), projectDir);
}

void ProjectManager::save()
{
	string projectFile = projectDir + "\\" + projectName + ".kproj";
	saveProject(projectFile);
}

/// this function saves a project to filepath from doc
void ProjectManager::saveProject(string &filepath)
{
	//no save as for now...later maybe, needs to cp all external files
	cout << "saving project file to: " << filepath << "\n";
	ofstream myfile;
	myfile.open(filepath.c_str());
	myfile << codeTree;
	myfile.close();
}

void ProjectManager::findTemplateCallback(vector<string> args)
{
	templateName = args[0];
	cout << "template: " << templateName << "\n" << flush;
	cout << "findingfile: " << virtualdirectory.findingFile << "\n" << flush;
	virtualdirectory.startBrowse("findprojdir", &ProjectManager::findProjectCallback,"Select project directory: ", SANDBOXPATH, true);
	cout << "findingfile: " << virtualdirectory.findingFile << "\n" << flush;
}

void ProjectManager::findProjectCallback(vector<string> args)
{
	projectDir = args[0];
	unsigned found = projectDir.find_last_of("/\\");
	string pname = projectDir.substr(found+1);
	setProjectName(pname);
	createNewProject(projectDir, templateName);
	
	/*
	vector<string> handy;
	handy.push_back("right");
	ExtendBlock::insertBlock(handy);
	*/
	
	/*vector<string> importy;
	importy.push_back("C:\\obj\\paperlantern.obj");
	importy.push_back("C:\\obj\\");
	Import::importCallback(importy);*/
	
	string projectFile = projectDir + "\\" + projectName + ".kproj";
	saveProject(projectFile);
	
	//cout << codeTree;
	
	cout << "generate recur" << "\n" << flush;
	generateRecur(codeTree.first_node()->first_node()->first_node(), projectDir); //output code, folder, etc
	cout << "finished generate recur" << "\n" << flush;
	
	/*
	vector<string> importy;
	importy.push_back("C:\\obj\\paperlantern.obj");
	importy.push_back("C:\\obj\\");
	Import::importCallback(importy);
	*/
	
	//string projectFile = projectDir + "\\" + projectName + ".kproj";
	//ProjectManager::saveProject(projectFile);
}

void ProjectManager::loadProjectCallback(vector<string> args)
{
	printf("attempting to load project...i hope\n");
	cout.flush();
	projectDir = args[0];
	printf("projectDir=%s\n",projectDir.c_str());
	cout.flush();
	unsigned found = projectDir.find_last_of("/\\");
	string filename = projectDir.substr(found+1);
	findAndReplace(projectDir, "\\\\"+filename, "");
	printf("projectDir=%s\n",projectDir.c_str());
	cout.flush();
	found = projectDir.find_last_of("/\\");
	string pname = projectDir.substr(found+1);
	//remove .kproj
	
	//findAndReplace(pname, ".kproj", "");
	printf("pname=%s\n",pname.c_str());
	cout.flush();
	setProjectName(pname);
	
	string projectFile = projectDir + "\\" + projectName + ".kproj";
	printf("loading %s\n",projectFile.c_str());
	cout.flush();
	loadProject(projectFile);
	printf("loaded %s\n",projectFile.c_str());
	cout.flush();
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
	
	
	//searchForParent(doc.first_node());
	//cout << doc;
	//print(cout, doc, 0);
	//removeNodesWithAttribute(doc.first_node(), "parent");
	//searchForParent(doc.first_node());
	//cout << doc;
	
}
*/
