#include <iostream>
#include <string>

#include "assembler/headers/pass1.h"
#include "assembler/headers/pass2.h"

using namespace std;

int main() {
//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
//	cout<<std::hex<<"16"<<endl;
//	std::string str("123");
//	  int n = std::stoi(str);

	 cout<<pass1("A.txt")<<endl;
	 pass2();
	return 0;
}
