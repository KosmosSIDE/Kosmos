//************************************************************************************************
// Name:		Kosmos
// Version:		0.1.2
// Description:	This arMasterSlaveFramework application creates instrument objects which can be
//				turned on and off to make music. An augmentation of the SQUAD selection technique
//				is implemented in which the user can hold down a button to bring up a non-invasive
//				SQUAD menu to select an instrument. Features import functionality for OBJ files
// Author: Aaron Hardin
//************************************************************************************************

// The precompiled header include MUST appear as the first non-commented line. See 
// arCallingConventions.h for details.
#include "arPrecompiled.h"

// Includes for a MasterSlave framework application with interactable things.
#define SZG_DO_NOT_EXPORT
#include <stdio.h>
#include <stdlib.h>
#include "arMasterSlaveFramework.h"
#include "arInteractableThing.h"
#include "arInteractionUtilities.h"
#include "arNavigationUtilities.h"
#include "arGlut.h"
#include "arOBJ.h"
#include "main.h"
#include "Object.h"
#include "LeftVirtualHand.h"
#include "RightVirtualHand.h"
#include "selectedObjects.h"
#include "CallFunction.h"
#include "Import.h"
#include "VirtualDirectory.h"
#include "ProjectManager.h"
#include "TreeMenu.h"
#include "ExtendBlock.h"
#include "VirtualHandBlock.h"

using namespace rapidxml;

string projectName = "kosmosGenerated";
xml_document<> codeTree;
vector<char> document;
string templateName = "";
string projectDir = "";

int numObjects = 0;

bool sandboxed = false;
bool rightSelected = false;
bool showFloor = true;
bool scalingActive = false;
float distBetweenHands = 0.0;

bool tabletOn = true;

TreeMenu *wiiNodeMenu = NULL;
TreeMenu *userMenu = NULL;
TreeMenu *objectMenu = NULL;
/*Changes by Harish Babu Arunachalam*/
TreeMenu *nodeMenu = NULL;
TreeMenu *parentMenu = NULL;
TreeMenu *currentPtr = NULL;
int curTreeLevel = 0;
int treeIndex = 0;

void goForward();
void goBack();
void goUp();
void goDown();
/*end of Changes by Harish Babu Arunachalam*/
// Feet to local units conversion.
// For example, if you use centimeters to create your models, then set this to 12*2.54 or 30.48 to
// account for 12 inches per foot and 2.54 centimeters per inch.
// By default, Syzygy uses feet so this is set to 1.0.
const float FEET_TO_LOCAL_UNITS = 1.0;

// Near and far clipping plane distances
const float nearClipDistance = 0.1 * FEET_TO_LOCAL_UNITS;
const float farClipDistance = 100.0 * FEET_TO_LOCAL_UNITS;

// decides whether the 'flashlight' is currently on
bool coneselection = false;

//used to animate music note
double currentTimeGlobal; //in millis
arOBJRenderer musicNotey;
arOBJRenderer mrTablet;
arOBJRenderer mrFloor;

int selectionMode = 0; //0 means null, 1 means just entered, 2 means in selection, 3 means just point and click

// has it been pressed
static int button1 = 0;
// how long has it been pressed
static double pressed1 = 0.0;
// how long do we need to hold it
const double threshold = 1000.0;    // in millis
static double pressedImport = 0.0;
static double pressedMenu = 0.0;
const double thresholdMenu = 0.0;//250.0;
static double dirButtonPress = 0.0;
static double handButtonPress = 0.0;


list<Object*> leftSelectedObjects;
list<Object*> upSelectedObjects;
list<Object*> rightSelectedObjects;
list<Object*> downSelectedObjects;

Object* _leftSelectedObject;
Object* _rightSelectedObject;
Object* rightWiimote;
Object* leftWiimote;
Object* userObject;
bool _rightMoving;
bool _leftMovering;


VirtualDirectory virtualdirectory;



// Global objects
//Object theUser(2, 0.5, 0.5, 0.5, "user.obj"); //set the user scale to 0.5,0.5,0.5 and the type to 2

// List of objects.
vector<arInteractable*> objects;

// Global effectors.
RightVirtualHand rightHand(false, "handy.obj"); // loads obj for hand
LeftVirtualHand leftHand("staff.obj");

// Global sound variables.
int soundTransformID;
int clickSound;

// MasterSlave transfer variables for shared memory. Only used in cluster-based systems. Hence,
// not needed in our system, but here's some examples.
// For this program, we just need to transfer each object's placement matrix and whether it is
// highlighted or not.
//arMatrix4 celloMatrix;
//arMatrix4 violinMatrix;
//arMatrix4 pianoMatrix;

void setProjectName (string &pname) { projectName = pname; }



// start callback
// Purposes:
// 		- Register shared memory by adding transfer fields. This is to syncronize cluster-based 
//		  systems, hence we don't need to do this. But there's a few examples anyway.
// 		- Set up navigation through the framework by specifying translation conditions, rotation
//		  conditions, translation speed, and rotation speed. We provide a couple examples here.
//		- Initialize any global variables specific to your application.
// Notes: 
//		DO NOT initialize OpenGL here. The start callback is called before window creation. 
//		Instead initialize OpenGL in the windowStartGL callback.
bool start(arMasterSlaveFramework& framework, arSZGClient& client ) 
{

	CallFunction cf;
	
	_rightMoving = false;
	_leftMovering = false;
	
	// Register shared memory. Not needed for non-cluster-based systems.
	// framework.addTransferField(char* name, void* address, arDataType type, int numElements);
	//list<arMatrix4*> objMatrices;
	
	vector<arInteractable*>::iterator i;
	for(i=objects.begin(); i != objects.end(); ++i) 
	{
		Object* obj = ((Object*)(*i));
		ostringstream ostr;
		ostr << "objMatrix" << &i;
		arMatrix4 obM = obj->matrix;
		framework.addTransferField(ostr.str(), &obM, AR_FLOAT, 16);
	}

	//framework.addTransferField("celloMatrix", &celloMatrix, AR_FLOAT, 16);
	//framework.addTransferField("violinMatrix", &violinMatrix, AR_FLOAT, 16);
	//framework.addTransferField("pianoMatrix", &pianoMatrix, AR_FLOAT, 16);
	
	
	// Set up navigation. 
	
	// A traditional pointing technique using the joystick with left and right rotating the scene.
	// Translate along Z-axis (forwards/backwards) if joystick is pressed more than 20% along axis 1.
	framework.setNavTransCondition('z', AR_EVENT_AXIS, 1, 0.2);
	// Rotate around Y-axis (vertical) if joystick is pressed more than 20% along axis 0.
	framework.setNavRotCondition('y', AR_EVENT_AXIS, 0, 0.2);  //FOR ROTATE
	//framework.setNavTransCondition('x', AR_EVENT_AXIS, 0, 0.2);  //FOR STRAFING
	// Set translation speed to 5 feet/second.
	framework.setNavTransSpeed(5.0);
	// Set rotation speed to 30 degrees/second.
	framework.setNavRotSpeed(30.0);
	
	
	// Initialize application variables here.
	
	// Move object's to initial positions.
	//theUser.setMatrix(ar_translationMatrix(0, 0, 0)); //set user to 0,0,0
	// Keep list of objects to interact with.
	//objects.push_back(&theUser);
	
	// Create sound transform.
	soundTransformID = dsTransform("world", framework.getNavNodeName(), ar_scaleMatrix(1.0));
	// Parameters are:
	//		name - string name for sound
	//		transformName - string name for dsTransform
	//		loopType - 1 for continuous, -1 for one-time, 0 to stop
	//		loudness - float from 0.0 (quiet) to 1.0 (max)
	//		positionVector - vector position of sound origin
	// Create loop for click sound.
	clickSound = dsLoop("click", "world", "click.mp3", 0, 1.0, arVector3(0, 0, 0));
	
	musicNotey.readOBJ("MusicNote.obj","data/obj");
	mrTablet.readOBJ("MrTablet.obj","data/obj");
	mrFloor.readOBJ("MrHDeckFloor.obj","data/obj");
	
	ar_navRotate( arVector3(0,1,0), 180 );
	ar_navTranslate( arVector3(0,0,10) );
	
	
	
		templateName = PATH+"Kosmos\\templates\\newproject.kide";
		vector<string> projman;
		projman.push_back(SANDBOXPATH+"newproj");
		ProjectManager::findProjectCallback(projman);
		
		
		
	
	wiiNodeMenu = new TreeMenu();
	wiiNodeMenu = wiiNodeMenu->makeWiiMenu(wiiNodeMenu);
	
	userMenu = new TreeMenu();
	userMenu = userMenu->makeUserMenu(userMenu);
	
	objectMenu = new TreeMenu();
	TreeMenu::makeMenu(objectMenu, PATH+"Kosmos\\conf\\menuFileObject.xml");
	
	/*Changes by Harish Babu Arunachalam*/
	nodeMenu =  new TreeMenu();
	currentPtr = new TreeMenu();
	parentMenu = new TreeMenu();
	//
	nodeMenu = nodeMenu->makeMenu(nodeMenu);
	nodeMenu->name = "menus";
	nodeMenu->level = -1;
	
	nodeMenu = nodeMenu->forwardPtrs[0];
	parentMenu = nodeMenu;
	currentPtr = parentMenu;
	curTreeLevel = currentPtr->level;
	currentPtr = nodeMenu;//->makeMenu(nodeMenu);
	/*End of changes by Harish Babu Arunachalam*/
	
	// Return true if everything is initialized correctly.
	return true;
}


// windowStartGL callback
// Purposes:
// 		- Initialize each window (a Syzygy application can have more than one window).
//		- Initialize OpenGL.
void windowStartGL(arMasterSlaveFramework& framework, arGUIWindowInfo* windowInfo) {
	
	// Initialize OpenGL. Set clear color to black.
	glClearColor(1, 1, 1, 0);
}


// windowEvent callback
// Purposes:
//		- Process window events, such as resizing
// Notes:
//		The values are defined in src/graphics/arGUIDefines.h.
//		arGUIWindowInfo is in arGUIInfo.h.
//		The window manager is in arGUIWindowManager.h.
void windowEvent(arMasterSlaveFramework& framework, arGUIWindowInfo* windowInfo) {

	// Process window events. Here, we handle window resizing.
	if(windowInfo->getState() == AR_WINDOW_RESIZE) {
		const int windowID = windowInfo->getWindowID();
#ifdef UNUSED
		const int x = windowInfo->getPosX();
		const int y = windowInfo->getPosY();
#endif
		const int width = windowInfo->getSizeX();
		const int height = windowInfo->getSizeY();
		framework.getWindowManager()->setWindowViewport(windowID, 0, 0, width, height);
	}
}


// preExchange callback
// Purposes:
//		- Handle navigation updates.
//		- Process user input.
//		- Set random variables.
//		- Update shared memory.
// Notes:
//		This is only called on the master node of the cluster and before shared memory is 
//		transferred to the slave nodes.
void preExchange(arMasterSlaveFramework& framework) {

	// Handle navigation update. The resulting navigation matrix is automatically transferred
	// to the slave nodes.
	framework.navUpdate();
	
	currentTimeGlobal = framework.getTime();
	
	// Process user input.
	bool anyButton = false;
	for (int i=0;i<=10;++i)
	{
		if(leftHand.getOnButton(i) || rightHand.getOnButton(i))
		{
			anyButton = true;
			break;
		}
	}
	
	if( scalingActive && anyButton)
	{
		scalingActive = false;
	}
	else if(scalingActive)
	{
		float xdist = rightHand.getX() - leftHand.getX();
		float ydist = rightHand.getY() - leftHand.getY();
		float zdist = rightHand.getZ() - leftHand.getZ();
		float currentDist = sqrt(xdist*xdist+ydist*ydist+zdist*zdist);
		float scaleBy = currentDist / distBetweenHands;
		distBetweenHands = currentDist;
		vector<arInteractable*>::iterator i;
		for(i=objects.begin(); i != objects.end(); ++i) 
		{
			Object* obPointer = ((Object*)(*i));
			if(obPointer->_selected && (obPointer != rightWiimote && obPointer != leftWiimote && obPointer != userObject))
			{
				obPointer->setScale(scaleBy);
			}
		}
	}
	
	
	if( _leftMovering == true && leftHand.getButton(2) == 0)
	{
		//_leftSelectedObject->snapMatrix();
		_leftMovering = false;
		_leftSelectedObject = NULL;
	}

	//const unsigned int numButtons = framework.getNumberButtons();

	// in milliseconds
	double currentTime = framework.getTime();

	/*if((!virtualdirectory.findingFile) && rightHand.getOnButton(0) && (currentTime-pressedImport)>1000)
	{
		pressedImport = currentTime;
	//Harish Babu Arunachalam
	//call back navigation function for TreeMenu
		goBack();
	//	virtualdirectory.startBrowse("import", &Import::importCallback, "Select obj to import: ");
	}
	else if((!virtualdirectory.findingFile) && rightHand.getOnButton(1) && (currentTime-pressedImport)>1000)
	{
		pressedImport = currentTime;
	//Harish Babu Arunachalam
	//call forward navigation function for TreeMenu
		goForward();
		//virtualdirectory.startBrowse("template", &ProjectManager::findTemplateCallback,"Select template file: ", TEMPLATEPATH);
	}
	else if( rightHand.getOnButton(2) && (currentTime-pressedImport)>1000)
	{
	//Harish Babu Arunachalam
	//call upward navigation function for TreeMenu
		pressedImport = currentTime;
		//currentPtr->printValues(currentPtr);
		goUp();
				
	}
	else if(rightHand.getOnButton(3)&&(currentTime-pressedImport)>1000)
	{
	//Harish Babu Arunachalam
	//call downward navigation function for TreeMenu
		pressedImport = currentTime;
		goDown();
	}*/
	if(leftHand.getOnButton(5))
	{
		tabletOn = !tabletOn;
	}
	
	if(leftHand.getOnButton(10))
	{
		currentPtr = nodeMenu;
		leftWiimote->_selected = false;
		rightWiimote->_selected = false;
	}
	
	if(rightHand.getOnButton(2))
	{
		vector<arInteractable*>::iterator i;
		for(i=objects.begin(); i != objects.end(); ++i) 
		{
			Object* oby = ((Object*)(*i));
			oby->_selected = false;
		}
	}
	
	
	if((!virtualdirectory.findingFile) && tabletOn && leftHand.getOnButton(8) && (currentTime-pressedMenu)>thresholdMenu)
	{
		pressedMenu = currentTime;
	//Harish Babu Arunachalam
	//call back navigation function for TreeMenu
		goBack();
	//	virtualdirectory.startBrowse("import", &Import::importCallback, "Select obj to import: ");
	}
	else if((!virtualdirectory.findingFile) && tabletOn && (leftHand.getOnButton(9) || leftHand.getOnButton(3)) && (currentTime-pressedMenu)>thresholdMenu)
	{
		pressedMenu = currentTime;
	//Harish Babu Arunachalam
	//call forward navigation function for TreeMenu
		goForward();
		//virtualdirectory.startBrowse("template", &ProjectManager::findTemplateCallback,"Select template file: ", TEMPLATEPATH);
	}
	else if((!virtualdirectory.findingFile) && tabletOn && leftHand.getOnButton(6) && (currentTime-pressedMenu)>thresholdMenu)
	{
	//Harish Babu Arunachalam
	//call upward navigation function for TreeMenu
		pressedMenu = currentTime;
		//currentPtr->printValues(currentPtr);
		goUp();
				
	}
	else if((!virtualdirectory.findingFile) && tabletOn && leftHand.getOnButton(7)&&(currentTime-pressedMenu)>thresholdMenu)
	{
	//Harish Babu Arunachalam
	//call downward navigation function for TreeMenu
		pressedMenu = currentTime;
		goDown();
	}
	else if(virtualdirectory.findingFile)
	{
		if (leftHand.getOnButton(6) || (leftHand.getButton(6) && (currentTime-dirButtonPress)>150))
		{
			virtualdirectory.upPressed();
			dirButtonPress = currentTime;
		}
		else if (leftHand.getOnButton(7) || (leftHand.getButton(7) && (currentTime-dirButtonPress)>150))
		{
			virtualdirectory.downPressed();
			dirButtonPress = currentTime;
		}
		else if (leftHand.getOnButton(9) || leftHand.getOnButton(3))// && (currentTime-dirButtonPress)>200)
		{
			pressedImport = currentTime;
			
			if (sandboxed)
			{
				virtualdirectory.selectDirectory();
			}
			else
			{
				virtualdirectory.selectFile();
			}
			
			dirButtonPress = currentTime;
		}
		/*else if (rightHand.getOnButton(5))
		{
			pressedImport = currentTime;
			virtualdirectory.findingFile = false;
		}*/
	}
	
	if (rightHand.getOnButton(10) && (currentTime-handButtonPress)>150)
	{
		bool tempy = (rightHand.ray==true)?false:true;
		//cout << "changed hand" << tempy << rightHand.ray << "\n" << flush;
		
		//rightHand.~RightVirtualHand();
		//rightHand = new (&rightHand) RightVirtualHand(tempy, "handy.obj"); 
		rightHand.reInitialize(tempy, "handy.obj");
		handButtonPress = currentTime;
	}
	
	
	if(leftHand.getButton(2) || leftHand.getButton(3) || leftHand.getButton(4) || leftHand.getButton(5) || leftHand.getButton(10))
	{
		selectionMode = 0;
		coneselection = false;
	}
	
	if (selectionMode == 2)
	{
		if(leftHand.getButton(6))	//		6	"L"
		{
			if(leftSelectedObjects.size() == 0)
			{
				selectionMode = 0;
			}
			else if(leftSelectedObjects.size() == 1)
			{
				Object* oby = leftSelectedObjects.front();
				oby->_selected = !oby->_selected;
				if(oby->_selected && oby == rightWiimote)
				{
					cout << "hi :3\n" << flush;
				}
				selectionMode = 0;
			}
			else
			{
				upSelectedObjects.clear();
				rightSelectedObjects.clear();
				downSelectedObjects.clear();
				int i=0;
				int count = leftSelectedObjects.size();
				for(i=0; i<count; ++i)
				{
					Object* oby = leftSelectedObjects.front();
					if(i % 4 == 0)
					{
						leftSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 1)
					{
						upSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 2)
					{
						rightSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 3)
					{
						downSelectedObjects.push_back(oby);
					}
					leftSelectedObjects.pop_front();
				}
			}
		}
		else if(leftHand.getButton(9))	//		9	"U"
		{
			if(upSelectedObjects.size() == 0)
			{
				selectionMode = 0;
			}
			else if(upSelectedObjects.size() == 1)
			{
				Object* oby = upSelectedObjects.front();
				oby->_selected = !oby->_selected;
				if(oby->_selected && oby == rightWiimote)
				{
					cout << "hi :3\n" << flush;
				}
				selectionMode = 0;
			}
			else
			{
				leftSelectedObjects.clear();
				rightSelectedObjects.clear();
				downSelectedObjects.clear();
				int i=0;
				int count = upSelectedObjects.size();
				for(i=0; i<count; ++i)
				{
					Object* oby = upSelectedObjects.front();
					if(i % 4 == 0)
					{
						leftSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 1)
					{
						upSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 2)
					{
						rightSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 3)
					{
						downSelectedObjects.push_back(oby);
					}
					upSelectedObjects.pop_front();
				}
			}
		}
		else if(leftHand.getButton(7)) 	//		8	"R"
		{
			if(rightSelectedObjects.size() == 0)
			{
				selectionMode = 0;
			}
			else if(rightSelectedObjects.size() == 1)
			{
				Object* oby = rightSelectedObjects.front();
				oby->_selected = !oby->_selected;
				if(oby->_selected && oby == rightWiimote)
				{
					cout << "hi :3\n" << flush;
				}
				selectionMode = 0;
			}
			else
			{
				leftSelectedObjects.clear();
				upSelectedObjects.clear();
				downSelectedObjects.clear();
				int i=0;
				int count = rightSelectedObjects.size();
				for(i=0; i<count; ++i)
				{
					Object* oby = rightSelectedObjects.front();
					if(i % 4 == 0)
					{
						leftSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 1)
					{
						upSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 2)
					{
						rightSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 3)
					{
						downSelectedObjects.push_back(oby);
					}
					rightSelectedObjects.pop_front();
				}
			}
		}
		else if(leftHand.getButton(8))	//		7	"D"
		{
			if(downSelectedObjects.size() == 0)
			{
				selectionMode = 0;
			}
			else if(downSelectedObjects.size() == 1)
			{
				Object* oby = downSelectedObjects.front();
				oby->_selected = !oby->_selected;
				if(oby->_selected && oby == rightWiimote)
				{
					cout << "hi :3\n" << flush;
				}
				selectionMode = 0;
			}
			else
			{
				leftSelectedObjects.clear();
				upSelectedObjects.clear();
				rightSelectedObjects.clear();
				int i=0;
				int count = downSelectedObjects.size();
				for(i=0; i<count; ++i)
				{
					Object* oby = downSelectedObjects.front();
					if(i % 4 == 0)
					{
						leftSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 1)
					{
						upSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 2)
					{
						rightSelectedObjects.push_back(oby);
					}
					else if(i % 4 == 3)
					{
						downSelectedObjects.push_back(oby);
					}
					downSelectedObjects.pop_front();
				}
			}
		}
		// if arrow key pressed
		//  if selected quadrant has one, select it, selectionMode = 0
		//  else redistribute objects
	}
	
	button1 = leftHand.getButton(1);

	if(button1) //button is down
	{
		if(pressed1 == 0.0) //was just pressed
		{
			// get time since first I/O in milliseconds
			pressed1 = currentTime;
		}
	}
	else //button is up
	{
		if(pressed1 > 0.0) //button was down, now is up
		{
			if(coneselection)
			{
				selectionMode = 1; // just entered selection mode
				coneselection = false;
			}
			else //just pointed and clicked but didn't hold down
			{
				selectionMode = 3;
			}
		}
		pressed1 = 0.0;
	}

	if(pressed1 > 0.0) //button was held down
	{
		if((currentTime - pressed1) > threshold) //was it held down long enough?
		{
			coneselection = true; //turn flashlight on
		}
	}
	
	
	
	// Detect right hand collisions.
	if(rightHand.ray)
	{
		rightHand.extend(rightHand, objects);
	}
	else
	{
		rightHand.detectCollisions(rightHand, objects);
	}
	
	// Extend left ray to collision point.
	leftHand.extend(leftHand, objects);
	// Update input state (placement matrix & button states) of our effectors.
	rightHand.updateState(framework.getInputState());
	leftHand.updateState(framework.getInputState());
	// Handle any interaction with the objects (see interaction/arInteractionUtilities.h).
	
	list<arInteractable*> objectlist;
	std::copy(objects.begin (), objects.end (), std::back_inserter(objectlist));
	
	ar_pollingInteraction(rightHand, objectlist);
	ar_pollingInteraction(leftHand, objectlist);
	
	
	// Play click sound if right hand has grabbed an object.
	if(rightHand.getGrabbedObject() != 0) 
	{
		dsLoop(clickSound, "click.mp3", -1, 1.0, arVector3(0, 0, 0));
	}
	// Or reset the trigger
	else 
	{
		dsLoop(clickSound, "click.mp3", 0, 1.0, arVector3(0, 0, 0));
	}
	
	
	
	// Update shared memory.
	
	// Transfer data about objects to slave nodes.
//	celloMatrix = theCello.getMatrix();
//	violinMatrix = theViolin.getMatrix();
//	pianoMatrix = thePiano.getMatrix();

	vector<arInteractable*>::iterator i;
	for(i=objects.begin(); i != objects.end(); ++i) 
	{
		Object* oby = ((Object*)(*i));
		oby->matrix = oby->getMatrix();
	}
}


// postExchange callback
// Purposes:
//		- Update effectors based on input state transferred from master node.
//		- Synchronize slave nodes with master node based on transferred data. This is not necessary
//		  for our system, but here's an example for cluster-based systems.
void postExchange(arMasterSlaveFramework& framework) {

	// Presumably the master node already is up-to-date, so we ignore it.
	if(!framework.getMaster()) {
	
		// Update effectors.
		rightHand.updateState(framework.getInputState());
		leftHand.updateState(framework.getInputState());
		
		// Synchronize shared memory.		
		vector<arInteractable*>::iterator i;
		for(i=objects.begin(); i != objects.end(); ++i) 
		{
			Object* oby = ((Object*)(*i));
			oby->setMatrix(oby->matrix.v);
		}
		
	}
}

/*
Thanks to swiftless tutorials: http://www.swiftless.com/tutorials/opengl/square.html
For code on drawing a square

This function draws a square with separating lines for quadrants
*/
void renderPrimitive (float distance, bool separationLines = true) 
{  
	glPushMatrix();
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations  
	  
	glTranslatef(0.0f, 0.0f, distance); // Push eveything 5 units back into the scene, otherwise we won't see the primitive  
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	//glClearColor(0, 0, 0, 0);
	glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_QUADS); // Start drawing a quad primitive  
		  
			glVertex3f(-1.0f, -1.0f, 0.0f); // The bottom left corner  
			glVertex3f(-1.0f, 1.0f, 0.0f); // The top left corner  
			glVertex3f(1.0f, 1.0f, 0.0f); // The top right corner  
			glVertex3f(1.0f, -1.0f, 0.0f); // The bottom right corner  
		  
		glEnd();  
	glPopAttrib();
	/*
	Thanks to Gavin: http://www.opengl.org/discussion_boards/showthread.php/124875-How-to-draw-a-line-using-OpenGL-programme
	For code on drawing lines
	*/
	if (separationLines)
	{
		glTranslatef(0.0f, 0.0f, 0.1f);
		glPushAttrib(GL_LINE_BIT);
		glLineWidth(2.5); 
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.95f, 0.95f, 0.0f);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(-0.95f, 0.95f, 0.0f);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.95f, -0.95f, 0.0f);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(-0.95f, -0.95f, 0.0f);
		glEnd();
		glPopAttrib();
	}
	glPopMatrix();
}  

void drawObjects(float distance)
{
	glPushMatrix();
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations  
	  
	glTranslatef(0.0f, 0.0f, distance+0.2f);
	
	glScalef(0.3f,0.3f,0.3f);
	
	int count = 0;
	list<Object*>::iterator i;
	for(i=leftSelectedObjects.begin(); i != leftSelectedObjects.end(); ++i) 
	{
		glPushMatrix();
		Object* oby = ((Object*)(*i));
		
		++count;
		glTranslatef(-1.5f*(leftSelectedObjects.size()/count), 0.0f, 0.0f);
		
		
		glScalef(oby->getLength()/leftSelectedObjects.size(), oby->getHeight()/leftSelectedObjects.size(), oby->getWidth()/leftSelectedObjects.size());
		arOBJRenderer* ren = oby->getOBJ();
		ren->draw();
		glPopMatrix();
	}
	count = 0;
	for(i=upSelectedObjects.begin(); i != upSelectedObjects.end(); ++i) 
	{
		glPushMatrix();
		Object* oby = ((Object*)(*i));
		
		++count;
		glTranslatef(0.0f, 1.5f*(upSelectedObjects.size()/count), 0.0f);
		
		glScalef(oby->getLength()/upSelectedObjects.size(), oby->getHeight()/upSelectedObjects.size(), oby->getWidth()/upSelectedObjects.size());
		arOBJRenderer* ren = oby->getOBJ();
		ren->draw();
		glPopMatrix();
	}
	count = 0;
	for(i=rightSelectedObjects.begin(); i != rightSelectedObjects.end(); ++i) 
	{
		glPushMatrix();
		Object* oby = ((Object*)(*i));
		
		++count;
		glTranslatef(1.5f*(rightSelectedObjects.size()/count), 0.0f, 0.0f);
		
		glScalef(oby->getLength()/rightSelectedObjects.size(), oby->getHeight()/rightSelectedObjects.size(), oby->getWidth()/rightSelectedObjects.size());
		arOBJRenderer* ren = oby->getOBJ();
		ren->draw();
		glPopMatrix();
	}
	count = 0;
	for(i=downSelectedObjects.begin(); i != downSelectedObjects.end(); ++i) 
	{
		glPushMatrix();
		Object* oby = ((Object*)(*i));
		
		++count;
		glTranslatef(0.0f, -1.5f*(downSelectedObjects.size()/count), 0.0f);
		
		glScalef(oby->getLength()/downSelectedObjects.size(), oby->getHeight()/downSelectedObjects.size(), oby->getWidth()/downSelectedObjects.size());
		arOBJRenderer* ren = oby->getOBJ();
		ren->draw();
		glPopMatrix();
	}
	
	glPopMatrix();
}


// draw callback
// Purposes:
//		- Load the navigation matrix to reflect frame's navigation
//		- Use OpenGL to generate computer graphics.
void draw(arMasterSlaveFramework& framework) {

	// Load the navigation matrix.
	framework.loadNavMatrix();
	
	// Generate graphics.
	
	if(selectionMode == 2)
	{
		renderPrimitive(-2.5f); // draws square with quadrants
		drawObjects(-2.5f); // draw the mini versions
	}
	
	if(virtualdirectory.findingFile)
	{
		virtualdirectory.draw();
	}
	
	// Draw the objects.
//	theCello.draw();
//	theViolin.draw();
//	thePiano.draw();
	
	vector<arInteractable*>::iterator i;
	for(i=objects.begin(); i != objects.end(); ++i) 
	{
		Object* oby = ((Object*)(*i));
		oby->draw();
	}

	if (showFloor)
	{
		glPushMatrix();
		glScalef(0.05f,0.05f,0.05f);
		mrFloor.draw();
		glPopMatrix();
	}
	
	// Draw the effectors.
	rightHand.draw();
	leftHand.draw();
}


void playSound(vector<string> args)
{
	// Parameters are:
	//		name - string name for sound
	//		transformName - string name for dsTransform
	//		loopType - 1 for continuous, -1 for one-time, 0 to stop
	//		loudness - float from 0.0 (quiet) to 1.0 (max)
	//		positionVector - vector position of sound origin
	// Create loop for click sound.
	int eeksound = dsLoop("click", "world", args[0], 0, 1.0, arVector3(0, 0, 0));
	
	// Play click sound if right hand has grabbed an object.
//	if(rightHand.getGrabbedObject() != 0) 
//	{
		dsLoop(eeksound, args[0], -1, 1.0, arVector3(0, 0, 0));
//	}
	// Or reset the trigger
//	else 
//	{
//		dsLoop(clickSound, "click.mp3", 0, 1.0, arVector3(0, 0, 0));
//	}
}


/*Changed by Harish Babu Arunachalam*/
/*Method to navigate forward for TreeMenu*/
void goForward()
{
	//cout<<"curTreeLevel "<<curTreeLevel<<endl<<flush;

	//currentPtr->printValues(currentPtr);
	//cout<<"currentPointer name is "<<currentPtr->name<<endl<<flush;
	//cout<<"treeIndex value is "<<treeIndex<<endl<<flush;
	if(currentPtr->noOf_FwdPtrs>0)
	{
		bool leaf = true;
		parentMenu = currentPtr;
		currentPtr = currentPtr->forwardPtrs[treeIndex];
		curTreeLevel = currentPtr->level;
		if (strcmp(currentPtr->name.c_str(),"New project")==0)
		{
			/*cout << "you selected New project pim\n" << flush;
			templateName = PATH+"Kosmos\\templates\\template3.kide";
			vector<string> projman;
			projman.push_back(PATH+"newproj");
			ProjectManager::findProjectCallback(projman);
			currentPtr = nodeMenu;*/
			virtualdirectory.startBrowse("template", &ProjectManager::findTemplateCallback,"Select template file: ", TEMPLATEPATH);
			tabletOn = false;
		}
		else if (strcmp(currentPtr->name.c_str(),"Object")==0)
		{
			virtualdirectory.startBrowse("import", &Import::importCallback, "Select obj to import: ", OBJECTPATH);
			tabletOn = false;
		}
		else if (strcmp(currentPtr->name.c_str(),"Load project")==0)
		{
			virtualdirectory.startBrowse("loadProjectCallback", &ProjectManager::loadProjectCallback,"Select kproj file to load: ", SANDBOXPATH);
			cout << "loading project from path " << SANDBOXPATH << "\n" << flush;
			tabletOn = false;
		}
		else if (strcmp(currentPtr->name.c_str(),"Save project")==0)
		{
			ProjectManager::save();
		}
		else if (strcmp(currentPtr->name.c_str(),"Generate code")==0)
		{
			ProjectManager::generate();
		}
		else if (strcmp(currentPtr->name.c_str(),"Extend")==0)
		{
			if(rightSelected)
			{
				cout << "replacing right hand with extend\n" << flush;
				vector<string> handy;
				handy.push_back("right");
				ExtendBlock::insertBlock(handy);
				ProjectManager::save();
			}
			else
			{
				cout << "replacing left hand with extend\n" << flush;
				vector<string> handy;
				handy.push_back("left");
				ExtendBlock::insertBlock(handy);
				ProjectManager::save();
			}
		}
		else if (strcmp(currentPtr->name.c_str(),"Virtual Hand")==0)
		{
			if(rightSelected)
			{
				cout << "replacing right hand with vhand\n" << flush;
				vector<string> handy;
				handy.push_back("right");
				VirtualHandBlock::insertBlock(handy);
				ProjectManager::save();
			}
			else
			{
				cout << "replacing left hand with vhand\n" << flush;
				vector<string> handy;
				handy.push_back("left");
				VirtualHandBlock::insertBlock(handy);
				ProjectManager::save();
			}
		}
		else if (strcmp(currentPtr->name.c_str(),"Show Floor")==0)
		{
			showFloor = !showFloor;
		}
		else if (strcmp(currentPtr->name.c_str(),"Scale")==0)
		{
			cout << "scale active\n"<<flush;
			float xdist = rightHand.getX() - leftHand.getX();
			float ydist = rightHand.getY() - leftHand.getY();
			float zdist = rightHand.getZ() - leftHand.getZ();
			distBetweenHands = sqrt(xdist*xdist+ydist*ydist+zdist*zdist);
			scalingActive = true;
		}
		else if (strcmp(currentPtr->name.c_str(),"Session")==0)
		{
			virtualdirectory.startBrowse("playSound", &playSound,"Select sound file to play: ", PATH);
			tabletOn = false;
		}
		else if (strcmp(currentPtr->name.c_str(),"Delete")==0)
		{
			/*
			vector< string >::it = curFiles.begin();
			while(it != curFiles.end()) 
			{
				if(aConditionIsMet) 
				{
					it = curFiles.erase(it);
				}
				else ++it;
			}
			*/
			vector<Object*> toDelete;
		
			vector<arInteractable*>::iterator i;
			for(i=objects.begin(); i != objects.end(); ++i) 
			{
				Object* oby = ((Object*)(*i));
				if(oby->_selected && oby!=rightWiimote && oby!=leftWiimote && oby!=userObject)
				{
					toDelete.push_back(oby);
					oby->deleteObject();
					
				}
			}
			
			vector<Object*>::iterator ii;
			for(ii=toDelete.begin(); ii != toDelete.end(); ++ii) 
			{
				Object* oby = ((Object*)(*ii));
				std::vector<arInteractable*>::iterator position = std::find(objects.begin(), objects.end(), oby);
				if (position != objects.end()) // == vector.end() means the element was not found
				{
					objects.erase(position);
				}
			}
		}
		else if (strcmp(currentPtr->name.c_str(),"Horizontal")==0)
		{
			ostringstream lines;

			lines << "							<code parent=\"dpadUD\">" << endl;
			lines << "	// Translate along Z-axis (forwards/backwards) if joystick is pressed more than 20% along axis 1." << endl;
			lines << "	framework.setNavTransCondition(&apos;z&apos;, AR_EVENT_AXIS, 1, 0.2);" << endl;
			lines << "							</code>" << endl;
			std::string horizontalMovementStr = lines.str();
			std::vector<char> horizontalMovementVec(horizontalMovementStr.begin(), horizontalMovementStr.end());
			horizontalMovementVec.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
			rapidxml::xml_document<> horizontalMovementDoc;
			horizontalMovementDoc.parse<0>( &horizontalMovementVec[0] );
			rapidxml::xml_node<>* horizontalMovementNode = horizontalMovementDoc.first_node();
			rapidxml::xml_node<> *firstCodeBlock = codeTree.first_node("project")->first_node("directory")->first_node("directory")->next_sibling()->next_sibling()->first_node("file")->next_sibling()->next_sibling()->first_node("codeblocks")->first_node("codeblock");
			rapidxml::xml_node<> *codenode = firstCodeBlock->next_sibling()->next_sibling()->first_node("functioncode")->first_node("code");//prepend_node( horizontalMovementAppendNode);
			while((codenode->first_attribute("parent") == 0) || (strcmp(codenode->first_attribute("parent")->value(),"dpadUD")!=0))
			{
				codenode = codenode->next_sibling();
			}
			codeTree.clone_node( horizontalMovementNode, codenode );
			
			char* val = codeTree.allocate_string("Horizontal");
			codeTree.first_node("project")->first_node("profile")->first_node("user")->first_node("movement")->first_node("dpadUD")->value(val);
			
			cout << "set horizontal movement...\n" << flush;
		}
		else if (strcmp(currentPtr->name.c_str(),"Vertical")==0)
		{
			ostringstream lines;

			lines << "							<code parent=\"dpadUD\">" << endl;
			lines << "	// Translate along Y-axis (up/down) if joystick is pressed more than 20% along axis 1." << endl;
			lines << "	framework.setNavTransCondition(&apos;y&apos;, AR_EVENT_AXIS, 1, 0.2);" << endl;
			lines << "							</code>" << endl;
			std::string verticalMovementStr = lines.str();
			std::vector<char> verticalMovementVec(verticalMovementStr.begin(), verticalMovementStr.end());
			verticalMovementVec.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
			rapidxml::xml_document<> verticalMovementDoc;
			verticalMovementDoc.parse<0>( &verticalMovementVec[0] );
			rapidxml::xml_node<>* verticalMovementNode = verticalMovementDoc.first_node();
			rapidxml::xml_node<> *firstCodeBlock = codeTree.first_node("project")->first_node("directory")->first_node("directory")->next_sibling()->next_sibling()->first_node("file")->next_sibling()->next_sibling()->first_node("codeblocks")->first_node("codeblock");
			rapidxml::xml_node<> *codenode = firstCodeBlock->next_sibling()->next_sibling()->first_node("functioncode")->first_node("code");//prepend_node( verticalMovementAppendNode);
			while((codenode->first_attribute("parent") == 0) || (strcmp(codenode->first_attribute("parent")->value(),"dpadUD")!=0))
			{
				codenode = codenode->next_sibling();
			}
			codeTree.clone_node( verticalMovementNode, codenode );
			
			char* val = codeTree.allocate_string("Vertical");
			codeTree.first_node("project")->first_node("profile")->first_node("user")->first_node("movement")->first_node("dpadUD")->value(val);
			
			cout << "set vertical movement...\n" << flush;
		}
		else if (strcmp(currentPtr->name.c_str(),"Strafe")==0)
		{
			ostringstream lines;

			lines << "							<code parent=\"dpadLR\">" << endl;
			lines << "	// Rotate around Y-axis (vertical) if joystick is pressed more than 20% along axis 0." << endl;
			lines << "	//framework.setNavRotCondition(&apos;y&apos;, AR_EVENT_AXIS, 0, 0.2);  //FOR ROTATE" << endl;
			lines << "	framework.setNavTransCondition(&apos;x&apos;, AR_EVENT_AXIS, 0, 0.2);  //FOR STRAFING" << endl;
			lines << "							</code>" << endl;

			std::string strafeMovementStr = lines.str();
			std::vector<char> strafeMovementVec(strafeMovementStr.begin(), strafeMovementStr.end());
			strafeMovementVec.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
			rapidxml::xml_document<> strafeMovementDoc;
			strafeMovementDoc.parse<0>( &strafeMovementVec[0] );
			rapidxml::xml_node<>* strafeMovementNode = strafeMovementDoc.first_node();
			rapidxml::xml_node<> *firstCodeBlock = codeTree.first_node("project")->first_node("directory")->first_node("directory")->next_sibling()->next_sibling()->first_node("file")->next_sibling()->next_sibling()->first_node("codeblocks")->first_node("codeblock");
			rapidxml::xml_node<> *codenode = firstCodeBlock->next_sibling()->next_sibling()->first_node("functioncode")->first_node("code");//prepend_node( strafeMovementAppendNode);
			while((codenode->first_attribute("parent") == 0) || (strcmp(codenode->first_attribute("parent")->value(),"dpadLR")!=0))
			{
				codenode = codenode->next_sibling();
			}
			codeTree.clone_node( strafeMovementNode, codenode );
			
			char* val = codeTree.allocate_string("Strafe");
			codeTree.first_node("project")->first_node("profile")->first_node("user")->first_node("movement")->first_node("dpadLR")->value(val);
			
			cout << "set strafe movement...\n" << flush;
		}
		else if (strcmp(currentPtr->name.c_str(),"Rotate")==0)
		{
			ostringstream lines;

			lines << "							<code parent=\"dpadLR\">" << endl;
			lines << "	// Rotate around Y-axis (vertical) if joystick is pressed more than 20% along axis 0." << endl;
			lines << "	framework.setNavRotCondition(&apos;y&apos;, AR_EVENT_AXIS, 0, 0.2);  //FOR ROTATE" << endl;
			lines << "	//framework.setNavTransCondition(&apos;x&apos;, AR_EVENT_AXIS, 0, 0.2);  //FOR STRAFING" << endl;
			lines << "							</code>" << endl;

			std::string rotateMovementStr = lines.str();
			std::vector<char> rotateMovementVec(rotateMovementStr.begin(), rotateMovementStr.end());
			rotateMovementVec.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
			rapidxml::xml_document<> rotateMovementDoc;
			rotateMovementDoc.parse<0>( &rotateMovementVec[0] );
			rapidxml::xml_node<>* rotateMovementNode = rotateMovementDoc.first_node();
			rapidxml::xml_node<> *firstCodeBlock = codeTree.first_node("project")->first_node("directory")->first_node("directory")->next_sibling()->next_sibling()->first_node("file")->next_sibling()->next_sibling()->first_node("codeblocks")->first_node("codeblock");
			rapidxml::xml_node<> *codenode = firstCodeBlock->next_sibling()->next_sibling()->first_node("functioncode")->first_node("code");//prepend_node( rotateMovementAppendNode);
			while((codenode->first_attribute("parent") == 0) || (strcmp(codenode->first_attribute("parent")->value(),"dpadLR")!=0))
			{
				codenode = codenode->next_sibling();
			}
			codeTree.clone_node( rotateMovementNode, codenode );
			
			char* val = codeTree.allocate_string("Rotate");
			codeTree.first_node("project")->first_node("profile")->first_node("user")->first_node("movement")->first_node("dpadLR")->value(val);
			
			cout << "set rotate movement...\n" << flush;
		}
		else
		{
			cout << "you selected " << currentPtr->name << "\n" << flush;
			leaf = false;
		}
		if(leaf)
		{
			currentPtr = nodeMenu;
		}
		treeIndex = 0;
	}
}

/*Method to navigate backward for TreeMenu*/
void goBack()
{
	//Check if the backward navigation has reached the root node - check for currentPtr level.
	treeIndex=0;
		if(currentPtr->level==-1)
			return;
		else
		{
			currentPtr = parentMenu;		//current pointer is the parent pointer
			parentMenu = parentMenu->backwardPtr; // parent pointer is parent's backward pointer
			curTreeLevel = currentPtr->level; //current tree level is one less
		}
	return;
}

/*method to navigate up for TreeMenu*/
void goUp()
{
	//Check if treeIndex decrement will be negative
		/*if(treeIndex-1>=0)
		{
			treeIndex--;
		}
		else
		{
			treeIndex=0;
		}		
	return;
	*/
	treeIndex = (treeIndex-1+currentPtr->noOf_FwdPtrs)%currentPtr->noOf_FwdPtrs;
}

/*Method to navigate backwards*/
void goDown()
{
	//Check if index increment will exceed the maximum forward pointers
		/*if(treeIndex+1 > currentPtr->backwardPtr->noOf_FwdPtrs)
			treeIndex = currentPtr->backwardPtr->noOf_FwdPtrs;
		else
			treeIndex++;
	return;*/
	treeIndex = (treeIndex+1)%currentPtr->noOf_FwdPtrs;
}




int main(int argc, char** argv) {

	// Declare a MasterSlave framework.
	arMasterSlaveFramework framework;
	// Inform the framework what units you're using.
	framework.setUnitConversion(FEET_TO_LOCAL_UNITS);
	// Set the clipping plane distances.
	framework.setClipPlanes(nearClipDistance, farClipDistance);
	// Set start callback to register shared memory, set up navigation, and initialize variables.
	framework.setStartCallback(start);
	// Set windowStartGL callback to initialize OpenGL.
	framework.setWindowStartGLCallback(windowStartGL);
	// Set windowEvent callback to handle window events, like resizing.
	framework.setWindowEventCallback(windowEvent);
	// Set preExchange callback for master to process before shared memory is exchanged.
	framework.setPreExchangeCallback(preExchange);
	// Set postExchange callback for slaves to update themselves based on shared memory.
	framework.setPostExchangeCallback(postExchange);
	// Set draw callback for computer graphics.
	framework.setDrawCallback(draw);
	
	// Initialize MasterSlave framework.
	if(!framework.init(argc, argv)) {
		return 1;
	}
	
	// Start framework. Never returns unless something goes wrong.
	return framework.start() ? 0 : 1;
}
