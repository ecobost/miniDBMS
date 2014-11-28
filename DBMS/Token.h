#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum Token_type{
	OPEN_CMD, 
	CLOSE_CMD, 
	EXIT_CMD, 
	WRITE_CMD, 
	CREATE_CMD, 
	UPDATE_CMD, 
	INSERT_CMD, 
	DELETE_CMD, 
	SHOW_CMD, 
	QUERY, 
	SELECTION, 
	PROJECTION, 
	RENAMING, 
	UNION, 
	DIFFERENCE, 
	PRODUCT, 
	GREATER, 
	GREATER_EQUAL, 
	LESS, 
	LESS_EQUAL, 
	EQUAL, 
	NOT_EQUAL,	
	DISJUNCTION,
	CONJUNCTION, 
	OPERAND_ATTR, 
	VARCHAR, 
	INTEGER, 
	STRING, 
	LEFT_PAREN, 
	RIGHT_PAREN 
};

class Token{
public:
	Token_type kind;
	std::string value;
	
	//Constructors
	Token(){}
	Token(Token_type t):kind(t), value(" "){}
	Token(Token_type t, std::string val){
		value=val;
		kind=t;
	}
};

#endif