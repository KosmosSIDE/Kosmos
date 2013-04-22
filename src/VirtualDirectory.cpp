/*
Summary: this function is used to browse directories and implements a callback to handle the file result
TODO: some functions need to be moved from main to help modularity

Author: Aaron Hardin

*/

#include "VirtualDirectory.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

///starts the browsing, done asynchronously! uses callback class to do so
///cb is the name of function, funPtr is the function, titley is a title to display (optional), 
/// startDir is the directory to start in (optional)
void VirtualDirectory::startBrowse(const string& cb,void(*funPtr)(vector<string>), const string &titley, const string &startDir, bool sandy)
{
tabletOn = false;
	sandboxed = sandy;
	currentIndex = 0;
	currentDir.clear();
	findingFile = true;
	foundFile = false;
	filename = "";
	callback = cb;
	cf.add(cb, funPtr);
	dirName = ROOTPATH;
	if(startDir != "")
	{
		dirName = startDir;
		string opendirectory = "";
		opendirectory += startDir.c_str();
		openDir(opendirectory);
	}
	else
	{
		openDir(ROOTPATH);
	}
	
	if (titley != "")
	{
		title = titley;
	}
	else
	{
		title = "";
	}
}

///the up button has been pressed, move the selected index up one
void VirtualDirectory::upPressed()
{
	currentIndex = (currentIndex-1+currentDir.size()) % currentDir.size();
}

///the down button has been pressed, move the selected index down one
void VirtualDirectory::downPressed()
{
	printf("in down\n");
	cout.flush();
	printf("down pressed, currentIndex=%d, currentDir.size=%d\n",currentIndex,currentDir.size());
	cout.flush();
	currentIndex = (currentIndex+1) % currentDir.size();
	printf("end down\n");
	cout.flush();
}

///choose a file or directory, if a directory is chosen then move into it
void VirtualDirectory::selectFile()
{
	if (sandboxed && (currentIndex == 0 || currentIndex == 1))
	{
		return;
	}
	
	string chosenFile = currentDir[currentIndex];
	
	chosenFile = dirName + "\\" + chosenFile;
	
	struct stat st;
	if(stat(chosenFile.c_str(), &st) == -1)
	{
		perror("");
	}
	else
	{
		if(S_ISDIR(st.st_mode))
		{
			//printf("\t DIRECTORY\n");
			dirName = chosenFile +"\\";
			printf("%s\n%s\n",chosenFile.c_str(),dirName.c_str());
			cout.flush();
			currentIndex = 0;
			openDir(chosenFile);
		}
		else
		{
			//printf("\t FILE\n");
			currentIndex = 0;
			vector<string> filenamev;
			filenamev.push_back(chosenFile);
			filenamev.push_back(dirName);
			printf("%s\n%s\n",chosenFile.c_str(),dirName.c_str());
			cout.flush();
			findingFile = false;
			sandboxed = false;
			tabletOn = true;
			cf.call(callback,filenamev);
		}
	}
}

///select a directory, if its a file choose it, if its a directory choose it
/// really should be combined with selectFile in some way...oh well
void VirtualDirectory::selectDirectory()
{
	if (sandboxed && (currentIndex == 0 || currentIndex == 1))
	{
		return;
	}
	
	string chosenFile = currentDir[currentIndex];
	
	chosenFile = dirName + "\\" + chosenFile;
	
	struct stat st;
	if(stat(chosenFile.c_str(), &st) == -1)
	{
		perror("");
	}
	else
	{
		if(S_ISDIR(st.st_mode))
		{
			//printf("\t DIRECTORY\n");
			currentIndex = 0;
			vector<string> filenamev;
			filenamev.push_back(chosenFile);
			filenamev.push_back(dirName);
			findingFile = false;
			sandboxed = false;
			tabletOn = true;
			cf.call(callback,filenamev);
		}
		else
		{
			//printf("\t FILE\n");
			currentIndex = 0;
			vector<string> filenamev;
			filenamev.push_back(chosenFile);
			filenamev.push_back(dirName);
			findingFile = false;
			sandboxed = false;
			tabletOn = true;
			cf.call(callback,filenamev);
		}
	}
}

///opens the directory and adds the contents to currentDir
void VirtualDirectory::openDir(string directory)
{
	currentDir.clear();
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (directory.c_str())) != NULL) 
	{
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) 
		{
			currentDir.push_back(ent->d_name);
		}
		closedir (dir);
	} 
	else 
	{
		/* could not open directory */
		perror ("");
		//return EXIT_FAILURE;
	}
}

///the draw function, draws what is in currentDir
void VirtualDirectory::draw() const 
{

	// Always start with glPushMatrix to avoid matrix multiplications done here from
	// affecting other portions of the scene.
	glPushMatrix();
		int upperbound = currentIndex+15;
		renderPrimitive(-2.5f, false);
		
		int yshift = 0;
		if (title != "")
		{
			yshift -= 120;
			drawText(-2.4f, 800, title,false);
			--upperbound;
		}
		
		for (int i=currentIndex;i<upperbound;++i)
		{
			if(i > (int)currentDir.size())
			{
				break;
			}
		
			//display filename
			if (sandboxed && (i == 0 || i == 1))
			{
				drawText(-2.4f, 800-(i-currentIndex)*120+yshift, "------------------------------", false);
			}
			else if(i==currentIndex)
			{
				drawText(-2.4f, 800-(i-currentIndex)*120+yshift, currentDir[i%(currentDir.size()+1)],true);
			}
			else if (i == (int)currentDir.size()) //leave a blank
			{
				drawText(-2.4f, 800-(i-currentIndex)*120+yshift, "------------------------------", false);
			}
			else
			{
				drawText(-2.4f, 800-(i-currentIndex)*120+yshift, currentDir[i%(currentDir.size()+1)], false);
			}
		}
	// Always finish with glPopMatrix to match glPushMatrix above.
	glPopMatrix();
}

///draw text on the primitive (in front of)
void VirtualDirectory::drawText(float distance, float ypos, string text, bool selected = false) const
{
	glPushMatrix();
		glPushAttrib(GL_COLOR_BUFFER_BIT);
		glColor3f(1, 1, 1);
		glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations  
		glTranslatef(0.0f, 0.0, distance); // Push eveything 5 units back into the scene, otherwise we won't see the primitive  
		glScalef(0.001, 0.001, 0.001);
		glTranslatef(0.0, ypos, 0.0);
			int lengthOfText = min((int)text.length(),300);
			glPushMatrix();
			glTranslatef(-770, 0.0, 0.0);
			if(selected)
			{
				glPushAttrib(GL_COLOR_BUFFER_BIT);
				glColor3f(0.9, 0, 0.0);
			}
			if(lengthOfText < 300)
			{
				for (int i = 0; i < lengthOfText; i++)
				{
					glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
				}
			}
			if(selected)
			{
				glPopAttrib();
			}
			glPopMatrix();
		glPopAttrib();
	glPopMatrix();
}
