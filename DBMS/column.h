//column.h : a vector consisting of entries and its associated properties
//
#ifndef COLUMN_H
#define COLUMN_H

#include <vector>
#include <string>

#include "entry.h"

class column{
public:
	bool isInt;
	int stringlen;
	std::string name;
	std::vector<entry> elements;
	
	column(){}
	column(std::string n,bool b):name(n),isInt(b){}
	column(std::string n, bool b, int i):name(n),isInt(b),stringlen(i){}


	entry &entryAt(int);
	int entryPosition(entry);
	int entryPosition(int);
	int entryPosition(char*); 
	void renameColumn(std::string);
	void clearEntry(int);
	bool isEmpty();
	bool keyCol(std::vector<std::string>);
	void column::insertEntry(entry, int);
};

#endif
