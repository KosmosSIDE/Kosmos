#include "Object.h"
#include "selectedObjects.h"

void Object::setHPR(float h, float p, float r)
{
	arVector3 angles;
	arEulerAngles* eulers = new arEulerAngles(AR_XYZ, angles);
	float heading = (h*3.141592)/180;
	float pitch = (p*3.141592)/180;
	float roll = (r*3.141592)/180;
	eulers->setAngles(heading, pitch, roll);
	arMatrix4 rotation = eulers->toMatrix();
	arMatrix4 translation = ar_extractTranslationMatrix(_matrix);
	arMatrix4 scale = ar_extractScaleMatrix(_matrix);
	_matrix = translation*scale*rotation;
}

void Object::setMatrix(const arMatrix4& matrix)
{
	arInteractableThing::setMatrix(matrix);
	if(_type == 5)
	{
		/*if(rightWiimote != NULL)
		{
			arMatrix4 thisMatrix(matrix);
			//thisMatrix[12] = matrix.v[12]+3;
			//thisMatrix[13] = matrix.v[13]+2;
			//thisMatrix[14] = matrix.v[14]-1.6;
			rightWiimote->setMatrix(thisMatrix);
		}*/
	}
}

void Object::snapMatrix()
{
	int inchesToSnap = 6;
	int degreesToSnap = 15;
	float radiansToSnap = (degreesToSnap*3.141592)/180;
	_matrix[12] = ((int)round(_matrix[12]*12/inchesToSnap))*inchesToSnap/12;
	_matrix[13] = ((int)round(_matrix[13]*12/inchesToSnap))*inchesToSnap/12;
	_matrix[14] = ((int)round(_matrix[14]*12/inchesToSnap))*inchesToSnap/12;
	
	arVector3 angles;
	arEulerAngles* eulers = new arEulerAngles(AR_XYZ, angles);
	angles = eulers->extract(_matrix);
	float x = (int)round(angles[0]/radiansToSnap)*radiansToSnap;
	float y = (int)round(angles[1]/radiansToSnap)*radiansToSnap;
	float z = (int)round(angles[2]/radiansToSnap)*radiansToSnap;
	eulers->setAngles(x, y, z);
	arMatrix4 rotation = eulers->toMatrix();
	arMatrix4 translation = ar_extractTranslationMatrix(_matrix);
	arMatrix4 scale = ar_extractScaleMatrix(_matrix);
	_matrix = translation*scale*rotation;
	
	//TODO: update the xml
	
}

// Interactable object's draw function.
void Object::draw() 
{
  	// Always start with glPushMatrix to avoid matrix multiplications done here from
	// affecting other portions of the scene.
	glPushMatrix();
		// Multiply by object's matrix.
		glMultMatrixf( getMatrix().v );
		// Scale to meet length, height, and width requirements.
		glScalef(_length, _height, _width);
		// Set color based on whether object has been selected.
		glColor3f(0.0, 1.0, 0.0);
		// Draw solid based on type.
		if(_type == 0) {
			// Create 1 ft. cube with black or highlighted wireframe.
			glutSolidCube(1.0);
			if(getHighlight()) { glColor3f(1.0, 1.0, 0.0); }
			else { glColor3f(0.0, 0.0, 0.0); }
			glutWireCube(1.01);
		}
		else if(_type == 1) {
			// Create 1 ft. diameter sphere with black or highlighted wireframe.
			glutSolidSphere(0.5, 16, 16);
			if(getHighlight()) { glColor3f(1.0, 1.0, 0.0); }
			else { glColor3f(0.0, 0.0, 0.0); }
			glutWireSphere(0.505, 16, 16);
		}
		else if(_type == 2) { 
			// Draw loaded OBJ file.
			if (this->_selected)
			{
				glutWireCube(1.01);
			}
			loadedOBJ.draw();			
		}
		else if(_type == 4) //wiimote
		{
			//glutWireCube(0.8);
			loadedOBJ.draw();
			/*if (this == rightWiimote && rightWiimote->_selected) //rightWiimote->getHighlight()
			{
				glutWireCube(1.01);
			}
			if (this == leftWiimote && leftWiimote->_selected) //rightWiimote->getHighlight()
			{
				glutWireCube(1.01);
			}*/
			if (this->_selected)
			{
				glutWireCube(1.01);
			}
		}
		else if(_type == 5) //person
		{
			if (this->_selected)
			{
				glutWireCube(1.01);
			}
			loadedOBJ.draw();
			//rightWiimote->loadedOBJ.normalizeModelSize();
			glPushMatrix();
				//glScalef(1/_length, 1/_height, 1/_width);
				/*glTranslatef(3, 2, -1.6);
				glRotatef(180, 0.0,1.0,0.0); //pitch
				glRotatef(180, 0.0,0.0,1.0); //yaw/heading
				glRotatef(-45, 1.0,0.0,0.0); //roll
				//glScalef(0.5, 0.5, 0.5);
				//glTranslatef(3, 2, -1.6);
				rightWiimote->loadedOBJ.draw();
				if (rightWiimote->_selected) //rightWiimote->getHighlight()
				{
					glutWireCube(1.01);
				}*/
			glPopMatrix();
			glPushMatrix();
				/*glScalef(1/_length, 1/_height, 1/_width);
				glTranslatef(-3, 2, -1.6);
				glRotatef(180, 0.0,1.0,0.0); //pitch
				glRotatef(180, 0.0,0.0,1.0); //yaw/heading
				glRotatef(-45, 1.0,0.0,0.0); //roll
				//glScalef(0.5, 0.5, 0.5);
				//glTranslatef(3, 2, -1.6);
				leftWiimote->loadedOBJ.draw();
				if (leftWiimote->_selected)
				{
					glutWireCube(1.01);
				}*/
			glPopMatrix();
		}
		else if(_type > 2) { 
			// Draw loaded OBJ file.
			if (this->_selected)
			{
				glutWireCube(1.01);
			}
			loadedOBJ.draw();
			if (_selected)
			{
				//draw little music notes
			/*	arOBJRenderer musicNote;
				musicNote.readOBJ("MusicNote.obj","data");
				musicNote.draw();
			*/
				
				//cout << "drawing note" << '\n';
				glPushMatrix();
				
				float scale = 2.f;
				
				glScalef(scale/_length, scale/_height, scale/_width);
			
				int timetrans = (int)(currentTimeGlobal/100);
				int modded = timetrans % 100;
				float ytranslat = (float)((float)modded / 100.0f)/scale;
				
				float xtranslat = (float)((float)(2-((int(currentTimeGlobal/1000)) % 5))/4.0f)/scale;
				
				glTranslatef(xtranslat,ytranslat,1.0f);
				//loadedOBJ.draw();
				musicNotey.draw();
				//glTranslatef(-xtranslat,-ytranslat,-1.0f);
				
				//glScalef(_length, _height, _width);
				glPopMatrix();
			}
		}
			
	// Always finish with glPopMatrix to match glPushMatrix above.
	glPopMatrix();
}

// Main method. Let the
// effector manipulate the interactable. If not
// already touching the object, touch it. If we try to
// touch it, call the virtual _touch method (which, for instance
// in the arCallbackInteractable subclass, ends up calling the touch
// callback).
bool Object::processInteraction( arEffector& effector )
{
	if(this == leftWiimote || this == rightWiimote || this == userObject)
	{
		// The interactable cannot be manipulated if it hasn't been enabled.
		if (!_enabled)
		{
			return false;
		}

		// Try to touch the interactable with the effector.
		if (!touched( effector ))
		{ 
			// Not already touching
			if (!touch( effector ))
			{ 
				// failed to touch this object
				return false;
			}
		}

		// Handle grabbing. If another effector is grabbing us, go on to
		// the virtual _processInteraction method (as can be changed in subclasses
		// like arCallbackInteractable).
		const bool otherGrabbed = grabbed() && (_grabEffector != &effector);
		// true iff object locked to different effector
		if (!otherGrabbed) 
		{
			// The drag manager is an object that associates grabbing conditions
			// with dragging behaviors. Essentially, this object defines how
			// the interactable will be manipulated. We either use the drag manager
			// associated with the effector OR the drag manager associated with the
			// interactable (as in the case of scene navigation).
			const arDragManager* dm = _useDefaultDrags ?
			effector.getDragManager() : (const arDragManager*)&_dragManager;
			if (!dm) 
			{
				cerr << "arInteractable error: NULL grab manager pointer.\n";
				return false;
			}

			// The main method of arDragManager is getActiveDrags.
			// The drag manager maintains a list of grab-conditions/ drag-behavior
			// pairs. As the grab conditions are met (based on the effector's input
			// state), they get added to the active drag list. As they are no
			// longer are met, they get removed. 
			dm->getActiveDrags( &effector, (const arInteractable*)this, _activeDrags );
			if (_activeDrags.empty()) 
			{
				if (grabbed())
				{
					_ungrab();
					_selected = !_selected;
					cout << "handy\n" << flush;
					if(_selected)
					{
						if(this == rightWiimote)
						{
							rightSelected = true;
							leftWiimote->_selected=false;
							userObject->_selected = false;
							currentPtr = wiiNodeMenu->forwardPtrs[0];
							cout << "hi :3 changing menu?\n" << flush;
						}
						else if(this == leftWiimote)
						{
							rightWiimote->_selected=false;
							userObject->_selected = false;
							rightSelected = false;
							currentPtr = wiiNodeMenu->forwardPtrs[0];
						}
						else if(this == userObject)
						{
							leftWiimote->_selected=false;
							rightWiimote->_selected=false;
							currentPtr = userMenu->forwardPtrs[0];
						}
						else
						{
							currentPtr = nodeMenu;
							leftWiimote->_selected=false;
							rightWiimote->_selected=false;
							userObject->_selected = false;
						}
					}
					else
					{
						currentPtr = nodeMenu;
						leftWiimote->_selected=false;
						rightWiimote->_selected=false;
					}
				}
			} 
			else 
			{
				//_selected = true;
				
				if (effector.requestGrab( this )) 
				{
					_grabEffector = &effector;  // ASSIGNMENT
				} 
				else 
				{
					cerr << "arInteractable error: lock request failed.\n";
					_ungrab();
				}
				/*for (arDragMap_t::iterator iter = _activeDrags.begin();iter != _activeDrags.end(); iter++) 
				{
					// The drag behavior updates the interactable's matrix using the
					// grab condition.
					iter->second->update( &effector, this, iter->first );
				}*/
			}
		}
		// Do other event processing. This is defined by a virtual method of the
		// arInteractable so that subclasses can create various sorts of behaviors.
		// For instance, an object might change color when it is grabbed or touched.
		return true;//_processInteraction( effector ); //arInteractableThing does not use _processInteraction
	}
	else
	{
		//return arInteractable::processInteraction( effector );
		// The interactable cannot be manipulated if it hasn't been enabled.
		if (!_enabled)
		{
			return false;
		}

		// Try to touch the interactable with the effector.
		if (!touched( effector ))
		{ 
			// Not already touching
			if (!touch( effector ))
			{ 
				// failed to touch this object
				return false;
			}
		}

		// Handle grabbing. If another effector is grabbing us, go on to
		// the virtual _processInteraction method (as can be changed in subclasses
		// like arCallbackInteractable).
		const bool otherGrabbed = grabbed() && (_grabEffector != &effector);
		// true iff object locked to different effector
		if (!otherGrabbed) 
		{
			// The drag manager is an object that associates grabbing conditions
			// with dragging behaviors. Essentially, this object defines how
			// the interactable will be manipulated. We either use the drag manager
			// associated with the effector OR the drag manager associated with the
			// interactable (as in the case of scene navigation).
			const arDragManager* dm = _useDefaultDrags ?
			effector.getDragManager() : (const arDragManager*)&_dragManager;
			if (!dm) 
			{
				cerr << "arInteractable error: NULL grab manager pointer.\n";
				return false;
			}

			// The main method of arDragManager is getActiveDrags.
			// The drag manager maintains a list of grab-conditions/ drag-behavior
			// pairs. As the grab conditions are met (based on the effector's input
			// state), they get added to the active drag list. As they are no
			// longer are met, they get removed. 
			dm->getActiveDrags( &effector, (const arInteractable*)this, _activeDrags );
			if (_activeDrags.empty()) 
			{
				if (grabbed())
				{
					_ungrab();
					snapMatrix();
				}
			} 
			else 
			{
				if (effector.requestGrab( this )) 
				{
					_grabEffector = &effector;  // ASSIGNMENT
						rightWiimote->_selected = false;
						leftWiimote->_selected = false;
				} 
				else 
				{
					cerr << "arInteractable error: lock request failed.\n";
					_ungrab();
				}
				for (arDragMap_t::iterator iter = _activeDrags.begin();iter != _activeDrags.end(); iter++) 
				{
					// The drag behavior updates the interactable's matrix using the
					// grab condition.
					iter->second->update( &effector, this, iter->first );
				}
			}
		}
		// Do other event processing. This is defined by a virtual method of the
		// arInteractable so that subclasses can create various sorts of behaviors.
		// For instance, an object might change color when it is grabbed or touched.
		return true;//_processInteraction( effector ); //arInteractableThing does not use _processInteraction
	}
	return false;
}

bool Object::touch( arEffector& effector ) 
{
	/*if (this == rightWiimote || this == leftWiimote)
	{
		cout << "stop touching me\n" << flush;
		return true;
	}*/

	if (!_enabled)
		return false;
	std::vector<arEffector*>::iterator effIter = std::find( _touchEffectors.begin(), _touchEffectors.end(), &effector );
	if (effIter != _touchEffectors.end()) 
	{
		// This effector is already touching this interactable. Nothing to do!
		return true;
	}
	// Call the virtual _touch method on this effector.
	const bool ok = _touch( effector );
	if (ok) 
	{
		// The touch succeeded.
		effector.setTouchedObject( this );
		_touchEffectors.push_back( &effector ); // ASSIGNMENT
	}
	return ok;
}

bool Object::_touch( arEffector& /*effector*/ ) {
	setHighlight(true);
	return true;
}
