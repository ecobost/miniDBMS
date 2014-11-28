#include <string>
#include <fstream>
#include <queue>

#include "DBMS.h"
#include "Parser.h"

using namespace std;


/*CREATE TABLE students (sid INTEGER , name VARCHAR(20), classification VARCHAR(20)) PRIMARY KEY (sid);
CREATE TABLE courses (cid INTEGER, department VARCHAR (4), number INTEGER , name VARCHAR(20)) PRIMARY KEY (cid);
CREATE TABLE takes (sid INTEGER , cid INTEGER) PRIMARY KEY (sid, cid);*/


DBMS Student_database;
queue<Token> queueLine;

void run_file(string);
char checkInput(string);

void openTable(char );
void closeTable(char );

void saveTable(char );

void showTable(char);
void updateTable(char);
void insertIntoTable(char );

void studentsIn315();
void delete_course();
void student_courses();

void execute_cmd(string);




int main(int argc, char* argv[]){
	
	

	
	while (true){ 
		cout<<endl<<endl;
		cout<<"Welcome to the Student Database"<<endl;
		cout<<"Please chose one of the following options"<<endl;
		cout<<"Enter the respective number for each operation"<<endl;
		cout<<"z. Load database into memory"<<endl;
		cout<<"a. Load Database"<<endl;
		cout<<"b. Close Database"<<endl;
		cout<<"c. Save Database"<<endl;
		cout<<"d. Show Database"<<endl;
		cout<<"e. Update Database"<<endl;
		cout<<"f. Insert into Database"<<endl;
		cout<<"g. Name of the students taking CSCE 315 "<<endl;
		cout<<"h. List Computer Science courses"<<endl;
		cout<<"i. list the Junior and Senior students"<<endl;
		cout<<"j. Number and Name of students that are not taking any course"<<endl;
		cout<<"k. Delete a specific course"<<endl;
		cout<<"l. Show students who are frehman or age less than equal 18"<<endl;
		cout<<"m. Show students with age greater than equal to 21 and less than equal to 23"<<endl;
		cout<<"n. Show courses with course number greater than 200 or (Union) Computer Science courses"<<endl;
		cout<<"o. Show students with attributes with the first letter (Rename)"<<endl;
		cout<<"p. Show courses taken by a specific student"<<endl;
		cout<<"q. enter the DMBL query for any other functionality"<<endl;
		string option1,option2,dept_name,num,dbml_query;
		getline(cin,option1); 
		
		char c=checkInput(option1);
		
		

		switch (c){
		case 'z':
			run_file("testfile.txt");
			break;

		case 'a':
			cout<<"1. Load Students Table"<<endl;
			cout<<"2. Load courses Table"<<endl;
			cout<<"3. Load Student Taking courses Table"<<endl;
			cout<<"4. Load all tables"<<endl;
			cout<<"5. Enter the table name "<<endl;
			getline(cin,option2);
			c=checkInput(option2);
			openTable(c);	
			break;

		case 'b':
			cout<<"1. Close Students Table"<<endl;
			cout<<"2. Close courses Table"<<endl;
			cout<<"3. Close Student Taking courses Table"<<endl;
			cout<<"4. Close all tables"<<endl;
			cout<<"5. Close the table name "<<endl;
			getline(cin,option2);
			c=checkInput(option2);
			closeTable(c);	
			break;

		case 'c':
			cout<<"1. Save Students Table"<<endl;
			cout<<"2. Save courses Table"<<endl;
			cout<<"3. Save Student Taking courses Table"<<endl;
			cout<<"4. Save all tables"<<endl;
			cout<<"5. Enter the table name "<<endl;
			getline(cin,option2);
			c=checkInput(option2);
			saveTable(c);	
			break;

		case 'd':
			cout<<"1. Show Students Table"<<endl;
			cout<<"2. Show courses Table"<<endl;
			cout<<"3. Show Student Taking courses Table"<<endl;
			cout<<"4. Show all tables"<<endl;
			cout<<"5. Enter the table name "<<endl;
			getline(cin,option2);
			c=checkInput(option2);
			showTable(c);	
			break;

		case 'e':
			cout <<"1. Update Student Classification"<<endl;
			cout <<"2. Update Student Name"<<endl;
			cout <<"3. Update Student ID"<<endl;
			cout <<"4. Update Department Name"<<endl;
			cout <<"5. Update courses ID"<<endl;
			getline(cin,option2);
			c=checkInput(option2);
			updateTable(c);	
			break;
			
		case 'f':
			cout <<"1. Insert into Students"<<endl;
			cout <<"2. Insert into courses"<<endl;
			cout <<"3. Make an existing student take a new courses"<<endl;
			getline(cin,option2);
			c=checkInput(option2);
			insertIntoTable(c);	
			break;

		case 'g':
			studentsIn315();
			break;
		case 'h':
			execute_cmd("SHOW (select ( department == \"CSCE\") courses);");
			//execute_cmd("SHOW temp;");
			break;

		case 'i':
			execute_cmd("SHOW ( project (sname, classification) ( select (classification == \"Junior\" || classification == \"Senior\") students ));");
			break;

		case 'j':
			execute_cmd("temp <-  ( project (uin) students) - (project (tuin) takes);");
			execute_cmd("temp <- rename (tempuin) temp;");
			execute_cmd("temp <- project ( uin, sname ) (select ( uin == tempuin) (temp * students ));");
			cout<<"Students not taking any courses are "<<Student_database.rowCountInRelation("temp")<<endl;
			execute_cmd("SHOW temp;");
			break;
		case 'k':
			delete_course();
			break;

		case 'l':
			execute_cmd("temp <- select ( classification == \"Freshman\" || age <= 18) students;");
			execute_cmd("SHOW temp; ");
			break;
		case 'm':
			execute_cmd("temp <- select ( age >= 21 && age <= 23 ) students;");
			execute_cmd("SHOW temp;");
			break;
		case 'n':
			execute_cmd("temp <-  ( select ( number > 200) courses ) + ( select ( department == \"CSCE\" ) courses );");
			execute_cmd("SHOW temp;");
			break;
		case 'o':
			execute_cmd("temp <- rename (u,n,d,a) students; ");
			execute_cmd("SHOW temp;");
			break;
		case 'p':
			student_courses();
			break;
		case 'q':
			cout<<"Enter the DBML query"<<endl;
			getline(cin,dbml_query);
			execute_cmd(dbml_query);
			break;
		case 'r':
			execute_cmd("EXIT;");
			break;
		default:
			cout<<"Sorry you entered the wrong value"<<endl;
			break;
		}
	}
}

	

char checkInput(string option){
		if (option.size()>1)
			return 'z';
		else
			return option[0];
	}

void run_file(string filename){
	string fullline;
	string myline;
	queue<Token> queueLine;


	ifstream readingfile(filename);
	if(readingfile.is_open()){
		while (readingfile.good()){

			getline (readingfile,fullline);
			string::size_type position;
			 

			while(fullline.length()> 1){ // assumes no space between ';' and start of next command or query, if on same line
				position=fullline.find(';',0);	
				myline = fullline.substr(0,position+1);	

				fullline= fullline.substr(position+1);

				queueLine = Parser::tokenLine(myline);// myline= OPEN students; (SQL command) 

				Student_database.readToken(queueLine);
			}
		} 
		readingfile.close();
	} 
	else cout << "Not able to open file\n";
	cout << "\n";
}

void openTable(char option){
	string cmd_line;
	switch (option){
		case '1':
			execute_cmd ("OPEN students;");
			break;
		case '2':
			execute_cmd ("OPEN courses;");
			break;
		case '3':
			execute_cmd ("OPEN takes;");
			break;
		case '4':
			execute_cmd ("OPEN students;");
			execute_cmd ("OPEN courses;");
			execute_cmd ("OPEN takes;");
			break;
		case '5':
			cout<<"Enter the table name you wish to open "<<endl;
			getline(cin,cmd_line);
			cmd_line="OPEN "+cmd_line+";";
			execute_cmd (cmd_line);
			break;
		default:
			cout<<"Sorry you entered the wrong value"<<endl;
			break;
	}
	
}

void closeTable(char option){
	string cmd_line;
	switch (option){
		case '1':
			execute_cmd ("CLOSE students;");
			break;
		case '2':
			execute_cmd ("CLOSE courses;");
			break;
		case '3':
			execute_cmd ("CLOSE takes;");
			break;
		case '4':
			execute_cmd ("CLOSE students;");
			execute_cmd ("CLOSE courses;");
			execute_cmd ("CLOSE takes;");
			break;
		case '5':
			cout<<"Enter the table name you wish to close "<<endl;
			getline(cin,cmd_line);
			cmd_line="CLOSE "+cmd_line+";";
			execute_cmd (cmd_line);
			break;
		default:
			cout<<"Sorry you entered the wrong value"<<endl;
			break;
	}

}

void saveTable(char option){
	string cmd_line;
	switch (option){
		case '1':
			execute_cmd ("WRITE students;");
			break;
		case '2':
			execute_cmd ("WRITE courses;");
			break;
		case '3':
			execute_cmd ("WRITE takes;");
			break;
		case '4':
			execute_cmd ("WRITE students;");
			execute_cmd ("WRITE courses;");
			execute_cmd ("WRITE takes;");
			break;
		case '5':			
			cout<<"Enter the table name you wish to save "<<endl;
			getline(cin,cmd_line);
			cmd_line="WRITE "+cmd_line+";";
			execute_cmd (cmd_line);
			break;
		default:
			cout<<"Sorry you entered the wrong value"<<endl;
			break;
	}

}

void showTable(char option){
	string cmd_line;
	switch (option){
		case '1':
			execute_cmd ("SHOW students;");
			break;
		case '2':
			execute_cmd ("SHOW courses;");
			break;
		case '3':
			execute_cmd ("SHOW takes;");
			break;
		case '4':
			execute_cmd ("SHOW students;");
			execute_cmd ("SHOW courses;");
			execute_cmd ("SHOW takes;");
			break;
		case '5':
			cout<<"Enter the table name you wish to see "<<endl;
			getline(cin,cmd_line);
			cmd_line="SHOW "+cmd_line+";";
			execute_cmd (cmd_line);
			break;
		default:
			cout<<"Sorry you entered the wrong value"<<endl;
			break;
	}

}

void updateTable(char option){
	string classif,id,cmd_line,new_id,old_id,new_name,old_name;
	switch (option){
		case '1':
			cout<<"Enter the new classification"<<endl;
			getline(cin,classif);
			cout <<"Enter the Student ID you want to be updated"<<endl;
			getline(cin,id);
			cmd_line = "UPDATE students SET classification= "+ classif +" WHERE uin == " + id + ";";
			execute_cmd(cmd_line);			
			break;
		case '2':
			cout<<"Enter the new name"<<endl;
			getline(cin,new_name);
			cout <<"Enter the Student ID you want to be updated"<<endl;
			getline(cin,id);
			cmd_line = "UPDATE students SET sname = "+ new_name +" WHERE uin == " + id + ";";
			execute_cmd(cmd_line);
			break;
		case '3':
			cout<<"Enter the new ID"<<endl;
			getline(cin,classif);
			cout <<"Enter the old ID"<<endl;
			getline(cin,old_id);
			cmd_line = "UPDATE students SET uin= "+ new_id +" WHERE uin == " + old_id + ";";
			execute_cmd(cmd_line);
			break;
		case '4':
			cout<<"Enter the new Department name"<<endl;
			getline(cin,new_name);
			cout <<"Enter the old Department name"<<endl;
			getline(cin,old_name);
			cmd_line = "UPDATE courses SET department= "+ new_name +" WHERE department == " + old_name + ";";
			execute_cmd(cmd_line);
			break;
		case '5':
			cout<<"Enter the new courses ID"<<endl;
			getline(cin,new_id);
			cout <<"Enter the old courses ID"<<endl;
			getline(cin,old_id);
			cmd_line = "UPDATE courses SET crn = "+ new_id +" WHERE crn == " + old_id + ";";
			execute_cmd(cmd_line);
			break;
		default:
			cout<<"Sorry you entered the wrong value"<<endl;
			break;

	}
}

void insertIntoTable(char option){
	string id,name,classif,cmd_line, age,dept_name,courses_name,courses_num,crn,uin;
	switch (option){
	case '1':
		cout <<"Enter the Student ID "<<endl;
		getline(cin,id);
		cout <<"Enter the Student name"<<endl;
		getline(cin,name);
		cout <<"Enter the student classification"<<endl;
		getline(cin,classif);
		cout <<"Enter the age"<<endl;
		getline(cin,age);
		cmd_line = "INSERT INTO students VALUES FROM (" + id +" , \""+name+"\" , \""+classif+"\" , " + age+");" ;
		execute_cmd(cmd_line);
		break;
	case '2':
		cout<<"Enter the department name you wish to add the courses "<<endl;
		getline(cin,dept_name);
		cout<<"enter the courses ID "<<endl;
		getline(cin,id);
		cout<<"Enter the courses name"<<endl;
		getline(cin,courses_name);
		cout<<"enter the courses number"<<endl;
		getline(cin,courses_num);
		cmd_line="INSERT INTO courses VALUES FROM (" +id+ " , \""+dept_name+ "\" , " +courses_num + " , \""+courses_name+"\");";
		execute_cmd(cmd_line);
		break;
	case '3':
		cout<<"Enter the courses ID you wish to take"<<endl;
		getline(cin,crn);
		cout<<"Enter the studetnt's UIN"<<endl;
		getline(cin,uin);
		cmd_line="temp <- select  (uin == "+ uin+ ") students;";
		execute_cmd(cmd_line);
		if (Student_database.rowCountInRelation("temp") >0){
			cmd_line="temp <- select  (crn == "+ crn+ ") courses;";
			execute_cmd(cmd_line);
			if (Student_database.rowCountInRelation("temp") >0){
				cmd_line="INSERT INTO takes VALUES FROM ("+uin+" , "+ crn+");";
				execute_cmd(cmd_line);
			}
			else
				cout<<"Sorry the courses ID does not exist"<<endl;
		}
		else
			cout<<"Sorry the student ID entered is wrong"<<endl;
		break;
	default:
		cout<<"Sorry you entered the wrong value"<<endl;
		break;
	}
}

void studentsIn315(){
	execute_cmd("temp <- select (department == \"CSCE\" && number == 315) courses;");
	execute_cmd("temp <- select (tcrn == crn) (temp * takes);");
	execute_cmd("temp <- project (sname, classification) ( select (uin == tuin) (temp * students) );");
	//execute_cmd("temp <- select (uin == tuin) (temp * students);");
	//execute_cmd("temp <- project (sname, classification) temp;");
	execute_cmd("SHOW temp;");
}

void delete_course(){
	string dept_name,num;
	cout<<"Enter the department name"<<endl;
	getline(cin,dept_name);
	cout<<"Enter the course number"<<endl;
	getline(cin,num);
	execute_cmd("DELETE FROM courses WHERE ( department == \""+dept_name+"\" && number == "+num +");");
}

void student_courses(){
	string name,uin;
	cout<<"Enter the student name"<<endl;
			getline(cin,name);
			cout<<"Enter the student uin"<<endl;
			getline(cin,uin);
			execute_cmd("temp <- project (uin) (select ( uin == "+uin+ " && sname == \""+name+"\" ) students);");
			if (Student_database.rowCountInRelation("temp")==0)
				cout<<"Sorry the UIN and name did not match"<<endl;
			else
			{
				execute_cmd("temp <- select ( tuin == "+uin+" ) (temp * takes);");
				if (Student_database.rowCountInRelation("temp") == 0 )
					cout<<"Sorry the student is not taking any courses"<<endl;
				else
				{
					execute_cmd("temp <- select ( tcrn == crn ) (temp * courses);");
					cout<<"the courses taken by "<<name<<" are : "<<endl;
					execute_cmd("temp <- project ( crn , department , number , cname) temp;");
					execute_cmd("SHOW temp;");
				}
			}
}
void execute_cmd(string cmd_line){
	

	queueLine = Parser::tokenLine(cmd_line);
	Student_database.readToken(queueLine);
}