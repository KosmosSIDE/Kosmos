Kosmos
======

the kosmos project!

An <a href="http://syzygy.isl.uiuc.edu/aszgard/index.html" title="Aszgard page">Aszgard</a> application.
The above link is the homepage for Aszgard which in theory will run this application, however we are using a modified version.

This project is an IDE for developing Aszgard applications. 

**Features**
--------------
- *Selection*  
The '1' button on either wiimote selects objects.  
The left wiimote is a ray and selects the object it is pointed at.  
The right wiimote is a virtual hand and selects the object it is touching.  
Select the original (see import below) musical objects to make them play music.   
Press the 'A' button on either wiimote to move any object.  

- *Import*
The '-' button on the right hand opens the directory menu.  
The left wiimote up and down arrows are used to navigate the directory menu.  
The right '-' button selects the highlighted file or directory (folder).  
If a directory is selected the menu will navigate to that directory.  
If a file is selected it will be imported (if it is an OBJ).  
The Home button on the right wiimote exits selection (without importing a file).  

- *SQUAD*
The modified SQUAD technique uses the left wiimote.  
Hold down the left '1' button to open the selection menu.  
Once the menu is open, the arrow keys on the left d-pad are used for selection (see above).  
The SQUAD works by filtering through objects in certain quadrants (areas, seperated by lines).  
ie. If the top quadrant contains more than one item, and you press the up arrow, then  
	the items in the top quadrant will redistribute themselves in all the quadrants.  

<b>Running notes</b>
<br />
<i>Note</i>: in order for the music to work the mp3 files need to be copied from the 
data/music folder into aszgard5/szg/rsc
<br />
Development path = /c/aszgard5/szg/projects/musicMaker
