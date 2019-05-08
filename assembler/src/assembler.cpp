#include <iostream>
#include <string>

#include "assembler/headers/pass1.h"
#include "assembler/headers/pass2.h"
#include <regex>

using namespace std;
regex r("^\\s*([\\w]+\\.[\\w]+)$");
void evaluate(string s) {
	smatch sm;
	if (regex_match(s, sm, r)) {
		cout << "file :" << sm[1].str() << endl;
		if (!sm[1].str().empty()) {
			cout << pass1(sm[1].str()) << endl;
			if(pass2()){
				cout<<"Successful Pass2"<<endl;
			}else{
				cout<<"Failed Pass2"<<endl;
			}
		} else {
			cout << "Empty path" << endl;
		}
	} else {
		cout << "Invalid command or path" << endl;
	}
}
int main(int argc, char* argv[]) {
	if (argc > 1) {
		string s(argv[1]);
		evaluate(s);
	} else {
		string s;
		cin >> s;
		evaluate(s);
	}
	return 0;
}
