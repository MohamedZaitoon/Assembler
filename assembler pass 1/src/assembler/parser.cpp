#include "parser.h"
#include <map>
#include <string>
#include <iostream>
using namespace std;
typedef mnemonic_instruction instruct;

map<string, info> optab;
parser::parser() {
	load_optab();
}

parser::~parser() {
}

instruct parser::parse(string ins) {

	instruct x;
	return x;
}

void parser::load_optab() {
	add("ADD", make_info(3, "18", 1));
	add("ADDR", make_info(2, "90", 2));
	add("ADDF", make_info(3, "58", 1));
	add("AND", make_info(3, "40", 1));
	add("CLEAR", make_info(2, "B4", 1));
	add("COMP", make_info(3, "28", 1));
	add("COMPF", make_info(3, "88", 1));
	add("COMPR", make_info(2, "A0", 2));
	add("DIV", make_info(3, "24", 1));
	add("DIVF", make_info(3, "64", 1));
	add("DIVR", make_info(2, "9C", 2));
	add("J", make_info(3, "3C", 1));
	add("JEQ", make_info(3, "30", 1));
	add("JGT", make_info(3, "34", 1));
	add("JLT", make_info(3, "38", 1));
	add("JSUB", make_info(3, "48", 1));
	add("LDA", make_info(3, "00", 1));
	add("LDB", make_info(3, "68", 1));
	add("LDCH", make_info(3, "50", 1));
	add("LDF", make_info(3, "70", 1));
	add("LDL", make_info(3, "08", 1));
	add("LDS", make_info(3, "6C", 1));
	add("LDT", make_info(3, "74", 1));
	add("LDX", make_info(3, "04", 1));
	add("LPS", make_info(3, "D0", 1));
	add("MUL", make_info(3, "20", 1));
	add("MULF", make_info(3, "60", 1));
	add("MULR", make_info(2, "98", 2));
	add("OR", make_info(3, "44", 1));
	add("RD", make_info(3, "D8", 1));
	add("RMO", make_info(2, "AC", 2));
	add("RSUB", make_info(3, "4C", 0));
	add("STA", make_info(3, "0C", 1));
	add("STB", make_info(3, "78", 1));
	add("STCH", make_info(3, "54", 1));
	add("STF", make_info(3, "80", 1));
	add("STI", make_info(3, "D4", 1));
	add("STL", make_info(3, "D4", 1));
	add("STS", make_info(3, "14", 1));
	add("STSW", make_info(3, "E8", 1));
	add("STT", make_info(3, "84", 1));
	add("STX", make_info(3, "10", 1));
	add("SUB", make_info(3, "1C", 1));
	add("SUBF", make_info(3, "5C", 1));
	add("SUBR", make_info(2, "94", 2));
	add("SVC", make_info(2, "B0", 2));
	add("TD", make_info(3, "E0", 1));
	add("TIO", make_info(1, "F8", 0));
	add("TIX", make_info(3, "2C", 1));
	add("TIXR", make_info(2, "B8", 1));
	add("WD", make_info(3, "DC", 1));

	add("SHIFTL", make_info(2, "A4", 2));
	add("SHIFTR", make_info(2, "A8", 2));
	add("SIO", make_info(1, "F0", 0));
	add("SSK", make_info(3, "EC", 1));
	add("NORM", make_info(1, "C8", 0));
	add("HIO", make_info(1, "F4", 0));
	add("FLOAT", make_info(1, "C0", 0));
	add("FIX", make_info(1, "C4", 0));
}

void parser::add(string name, info i) {
	this->optab.insert(make_pair(name, i));
}
info parser::make_info(unsigned int f, string s, unsigned int opr) {
	info i(f, s, opr);
	return i;
}
