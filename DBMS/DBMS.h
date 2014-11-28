#ifndef DBMS_H
#define DBMS_H

#include <queue>
#include <vector>
#include <string>

#include "relation.h"
#include "Token.h"

class DBMS{
//the master database function - analyzes each string of tokens and send its to the appropriate function.
public:
	void readToken(std::queue<Token>);
	int rowCountInRelation(std::string);

private:

	std::vector<relation> queryResults;
	//Commands
	void createTable(std::queue<Token>);	
	void updateTable(std::queue<Token>);	
	void insertIntoTable(std::queue<Token>);
	void deleteFromTable(std::queue<Token>);
	void openTable(std::queue<Token>);
	void closeTable(std::queue<Token>);
	void writeTable(std::queue<Token>);
	void exitDB();
	void showTable(std::queue<Token>);

	//Query
	relation query(std::queue<Token>&);
	relation atomicExpr(std::queue<Token>&);
	relation expr(std::queue<Token>&);
	
	//the 6 queries
	relation selection(std::queue<Token>&);
	relation projection(std::queue<Token>&);
	relation renaming(std::queue<Token>&);
	relation setUnion(std::queue<Token>&);
	relation setDifference(std::queue<Token>&);
	relation crossProduct(std::queue<Token>&);

	//Evaluate a condition.
	bool condition(std::queue<Token>&, std::vector<entry>, relation);
	bool conjunction(std::queue<Token>&, std::vector<entry>, relation);
	bool comparison(std::queue<Token>&, std::vector<entry>, relation);
	bool evaluateOneCondition(std::queue<Token>&, std::vector<entry>, relation);

	//this utility function converts strings to integers (needed in creating and reading tables)
	int stringToInt(const std::string &);
	std::string intToString(int);
};

#endif