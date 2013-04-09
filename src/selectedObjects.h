#ifndef SELECTEDOBJECTS_H
#define SELECTEDOBJECTS_H

#include "Object.h"
#include "VirtualDirectory.h"

extern list<Object*> leftSelectedObjects;
extern list<Object*> upSelectedObjects;
extern list<Object*> rightSelectedObjects;
extern list<Object*> downSelectedObjects;

extern Object* _rightSelectedObject;
extern Object* _leftSelectedObject;
extern bool _rightMoving;
extern bool _leftMovering;

extern VirtualDirectory virtualdirectory;

#endif
