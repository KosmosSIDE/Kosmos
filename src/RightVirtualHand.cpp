#include "RightVirtualHand.h"


// Right hand effector's collision detection function for multiple objects.
// Purpose:
//		- Determine closest object (based on center) that effector is touching.
// Notes:
//		The list must be a list of Object instances, defined above.
//		This function does not account for rotation of the object or the effector.
void RightVirtualHand::detectCollisions(arEffector& self, vector<arInteractable*>& objects) {

	// Return if grabbing an object.
	if(getGrabbedObject() != 0) return;
	
	// Set maximum distance for testing collision dection to 1000 ft.
	float maxDistance = 1000.0;
	
	// Track closest object and its distance. No object is initially closest.
	arInteractable* closestObject = 0;
	float closestDistance = maxDistance;
	
	// Step through list of objects and test each one for collisions.
	for(vector<arInteractable*>::iterator it=objects.begin(); it != objects.end(); ++it) {
	
		// Get information about object's position and dimensions.
		const arMatrix4 objectMatrix = (*it)->getMatrix();
		arMatrix4 objectTransMatrix = ar_extractTranslationMatrix(objectMatrix);
		const float objectX = objectTransMatrix[12];
		const float objectY = objectTransMatrix[13];
		const float objectZ = objectTransMatrix[14];
		const float objectLength = ((Object*)(*it))->getLength();
		const float objectHeight = ((Object*)(*it))->getHeight();
		const float objectWidth = ((Object*)(*it))->getWidth();
		
		// Get information about effector's position and dimensions. 
		arMatrix4 effectorTransMatrix = ar_extractTranslationMatrix(getMatrix());
		const float effectorX = effectorTransMatrix[12];
		const float effectorY = effectorTransMatrix[13];
		const float effectorZ = effectorTransMatrix[14];
		const float effectorLength = _size;
		const float effectorHeight = _size;
		const float effectorWidth = _size;
		
		// Determine if effector is within object along X-axis.
		if((objectX - objectLength/2.0) <= (effectorX + effectorLength/2.0) && (effectorX - effectorLength/2.0) <= (objectX + objectLength/2.0)) {
		// Determine if effector is within object along Y-axis.
		if((objectY - objectHeight/2.0) <= (effectorY + effectorHeight/2.0) && (effectorY - effectorHeight/2.0) <= (objectY + objectHeight/2.0)) {
		// Determine if effector is within object along Z-axis.
		if((objectZ - objectWidth/2.0) <= (effectorZ + effectorWidth/2.0) && (effectorZ - effectorWidth/2.0) <= (objectZ + objectWidth/2.0)) {
		
			// Collision detected. Now use selector to determine distance to center of the object.
			_selector.setMaxDistance(maxDistance);
			float objectDistance = _selector.calcDistance(self, objectMatrix);
			
			// Determine if object is closest so far.
			if(objectDistance < closestDistance) {
				// If so, remember object and distance to object.
				closestObject = *it;
				closestDistance = objectDistance;
			}
		}}}
	}
	
	// Check if an object was touched.
	if(closestObject != 0) {
		// If so, set selector's distance to match object's distance.
		_selector.setMaxDistance(closestDistance);
		setInteractionSelector(_selector);
		
		//cout << "object was selected?" << '\n';
		
		if(getOnButton(1) == 1)
		{
			//cout << "object was selected" << '\n';
			Object* oby = (Object*)closestObject;
			oby->_selected = !oby->_selected;
		}
	}
	else {
		// If not, set selector's distance to size of effector.
		_selector.setMaxDistance(_size);
		setInteractionSelector(_selector);
	}
}

void RightVirtualHand::extend(arEffector& self, arInteractable* object, float maxLength) {

	// Return if grabbing an object.
	if(getGrabbedObject() != 0) return;

	// Reset tip to 0.0 ft. length.
	_currentLength = 0.0;
	setTipOffset(arVector3(0, 0, -_currentLength));

	// Check if the maximum length has been reached or an object has been touched.
	while(_currentLength < maxLength && !ar_pollingInteraction(self, object)) {
		// If not, increase tip length by interaction distance.
		_currentLength += _interactionDistance;
		setTipOffset(arVector3(0, 0, -_currentLength));
	}
}


void RightVirtualHand::extend(arEffector& self, vector<arInteractable*>& objects, float maxLength) {

	list<arInteractable*> objectlist;
	std::copy(objects.begin (), objects.end (), std::back_inserter(objectlist));

	// Return if grabbing an object.
	if(getGrabbedObject() != 0) return;

	// Reset tip to 0.0 ft. length.
	_currentLength = 0.0;
	setTipOffset(arVector3(0, 0, -_currentLength));

	// Check if the maximum length has been reached or an object has been touched.
	while(_currentLength < maxLength && !ar_pollingInteraction(self, objectlist)) {
		// If not, increase tip length by interaction distance.
		_currentLength += _interactionDistance;
		setTipOffset(arVector3(0, 0, -_currentLength));
	}
}

// Right hand effector's draw function.
void RightVirtualHand::draw() //const {
{

	// Always start with glPushMatrix to avoid matrix multiplications done here from
	// affecting other portions of the scene.
	glPushMatrix();
		// Set to center of effector.
		glMultMatrixf(getCenterMatrix().v);
		if(!ray)
		{
			glRotatef(180,0,1,0);
			glRotatef(90,0,0,1);
			glScalef(-1,1,1);
			loadedOBJ.draw();
		}
		else
		{
			glPushAttrib(GL_COLOR_BUFFER_BIT);
			
			glScalef(2.0/12.0, 2.0/12.0, _currentLength);
			// Draw the left hand red.
			glColor3f(1.0, 0.0, 0.0);
			// Create the 1 ft. cube.
			glutSolidCube(1.0);
			glColor3f(0.0, 0.0, 0.0);
			glutWireCube(1.01);
			
			glPopAttrib();
		}
	
	// Always finish with glPopMatrix to match glPushMatrix above.
	glPopMatrix();
}


