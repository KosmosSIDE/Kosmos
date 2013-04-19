#ifndef VIRTUALHANDBLOCK_H
#define VIRTUALHANDBLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include "main.h"
#include "ProjectManager.h"

class VirtualHandBlock
{
	public:
		static void insertBlock(std::vector<std::string> args);
		static void replaceNodesByAttribute(rapidxml::xml_node<> *node, char* blockName, bool isRightHand);
	
	private:		
		static rapidxml::xml_document<> vhandblockdoc;
		static std::vector<char> vhandblockfile;
		static void addVhandToProfile(bool isRightHand);
};

#endif
