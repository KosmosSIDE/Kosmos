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

using namespace rapidxml;

string projectName = "kosmosGenerated";
xml_document<> codeTree;
vector<char> document;
string templateName = "";
string projectDir = "";

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

int selectionMode = 0; //0 means null, 1 means just entered, 2 means in selection, 3 means just point and click

// has it been pressed
static int button1 = 0;
// how long has it been pressed
static double pressed1 = 0.0;
// how long do we need to hold it
const double threshold = 1000.0;    // in millis
static double pressedImport = 0.0;
static double dirButtonPress = 0.0;
static double handButtonPress = 0.0;


list<Object*> leftSelectedObjects;
list<Object*> upSelectedObjects;
list<Object*> rightSelectedObjects;
list<Object*> downSelectedObjects;

Object* _leftSelectedObject;
Object* _rightSelectedObject;
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


void uselessCallback(vector<string> args)
{
	cout << "useless callback" << flush;
}

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
	
	ar_navRotate( arVector3(0,1,0), 135 );
	ar_navTranslate( arVector3(0,0,6) );
	
	
	
		templateName = "C:\\aszgard5\\szg\\projects\\Kosmos\\templates\\template3.kide";
		vector<string> projman;
		projman.push_back("C:\\aszgard5\\szg\\projects\\newproj");
		ProjectManager::findProjectCallback(projman);
	
	
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
	
	if( _leftMovering == true && leftHand.getButton(3) == 0)
	{
		_leftSelectedObject->snapMatrix();
		_leftMovering = false;
		_leftSelectedObject = NULL;
	}

	//const unsigned int numButtons = framework.getNumberButtons();

	// in milliseconds
	double currentTime = framework.getTime();

	if((!virtualdirectory.findingFile) && rightHand.getOnButton(4) && (currentTime-pressedImport)>1000)
	{
		pressedImport = currentTime;
		virtualdirectory.startBrowse("import", &Import::importCallback, "Select obj to import: ");
	}
	else if((!virtualdirectory.findingFile) && rightHand.getOnButton(0) && (currentTime-pressedImport)>1000)
	{
		pressedImport = currentTime;
		virtualdirectory.startBrowse("template", &ProjectManager::findTemplateCallback,"Select template file: ", "c:\\aszgard5\\szg\\projects\\Kosmos\\");
	}
	else if(virtualdirectory.findingFile)
	{
		if (leftHand.getOnButton(9) || (leftHand.getButton(9) && (currentTime-dirButtonPress)>150))
		{
			virtualdirectory.upPressed();
			dirButtonPress = currentTime;
		}
		else if (leftHand.getOnButton(8) || (leftHand.getButton(8) && (currentTime-dirButtonPress)>150))
		{
			virtualdirectory.downPressed();
			dirButtonPress = currentTime;
		}
		else if (leftHand.getOnButton(7))// && (currentTime-dirButtonPress)>200)
		{
			pressedImport = currentTime;
			virtualdirectory.selectFile();
			dirButtonPress = currentTime;
			//cout << "wth" << flush;
		}
		else if (rightHand.getOnButton(4))// && (currentTime-dirButtonPress)>200)
		{
			pressedImport = currentTime;
			virtualdirectory.selectDirectory();
			dirButtonPress = currentTime;
			//cout << "wth" << flush;
		}
		else if (rightHand.getOnButton(5))
		{
			pressedImport = currentTime;
			virtualdirectory.findingFile = false;
		}
	}
	
	if (rightHand.getOnButton(10) && (currentTime-handButtonPress)>150)
	{
		bool tempy = (rightHand.ray==true)?false:true;
		cout << "changed hand" << tempy << rightHand.ray << "\n" << flush;
		
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
		//cout << "starting vd.draw\n" << flush;
		virtualdirectory.draw();
		//cout << "end vd.draw\n" << flush;
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

	
	// Draw the effectors.
	rightHand.draw();
	leftHand.draw();
}


// main entry to MasterSlave application
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
