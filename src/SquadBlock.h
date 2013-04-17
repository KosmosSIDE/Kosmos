#ifndef SQUADBLOCK_H
#define SQUADBLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include "main.h"
#include "ProjectManager.h"

using namespace std;

class SquadBlock
{
	public:
		static void insertBlock(vector<string> args);
		static void replaceNodesByAttribute(rapidxml::xml_node<> *node, char* blockName, bool isRightHand);
	
	private:		
		static rapidxml::xml_document<> squadblockdoc;
		static vector<char> squadblockfile;
};

#endif
