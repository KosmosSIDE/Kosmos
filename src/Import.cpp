/*
Summary: this function is used to import files into the environment
TODO: some functions may need to be moved from main to here for modularity

Author: Aaron Hardin

*/

#include "Import.h"

rapidxml::xml_document<> Import::importdoc;
vector<char> Import::importstr;
rapidxml::xml_document<> Import::profileimportdoc;
vector<char> Import::profileimportstr;

//find a string in a string and replace with a string
int inline Import::findAndReplace(string& source, const string& find, const string& replace)
{
    int num=0;
    int fLen = find.size();
    int rLen = replace.size();
    for (unsigned int pos=0; (pos=source.find(find, pos))!=string::npos; pos+=rLen)
    {
        num++;
        source.replace(pos, fLen, replace);
    }
    return num;
}

///import obj from filename and given path...the filename is the full path but
/// we need path for the mtl, i guess we could parse from filename but this is
/// how i chose to do it
Object* Import::import(const string &filename,const string &path = "data/obj")
{
	int length = filename.length();
	if (filename[length-1] == 'j' && filename[length-2] == 'b' && filename[length-3] == 'o')
	{
		Object* newObj = new Object(2,1,1,1,filename,path);
		newObj->normalize();
		newObj->setMatrix(ar_translationMatrix(0, 4, -8)); // initial position
		objects.push_back(newObj);
		
		return newObj;
	}
	return NULL;
}

///import with xyz position and hpr rotation and scale, see above import for more information
Object* Import::import(const string &filename, float x, float y, float z, float h, float p, float r, float scale, const string &path, int type, const char* name)
{
	int length = filename.length();
	if (filename[length-1] == 'j' && filename[length-2] == 'b' && filename[length-3] == 'o')
	{
		Object* newObj = (path != "")?new Object(type,scale,scale,scale,filename,path,name):new Object(2,scale,scale,scale,filename,"data/obj",name);
		newObj->normalize();
		newObj->setMatrix(ar_translationMatrix(x, y, z)); // initial position
		newObj->setHPR(h,p,r);
		
		objects.push_back(newObj);
		
		return newObj;
	}
	return NULL;
}

///callback for use via the callback class
void Import::importCallback(vector<string> args)
{
	int length = args[0].length();
	if (args[0][length-1] == 'j' && args[0][length-2] == 'b' && args[0][length-3] == 'o')
	{
		cout << "importing obj...\n" << flush;
	}
	else
	{
		cout << "not a valid obj\n" << flush;
		return;
	}
	
	if(args.size() > 1)
	{
		Object* oby = import(args[0],args[1]);
		oby->insertObject();
	}
	else
	{
		Object* oby = import(args[0]);
		oby->insertObject();
	}
	
	
	//copy (obj, mtl) to data dir
	string frompath = args[0];
	unsigned found = args[0].find_last_of("/\\");
	string filename = args[0].substr(found+1);
	string topath = projectDir + "\\data\\obj\\"+filename;
	CopyFile(frompath.c_str(),topath.c_str(), true);
	
	findAndReplace(frompath, ".obj",".mtl");
	findAndReplace(topath, ".obj",".mtl");
	//TODO parse mtl look for and add jpg ppm
	CopyFile(frompath.c_str(),topath.c_str(), true);
	
	std::string srcdataobj = "<file><name>"+filename+"</name><type>external</type><locationType>filesystem</locationType></file><file><name>"+filename.substr(0,filename.size()-4)+".mtl</name><type>external</type><locationType>filesystem</locationType></file>";
	
	cout << "fetching dependencies\n" << flush;
	vector<string> dependencies = TreeMenu::getDependency(topath);
	
	findAndReplace(frompath, filename.substr(0,filename.size()-4)+".mtl", "");
	findAndReplace(topath, filename.substr(0,filename.size()-4)+".mtl", "");
	cout << "iterating through\n" << flush;
	vector<string>::iterator i;
	for(i=dependencies.begin(); i != dependencies.end(); ++i) 
	{
		string str = *i;
		cout << "dependecy: " << str << "\n" << flush;
		
		string fromy = frompath+str;
		string tooy = topath+str;
		CopyFile(fromy.c_str(),tooy.c_str(), true);
		
		srcdataobj += "<file><name>"+str+"</name><type>external</type><locationType>filesystem</locationType></file>";
	}
	
	cout << "adding obj to xml file...\n" << flush;
	//add to xml
	
	std::vector<char> dataobj(srcdataobj.begin(), srcdataobj.end());
	dataobj.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
	profileimportstr = dataobj;
	profileimportdoc.parse<0>( &profileimportstr[0] );
	rapidxml::xml_node<>* dataobjnode = profileimportdoc.first_node();
	while (dataobjnode != 0)
	{
		rapidxml::xml_node<> *appendnode = codeTree.clone_node( dataobjnode );
		codeTree.first_node("project")->first_node("directory")->first_node("directory")->next_sibling()->first_node("directory")->append_node( appendnode ); //Appending node a to the tree in src
		dataobjnode = dataobjnode->next_sibling();
	}
	cout << "added obj to profile...\n" << flush;
	// append to data/obj
	/*
				<file>
					<name>cello.obj</name>
					<type>external</type>
					<locationType>filesystem</locationType>
				</file>
	*/
}

