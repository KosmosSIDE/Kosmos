/*
Summary: this function is used to call functions by string name

Author: Aaron Hardin

*/

#include "CallFunction.h"

///add a function callable by string funName
/// then funPtr is a pointer reference to a function that can be called
/// note: function must take a vector<string> as its only argument
void CallFunction::add( const string& funName, void(*funPtr)(vector<string>) )
{
	data.insert(std::pair<std::string, void(*)(vector<string>)>(funName,funPtr) );
}

///calls funName, which is a function that has previously been added
/// and passes args, a vector<string> as the argument to the function
void CallFunction::call( const string& funName, vector<string> args )
{
	data[funName](args);
}
