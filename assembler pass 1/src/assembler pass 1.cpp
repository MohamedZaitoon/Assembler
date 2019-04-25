#include <iostream>
#include "assembler/assembler.h"
#include "assembler/parser.h"
#include "assembler/mnemonicinstruction.h"
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdio.h>
#include <fstream>

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
	run();
	//pass1("T_F.txt");
//	int jf = 20;
//	ofstream write;
//	write.open("LISTFILE.txt", ios::trunc);
//	write<<"asdiuhaufuia"<<endl;
//	write << std::left << setw(jf) << "line no." << setw(jf) << "address"
//					<< setw(jf) << "label" << setw(jf) << "op-code" << setw(jf)
//					<< "operand" << setw(jf) << "comment"<<endl;
//	write.close();
	cout<<"Exit";
	return 0;
}


