#include <iostream>
#include <sstream>
#include <string>
#include "assembler/pass1.h"
using namespace std;

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	cout<<std::hex<<"16"<<endl;
	std::string str("123");
	stringstream s;
	  int n = std::stoi(str);
//
	  std::cout << str << " --> " << n <<endl;
//	  int c = 'd';
//	  cout<<c<<endl;
	 cout<<pass1("A.txt")<<endl;
	return 0;
}
