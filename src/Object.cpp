#include "Object.h"
#include "selectedObjects.h"
#include "ProjectManager.h"

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
	
	updateProjectFile();
}

void Object::deleteObject()
{

cout<<"delete...remove main.start\n"<<flush;
	rapidxml::xml_node<> *firstCodeBlock = codeTree.first_node("project")->first_node("directory")->first_node("directory")->next_sibling()->next_sibling()->first_node("file")->next_sibling()->next_sibling()->first_node("codeblocks")->first_node("codeblock");
	rapidxml::xml_node<> *cloney = firstCodeBlock->next_sibling()->next_sibling()->first_node("functioncode")->first_node();
	while(cloney->first_attribute("parent") == 0 || strcmp(cloney->first_attribute("parent")->value(),name.c_str()) != 0)
	{
		cloney = cloney->next_sibling(); 
	}
	rapidxml::xml_node<> *parent = cloney->parent();
	parent->remove_node(cloney);
	
cout<<"delete...remove main.globalvars\n"<<flush;
	
	cloney = firstCodeBlock->next_sibling()->first_node("code");
	while(cloney->first_attribute("parent") == 0 || strcmp(cloney->first_attribute("parent")->value(),name.c_str()) != 0)
	{
		cloney = cloney->next_sibling(); 
	}
	parent = cloney->parent();
	parent->remove_node(cloney);
	
cout<<"delete...remove profile\n"<<flush;
	
	cloney = codeTree.first_node("project")->first_node("profile")->first_node("object");
	while(cloney->first_node("name") == 0 || strcmp(cloney->first_node("name")->value(),name.c_str()) != 0)
	{
		cloney = cloney->next_sibling();
	}
	parent = cloney->parent();
	parent->remove_node(cloney);
	
	
	ProjectManager::removeNodesWithAttribute(codeTree.first_node(), "parent", name.c_str());
cout<<"delete...done\n"<<flush;
	
}

void Object::insertObject()
{

	unsigned found = filenm.find_last_of("/\\");
	string filename = filenm.substr(found+1);
	
	ostringstream lines;

	lines << "<code parent=\"" << name << "\">" << endl;
	lines << "the" << name << ".setMatrix(ar_translationMatrix(0, 4, -8));" << endl;
	lines << "objects.push_back(&amp;the" << name << ");" << endl;
	lines << "</code>" << endl;
	std::string mainStartObjectStr = lines.str();
	std::vector<char> mainStartObjectVec(mainStartObjectStr.begin(), mainStartObjectStr.end());
	mainStartObjectVec.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
	rapidxml::xml_document<> mainStartObjectDoc;
	mainStartObjectDoc.parse<0>( &mainStartObjectVec[0] );
	rapidxml::xml_node<>* mainStartObjectNode = mainStartObjectDoc.first_node();
	rapidxml::xml_node<> *mainStartObjectAppendNode = codeTree.clone_node( mainStartObjectNode );
	rapidxml::xml_node<> *firstCodeBlock = codeTree.first_node("project")->first_node("directory")->first_node("directory")->next_sibling()->next_sibling()->first_node("file")->next_sibling()->next_sibling()->first_node("codeblocks")->first_node("codeblock");
	firstCodeBlock->next_sibling()->next_sibling()->first_node("functioncode")->prepend_node( mainStartObjectAppendNode );
	
	cout << "added obj to main.start...\n" << flush;
	// prepend to main.start callback
	/*
							<code parent="cello">
	theCello.setMatrix(ar_translationMatrix(0, 4, -8));
	objects.push_back(&amp;theCello);
							</code>
	*/
	
	std::string mainGlobalObjectStr = "<code parent=\""+name+"\">Object the"+name+"(2, 1, 1, 1, &quot;"+filename+"&quot;);</code>";
	std::vector<char> mainGlobalObjectVec(mainGlobalObjectStr.begin(), mainGlobalObjectStr.end());
	mainGlobalObjectVec.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
	rapidxml::xml_document<> mainGlobalObjectDoc;
	mainGlobalObjectDoc.parse<0>( &mainGlobalObjectVec[0] );
	rapidxml::xml_node<>* mainGlobalObjectNode = mainGlobalObjectDoc.first_node();
	rapidxml::xml_node<> *mainGlobalObjectAppendNode = codeTree.clone_node( mainGlobalObjectNode );
	firstCodeBlock->next_sibling()->append_node( mainGlobalObjectAppendNode );
	
	cout << "added obj to main.global vars...\n" << flush;
	// append to main.global vars
	/*
						<code parent="cello">
Object theCello(3, 0.5, 0.5, 0.5, &quot;cello.obj&quot;);
						</code>
	*/
	
	std::string src2 = "<object><type>OBJ</type><name>"+name+"</name><resourceName>"+filename+"</resourceName>"
		"<x>0</x><y>4</y><z>-8</z><heading>0</heading><pitch>0</pitch><roll>0</roll><scale>1</scale></object>";
	std::vector<char> data(src2.begin(), src2.end());
	data.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
	rapidxml::xml_document<> profiledoc;
	profiledoc.parse<0>( &data[0] );
	rapidxml::xml_node<>* a = profiledoc.first_node();
	rapidxml::xml_node<> *node = codeTree.clone_node( a );
	codeTree.first_node("project")->first_node("profile")->append_node( node ); /* Appending node a to the tree in src */
	// append to profile
	/*
		<object>
			<type>OBJ</type>
			<name>cello</name>
			<resourceName>cello.obj</resourceName>
			<x>0</x>
			<y>4</y>
			<z>-4</z>
			<heading>0</heading>
			<pitch>0</pitch>
			<roll>0</roll>
		</object>
	*/
}

void Object::updateProjectFile()
{
	float scaly = _length;
	arVector3 angles;
	arEulerAngles* eulers = new arEulerAngles(AR_XYZ, angles);
	angles = eulers->extract(_matrix);
	float x = angles[0];
	float y = angles[1];
	float z = angles[2];
	
	//TODO: update the xml
cout << "obj has moved update xml...\n" << flush;
	
	ostringstream lines;


	lines << "<code parent=\"" << name << "\">" << endl;
	lines << "	the" << name << ".setMatrix(ar_translationMatrix("<<_matrix[12]<<","<<_matrix[13]<<","<<_matrix[14]<<"));" << endl;
	lines << "	objects.push_back(&amp;the" << name << ");" << endl;
	lines << "							</code>" << endl;
	std::string mainStartObjectStr = lines.str();
	std::vector<char> mainStartObjectVec(mainStartObjectStr.begin(), mainStartObjectStr.end());
	mainStartObjectVec.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
	rapidxml::xml_document<> mainStartObjectDoc;
	mainStartObjectDoc.parse<0>( &mainStartObjectVec[0] );
	rapidxml::xml_node<>* mainStartObjectNode = mainStartObjectDoc.first_node();
	//rapidxml::xml_node<> *mainStartObjectAppendNode = codeTree.clone_node( mainStartObjectNode );
	rapidxml::xml_node<> *firstCodeBlock = codeTree.first_node("project")->first_node("directory")->first_node("directory")->next_sibling()->next_sibling()->first_node("file")->next_sibling()->next_sibling()->first_node("codeblocks")->first_node("codeblock");
cout << "find code block..."<<name<<"\n" << flush;
	rapidxml::xml_node<> *cloney = firstCodeBlock->next_sibling()->next_sibling()->first_node("functioncode")->first_node();
	while(cloney->first_attribute("parent") == 0 || strcmp(cloney->first_attribute("parent")->value(),name.c_str()) != 0)
	{
		cloney = cloney->next_sibling(); 
	}
	codeTree.clone_node( mainStartObjectNode, cloney );
	
	cout << "added obj to main.start...\n" << flush;
	// prepend to main.start callback
	/*
							<code parent="cello">
	theCello.setMatrix(ar_translationMatrix(0, 4, -8));
	objects.push_back(&amp;theCello);
							</code>
	*/
	ostringstream stringify;
	
	stringify << "<code parent=\""<<name<<"\">Object the"<<name<<"(2,"<<scaly<<","<< scaly<<","<< scaly<<", &quot;"<<name<<".obj&quot;);</code>";
	std::string mainGlobalObjectStr = stringify.str();
	std::vector<char> mainGlobalObjectVec(mainGlobalObjectStr.begin(), mainGlobalObjectStr.end());
	mainGlobalObjectVec.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
	rapidxml::xml_document<> mainGlobalObjectDoc;
	mainGlobalObjectDoc.parse<0>( &mainGlobalObjectVec[0] );
	rapidxml::xml_node<>* mainGlobalObjectNode = mainGlobalObjectDoc.first_node();
	cloney = firstCodeBlock->next_sibling()->first_node("code");
	while(cloney->first_attribute("parent") == 0 || strcmp(cloney->first_attribute("parent")->value(),name.c_str()) != 0)
	{
		cloney = cloney->next_sibling(); 
	}
	codeTree.clone_node( mainGlobalObjectNode, cloney );
	cout << "added obj to main.global vars...\n" << flush;
	// append to main.global vars
	/*
						<code parent="cello">
Object theCello(3, 0.5, 0.5, 0.5, &quot;cello.obj&quot;);
						</code>
	*/
	
	ostringstream linies;


	linies << "<object><type>OBJ</type><name>"<<name<<"</name><resourceName>"<<name<<".obj</resourceName>";
	linies << "<x>"<<_matrix[12]<<"</x><y>"<<_matrix[13]<<"</y><z>"<<_matrix[14]<<"</z><heading>"<<x<<"</heading><pitch>"<<y<<"</pitch><roll>"<<z<<"</roll><scale>"<<scaly<<"</scale></object>" << endl;
	std::string src2 = linies.str();
	std::vector<char> data(src2.begin(), src2.end());
	data.push_back( '\0' );// make it zero-terminated as per RapidXml's docs
	rapidxml::xml_document<> newdoc;
	newdoc.parse<0>( &data[0] );
	rapidxml::xml_node<>* a = newdoc.first_node();
	rapidxml::xml_node<>* cloneInto = codeTree.first_node("project")->first_node("profile")->first_node("object");
	while(cloneInto->first_node("name") == 0 || strcmp(cloneInto->first_node("name")->value(),name.c_str()) != 0)
	{
		cloneInto = cloneInto->next_sibling();
	}
	codeTree.clone_node( a, cloneInto );
	
	// append to profile
	/*
		<object>
			<type>OBJ</type>
			<name>cello</name>
			<resourceName>cello.obj</resourceName>
			<x>0</x>
			<y>4</y>
			<z>-4</z>
			<heading>0</heading>
			<pitch>0</pitch>
			<roll>0</roll>
		</object>
	*/
	
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
