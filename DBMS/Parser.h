//Parser.h : enumerates the types of token and defines various functions used in parsing them.

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <queue>
#include <iostream>

#include "Token.h"

namespace Parser {

using namespace std;

string takeOutSpaces(string line);
string nextWord(string line);
string minusWord(string line);
string readStringUpToChar(string line, char delimiter);
string minusThing(string line, string thing);
string removeQuotes(string& myline,string partOfLine);
string getWordInsideQuotes(string& myline);


void getWord(queue<Token>& queueLine, string& myline, Token_type kind);
bool isNumber(string number);
void getLeftParenthesis(queue<Token>& queueLine, string& myline);
void getRightParenthesis(queue<Token>& queueLine, string& myline);
void addRightParenthesis(queue<Token>& queueLine);
void getOp(queue<Token>& queueLine, string& myline);

void getRelationName(queue<Token>& queueLine, string& myline, string partOfLine);
void getType(queue<Token>& queueLine, string& myline);
void getTypedAttributeList(queue<Token>& queueLine, string& myline);
void getAttributeList(queue<Token>& queueLine, string& myline);
void getLiteralList(queue<Token>& queueLine, string& myline);
void getUpdateAssignments(queue<Token>& queueLine, string& myline);

void getConjunction(queue<Token>&,string&);
void getComparison(queue<Token>&, string&);
void getCondition(queue<Token>& queueLine, string& myline);
void getAtomicExpr(queue<Token>& ,string&);
void getExpr(queue<Token>& queueLine, string& myline);

// commands
queue<Token> createCommand(string myline);
queue<Token> insertCommand(string myline);
queue<Token> showCommand(string myline);
queue<Token> deleteCommand(string myline);
queue<Token> updateCommand(string myline);
queue<Token> openCommand(string myline);
queue<Token> closeCommand(string myline);
queue<Token> exitCommand(string myline);
queue<Token> writetCommand(string myline);

// queries
queue<Token> query(string myline, string partOfLine);
queue<Token> tokenLine(string myline);


string takeOutSpaces(string line){
	size_t pos = line.find_first_not_of(' ');
	if(pos==-1) return "";//It only has spaces
	return line.substr(pos);
}

string nextWord(string line){ // returns next word, of string until next space
	string::size_type position;
	position=line.find(' ',0);	//reading 1st word
	string partOfLine = line.substr(0,position);	
	return partOfLine;
}

string minusWord(string line){ // removes word from line and ONE space
	string::size_type position;
	position=line.find(' ',0);
	if(position==-1)
		return "";
	line= line.substr(position+1);
	return takeOutSpaces(line);
}

string readStringUpToChar(string line, char delimiter){ // reads next group of charactes until specified character or string
	string::size_type position;
	position=line.find(delimiter,0);	
	string partOfLine = line.substr(0,position);	
	return partOfLine;
}

string minusThing(string line, string thing){ // removes next string or character from line
	string::size_type position;
	position=line.find(thing,0);
	if(position==-1)
		return "";
	line= line.substr(position+1);
	return takeOutSpaces(line);
}

string removeQuotes(string& myline,string partOfLine){ // as not need to parse with quotes removes them from strings
	if(partOfLine[0] == '"'){ // if does not start with a quote then will return same string
		partOfLine = minusThing(partOfLine, "\"");
				
		if(partOfLine[partOfLine.length()-1] != '"'){
			partOfLine = partOfLine+ " "+nextWord(myline);
			myline= minusWord(myline);
		}
		string::size_type position;
		position=partOfLine.find('"',0);	
		partOfLine= partOfLine.substr(0,position);
	}
	return partOfLine;
}

string getWordInsideQuotes(string& myline){
	string partOfLine;
	if(myline[0]=='"'){
		myline = minusThing(myline, "\"");
		partOfLine= readStringUpToChar(myline,'"');
		myline = minusThing(myline, "\"");
	}
	else{
		partOfLine=nextWord(myline);
		myline=minusWord(myline);

		if(partOfLine[partOfLine.length()-1]==';'){
			partOfLine = partOfLine.substr(0,partOfLine.length()-1);
			myline= ";" + myline;//Not really need to add the old my line in because what comes next is just the parentheses.
		}

		switch(partOfLine[partOfLine.length()-1]){//If what comes at the end of the word I take is:
			case ')':
				myline= ") " + myline;
				partOfLine = partOfLine.substr(0,partOfLine.length()-1);
				break;
			case ',':
				myline= ", " + myline;
				partOfLine = partOfLine.substr(0,partOfLine.length()-1);
				break;

		}

		string residual= minusThing( partOfLine , "," );
		if( residual.compare("")!=0){
			partOfLine= readStringUpToChar(partOfLine,',');
			myline = "," + residual + myline;
		}
	}
	return partOfLine;
}

void getWord(queue<Token>& queueLine, string& myline, Token_type kind){ //puts next word into queue
	string partOfLine=nextWord(myline); // usualy is attribute name
	myline = minusWord(myline);
	partOfLine= removeQuotes(myline, partOfLine);	
	Token nToken(kind, partOfLine);
	queueLine.push(nToken);
}

bool isNumber(string number){
	if(number.empty()) return false;
	int size =number.length();
	bool firstCharIsValid = number[0]=='-' || number[0]=='+' || (number[0]>='0' && number[0]<='9');
	if (! firstCharIsValid)	
		return false;
	for(int i=1;i<size;i++)
		if(number[i]<'0' || number[i]>'9')
			return false;
	return true;
}

void getLeftParenthesis(queue<Token>& queueLine, string& myline){ 
	myline = minusThing(myline, "(");
	Token pToken(LEFT_PAREN, "(");
	queueLine.push(pToken);;
}

void getRightParenthesis(queue<Token>& queueLine, string& myline){ 
	myline = minusThing(myline, ")");
	Token pToken(RIGHT_PAREN, ")");
	queueLine.push(pToken);;
}

void addRightParenthesis(queue<Token>& queueLine){
		Token pToken(RIGHT_PAREN,")");
		queueLine.push(pToken);
}

void getOp(queue<Token>& queueLine, string& myline){ // which opperation from condition
	string partOfLine = nextWord(myline);
	myline= minusWord(myline);
	if(partOfLine == "=="){
		Token pToken(EQUAL,"==");
		queueLine.push(pToken);
	}
	else if(partOfLine == "!="){
		Token pToken(NOT_EQUAL,"!=");
		queueLine.push(pToken);
	}
	else if(partOfLine == ">"){
		Token pToken(GREATER,">");
		queueLine.push(pToken);
	}
	else if(partOfLine == "<"){
		Token pToken(LESS,"<");
		queueLine.push(pToken);
	}
	else if(partOfLine == ">="){
		Token pToken(GREATER_EQUAL,">=");
		queueLine.push(pToken);
	}
	else if(partOfLine == "<="){
		Token pToken(LESS_EQUAL,"<=");
		queueLine.push(pToken);
	}
	else{
		//Error
		Token pToken(STRING,"!!!!!!");
		queueLine.push(pToken);
	}
}

void getRelationName(queue<Token>& queueLine, string& myline, string partOfLine){

	if(partOfLine[partOfLine.length()-1] == ';'){		// if it is the last word in line
		myline = minusWord(myline); // is name
		partOfLine = partOfLine.substr(0,partOfLine.length()-1);
		int rightCount=0;
		while(partOfLine[partOfLine.length()-1] == ')'){ // removes any extra ')'
			rightCount++;
			partOfLine = partOfLine.substr(0,partOfLine.length()-1);
		}
		Token lastToken(STRING, partOfLine);
		queueLine.push(lastToken);	
		for(int i=0; i<rightCount; i++){
			addRightParenthesis(queueLine);
		}
	}
	else{					// if is not last word in the line
		int rightCount=0;

		//See when are we using this condition of looking for  ')'
		while(partOfLine[partOfLine.length()-1] == ')'){
			rightCount++;
			partOfLine = partOfLine.substr(0,partOfLine.length()-1);
		}
		partOfLine=nextWord(myline); // relation name
		myline = minusWord(myline);
		Token mToken(STRING, partOfLine);
		queueLine.push(mToken);
		for(int i=0; i<rightCount; i++){
			addRightParenthesis(queueLine);
		}
	}	
}

void getType(queue<Token>& queueLine, string& myline){
	Token attToken;

	if(myline[0] == 'V'){
		attToken.kind=VARCHAR;
		myline = minusThing(myline, "(");
		//if (myline[0]>='0' && myline[0]<=
		string number = readStringUpToChar(myline,')');
		if(isNumber(number)==false){
			cout<<"Error in declaration of Varchar length. Assuming 80"<<endl;
			attToken.value="80";
		}
		else
			attToken.value= number;
		myline = minusThing(myline, ")");
		
	}
	else{ //Is an Int
		myline = minusThing(myline, "R");
		attToken.kind=INTEGER;
	}
	queueLine.push(attToken);
}

void getTypedAttributeList(queue<Token>& queueLine, string& myline){
	getWord(queueLine, myline, STRING);
	getType(queueLine, myline);
	while(myline[0] == ','){
		myline= minusThing(myline, ",");
		getWord(queueLine, myline, STRING);
		getType(queueLine, myline);
	}
}

void getAttributeList(queue<Token>& queueLine, string& myline){
	string partOfLine;

	partOfLine=getWordInsideQuotes(myline);
	Token attToken(STRING, partOfLine);
	queueLine.push(attToken);

	while(myline[0] == ','){
		myline= minusThing(myline, ",");

		partOfLine=getWordInsideQuotes(myline);
		Token attToken(STRING, partOfLine);
		queueLine.push(attToken);
	}
}

void getLiteralList(queue<Token>& queueLine, string& myline){
	string partOfLine;
	Token attToken;
	bool isString;

	isString= (myline[0]=='"');

	partOfLine = getWordInsideQuotes(myline);
	attToken.value=partOfLine;
	if(!isString && isNumber(partOfLine))
		attToken.kind=INTEGER;			
	else
		attToken.kind=STRING;
	queueLine.push(attToken);

	while(myline[0] == ','){
		myline= minusThing(myline, ",");

		isString= (myline[0]=='"');

		partOfLine = getWordInsideQuotes(myline);
		attToken.value=partOfLine;
		if(!isString && isNumber(partOfLine))
			attToken.kind=INTEGER;			
		else
			attToken.kind=STRING;

		queueLine.push(attToken);
	}
}

void getUpdateAssignments(queue<Token>& queueLine, string& myline){
	Token newToken;
	string partOfLine;
	bool isString;
	string residual;

	//Atribute name
	partOfLine=nextWord(myline);
	myline=minusWord(myline);
	

	residual= minusThing( partOfLine , "=" );
	if( residual.compare("")!=0){
		partOfLine= readStringUpToChar(partOfLine,'=');
		myline = "=" + residual +" "+ myline;
	}
	else if(partOfLine[partOfLine.length()-1]=='='){
		myline= "= " + myline;
		partOfLine = partOfLine.substr(0,partOfLine.length()-1);
	}

	newToken.kind=STRING;
	newToken.value=partOfLine;
	queueLine.push(newToken);
	
	//Take out the equals
	myline=minusThing(myline, "=");

	//Literal
	isString= (myline[0]=='"');
	partOfLine = getWordInsideQuotes(myline);
	newToken.value=partOfLine;
	if(!isString && isNumber(partOfLine))
		newToken.kind=INTEGER;			
	else
		newToken.kind=STRING;
	queueLine.push(newToken);

	
	
	while(myline[0] == ','){
		myline= minusThing(myline, ",");

		//Atribute name
		partOfLine=nextWord(myline);
		myline=minusWord(myline);

		residual= minusThing( partOfLine , "=" );
		if( residual.compare("")!=0){
			partOfLine= readStringUpToChar(partOfLine,'=');
			myline = "=" + residual +" "+ myline;
		}
		else if(partOfLine[partOfLine.length()-1]=='='){
			myline= "= " + myline;
			partOfLine = partOfLine.substr(0,partOfLine.length()-1);
		}

		newToken.kind=STRING;
		newToken.value=partOfLine;
		queueLine.push(newToken);
	
		//Take out the equals
		myline=minusThing(myline, "=");

		//Literal
		isString= (myline[0]=='"');
		partOfLine = getWordInsideQuotes(myline);
		newToken.value=partOfLine;
		if(!isString && isNumber(partOfLine))
			newToken.kind=INTEGER;			
		else
			newToken.kind=STRING;
		queueLine.push(newToken);
	}
}

void getCondition(queue<Token>& queueLine, string& myline){
	getConjunction(queueLine, myline);
	if(myline[0]=='|'){
			
		//put the OR sign in
		Token new_token(DISJUNCTION, "||");
		queueLine.push(new_token);
		myline= minusThing(myline, "|");
		myline= minusThing(myline, "|");
			
		getConjunction(queueLine, myline);
	}
}

void getConjunction(queue<Token>& queueLine, string& myline){
	getComparison(queueLine, myline);
	if(myline[0]=='&'){
		//put the sign && in the queue
		Token new_token(CONJUNCTION, "&&");
		queueLine.push(new_token);
		myline= minusThing(myline, "&");
		myline= minusThing(myline, "&");

		getComparison(queueLine, myline);
	}
}

void getComparison(queue<Token>& queueLine, string& myline){
	if(myline[0]=='('){
		getLeftParenthesis(queueLine ,  myline);
		getCondition(queueLine, myline );
		getRightParenthesis(queueLine,  myline);
	}
	else{
		Token new_token;
		string partOfLine;
		bool isString;

		//getFirstOperand
		isString = (myline[0]=='"');
		partOfLine = getWordInsideQuotes(myline);
		new_token.value=partOfLine;
		if(!isString){
			if(isNumber(partOfLine))
				new_token.kind=INTEGER;
			else
				new_token.kind=OPERAND_ATTR;
		}
		else
			new_token.kind=STRING;
		queueLine.push(new_token);

		getOp(queueLine, myline);

		//get Second Operand
		isString = (myline[0]=='"');
		partOfLine = getWordInsideQuotes(myline);
		new_token.value=partOfLine;
		if(!isString){
			if(isNumber(partOfLine))
				new_token.kind=INTEGER;
			else
				new_token.kind=OPERAND_ATTR;
		}
		else
			new_token.kind=STRING;
		queueLine.push(new_token);

	}
}

void getExpr(queue<Token>& queueLine, string& myline){
	string partOfLine;
	partOfLine=nextWord(myline); // expression
	
	if(partOfLine == "select"){
		myline = minusWord(myline);
		Token prToken(SELECTION, partOfLine);
		queueLine.push(prToken);
		getLeftParenthesis(queueLine, myline);
		getCondition(queueLine, myline);
		getRightParenthesis(queueLine, myline);
		getAtomicExpr(queueLine, myline);
	}
	else if(partOfLine == "project"){
		myline = minusWord(myline);
		Token prToken(PROJECTION, partOfLine);
		queueLine.push(prToken);
		getLeftParenthesis(queueLine, myline);
		getAttributeList(queueLine, myline);
		getRightParenthesis(queueLine, myline);
		getAtomicExpr(queueLine, myline);
	}
	else if(partOfLine == "rename"){
		myline = minusWord(myline);
		Token prToken(RENAMING, partOfLine);
		queueLine.push(prToken);
		getLeftParenthesis(queueLine, myline);
		getAttributeList(queueLine, myline);
		getRightParenthesis(queueLine, myline);
		getAtomicExpr(queueLine, myline);
	}
	else{//ATOMIC EXPR {[+|-|*] ATOMIC EXPR}
		Token attToken;
			
		queue<Token> temp;
		getAtomicExpr(temp, myline);//Put first the operation nand then theis response.

		if( !myline.empty() ){
			switch(myline[0]){
			case '+':
				attToken.value="+";
				attToken.kind=UNION;
				queueLine.push(attToken);
				break;
			case '*':
				attToken.value="*";
				attToken.kind=PRODUCT;
				queueLine.push(attToken);
				break;
			case '-':
				attToken.value="-";
				attToken.kind=DIFFERENCE;
				queueLine.push(attToken);
				break;		
			}
		}

		//append temp to queueLine
		int tempsize=temp.size();//temp has var length so this line is needed.
		for(int i=0; i< tempsize ; i++){
			queueLine.push(temp.front());
			temp.pop();
		}

		if(! myline.empty()){
			switch(myline[0]){
			case '+':
				myline= minusThing(myline,"+");
				getAtomicExpr(queueLine, myline);
				break;
			case '*':
				myline= minusThing(myline,"*");
				getAtomicExpr(queueLine, myline);
				break;
			case '-':
				myline= minusThing(myline,"-");
				getAtomicExpr(queueLine, myline);
				break;		
			}
		}
	}
}

void getAtomicExpr(queue<Token>& queueLine,  string& myline){
	if(myline[0]=='('){
		getLeftParenthesis(queueLine, myline);
		getExpr(queueLine, myline);
		getRightParenthesis(queueLine, myline);
	}
	else{//relation_name
		Token attToken(STRING, getWordInsideQuotes(myline) );
		queueLine.push(attToken);
	}
}

queue<Token> createCommand(string myline){
	queue<Token> queueLine; 

	if(nextWord(myline) == "TABLE"){
		myline = minusWord(myline);

		Token myToken(CREATE_CMD, "CREATE TABLE");	
		queueLine.push(myToken);

		getRelationName(queueLine, myline, "CREATE TABLE");
		getLeftParenthesis(queueLine, myline);
		getTypedAttributeList(queueLine, myline);
		getRightParenthesis(queueLine, myline);

		myline = minusWord(myline);//PRIMARY						// reading primary key
		myline = minusWord(myline);//KEY
		
		getLeftParenthesis(queueLine, myline);
		getAttributeList(queueLine, myline);
		getRightParenthesis(queueLine, myline);
	}
	else
		cout<< "CreateCommand(): Not a command\n";
	return queueLine;
}

queue<Token> insertCommand(string myline){
	queue<Token> queueLine; 
	string partOfLine;

	if(nextWord(myline) == "INTO"){
		myline= minusWord(myline);//INTO

		Token myToken(INSERT_CMD, "INSERT INTO");
		queueLine.push(myToken);
	

		getRelationName(queueLine, myline, "INSERT INTO");

		myline = minusWord(myline);//VALUES						
		myline = minusWord(myline);//FROM

		if(nextWord(myline) == "RELATION"){
			myline = minusWord(myline);
			Token vfToken(STRING, "VALUES FROM RELATION");
			queueLine.push(vfToken);
			getExpr(queueLine, myline);
		}
		else{
			getLeftParenthesis(queueLine, myline);
			getLiteralList(queueLine, myline); //This shouldn't be getAttributeList, because they are literal
			getRightParenthesis(queueLine, myline);
		}
	}
	else
		cout<< "Error: Not a command\n"<<endl;
	return queueLine;
}	

queue<Token> updateCommand(string myline){
	queue<Token> queueLine; 

	Token attToken(UPDATE_CMD, "UPDATE"); 
	queueLine.push(attToken);

	getWord(queueLine, myline, STRING);// relation-name

	myline=minusWord(myline); //SET

	getUpdateAssignments(queueLine, myline);

	getWord(queueLine, myline, STRING); // WHERE

	getCondition(queueLine, myline);

	return queueLine;
}	

queue<Token> deleteCommand(string myline){
	queue<Token> queueLine; 
	string partOfLine = nextWord(myline);
	myline = minusWord(myline);
		
	if(partOfLine == "FROM"){ 
		Token myToken(DELETE_CMD, "DELETE FROM");	
		queueLine.push(myToken);
		partOfLine = nextWord(myline);

		getRelationName(queueLine, myline, partOfLine);
	
		getWord(queueLine, myline, STRING);  
		getCondition(queueLine, myline);
	}

	return queueLine;	
}	

queue<Token> showCommand(string myline){
	queue<Token> queueLine;
	string partOfLine;
	Token myToken(SHOW_CMD, "SHOW"); 
	queueLine.push(myToken);

	getAtomicExpr(queueLine, myline);

	return queueLine;
}	

queue<Token> openCommand(string myline){
	queue<Token> queueLine;
	string partOfLine;
	Token myToken(OPEN_CMD, "OPEN"); 
	queueLine.push(myToken);
	partOfLine = nextWord(myline); // should be rest of line
	if(partOfLine[partOfLine.length()-1] != ';'){
		cout << "Error in OPEN command. You need a delimiter(;)\n";
		exit(1);
	}
	getRelationName(queueLine, myline, partOfLine);
	return queueLine;
}

queue<Token> closeCommand(string myline){
	queue<Token> queueLine;
	string partOfLine;
	Token myToken(CLOSE_CMD, "CLOSE"); 
	queueLine.push(myToken);
	partOfLine = nextWord(myline); // should be rest of line
	if(partOfLine[partOfLine.length()-1] != ';'){
		cout << "Error in CLOSE command. You need a delimiter(;)\n";
		exit(1);
	}
	getRelationName(queueLine, myline, partOfLine);
	return queueLine;
	
}

queue<Token> writeCommand(string myline){
	queue<Token> queueLine;
	string partOfLine;
	Token myToken(WRITE_CMD, "WRITE"); 
	queueLine.push(myToken);
	partOfLine = nextWord(myline); // should be rest of line
	if(partOfLine[partOfLine.length()-1] != ';'){
		cout << "Error in WRITE command. You need a delimiter(;)\n";
		exit(1);
	}
	getRelationName(queueLine, myline, partOfLine);
	return queueLine;
}

queue<Token> exitCommand(string myline){
	queue<Token> queueLine;  
	Token myToken(EXIT_CMD, "EXIT"); // Clear
	queueLine.push(myToken);
	return queueLine;
}



////////////////////////////////////////////////////////////////////////////
queue<Token> query(string myline, string partOfLine){
		queue<Token> queueLine; 
		Token queryToken(QUERY, "Query"); 
		queueLine.push(queryToken);
		Token myToken(STRING, partOfLine); // relation-name
		queueLine.push(myToken);
		partOfLine = nextWord(myline);
		if(partOfLine != "<-"){
			cout<<"Error in query command\n";
			exit(1);
		}
		myline = minusWord(myline);//<-

		getExpr(queueLine, myline);

		return queueLine;
}

queue<Token> tokenLine(string myline){
	queue<Token> queueLine;
	cout<< myline << "\n"; 
	string partOfLine=nextWord(myline);
	//Check for parentheses in the beginning
	myline = minusWord(myline);
	if(partOfLine == "CREATE"){
		queueLine=createCommand(myline);	
	}
	else if(partOfLine =="INSERT"){
		queueLine=insertCommand(myline);
	}
	else if(partOfLine== "UPDATE"){
		queueLine=updateCommand(myline);
	}
	else if(partOfLine == "DELETE"){
		queueLine=deleteCommand(myline);
	}
	else if(partOfLine == "SHOW"){
		queueLine=showCommand(myline);
	}
	else if(partOfLine == "OPEN"){
		queueLine = openCommand(myline);
	}
	else if(partOfLine == "CLOSE"){
		queueLine = closeCommand(myline);
	}
	else if(partOfLine == "WRITE"){
		queueLine = writeCommand(myline);
	}
	else if(partOfLine == "EXIT;"){
		queueLine = exitCommand(myline);
	}
	else{
		queueLine=query(myline, partOfLine);
	}

	return queueLine;
}	// end namespace Parser

}
#endif