#include "VirtualHandBlock.h"


using namespace std;

rapidxml::xml_document<> VirtualHandBlock::vhandblockdoc;
vector<char> VirtualHandBlock::vhandblockfile;

/*
//Example code to call insertBlock
vector<string> handy;
handy.push_back("right");
VirtualHandBlock::insertBlock(handy);
*/
void VirtualHandBlock::insertBlock(vector<string> args)
{
	ifstream myfile("C:\\aszgard5\\szg\\projects\\Kosmos\\conf\\virtualhandblock.xml");
	vector<char> documenty((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>( ));
	documenty.push_back('\0');
	
	vhandblockfile = documenty;
	vhandblockdoc.parse<0>(&vhandblockfile[0]);

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


void VirtualHandBlock::replaceNodesByAttribute(rapidxml::xml_node<> *node, char* blockName)
{
	if (node->first_attribute() != 0)
	{
		rapidxml::xml_attribute<> *attr = node->first_attribute("parent");
		if (strcmp(blockName, attr->value()) == 0)
		{
			rapidxml::xml_attribute<> *type = node->first_attribute("type");
			if (type != 0)
			{			
				rapidxml::xml_node<>* a = vhandblockdoc.first_node();
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

