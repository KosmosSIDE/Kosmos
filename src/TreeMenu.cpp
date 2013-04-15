/*************************************************
* FileName : TreeMenu.cpp
* Author   : Harish Babu Arunachalam
* Description : contains classes and methods for Menu navigation
* Date	   : 27th March 2013
********************************************************************/

//declaration of header files
#include <iostream>
#include <string>
#include "TreeMenu.h"
#include <fstream>
//rapid xml library
//#include "../rapidxml/rapidxml.hpp"
//#include "../rapidxml/rapidxml_print.hpp"

using namespace rapidxml;
using namespace std;

int CURRENT_LEVEL = 0;
TreeMenu *rootNode = new TreeMenu();
TreeMenu *bkdPtr = new TreeMenu();

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

/*Constructor for attribute class*/
attribute::attribute()
{
	attributeName = NULL;
	for(int i=0;i<10;i++)
	{
		value[i] = NULL;
	}
	totalValues = 0;

}

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
/*
int TreeMenu::findNode(char*)
	{
		for(int i=0;i<this->noOf_FwdPtrs;i++)
		{
			if(tempTree == this->forwardPtrs[i])
			{
				return i;
			}
		}
		return -1;
	}*/


	/*method to construct the navigation menu node*/
	TreeMenu* TreeMenu::createMenu(xml_node<> *xmlNode)
	{
		if(xmlNode == NULL)
		{
			return NULL;
		}
		//Declare treemenu nodes, xml document nodes
		TreeMenu *node,*tempNode;
		xml_node <> *tempXmlNode;
		
		//declare level, child information, name of the xml Node
		char *level, *hasChild,*nodeName;

		//initialize the declared variables to null
		level = NULL;
		hasChild = "0";
		nodeName = NULL;

		//check if the node is set to options
		if(strcmp(xmlNode->name(),"options")==0)
		{
			xmlNode = xmlNode->first_node();
		}

		//cout<<"  "<<xmlNode->name()<<endl;
		//declare xml attribute to retrieve the node attribute
		rapidxml::xml_attribute<> *xmlAtt;
		//initialize the number of forward pointers
		int ptrCount = 0;
		//initialize TreeMenu nodes
		node = new TreeMenu();
		tempNode = new TreeMenu();
		node->backwardPtr = bkdPtr;

		//set the value of the node to Menu if the node is menu
		if(strcmp(xmlNode->name(),"menu")==0)
		{
			node->name = "menu";
		}
		
		//retrieve the level of the attribute
		if(xmlNode->first_attribute("level")!= NULL)
		{
			xmlAtt = xmlNode->first_attribute("level");
			level = (xmlAtt->value());
			//level = level;
		//	cout<<"level inside createMenu "<<level<<endl;
			xmlAtt = xmlNode->first_attribute("hasChild");
			//Check if the node has child attributes
			hasChild = (xmlAtt->value());
		
		}
		//retrieve the node named NAME
		if(xmlNode->first_node("name")!= NULL)
			nodeName = xmlNode->first_node("name")->value();
		
		if(nodeName!= NULL)
			node->name = nodeName;
		//assign level to the node
		if(level!= NULL)
		{
			node->level = getIntValue(level);
		}
		//cout<<"string compare "<<strcmp(hasChild,"Y");

		if(strcmp(hasChild,"Y")==0)
		{
			tempXmlNode = xmlNode->first_node("options")->first_node();
			//cout<<"inside hasChild = Y xmlNode is "<<tempXmlNode->name()<<endl;
			//	xml_node<> *tempOptionNode;
			//tempOptionNode = tempXmlNode->first_node("option");
			TreeMenu *tempPtr;
			bkdPtr = node;
			//call createMenu method
			tempPtr = createMenu(tempXmlNode);
			if(tempPtr!=NULL)
			{
				//Assign the children nodes to forward pointer
				node->forwardPtrs[ptrCount++] =	tempPtr;
			}
			//Find the attributes of the node and populate it to the tree menu
			node = populateAttr(node,tempXmlNode->first_node("attrList"));

			cout<<"node comparison "<<(tempXmlNode==xmlNode->last_node())<<endl;
			cout<<" \t temp xml node value "<<tempXmlNode->name()<<" -- xmlnode last node "<<xmlNode->first_node()->last_node()->name()<<endl;

			//while the current node is not the last node, goto the next sibling
			while((tempXmlNode!=xmlNode->first_node()->last_node())&&(tempXmlNode!=NULL))
			{
				tempXmlNode = tempXmlNode->next_sibling();
				tempPtr = createMenu(tempXmlNode);
				if(tempPtr!=NULL)
				{
					node->forwardPtrs[ptrCount++] =	tempPtr;
				}
			}
		}
		//If there are no children, then goto the next node. - happens in case of 'options' node
		else if (strcmp(hasChild,"0")==0)
		{
			xmlNode = xmlNode->first_node();
			node->forwardPtrs[ptrCount++] = createMenu(xmlNode);
		}
		node->noOf_FwdPtrs = ptrCount;
		cout<<" node return Name is "<<node->name<<endl;
		return node;
	}
	
	
	/*method to convert character level to integer level value*/
	int TreeMenu::getIntValue(char* value)
	{
		if(strcmp(value,"0")==0)
		{
			return 0;
		}
		else if(strcmp(value,"1")==0)
		{
			return 1;
		}
		else if(strcmp(value,"2")==0)
		{
			return 2;
		}
		else if(strcmp(value,"3")==0)
		{
			return 3;
		}
		else if(strcmp(value,"4")==0)
		{
			return 4;
		}
		else if(strcmp(value,"5")==0)
		{
			return 5;
		}

		return -1;
	}

	/*method to populate the attributes for the tree menu*/
	TreeMenu* TreeMenu::populateAttr(TreeMenu* processNode, xml_node<>* attribNode)
	{
		attribNode = attribNode->first_node();
		while(attribNode != NULL)
		{
			//Attribute nodes declaration for Tree Menu
			attribute* attrib = new attribute();
			//find the name of the attribute
			char* attribval = attribNode->value();
			//Assign the name of attribute to attribute pointer
			attrib->attributeName = attribval;
			cout<<" attribute value is "<<attribval<<endl;
			processNode->attPtr[processNode->noOfAttributes++] = attrib;
			attribNode = attribNode->next_sibling();

		}
		return processNode;
	}

	/*Method to print the node values*/
	void TreeMenu::printValues(TreeMenu* printNode)
	{
		int noAttr = printNode->noOfAttributes;
		int noFwdPtr = printNode->noOf_FwdPtrs;
		attribute attrib = attribute();

		cout<<"\nMenu Name "<<printNode->name<<endl<<flush;
		cout<<"   Attributes "<<endl;
		//Iterate through the attributes
		for(int i=0;i<noAttr;i++)
		{
			cout<<"  "<<printNode->attPtr[i]->attributeName<<"   ";
		}
		//Iterate through the forward pointers and call print function recursively
		for(int i=0;i<noFwdPtr;i++)
		{
			printValues(printNode->forwardPtrs[i]);
		}
		return;

	}

	//Method for forwaed navigation
	static void forwardNav(TreeMenu *node)
	{

		return;
	}
	//Method for backward navigation
	static void backwardNav(TreeMenu* node)
	{

		return;
	}
	//Method for upward navigation
	static void upNav(TreeMenu* node)
	{

		return;
	}
	//Method for downward navigation
	static void downNav(TreeMenu* node)
	{
		
		return;
	}
	//method to construct the menu
	 TreeMenu* TreeMenu::makeMenu(TreeMenu* rootNode)
	{
		//Declaration for variables
		ifstream myfile;
		string fileName = PATH+"Kosmos\\conf\\menuFile.xml";
		xml_document <> xml;
		xml_node <> *tempNode;
		cout<<"inside make menu \n\n"<<flush;
		myfile.open (fileName.c_str(), ifstream::in);
		
		vector<char> documenty((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>( ));
		documenty.push_back('\0');
		
		//Populate the vector with the contents of XML file
		xml.parse<0>(&documenty[0]);
		rootNode->backwardPtr = NULL;

		//Get the node 'MENUS' from the XML file
		tempNode = xml.first_node("menus");
		bkdPtr = rootNode;
		rootNode->backwardPtr = rootNode;
		//Assign the forward pointer of root node to the sub nodes
		rootNode->forwardPtrs[rootNode->noOf_FwdPtrs++] = createMenu(tempNode->first_node());
		
		//cout<<"name of the node is "<<tempNode->name()<<endl;
		//Assign the name of the root node of tree menu to name of the node
		rootNode->name=tempNode->name();
		cout<<" name of the root node is "<<rootNode->name<<endl;
		
		//Print the values of the root node of TreeMenu
		printValues(rootNode);
		
		return rootNode;
	}
/*
	//Main Function
	int main()
{
	int value;

	cout<<"enter the value\n";
	cin>>value;

	makeMenu();
	return 1;
}
*/
