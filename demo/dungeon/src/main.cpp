
//************************************************************************************************
// Name:		&projName;
// Version:		1.0
// Description:	This arMasterSlaveFramework application does stuff
// this application was created with Kosmos spatial IDE
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


list<Object*> leftSelectedObjects;
list<Object*> upSelectedObjects;
list<Object*> rightSelectedObjects;
list<Object*> downSelectedObjects;





// Global objects, instruments


// List of objects.
vector<arInteractable*> objects;

// Global effectors.

RightVirtualHand rightHand("handy.obj"); // loads obj for hand

LeftVirtualHand leftHand("staff.obj");


// Global sound variables.
int soundTransformID;
int clickSound;
Object theMrDungeonRoom5(2,37.3094,37.3094,37.3094, "MrDungeonRoom.obj");
Object theMrBookshelf6(2,1.84366,1.84366,1.84366, "MrBookshelf.obj");
Object theMrCreepyBook7(2,0.813935,0.813935,0.813935, "MrCreepyBook.obj");
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
	theMrCreepyBook7.setMatrix(ar_translationMatrix(-3,4,-7));
	objects.push_back(&theMrCreepyBook7);

	theMrBookshelf6.setMatrix(ar_translationMatrix(-3,3,-7));
	objects.push_back(&theMrBookshelf6);

	theMrDungeonRoom5.setMatrix(ar_translationMatrix(0,4,-5));
	objects.push_back(&theMrDungeonRoom5);

	// Register shared memory. Not needed for non-cluster-based systems.
	// framework.addTransferField(char* name, void* address, arDataType type, int numElements);
	vector<arInteractable*>::iterator i;
	for(i=objects.begin(); i != objects.end(); ++i) 
	{
		Object* obj = ((Object*)(*i));
		obj->loadedOBJ.normalizeModelSize();
		ostringstream ostr;
		ostr << "objMatrix" << &i;
		arMatrix4 obM = obj->matrix;
		framework.addTransferField(ostr.str(), &obM, AR_FLOAT, 16);
	}
	
	// Set up navigation.

	// A traditional pointing technique using the joystick with left and right rotating the scene.
	// Translate along Z-axis (forwards/backwards) if joystick is pressed more than 20% along axis 1.
	framework.setNavTransCondition('z', AR_EVENT_AXIS, 1, 0.2);

	// Rotate around Y-axis (vertical) if joystick is pressed more than 20% along axis 0.
	//framework.setNavRotCondition('y', AR_EVENT_AXIS, 0, 0.2);  //FOR ROTATE
	framework.setNavTransCondition('x', AR_EVENT_AXIS, 0, 0.2);  //FOR STRAFING

	// Set translation speed to 5 feet/second.
	framework.setNavTransSpeed(5.0);

	// Set rotation speed to 30 degrees/second.
	framework.setNavRotSpeed(30.0);

	
	
	// Initialize application variables here.
	
	// Move object's to initial positions.

	
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
	
	
	// Return true if everything is initialized correctly.
	return true;

}

// windowStartGL callback
// Purposes:
// 		- Initialize each window (a Syzygy application can have more than one window).
//		- Initialize OpenGL.
void windowStartGL(arMasterSlaveFramework& framework, arGUIWindowInfo* windowInfo)
{
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
void windowEvent(arMasterSlaveFramework& framework, arGUIWindowInfo* windowInfo)
{
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
void preExchange(arMasterSlaveFramework& framework)
{
	// Handle navigation update. The resulting navigation matrix is automatically transferred
	// to the slave nodes.
	framework.navUpdate();
	
	currentTimeGlobal = framework.getTime();
	
	double currentTime = framework.getTime();
	
	// Process user input.
	
	
	// Update shared memory.
	
	// Transfer data about objects to slave nodes.

	// Detect right hand collisions.
	rightHand.detectCollisions(rightHand, objects);

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

	vector<arInteractable*>::iterator i;
	for(i=objects.begin(); i != objects.end(); ++i) 
	{
		Object* oby = ((Object*)(*i));
		oby->matrix = oby->getMatrix();
	}

	
	arMatrix4 navMatrix = ar_getNavMatrix();

}

// postExchange callback
// Purposes:
//		- Update effectors based on input state transferred from master node.
//		- Synchronize slave nodes with master node based on transferred data. This is not necessary
//		  for our system, but here's an example for cluster-based systems.
void postExchange(arMasterSlaveFramework& framework)
{
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
	glColor3f(0.941, 0.902, 0.549);
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
	//if(leftSelectedObjects.size() > 0)
	{
		/*
		Object* oby = leftSelectedObjects.front();
		arOBJRenderer* ren = oby->getOBJ();
		ren->draw();
		*/
		//leftSelectedObjects.front()->getOBJ().draw();
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
void draw(arMasterSlaveFramework& framework)
{
	// Load the navigation matrix.
	framework.loadNavMatrix();
	
	// Generate graphics.
	if(selectionMode == 2)
	{
		renderPrimitive(-2.5f); // draws square with quadrants
		drawObjects(-2.5f); // draw the mini versions
	}
	
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
int main(int argc, char** argv)
{
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

