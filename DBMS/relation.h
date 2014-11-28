//relation.h : a vector of columns, our primary data structure
//

#ifndef RELATION_H
#define RELATION_H

#include <iostream>

#include "column.h"
#include "entry.h"

class relation{
public:
	std::string name;
	std::vector<column> table;
	std::vector<std::string> primaryKey;

	bool checkPrimaryKeys(std::vector<entry>);
	column &columnNamed(std::string);
	unsigned int numRows();
	void clearRow(int);
	void copyRow(relation, int);
	bool unionCompatible(relation);
	bool rowIsIn(std::vector<entry>);
	void copyEmptyColumns(relation);
	void copyEmptyColumns(relation, std::vector<std::string>);
	int atRow(entry, std::string);
	std::string intToString(int);
	void show();
	std::vector<entry> getRow(int);
	int getColumnPosition(std::string);
	bool attributeExist(std::string);
	void insertRow(std::vector<entry>, int);
	bool checkVarcharLength(std::vector<entry>);
};

#endif