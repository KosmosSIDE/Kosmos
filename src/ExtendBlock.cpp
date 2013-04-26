#include "ExtendBlock.h"


using namespace std;

rapidxml::xml_document<> ExtendBlock::extendblockdoc;
vector<char> ExtendBlock::extendblockfile;

/*
//Example code to call insertBlock
vector<string> handy;
handy.push_back("right");
ExtendBlock::insertBlock(handy);
*/
void ExtendBlock::insertBlock(vector<string> args)
{
	string extendfilepath = PATH+"Kosmos\\conf\\extendblock.xml";
	ifstream myfile(extendfilepath.c_str());
	vector<char> documenty((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>( ));
	documenty.push_back('\0');
	
	extendblockfile = documenty;
	extendblockdoc.parse<0>(&extendblockfile[0]);

	if (strcmp(args[0].c_str(),"right") == 0)
	{
		//get the name of the current one
		string currName = codeTree.first_node()->first_node("profile")->first_node()->first_node("rightHand")->first_node("block")->first_node("name")->value();
		//replace all nodes where attr parent = current name
		vector<char> writable(currName.size() + 1);
		copy(currName.begin(), currName.end(), writable.begin());
		
		replaceNodesByAttribute(codeTree.first_node(), (char *)(&writable[0]), true);
		addExtendToProfile(true);
	}
	else if (strcmp(args[0].c_str(),"left") == 0)
	{
		//get the name of the current one
		string currName = codeTree.first_node()->first_node("profile")->first_node()->first_node("leftHand")->first_node("block")->first_node("name")->value();
		//replace all nodes where attr parent = current name
		vector<char> writable(currName.size() + 1);
		copy(currName.begin(), currName.end(), writable.begin());
		
		replaceNodesByAttribute(codeTree.first_node(), (char *)(&writable[0]), false);
		addExtendToProfile(false);
	}
	else
	{
		cout << "\n\nERROR: " << args[0] << " is not a valid hand\nshould be left or right\n";
	}
}

void ExtendBlock::addExtendToProfile(bool isRightHand)
{
	//cout << "adding extend to profile\n" << flush;
	
	rapidxml::xml_node<>* handNode = (isRightHand?codeTree.first_node()->first_node("profile")->first_node()->first_node("rightHand"):codeTree.first_node()->first_node("profile")->first_node()->first_node("leftHand"));
	handNode->first_node("block")->first_node("name")->value(codeTree.allocate_string(isRightHand?"rextend\0":"lextend\0"));
	handNode->first_node("block")->first_node("type")->value(codeTree.allocate_string("extend"));
}

void ExtendBlock::replaceNodesByAttribute(rapidxml::xml_node<> *node, char* blockName, bool isRightHand)
{
	if (node->first_attribute() != 0)
	{
		rapidxml::xml_attribute<> *attr = node->first_attribute("parent");
		if (strcmp(blockName, attr->value()) == 0)
		{
			rapidxml::xml_attribute<> *type = node->first_attribute("type");
			if (type != 0)
			{			
				string parentToLookFor = (isRightHand?"rextend":"lextend");
				rapidxml::xml_node<>* a = extendblockdoc.first_node();
				while ((strcmp(a->first_attribute("type")->value(), type->value()) != 0) || (strcmp(parentToLookFor.c_str(), a->first_attribute("parent")->value()) != 0))
				{
					a = a->next_sibling();
				}
				//rapidxml::xml_node<> *child = codeTree.clone_node( a, node );
				codeTree.clone_node( a, node );
			}
		}
	}
	
	if (node->first_node() != 0)
	{
		replaceNodesByAttribute(node->first_node(), blockName, isRightHand);
	}
	if (node->next_sibling() != 0)
	{
		replaceNodesByAttribute(node->next_sibling(), blockName, isRightHand);
	}
}

