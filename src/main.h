#ifndef MAIN_H
#define MAIN_H
// The precompiled header include MUST appear as the first non-commented line. See 
// arCallingConventions.h for details.
#include "arPrecompiled.h"

#define SZG_DO_NOT_EXPORT
#include <stdio.h>
#include <stdlib.h>
#include "arMasterSlaveFramework.h"
#include "arInteractableThing.h"
#include "arInteractionUtilities.h"
#include "arNavigationUtilities.h"
#include "arGlut.h"
#include "arOBJ.h"
#include "Constants.h"

#include "TreeMenu.h"

//rapid xml library
#include "../lib/rapidxml/rapidxml.hpp"
#include "../lib/rapidxml/rapidxml_print.hpp"

using namespace rapidxml;

extern double currentTimeGlobal;
// music note to be used for animation
extern arOBJRenderer musicNotey;
extern arOBJRenderer mrTablet;
extern bool coneselection;
extern int selectionMode;
extern vector<arInteractable*> objects;
extern string projectName;
extern xml_document<> codeTree;
extern vector<char> document;
extern string templateName;
extern string projectDir;
extern bool tabletOn;
extern bool sandboxed;
extern bool rightSelected;
extern bool showFloor;
extern bool displaySuccess;



extern int numObjects;

extern TreeMenu *wiiNodeMenu;
extern TreeMenu *userMenu;
extern TreeMenu *objectMenu;
extern TreeMenu *hmdMenu;

//Changes by Harish Babu Arunachalam
extern TreeMenu *nodeMenu;
extern TreeMenu *parentMenu;
extern TreeMenu *currentPtr;
extern int curTreeLevel;
extern int treeIndex;
//End of changes by Harish Babu Arunachalam

void renderPrimitive (float distance, bool separationLines);
void setProjectName (string &pname);

#endif
