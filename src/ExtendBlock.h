#ifndef EXTENDBLOCK_H
#define EXTENDBLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include "main.h"
#include "ProjectManager.h"

using namespace std;

class ExtendBlock
{
	public:
		static void insertBlock(vector<string> args);
		static void replaceNodesByAttribute(rapidxml::xml_node<> *node, char* blockName, bool isRightHand);
	
	private:		
		static rapidxml::xml_document<> extendblockdoc;
		static vector<char> extendblockfile;
};

#endif
