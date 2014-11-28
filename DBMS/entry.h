//entry.h : the base unit of our relation data structure, each holds a string or integer value
//

#ifndef ENTRY_H
#define ENTRY_H

#include <string>

class entry{
public:
	std::string alpha;
	bool isInt;
	int numeric;

	entry(){}
	entry(std::string s, bool i):alpha(s),isInt(i){}
	entry(int s, bool i):numeric(s),isInt(i){}

	
	bool entry::operator==(entry);
	void updateEntry(std::string);
	void updateEntry(int);
	int stringToInt(const std::string &); 
};

#endif