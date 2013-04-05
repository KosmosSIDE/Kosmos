/*
Summary: this function is used to import files into the environment
TODO: some functions may need to be moved from main to here for modularity
		copyTo function needs to be implemented

Author: Aaron Hardin

*/

#include "Import.h"

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

void Import::import(const string &filename, int &x, int &y, int &z, int &h, int &p, int &r, int &scale, const string &path)
{
	cout << "attempting to import..." << filename << " from: " << path << "\n" << flush;
	int length = filename.length();
	if (filename[length-1] == 'j' && filename[length-2] == 'b' && filename[length-3] == 'o')
	{
		Object* newObj = (path != "")?new Object(2,scale,scale,scale,filename,path):new Object(2,scale,scale,scale,filename);
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
	//copy (obj, mtl) to data dir
	string frompath = args[0];
	unsigned found = args[0].find_last_of("/\\");
	string filename = args[0].substr(found+1);
	string topath = projectDir + "\\data\\obj\\"+filename;
	cout << "copying file from: " << frompath << " to: " << topath << "\n\n" << flush;
	CopyFile(frompath.c_str(),topath.c_str(), true);
	
	findAndReplace(frompath, ".obj",".mtl");
	findAndReplace(topath, ".obj",".mtl");
	cout << "copying file from: " << frompath << " to: " << topath << "\n\n" << flush;
	CopyFile(frompath.c_str(),topath.c_str(), true);
	//add to xml
	// append to data/obj
	/*
				<file>
					<name>cello.obj</name>
					<type>external</type>
					<locationType>filesystem</locationType>
				</file>
	*/
	
	// append to main.start callback
	/*
							<code parent="cello">
	theCello.setMatrix(ar_translationMatrix(0, 4, -8));
	objects.push_back(&amp;theCello);
							</code>
	*/
	
	// append to main.global vars
	/*
						<code parent="cello">
Object theCello(3, 0.5, 0.5, 0.5, &quot;cello.obj&quot;);
						</code>
	*/
	
	std::string src2 = "<object><type>OBJ</type><name>"+filename.substr(0,filename.size()-4)+"</name><resourceName>"+filename+"</resourceName>"
		"<x>0</x><y>4</y><z>-4</z><heading>0</heading><pitch>0</pitch><roll>0</roll></object>";
	rapidxml::xml_document<> xmlseg;
	std::vector<char> x(src2.begin(), src2.end());
	x.push_back( 0 ); // make it zero-terminated as per RapidXml's docs

	xmlseg.parse<0>( &x[0] );

	rapidxml::xml_node<>* a = xmlseg.first_node(); /* Node to append */

	rapidxml::xml_node<> *node = codeTree.clone_node( a );
//codeTree.first_node("project")->first_node("profile")->append_node( node ); /* Appending node a to the tree in src */
	cout << "save proj" << "\n" << flush;
	string projectFile = projectDir + "\\" + projectName + ".kproj";
//	ProjectManager::saveProject(projectFile);
	cout << "finished save" << "\n" << flush;
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

