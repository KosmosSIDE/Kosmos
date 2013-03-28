/*************************************************
* FileName : TreeMenu.cpp
* Author   : Harish Babu Arunachalam
* Description : contains classes and methods for Menu navigation
* Date     : 27th March 2013
********************************************************************/

//declaration of header files
#include <iostream>
#include <string>
#include "TreeMenu.h"
using namespace std;

int CURRENT_LEVEL = 0;

/* Class Attribute to hold the values of attributes
** Contains one string variable to hold the name of the attribute
** Contains one method setAttrName to set the attribute name */
//Method to set the Attribute name
    void attribute::setAttrName(char attName[])
	{
		strcpy(attributeName,attName);
		return;
	}
	//Method to get the Attribute name
	char* attribute::getAttrName()
	{
		return attributeName;
	}

/*Empty class that can serve as forward and backward pointer objects in TreeMenu class*/
class pointerClass
{
};

/* Class TreeMenu has the logic for menu implementation for tablet in KOSMOS SIDE
** Contains an array of attribute class pointers
** Contains an array of forward pointerClass pointers
** Contains one backward pointer from pointerClass
** Contains noOfAttributes, noOfForwardPtrs to hold the total values of attributes and forwardPointers
** Contains level variable to specify the level of the node
** Contains the methods  - createNode(),deleteNode()

class TreeMenu
{
public:
	attribute *attPtr[100];
	TreeMenu *forwardPtrs[100];
	TreeMenu *backwardPtr;
	int noOfAttributes, noOf_FwdPtrs, level,backPtrIndex;
*/
	/*Constructor method to initialize the values*/
	TreeMenu::TreeMenu()
	{
		noOfAttributes = 0;
		noOf_FwdPtrs = 0;
		backPtrIndex = 0;
		level = 0;
	}

	/* CreateNode method to create a new TreeMenu node
	** returns the newly created TreeMenu node
	*/
	TreeMenu* TreeMenu::createNode()
	{
		TreeMenu *tree = new TreeMenu();	//create a new TreeMenu node
		CURRENT_LEVEL = this->level+1;		//increment the current level  by 1

		tree->level = CURRENT_LEVEL;		//assign the new level to the newly created TreeMenu node
		tree->backwardPtr = this;			//assign the backward pointer of new TreeMenu node to the current TreeMenu node
		this->forwardPtrs[this->noOf_FwdPtrs++] = tree; //assign the next array of forward pointer of current TreeMenu node to newly created node and increment pointer
		tree->backPtrIndex = this->noOf_FwdPtrs-1;
		return tree;
	}

	/* deleteNode method deletes the current node
	** returns the previous TreeMenu node pointer
	*/
	TreeMenu* TreeMenu::deleteNode()
	{
		TreeMenu *tempNode = this->backwardPtr;			// assign the new TreeMenu pointer to the backward pointer of current node
		tempNode->forwardPtrs[this->backPtrIndex] = NULL;  //find the pointer index of previous node where the current node is attached and equate it to NULL
		this->backwardPtr = NULL;						//equate the backwardPointer of current node to NULL
		return tempNode;								// return tempNode as current node
	}

	/* findNode method returns the index of matched node
	Doesnt make sense
	*/
	int TreeMenu::findNode(TreeMenu* tempTree)
	{
		for(int i=0;i<this->noOf_FwdPtrs;i++)
		{
			if(tempTree == this->forwardPtrs[i])
			{
				return i;
			}
		}
		return -1;
	}
int main()
{
	int value;
	cout<<"enter the value\n";
	cin>>value;
	return 1;
}
