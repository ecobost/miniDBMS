//FileIO.h : The methods used to read relations from files, write back to them, and display relations at the command prompt, 
//along with some support functions
//

#ifndef FILEIO_H
#define FILEIO_H

#include <vector>

#include "relation.h"

class FileIO {
public:
	int stringToInt(const std::string &);
	std::vector<std::string> breakString(std::string); 
	relation readFromFile(std::string, int);
	void printToFile(relation);
};

#endif