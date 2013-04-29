Kosmos
======

**Important**: before running this project it should be correctly setup.
Any sound files should be copied to the rsc folder of Aszgard.  
The constants.h should be modified to reflect proper paths.  
On the same level as Kosmos there should be a 'sandbox' folder and any projects created will go in there, a new folder should be
created for each project before entering Kosmos. As an example, if Kosmos is at .../projects/Kosmos or .../projects/Prototype then
there should exist a folder .../projects/sandbox and .../projects/sandbox/myNewProject  
We would provide more information on this but it really depends on how your filesystem is set up. (McMahan: please don't count off if you have issues with this, none of the other groups have so many things dependant on paths :()
Before doing anything in the environment you should create or load a project. ie. do not import before creating a project.  
  
This project is compiled using 'make', you may need to perform a 'make clean' and/or delete the Kosmos/build/win32/.depend folder before making  
This project is run by 'Kosmos.exe'
Have fun!

An <a href="http://syzygy.isl.uiuc.edu/aszgard/index.html" title="Aszgard page">Aszgard</a> application.
The above link is the homepage for Aszgard which in theory will run this application, however we are using a modified version.

This project is an IDE for developing Aszgard applications. 

**Features**
--------------
- *Project*  
The tablet has a project menu, this is used to create, save, load and generate projects.  
Note: all templates end with '.kide' and all project files end in '.kproj'  
Creating a new project works by selecting a template, if you are unsure which to use, use blankProject.kide  
Saving a project saves the project file to the directory it was created or loaded from. It does NOT generate the code.  
Loading a project allows you to select a '.kproj' project file to load into Kosmos.  
Generate outputs the Syzygy code to the directory the project was created or loaded from.  

- *Selection*  
Use the '+' on the right wiimote to select objects, also see SQUAD below.
The left wiimote is a ray and selects the object it is pointed at.  
The right wiimote is a virtual hand and selects the object it is touching.  
Press the 'A' button on the right wiimote to move any object.  

- *Import*  
If a directory is selected the menu will navigate to that directory.  
If a file is selected it will be imported (if it is an OBJ).  

- *SQUAD*  
The modified SQUAD technique uses the left wiimote.  
Hold down the left '-' button to open the selection menu.  
Once the menu is open, the arrow keys on the left d-pad are used for selection (see above).  
The SQUAD works by filtering through objects in certain quadrants (areas, seperated by lines).  
ie. If the top quadrant contains more than one item, and you press the up arrow, then  
	the items in the top quadrant will redistribute themselves in all the quadrants.  
  
**Running notes**
--------------
*Note*: in order for the music to work the mp3 files need to be copied from the data/music folder into aszgard5/szg/rsc  
Development path = /c/aszgard5/szg/projects/Kosmos
