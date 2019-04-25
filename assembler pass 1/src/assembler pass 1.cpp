#include <iostream>
#include "assembler/assembler.h"
#include "assembler/parser.h"
#include "assembler/mnemonicinstruction.h"
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdio.h>
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
	pass1("T_F.txt");
	cout<<"Exit";
	return 0;
}


