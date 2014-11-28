#include <string>
#include <sstream>

#include "entry.h"
	
using namespace std;

bool entry::operator==(entry other) {
	if (isInt) return numeric == other.numeric;
	else return alpha == other.alpha; 
}

void entry::updateEntry(string s){
	if (!isInt) alpha = s;
	else numeric = stringToInt(s);
}

void entry::updateEntry(int n){
	if (isInt) numeric = n;
}

int entry::stringToInt(const string &Text) {
	stringstream ss(Text);
	int result;
	return ss >> result ? result : 0;
}
