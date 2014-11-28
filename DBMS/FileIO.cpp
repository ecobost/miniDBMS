#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "FileIO.h"

using namespace std;

int FileIO::stringToInt(const string &Text){
	stringstream ss(Text);
	int result;
	return ss >> result ? result : 0;
}

//breaks up a string along its spaces - needed for interpreting files
vector<string> FileIO::breakString(string s){
	vector<string> output;
	string word = "";
	for (unsigned int i = 0; i < s.size(); i++) {
		if (s[i] != ',') word.push_back(s[i]);
		else {
			output.push_back(word);
			word = "";
		}
	}
	output.push_back(word);
	return output;
}
	
relation FileIO::readFromFile(string name, int d){//if d=1, delete file at the end

	vector<string> splitInput;
	relation newTable;
	ifstream myFile;
	myFile.exceptions(ifstream::failbit | ifstream::badbit);
	string temp=name;
	temp.append(".txt");


	myFile.open(temp);
		
	string line;
	bool flag=false;
	int x=0;

	while(!myFile.eof())
	{
		if(flag==true)
			break;
		getline(myFile, line);
		splitInput=breakString(line);

		switch(splitInput[0][0])
		{
		case 'N':
			newTable.name=splitInput[1];
			break;
		case 'C':
			if (splitInput[2][0]=='I') newTable.table.push_back(column(splitInput[1], true));
			else newTable.table.push_back(column(splitInput[1], false, stringToInt(splitInput[2].substr(7,splitInput[2].size()-7))));
			for(unsigned int i=3;i<splitInput.size();i++)
				if (splitInput[2][0]!='I') newTable.table[x].elements.push_back(entry(splitInput[i], false));
				else newTable.table[x].elements.push_back(entry(stringToInt(splitInput[i]), true));
			x++;
			break;
		case 'K':
			flag=true;
			for (unsigned int i = 1; i < splitInput.size(); i++)
				newTable.primaryKey.push_back(splitInput[i]);
			break;
		}
	}
	myFile.close();
	if(d==1)
		remove(temp.c_str());
	return newTable;
}

void FileIO::printToFile(relation r){
	
	ofstream myFile;
	string temp=r.name;
	temp.append(".txt");
	myFile.open(temp);
	if(myFile.is_open())
	{
		myFile<<"NAME,"<<r.name<<endl;
		for(unsigned int i=0;i<r.table.size();i++)
		{
			myFile<<"COLUMN,"<<r.table[i].name;
			if (r.table[i].isInt) myFile<< ",INTEGER";
			else myFile<< ",VARCHAR"<<r.table[i].stringlen;
			for(unsigned int x=0;x<r.table[i].elements.size();x++)
			{
				if(r.table[i].elements[x].isInt==true)
					myFile<<","<<r.table[i].elements[x].numeric;
				else
					myFile<<","<<r.table[i].elements[x].alpha;
			}
			myFile<<endl;
		}
		myFile<<"KEY,";
		unsigned int i;
		for (i = 0; i < r.primaryKey.size()-1; i++)
			myFile<<r.primaryKey[i]<<",";
		myFile<<r.primaryKey[i]<<endl;
	}
}