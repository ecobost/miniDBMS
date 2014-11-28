// DBMS.cpp : Defines the entry point for the console application.
//
#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>

#include "DBMS.h"
#include "Token.h"
#include "FileIO.h"

using namespace std;



//the master database function - analyzes each string of tokens and send its to the appropriate function.
void DBMS::readToken(queue<Token> sentence) {
	
	Token sample = sentence.front();
	sentence.pop();
	switch (sample.kind){
		case OPEN_CMD:
			openTable(sentence);
			break;
		case CLOSE_CMD:
			closeTable(sentence);
			break;
		case EXIT_CMD:
			exitDB();
			break;
		case WRITE_CMD:
			writeTable(sentence);
			break;
		case CREATE_CMD:
			createTable(sentence);
			break;
		case UPDATE_CMD:
			try{
			updateTable(sentence);
			}catch(...){
				return;
			}
			break;
		case INSERT_CMD:
			insertIntoTable(sentence);
			break;
		case DELETE_CMD:
			deleteFromTable(sentence);
			break;
		case QUERY:
			try {
				queryResults.push_back(query(sentence));
			}
			catch(...) {
				printf("Sorry. We had problems with the query!\n");
			}
			break;
		case SHOW_CMD:
			try{
				showTable(sentence);
			}catch(...){
				return;
			}
			break;
	}
}

//@@@@@@@@@@@@@@@@-----------------------@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@-------COMMANDS--------@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@-----------------------@@@@@@@@@@@@@@

void DBMS::createTable(queue<Token> params){
	relation newTable;
		
	//first token in string is name of table
	newTable.name = params.front().value;
	params.pop();
	//should be followed by list of columns, in parentheses
	if (params.front().kind == LEFT_PAREN)  params.pop();
	else exit(1);
	while (params.front().kind != RIGHT_PAREN)
	{
		column newColumn;
		newColumn.name = params.front().value;
		params.pop();
		if (params.front().kind == VARCHAR)
		{
			newColumn.isInt = false;
			newColumn.stringlen = stringToInt(params.front().value);
		}
		else newColumn.isInt = true;
		params.pop();
		newTable.table.push_back(newColumn);
	}
	params.pop();
	//end with primary key
	if (params.front().kind == LEFT_PAREN)  params.pop();
	else exit(1);
	while (params.front().kind != RIGHT_PAREN)
	{
		unsigned int j = 0;
		while (j < newTable.table.size() && newTable.table[j].name != params.front().value) 
			j++;
		if(j==newTable.table.size()) {
			cout<<"CreateCommmand: Primary Key does not exist in the Table declaration."<<endl;
			return;
		}
		newTable.primaryKey.push_back(params.front().value);
		params.pop();
	}
	params.pop();

	queryResults.push_back(newTable);
}
	
void DBMS::updateTable(queue<Token> params){

	relation intoTable;

	vector<relation>::iterator it;
	for (it= queryResults.begin(); it < queryResults.end(); it++){
		if (it->name == params.front().value){
			intoTable= *it;
			break;
		}
	}
	if(it==queryResults.end()){
		cout<<"Table"<<params.front().value<<" does not exist in memory."<<endl;
		return;
	}
	params.pop();

	//record the list of columns in which an entry should be renamed
	vector<string> namesToChange;
	vector<string> toChangeTo;
	string tempAttributeName;
	int columnPosition;

	while (params.front().value != "WHERE") {
		tempAttributeName=params.front().value;
		params.pop();
		//Check that the attributes actually exist
		if(! intoTable.attributeExist(tempAttributeName)){
			cout<<"Error: Attribute does not exist in the table"<<endl;
			return;
		}
		//Check the type coincide with the value they are sending
		columnPosition= intoTable.getColumnPosition(tempAttributeName);
		if(intoTable.table[columnPosition].isInt){
			if(params.front().kind==STRING){
				cout<<"Error: Types in the update  don't match with the table"<<endl;
				return;
			}
		}
		else{
			if(params.front().kind==INTEGER){
				cout<<"Error: Types in the update  don't match with the table"<<endl;
				return;
			}
				
		}

		namesToChange.push_back(tempAttributeName);

		toChangeTo.push_back(params.front().value);
		params.pop();
	}
	params.pop();//WHERE
		
	//test the condition for each row, and update if necessary.
	vector<entry> new_row;
	vector<entry> old_row;
	queue<Token> params_copy;
	for(unsigned int i = 0; i < intoTable.numRows(); i++) {		//Over all rows	
		params_copy = params;
		if ( condition(params_copy, intoTable.getRow(i), intoTable ) ){
			old_row= intoTable.getRow(i);
				
			//Updates the table
			for (unsigned int j = 0; j < namesToChange.size(); j++){	//Over all columns to be changed
				intoTable.columnNamed(namesToChange[j]).entryAt(i).updateEntry(toChangeTo[j]);
			}

			new_row= intoTable.getRow(i);
			intoTable.clearRow(i);

			//Check primary key holds
			if(intoTable.checkPrimaryKeys(new_row)){  //Put the new row back.
				intoTable.insertRow(new_row , i);	
			}
			else{	//Put the old row back
				intoTable.insertRow(old_row , i);//-1 means at the end of the table
				cout<<"Error: Primary key repeated when updating row in line "<<i+1<<endl;	
			}
		}
	}
	params=params_copy;

	queryResults.erase(it);
	queryResults.push_back(intoTable);
	cout<<"Update Table Succesful!"<<endl;
}
	
void DBMS::insertIntoTable(queue<Token> params){
	relation intoTable;
		
	//first token in string is name of table
	vector<relation>::iterator it;
	for (it= queryResults.begin(); it < queryResults.end(); it++){
		if (it->name == params.front().value){
			intoTable= *it;
			break;
		}
	}
	if(it==queryResults.end()){
		cout<<"Error: Table "<<params.front().value<<" does not exist in memory."<<endl;
		return;
	}
	params.pop();

	//will be followed by either list of entries, or other table marked by "RELATION"
	if (params.front().value == "VALUES FROM RELATION")
	{
		params.pop();

		relation queriedRelation = expr(params);
		//Check number of attributes
		if (queriedRelation.table.size() != intoTable.table.size()){
			cout<<"Error: Number of attributes in the insert  don't match with the table"<<endl;
			return;
		}
			
		//Check types
		for(int i=0; i < intoTable.table.size(); i++){
			if(intoTable.table[i].isInt != queriedRelation.table[i].isInt){
				cout<<"Error: Types in the insert  don't match with the table"<<endl;
				return;
			}
		}
			
		//Check that the primary key is not repeated.
		vector<entry> row_to_insert;
		for (unsigned int i = 0; i < queriedRelation.numRows(); i++) {//Over all rows
			row_to_insert= queriedRelation.getRow(i);
			if( intoTable.checkVarcharLength(row_to_insert) ){
				if(intoTable.checkPrimaryKeys(queriedRelation.getRow(i)))
					intoTable.copyRow(queriedRelation, i);
				else 
					cout<<"Error: Primary key repeated	in line "<<i+1<<endl;
			}
			else 
				cout<<"Error: Varchar length surpassed in line "<<i+1<<endl;
		}
	}
	else {

		//parse list of entries and add as new row in table
		if(params.front().kind == LEFT_PAREN)  
			params.pop();
		else	
			return;	

		//---Checks
		vector<entry> row;
		for(int i=0; i < intoTable.table.size(); i++){
				
			if(params.front().kind == RIGHT_PAREN){
				cout<<"Error: Too few arguments"<<endl;
				return;
			}

			entry newEntry;
			if (params.front().kind == INTEGER) {
				newEntry.isInt = true;
				newEntry.numeric = stringToInt(params.front().value);
			}
			else {
				newEntry.isInt = false;
				newEntry.alpha = params.front().value;
			}

			//Check the type
			if(intoTable.table[i].isInt != newEntry.isInt){
				cout<<"Error: Types in the insert  don't match with the table at argument: "<<i+1<<endl;
				return;
			}
				
			//Collect the values of the row
			row.push_back(newEntry);

			params.pop();
		}
		if(params.front().kind != RIGHT_PAREN){
			cout<<"Error: Too many arguments"<<endl;
			return;
		}
		params.pop();//RIGHT_PAREN

		//Check the Varchar Length
		if( ! intoTable.checkVarcharLength(row) ){
			cout<<"Error: Varchar length surpassed"<<endl;
			return;
		}


		//check the primary key with the vector you collected
		if(intoTable.checkPrimaryKeys(row)){
			intoTable.insertRow(row,-1);
		}
		else{
			cout<<"Error: Primary key repeated"<<endl;
			return;
		}
	}

	queryResults.erase(it);
	queryResults.push_back(intoTable);

	cout<<"Insert Table Succesful!"<<endl;
}
	
void DBMS::deleteFromTable(queue<Token> params){
	relation intoTable;

	vector<relation>::iterator it;
	for (it= queryResults.begin(); it < queryResults.end(); it++){
		if (it->name == params.front().value){
			intoTable= *it;
			break;
		}
	}
	if(it==queryResults.end()){
		cout<<"Table"<<params.front().value<<" does not exist in memory."<<endl;
		return;
	}
	params.pop();
		
	//pop the WHERE parameter, and pull the condition out of parentheses
	params.pop();
	if (params.front().kind == LEFT_PAREN) {
		params.pop();
		queue<Token> whereCond;
		while (params.front().kind != RIGHT_PAREN) {
			whereCond.push(params.front());
			params.pop();
		}
		params = whereCond;

	}
	//test the condition for each row, and delete if necessary.
	queue<Token> params_copy;
	for(unsigned int i = 0; i < intoTable.numRows(); i++) {
		params_copy=params;
		if ( condition( params_copy, intoTable.getRow(i), intoTable ) ) {
			intoTable.clearRow(i);
			i--;
		}
	}
	params=params_copy;

	queryResults.erase(it);
	queryResults.push_back(intoTable);
	printf("Delete_table called\n");
}

void DBMS::openTable(queue<Token> params){
	FileIO fileManager;
	string table_name;
	relation intoTable;

	//first token in string is name of table
	table_name=params.front().value;
	params.pop();
	try{
		intoTable = fileManager.readFromFile(table_name, 0);
	}catch(...){
		cout<<"Error: Table "<<table_name<<" could not be found"<<endl;
		return;
	}
	
	//Table already in memory, delete it from queryResults and 6then push back the new one
	vector<relation>::iterator it;
	for (it= queryResults.begin(); it < queryResults.end(); it++){
		if (it->name == table_name){
			queryResults.erase(it);
			break;
		}
	}
		
	queryResults.push_back(intoTable);
}

void DBMS::closeTable(queue<Token> params){
	FileIO fileManager;
	vector<relation>::iterator it;
	for (it= queryResults.begin(); it < queryResults.end(); it++){
		if (it->name == params.front().value){
			fileManager.printToFile(*it);
			queryResults.erase(it);
			return;
		}
	}
	if(it==queryResults.end()){//We don't need this 
		cout<<"Table "<<params.front().value<<" is not charged in memory."<<endl;
		return;
	}
	params.pop();
}

void DBMS::writeTable(queue<Token> params){
	FileIO fileManager;
	vector<relation>::iterator it;
	for (it= queryResults.begin(); it < queryResults.end(); it++){
		if (it->name == params.front().value){
			fileManager.printToFile(*it);
			break;
		}
	}
	if(it==queryResults.end()){
		cout<<"Table "<<params.front().value<<" is not charged in memory."<<endl;
		return;
	}
	params.pop();
}

void DBMS::exitDB(){
	exit(1);
}

void DBMS::showTable(queue<Token> params){
	relation intoTable;
	intoTable = atomicExpr(params);
	intoTable.show();
}



//@@@@@@@@@@@@@@@@----------------------@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@-------QUERIES--------@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@----------------------@@@@@@@@@@@@@@

relation DBMS::query(queue<Token>& params) {
	string table_name = params.front().value;
	params.pop();//table _name

	relation queryResult;
	try{
		queryResult = expr(params);
	} catch(...){
		throw(std::out_of_range ("Problems with the query"));
	}
	queryResult.name = table_name;

	//Delete table if already in memory.
	vector<relation>::iterator it;
	for (it= queryResults.begin(); it < queryResults.end(); it++){
		if (it->name == table_name){
			queryResults.erase(it);
			break;
		}
	}

	return queryResult;
}

//this function returns the relation resulting from a query
relation DBMS::expr(queue<Token>& params){
	try{
		switch(params.front().kind) {
		case STRING:
			return atomicExpr(params);
		case LEFT_PAREN:
			return atomicExpr(params);
		case SELECTION:
			params.pop();
			return selection(params);
		case PROJECTION:
			params.pop();
			try{ 
				return projection(params);
			}catch(...) {
				throw std::out_of_range("");
			}
		case RENAMING:
			params.pop();
			return renaming(params);
		case UNION:
			params.pop();
			return setUnion(params);
		case DIFFERENCE:
			params.pop();
			return setDifference(params);
		case PRODUCT:
			params.pop();
			return crossProduct(params);
		default: throw::invalid_argument("Improper query");
		}
	} catch(...){
		throw::invalid_argument("Improper query");
	}
}

relation DBMS::atomicExpr(queue<Token>& params){
	relation tempRelation;
	vector<relation>::iterator it;
	switch(params.front().kind){
	case STRING:
		for (it= queryResults.begin(); it < queryResults.end(); it++){
			if ( it -> name == params.front().value){
				tempRelation= *it;
				break;
			}
		}
		if(it==queryResults.end()){
			cout<<"Error: Table "<<params.front().value<<" does not exist in memory."<<endl;
			throw::invalid_argument("Improper query");
		}
		params.pop();//STRING
		break;
	case LEFT_PAREN:
		params.pop();//LEFT_PAREN
		tempRelation= expr(params);
		params.pop();//RIGHT_PAREN
		break;
	}
	return tempRelation;
}



//the six queries
relation DBMS::selection(queue<Token>& params) {
	queue<Token> whereCond;
	relation selectedRelation;
	relation subjectRelation;

	params.pop();//LEFT_PAREN
	while (params.front().kind != RIGHT_PAREN){
		whereCond.push(params.front());
		params.pop();
	}
	params.pop();

	subjectRelation = atomicExpr(params);
	selectedRelation.copyEmptyColumns(subjectRelation);
	queue<Token> whereCond_copy;
	for (unsigned int i = 0; i < subjectRelation.numRows(); i++) {
		whereCond_copy=whereCond;
		if (condition( whereCond_copy, subjectRelation.getRow(i), subjectRelation))
			selectedRelation.copyRow(subjectRelation, i);
	}

	return selectedRelation;
}

relation DBMS::projection(queue<Token>& params) {
	vector<string> names;
	relation projectedRelation;
	relation subjectRelation;

	//get list of names to project
	params.pop();//LEFT_PAREN
	while (params.front().kind != RIGHT_PAREN){
		names.push_back(params.front().value);
		params.pop();
	}
	params.pop();

	subjectRelation = atomicExpr(params);
	unsigned int i;// So I can use it if there is an error
	try {
		for (i = 0; i < names.size(); i++) 		
			projectedRelation.table.push_back(subjectRelation.columnNamed(names[i]));
	}catch(...) {
		cout<<"Error: Column named "<<names[i]<<" does not exist in the table projected"<<endl;
		throw std::out_of_range("");
	}
	return projectedRelation;
}

relation DBMS::renaming(queue<Token>& params)
{
	vector<string> names;
	relation renamedRelation;

	params.pop();//LEFT_PAREN
	while (params.front().kind != RIGHT_PAREN){
		names.push_back(params.front().value);
		params.pop();
	}
	params.pop();

	renamedRelation = atomicExpr(params);
	
	try{
		for (unsigned int i = 0; i < names.size(); i++) 
			renamedRelation.table[i].renameColumn(names[i]);
	}catch(...){
		cout<<"Error: Number of attributes in the list and renamed table does not match"<<endl;
		throw std::out_of_range("");
	}

	return renamedRelation;
}

relation DBMS::setUnion(queue<Token>& params) {
	relation relation1;
	relation relation2;
	relation unionRelation;
	
	relation1 = atomicExpr(params);
	relation2 = atomicExpr(params);
	
	if (relation1.unionCompatible(relation2))
	{
		unionRelation = relation1;
		for (unsigned int i = 0; i < relation2.numRows(); i++) {
			if ( ! unionRelation.rowIsIn(relation2.getRow(i)) ) 
				unionRelation.copyRow(relation2, i);
		}
		return unionRelation;
	}
	else {
		cout<<"Error: Tables are not union compatible"<<endl;
		throw std::invalid_argument("Not union compatible");
	}
}

relation DBMS::setDifference(queue<Token>& params) {
	relation differenceRelation;
	relation relation1;
	relation relation2;

	relation1 = atomicExpr(params);
	relation2 = atomicExpr(params);

	if (relation1.unionCompatible(relation2))
	{
		differenceRelation = relation1;
		for (unsigned int i = 0; i < differenceRelation.numRows(); i++) {
			if ( relation2.rowIsIn(differenceRelation.getRow(i)) ) {
				differenceRelation.clearRow(i);
				i--;
			}
		}
		return differenceRelation;
	}
	else {
		cout<<"Error: Tables are not union compatible"<<endl;
		throw std::invalid_argument("Not union compatible");
	}
}

relation DBMS::crossProduct(queue<Token>& params) {
	relation productRelation;
	relation relation1;
	relation relation2;

	relation1 = atomicExpr(params);
	relation2 = atomicExpr(params);

	productRelation.copyEmptyColumns(relation1);
	productRelation.copyEmptyColumns(relation2);
	
	//this line of code multiplies the rows of relation1 times r2 - every possible combination of r1 and r2 rows.
	int relation1ColumnCount= relation1.table.size();
	int relation2ColumnCount= relation2.table.size();
	int productColumnCount= relation1ColumnCount+relation2ColumnCount;
	for (unsigned int i = 0; i < relation1.numRows(); i++) {
		for (unsigned int j = 0; j < relation2.numRows(); j++) {
			//Here it just copies the row ith of relation 1 + the row  jth of relation2.
			for(unsigned int k = 0; k < relation1ColumnCount; k++)
				productRelation.table[k].elements.push_back(relation1.table[k].elements[i]);
			for(unsigned int l = relation1ColumnCount; l < productColumnCount; l++)
				productRelation.table[l].elements.push_back(relation2.table[l-relation1ColumnCount].elements[j]);
		}
	}
	return productRelation;
}




//@@@@@@@@@@@@@@@@----------------------@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@------CONDITION-------@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@----------------------@@@@@@@@@@@@@@

bool DBMS::condition(queue<Token>& params, vector<entry> row, relation subject){
	bool answer;
	answer = conjunction(params,row,subject);
	if( !params.empty() && params.front().kind == DISJUNCTION ){
		params.pop();
		answer= answer || conjunction (params, row, subject);
	}
	return answer;
}

bool DBMS::conjunction(queue<Token>& params, vector<entry> row, relation subject){
	bool answer;
	answer = comparison(params,row,subject);
	if( !params.empty() && params.front().kind == CONJUNCTION ){
		params.pop();
		answer= answer && comparison(params, row,subject);
	}
	return answer;
}

bool DBMS::comparison(queue<Token>& params, vector<entry> row, relation subject){
	bool answer=false;
	if(params.empty()) return answer;
	if( params.front().kind == LEFT_PAREN){
		params.pop();
		answer = condition(params, row, subject);
		params.pop();//RIGHT_PAREN
	}
	else{
		answer = evaluateOneCondition(params, row, subject);
	}
	return answer;
}

//this utility function evaluates a 3-token comparison: operand1, operator, operand2
bool DBMS::evaluateOneCondition(queue<Token>& params, vector<entry> row, relation subject){
	entry temp_entry;
	bool int_data;
	//Take values
	Token operand1= params.front();
	params.pop();
	Token op= params.front();
	params.pop();
	Token operand2=params.front();
	params.pop();

	if(operand1.kind==OPERAND_ATTR){
		//Check that the atribute actually exist
		if(! subject.attributeExist(operand1.value)){
			cout<<"Error: Attribute "<<operand1.value<<" does not exist"<<endl;
			throw::invalid_argument("Improper query");
		}
		
		temp_entry = row[subject.getColumnPosition(operand1.value)];
		if(temp_entry.isInt){//INTEGER
			operand1.kind=INTEGER;
			operand1.value= intToString(temp_entry.numeric);
		}
		else{
			operand1.kind=STRING;
			operand1.value= temp_entry.alpha;
		}
	}

	if(operand2.kind==OPERAND_ATTR){
		//Check that the atribute actually exist
		if(! subject.attributeExist(operand2.value)){
			cout<<"Error: Attribute "<<operand2.value<<" does not exist"<<endl;
			throw::invalid_argument("Improper query");
		}
		//else
		temp_entry = row[subject.getColumnPosition(operand2.value)];
		if(temp_entry.isInt){//INTEGER
			operand2.kind=INTEGER;
			operand2.value= intToString(temp_entry.numeric);
		}
		else{
			operand2.kind=STRING;
			operand2.value= temp_entry.alpha;
		}
	}


	//Check types
	if(operand1.kind == STRING){
		if(operand2.kind==INTEGER){
			cout<<"Error: Data for the condition has different types"<<endl;
			throw::invalid_argument("Improper query");
		}
		else int_data=false;
	}else{
		if(operand2.kind==STRING){
			cout<<"Error: Data for the condition has different types"<<endl;
			throw::invalid_argument("Improper query");
		}
		else int_data=true;
	}

	if(int_data){		//INTEGER
		switch(op.kind){
			case EQUAL:
				return stringToInt(operand1.value) == stringToInt(operand2.value);
			case NOT_EQUAL:
				return stringToInt(operand1.value) != stringToInt(operand2.value);
			case LESS:
				return stringToInt(operand1.value) < stringToInt(operand2.value);
			case GREATER:
				return stringToInt(operand1.value) > stringToInt(operand2.value);
			case LESS_EQUAL:
				return stringToInt(operand1.value) <= stringToInt(operand2.value);
			case GREATER_EQUAL:
				return stringToInt(operand1.value) >= stringToInt(operand2.value);
			default:
				cout<<"Undefined operator "<<op.value<<endl;
				throw (std::invalid_argument("Undefined operator"));	
		}
	}
	else{			//STRING
		switch(op.kind){
			case EQUAL:
				if(operand1.value.compare(operand2.value) == 0) 
					return true;
				else
					return false;
			case NOT_EQUAL:
				if(operand1.value.compare(operand2.value) == 0) 
					return false;
				else
					return true;
			default: 
				cout<<"Operation "<<op.value<<" can not be used with strings"<<endl;
				throw::invalid_argument("Improper query");
		}
	}
}

	
int DBMS::stringToInt(const string &Text) {
	stringstream ss(Text);
	int result;
	return ss >> result ? result : 0;
}

string DBMS::intToString(int number){
	stringstream ss;
	ss << number;
	return ss.str();
}

int DBMS::rowCountInRelation(string relation_name){
	vector<relation>::iterator it;
	for (it= queryResults.begin(); it < queryResults.end(); it++){
		if (it->name == relation_name){
			break;
		}
	}
	if(it==queryResults.end())
		return -1;//If relation is not in memory, return is -1.
	else{
		return it->numRows();
	}
}
