/************************************************************************************
* File   : TreeMenu.h
* Author : Harish Babu Arunachalam
* Contains classes declarations, methods declarations that are implemented in TreeMenu.cpp
*/
#ifndef TREE_MENU
#define TREE_MENU

/* Class Attribute to hold the values of attributes
** Contains one string variable to hold the name of the attribute
** Contains one method setAttrName to set the attribute name */
class attribute
{
public:
  char attributeName[1024];
	void setAttrName(char[]);
	char* getAttrName();
};

/* Class TreeMenu has the logic for menu implementation for tablet in KOSMOS SIDE
** Contains an array of attribute class pointers
** Contains an array of forward pointerClass pointers
** Contains one backward pointer from pointerClass
** Contains noOfAttributes, noOfForwardPtrs to hold the total values of attributes and forwardPointers
** Contains level variable to specify the level of the node
** Contains the methods  - createNode(),deleteNode()
*/
class TreeMenu
{
public:
	attribute *attPtr[100];
	TreeMenu *forwardPtrs[100];
	TreeMenu *backwardPtr;
	int noOfAttributes, noOf_FwdPtrs, level,backPtrIndex;	
	
	TreeMenu();
	TreeMenu* createNode();
	TreeMenu* deleteNode();
	int findNode(TreeMenu*);
	//static int main();
};
#endif // !TREE_MENU
