#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

#include "relation.h"

using namespace std;

// returns true if the primary key (however many may be in the table) does not exist in the table 
bool relation::checkPrimaryKeys(vector<entry> row){
	bool answer;
	int columnPosition;
	int primaryKeyCount=primaryKey.size();
	for(int i=0; i<numRows();i++){ //All rows
		answer=true;
		for(int j=0; j < primaryKeyCount ; j++){//All primary keys Columns 
			columnPosition= getColumnPosition(primaryKey[j]);
			answer= answer && (row[columnPosition]== table[columnPosition].entryAt(i));
			if(!answer) break;//Primary keys didn't match if answer = false --Optimization
		}
		if(answer)
			return false;//Primary key already in the table answer = true
	}
	return true;
}
	
column& relation::columnNamed(string c){
	for (unsigned int i = 0; i < table.size(); i++){
		if (table[i].name == c) return table[i];
	}
	throw(std::out_of_range ("out of range"));
}

unsigned int relation::numRows() {
	return table.front().elements.size();
}

void relation::clearRow(int n) {
	for (unsigned int i = 0; i < table.size(); i++){
		table[i].clearEntry(n);
	}
}

void relation::copyRow(relation r, int n) {
	for (unsigned int i = 0; i < r.table.size(); i++){
		table[i].elements.push_back(r.table[i].entryAt(n));
	}
}

bool relation::unionCompatible(relation r) {
	int numColumns=table.size();
	if( r.table.size() != numColumns) 
		return false;
		
	for (unsigned int i = 0; i < numColumns; i++)
		if (table[i].isInt != r.table[i].isInt) 
			return false;
	
	return true;
}

bool relation::rowIsIn(vector<entry> row) {
	bool isIn;
	int numOfColumns= table.size();

	for (unsigned int i = 0; i < numRows(); i++) { //Over all rows
		isIn = true;
		for ( unsigned int j = 0; j < numOfColumns; j++ ) { //Over all columns
			if ( !( table[j].entryAt(i) == row[j]) ){ 
				isIn = false;
				break;
			}
		}
		if (isIn == true) 
			return true;
	}

	return false;
}

void relation::copyEmptyColumns(relation r) {
	for (unsigned int i = 0; i < r.table.size(); i++)
		table.push_back(column(r.table[i].name, r.table[i].isInt, r.table[i].stringlen));
}

void relation::copyEmptyColumns(relation r, vector<string> except) {
	for (unsigned int i = 0; i < r.table.size(); i++)
		if (!r.table[i].keyCol(except)) table.push_back(column(r.table[i].name, r.table[i].isInt, r.table[i].stringlen));
}

int relation::atRow(entry e, string inColumn) {
	column c= columnNamed(inColumn);
	for (unsigned int i = 0; i < c.elements.size(); i++)
		if (c.entryAt(i) == e)
			return i;
	throw std::invalid_argument("Error: Not in column");
}

string relation::intToString(int i) {
	stringstream ss;
	ss << i;
	string result;
	ss >> result;
	return result;
}

void relation::show(){
	// +--------------------------------------------------+     <---- top
	// | Relation: name                                   |
	// +-------------+---------------+--------------------+     <---- filler
	// | varchar(10) | varchar(20)   | integer             |     <---- whole table size based off of this line
	// +-------------+---------------+--------------------+     
	// | xyxyxyx     | fkflkdsjklfjd | 439                |
	// +-------------+---------------+--------------------+

	string filler="+", top="+";
	vector<int> splits;
	string temp="| ";
	//int length = 0;
	for(unsigned int x = 0;x<table.size();x++)
	{
		temp.append(table[x].name);
		temp.append(" ");
		if(table[x].isInt) temp.append("INTEGER ");
		else {
			temp.append("VARCHAR(");
			string t;
			temp.append(intToString(table[x].stringlen));
			temp.append(") ");
		}
		splits.push_back(temp.size()+1);
		temp.append(" | ");
	}
	bool flag=false;
	for(unsigned int i=1;i<temp.size()-2;i++)
	{
		top.append("-");
		for(unsigned int x=0;x<splits.size();x++)
			if(i==splits[x])
			{
				filler.append("+");
				flag=true;
			}
		if(flag==false)
			filler.append("-");
		flag=false;
	}
	string title="| Relation: ";
	title.append(name);
	if (temp.size() <= title.size()+1) {
		temp.erase(temp.size()-1);
		temp.erase(temp.size()-1);
		while (temp.size() <= title.size()) {
			temp.append(" ");
			top.append("-");
			filler.append("-");
			splits[splits.size()-1]++;
		}
		temp.append("| ");
	}
	top.append("+");
	filler.append("+");
	cout<<top<<endl;
	cout<<title<<setw(temp.size()-title.size()-1)<<"|"<<endl;
	cout<<filler<<endl;
	cout<<temp<<endl;
	cout<<filler<<endl;
	int size=temp.size();
	temp="| ";
	vector<string> entrys;
	for(unsigned int i=0;i<table[0].elements.size();i++)
	{
		for(unsigned int x=0;x<table.size();x++)
		{
			if (table[x].isInt) entrys.push_back(intToString(table[x].elements[i].numeric));
			else entrys.push_back(table[x].elements[i].alpha);
		}
		for(unsigned int i=0;i<entrys.size();i++)
		{
			if(i==0)
				cout<<temp<<entrys[i]<<setw(splits[i]-(entrys[i].size()+2))<<" ";
			else
				cout<<temp<<entrys[i]<<setw(splits[i]-(splits[i-1]+entrys[i].size()+2))<<" ";
		}
		cout<<"|"<<endl;
		cout<<filler<<endl;
		while(!entrys.empty()) entrys.pop_back();
	}
	cout<<endl;
}

vector<entry> relation::getRow(int row_position){
	vector<entry> new_row;
	vector<column>::iterator it;
	for(it=table.begin();it<table.end();it++){//For every column
		new_row.push_back(it->entryAt(row_position));
	}
	return new_row;
}

int relation::getColumnPosition(string column_name){
	for (unsigned int i = 0; i < table.size(); i++){
		if (table[i].name == column_name)
			return i;// Primary key constraint does not hold
	}
}

bool relation::attributeExist(string column_name){
	bool answer= false;
	for(int i=0; i<table.size();i++){
		answer= answer || (table[i].name == column_name);
	}
	return answer;
}



void relation::insertRow(vector<entry> row, int pos){
	if(pos==-1){	//pos= -1 means it inserted the row at the end
		for(int i=0; i < table.size(); i++) //Over all columns of the table
			table[i].elements.push_back(row[i]);
	}
	else{
		for(int i=0; i < table.size(); i++) //Over all columns of the table
			table[i].insertEntry(row[i], pos);
	}
}

bool relation::checkVarcharLength(std::vector<entry> row){
	for(int i=0 ; i < table.size() ; i++ ){ //Over all columns
		if(! table[i].isInt){ //varchar
			if(row[i].alpha.size()>table[i].stringlen)
				return false;
		}
	}
	return true;
}