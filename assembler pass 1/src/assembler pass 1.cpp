#include <iostream>
#include "assembler/assembler.h"
#include "assembler/parser.h"
#include <sstream>
using namespace std;
template<typename T>
long long hex_to_dec(T h) {
	long long l;
	std::stringstream ss;
	ss << h;
	ss >> std::hex >> l;
	return l;
}
template<typename T>
string dec_to_hex(T d){
	stringstream ss;
	ss<<std::hex<<d;
	return ss.str();
}
int main() {
	string path = "T_F.txt";
	pass1(path);
	parser p;
	cout<<(p.optab.find("SUB") == p.optab.end())<<endl;
	//cout<<c.opcode<<endl;
	cout<<"done"<<endl;
	return 0;
}
