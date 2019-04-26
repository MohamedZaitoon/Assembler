#include <iostream>
#include "assembler/assembler.h"
#include "assembler/parser.h"
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include "assembler/statement.h"

using namespace std;
regex r("^\\s*pass1\\s+([\\s\\S]+)$");
void run(){
	while(true){
		string s;
		cout<<">>>";
		getline(cin,s);
		cout<<s<<endl;
		smatch sm;
		if(regex_match(s,sm,r)){
			cout<<"file :"<<sm[1].str()<<endl;
			cout<<pass1(sm[1].str())<<endl;
		}
	}
}

int main() {
	//run();
	cout<<pass1("T_F.txt")<<endl;
	cout<<"end pass1";
	return 0;
}


