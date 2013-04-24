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
Press the 'A' button on either wiimote to move any object.  

- *Import*  
If a directory is selected the menu will navigate to that directory.  
If a file is selected it will be imported (if it is an OBJ).  

- *SQUAD*  
The modified SQUAD technique uses the left wiimote.  
Hold down the left '1' button to open the selection menu.  
Once the menu is open, the arrow keys on the left d-pad are used for selection (see above).  
The SQUAD works by filtering through objects in certain quadrants (areas, seperated by lines).  
ie. If the top quadrant contains more than one item, and you press the up arrow, then  
	the items in the top quadrant will redistribute themselves in all the quadrants.  
  
**Running notes**
--------------
*Note*: in order for the music to work the mp3 files need to be copied from the data/music folder into aszgard5/szg/rsc  
Development path = /c/aszgard5/szg/projects/Kosmos
