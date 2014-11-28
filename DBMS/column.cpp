#include <string>

#include "column.h"

using namespace std;

//this function can return the encapsulated entry at a position in a column
entry& column::entryAt(int i) {
	return elements[i];
}
	
//these 3 versions can return the position in the column of an integer, string or encapsulated entry
int column::entryPosition(entry e) {
	for (unsigned int i = 0; i < elements.size(); i++)
		if (elements[i] == e) return i;
	return -1;
}
int column::entryPosition(int n) {
	for (unsigned int i = 0; i < elements.size(); i++)
		if (elements[i].numeric  == n) return i;
	return -1;
}

int column::entryPosition(char* c) {
	for (unsigned int i = 0; i < elements.size(); i++)
		if (elements[i].alpha  == c) return i;
	return -1;
}

void column::renameColumn(string newName){
	name = newName;
}

void column::insertEntry(entry e, int n){
	elements.insert(elements.begin()+n , e);
}

void column::clearEntry(int n){
	elements.erase(elements.begin()+n);
}

bool column::isEmpty(){
	return elements.empty();
}

bool column::keyCol(vector<string> keys){
	for (unsigned int i = 0; i < keys.size(); i++)
		if (name == keys[i]) return true;
	return false;
}