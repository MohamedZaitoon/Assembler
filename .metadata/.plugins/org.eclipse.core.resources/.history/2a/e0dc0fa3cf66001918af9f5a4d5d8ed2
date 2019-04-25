#include <iostream>
#include "assembler/assembler.h"
#include "assembler/parser.h"
#include "assembler/mnemonicinstruction.h"
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
	parser p;
	mnemonic_instruction m;
	m = p.parse("rsub");
	cout<<m.getMnemonic()<<"  m"<<endl;
	return 0;
}
