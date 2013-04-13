#include "SquadBlock.h"


using namespace std;

rapidxml::xml_document<> SquadBlock::squadblockdoc;
vector<char> SquadBlock::squadblockfile;

/*
//Example code to call insertBlock
vector<string> handy;
handy.push_back("right");
SquadBlock::insertBlock(handy);
*/
void SquadBlock::insertBlock(vector<string> args)
{
	string squadfilepath = PATH+"Kosmos\\conf\\squadblock.xml";
	ifstream myfile(squadfilepath.c_str());
	vector<char> documenty((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>( ));
	documenty.push_back('\0');
	
	squadblockfile = documenty;
	squadblockdoc.parse<0>(&squadblockfile[0]);

	if (strcmp(args[0].c_str(),"right") == 0)
	{
		//get the name of the current one
		string currName = codeTree.first_node()->first_node("profile")->first_node()->first_node("rightHand")->first_node("block")->first_node("name")->value();
		//replace all nodes where attr parent = current name
		vector<char> writable(currName.size() + 1);
		copy(currName.begin(), currName.end(), writable.begin());
		
		replaceNodesByAttribute(codeTree.first_node(), (char *)(&writable[0]));
	}
	else if (strcmp(args[0].c_str(),"left") == 0)
	{
		//get the name of the current one
		string currName = codeTree.first_node()->first_node("profile")->first_node()->first_node("leftHand")->first_node("block")->first_node("name")->value();
		//replace all nodes where attr parent = current name
		vector<char> writable(currName.size() + 1);
		copy(currName.begin(), currName.end(), writable.begin());
		
		replaceNodesByAttribute(codeTree.first_node(), (char *)(&writable[0]));
	}
	else
	{
		cout << "\n\nERROR: " << args[0] << " is not a valid hand\nshould be left or right\n";
	}
}


void SquadBlock::replaceNodesByAttribute(rapidxml::xml_node<> *node, char* blockName)
{
	if (node->first_attribute() != 0)
	{
		rapidxml::xml_attribute<> *attr = node->first_attribute("parent");
		if (strcmp(blockName, attr->value()) == 0)
		{
			rapidxml::xml_attribute<> *type = node->first_attribute("type");
			if (type != 0)
			{			
				rapidxml::xml_node<>* a = squadblockdoc.first_node();
				while ((strcmp(a->first_attribute("type")->value(), type->value()) != 0) && (strcmp(blockName, a->first_attribute("parent")->value()) != 0))
				{
					a = a->next_sibling();
				}
				rapidxml::xml_node<> *child = codeTree.clone_node( a, node );
			}
		}
	}
	
	if (node->first_node() != 0)
	{
		replaceNodesByAttribute(node->first_node(), blockName);
	}
	if (node->next_sibling() != 0)
	{
		replaceNodesByAttribute(node->next_sibling(), blockName);
	}
}

