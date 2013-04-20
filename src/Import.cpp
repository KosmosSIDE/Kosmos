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
    for (int pos=0; (pos=source.find(find, pos))!=string::npos; pos+=rLen)
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
Object* Import::import(const string &filename, float x, float y, float z, float h, float p, float r, float scale, const string &path, int type)
{
	int length = filename.length();
	if (filename[length-1] == 'j' && filename[length-2] == 'b' && filename[length-3] == 'o')
	{
		Object* newObj = (path != "")?new Object(type,scale,scale,scale,filename,path):new Object(2,scale,scale,scale,filename);
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
	if(args.size() > 1)
	{
		import(args[0],args[1]);
	}
	else
	{
		import(args[0]);
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
	
	cout << "replacing filenames\n" << flush;
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
	ostringstream lines;

	lines << "<code parent=\"" << filename.substr(0,filename.size()-4) << "\">" << endl;
	lines << "the" << filename.substr(0,filename.size()-4) << ".setMatrix(ar_translationMatrix(0, 4, -8));" << endl;
	lines << "objects.push_back(&amp;the" << filename.substr(0,filename.size()-4) << ");" << endl;
	lines << "</code>" << endl;
	std::string mainStartObjectStr = lines.str();
	std::vector<char> mainStartObjectVec(mainStartObjectStr.begin(), mainStartObjectStr.end());
	mainStartObjectVec.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
	rapidxml::xml_document<> mainStartObjectDoc;
	mainStartObjectDoc.parse<0>( &mainStartObjectVec[0] );
	rapidxml::xml_node<>* mainStartObjectNode = mainStartObjectDoc.first_node();
	rapidxml::xml_node<> *mainStartObjectAppendNode = codeTree.clone_node( mainStartObjectNode );
	rapidxml::xml_node<> *firstCodeBlock = codeTree.first_node("project")->first_node("directory")->first_node("directory")->next_sibling()->next_sibling()->first_node("file")->next_sibling()->next_sibling()->first_node("codeblocks")->first_node("codeblock");
	firstCodeBlock->next_sibling()->next_sibling()->first_node("functioncode")->prepend_node( mainStartObjectAppendNode );
	
	cout << "added obj to main.start...\n" << flush;
	// prepend to main.start callback
	/*
							<code parent="cello">
	theCello.setMatrix(ar_translationMatrix(0, 4, -8));
	objects.push_back(&amp;theCello);
							</code>
	*/
	
	std::string mainGlobalObjectStr = "<code parent=\""+filename.substr(0,filename.size()-4)+"\">Object the"+filename.substr(0,filename.size()-4)+"(2, 0.5, 0.5, 0.5, &quot;"+filename+"&quot;);</code>";
	std::vector<char> mainGlobalObjectVec(mainGlobalObjectStr.begin(), mainGlobalObjectStr.end());
	mainGlobalObjectVec.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
	rapidxml::xml_document<> mainGlobalObjectDoc;
	mainGlobalObjectDoc.parse<0>( &mainGlobalObjectVec[0] );
	rapidxml::xml_node<>* mainGlobalObjectNode = mainGlobalObjectDoc.first_node();
	rapidxml::xml_node<> *mainGlobalObjectAppendNode = codeTree.clone_node( mainGlobalObjectNode );
	firstCodeBlock->next_sibling()->append_node( mainGlobalObjectAppendNode );
	
	cout << "added obj to main.global vars...\n" << flush;
	// append to main.global vars
	/*
						<code parent="cello">
Object theCello(3, 0.5, 0.5, 0.5, &quot;cello.obj&quot;);
						</code>
	*/
	
	std::string src2 = "<object><type>OBJ</type><name>"+filename.substr(0,filename.size()-4)+"</name><resourceName>"+filename+"</resourceName>"
		"<x>0</x><y>4</y><z>-4</z><heading>0</heading><pitch>0</pitch><roll>0</roll></object>";
	std::vector<char> data(src2.begin(), src2.end());
	data.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
	importstr = data;
	importdoc.parse<0>( &importstr[0] );
	rapidxml::xml_node<>* a = importdoc.first_node();
	rapidxml::xml_node<> *node = codeTree.clone_node( a );
	codeTree.first_node("project")->first_node("profile")->append_node( node ); /* Appending node a to the tree in src */
	// append to profile
	/*
		<object>
			<type>OBJ</type>
			<name>cello</name>
			<resourceName>cello.obj</resourceName>
			<x>0</x>
			<y>4</y>
			<z>-4</z>
			<heading>0</heading>
			<pitch>0</pitch>
			<roll>0</roll>
		</object>
	*/
}

