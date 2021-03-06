#include "headers/common.h"

#include <iostream>
#include <algorithm>
#include <cctype>
#include <iterator>
#include <sstream>
#include <utility>
#include <iomanip>
#include "headers/pass1.h"

using namespace std;
string strToHex(string s);
unordered_map<string, info> optab;
unordered_map<string, info> derctivetab;
unordered_map<string, int> register_tab;
unordered_map<string, symbol> symtab;
vector<X> intermediate;
vector<Literal> littab;
bool completePass1 = false;
ll len;
const string reportfile = "LISTFILE.txt";

string to_string(int d) {
	stringstream ss;
	ss << d;
	return ss.str();
}
string to_upper(string s) {
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}
string trim(const string& str) {
	const string whitespace = " \t";
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}
template<typename T>
string decToHex(T d) {
	stringstream ss;
	ss << std::hex << to_int(d);
	return ss.str();
}

void addToOptab(string name, info i) {
	optab.insert(make_pair(name, i));
}

info make_info(unsigned int f, string s, unsigned int opr) {
	info i(f, s, opr);
	return i;
}

void load_derictve() {
	derctivetab.insert(make_pair("START", make_info(0, "", 1)));
	derctivetab.insert(make_pair("END", make_info(0, "", 0)));
	derctivetab.insert(make_pair("BYTE", make_info(0, "", 1)));
	derctivetab.insert(make_pair("WORD", make_info(0, "", 1)));
	derctivetab.insert(make_pair("RESB", make_info(0, "", 1)));
	derctivetab.insert(make_pair("RESW", make_info(0, "", 1)));
	derctivetab.insert(make_pair("BASE", make_info(0, "", 1)));
	derctivetab.insert(make_pair("NOBASE", make_info(0, "", 0)));
	derctivetab.insert(make_pair("EXTDEF", make_info(0, "", 1)));
	derctivetab.insert(make_pair("EXTREF", make_info(0, "", 1)));
	derctivetab.insert(make_pair("EQU", make_info(0, "", 1)));
	derctivetab.insert(make_pair("USE", make_info(0, "", 1)));
	derctivetab.insert(make_pair("CSECT", make_info(0, "", 1)));
	derctivetab.insert(make_pair("ORG", make_info(0, "", 1)));
	derctivetab.insert(make_pair("LTORG", make_info(0, "", 0)));

}

void load_register_tab() {
	register_tab.insert(make_pair("A", 0));
	register_tab.insert(make_pair("X", 1));
	register_tab.insert(make_pair("L", 2));
	register_tab.insert(make_pair("B", 3));
	register_tab.insert(make_pair("S", 4));
	register_tab.insert(make_pair("T", 5));
	register_tab.insert(make_pair("F", 6));
	register_tab.insert(make_pair("PC", 8));
	register_tab.insert(make_pair("SW", 9));
}

void load_optab() {
	addToOptab("addToOptab", make_info(3, "18", 1));
	addToOptab("ADDR", make_info(2, "90", 2));
	addToOptab("ADDF", make_info(3, "58", 1));
	addToOptab("AND", make_info(3, "40", 1));
	addToOptab("CLEAR", make_info(2, "B4", 1));
	addToOptab("COMP", make_info(3, "28", 1));
	addToOptab("COMPF", make_info(3, "88", 1));
	addToOptab("COMPR", make_info(2, "A0", 2));
	addToOptab("DIV", make_info(3, "24", 1));
	addToOptab("DIVF", make_info(3, "64", 1));
	addToOptab("DIVR", make_info(2, "9C", 2));
	addToOptab("J", make_info(3, "3C", 1));
	addToOptab("JEQ", make_info(3, "30", 1));
	addToOptab("JGT", make_info(3, "34", 1));
	addToOptab("JLT", make_info(3, "38", 1));
	addToOptab("JSUB", make_info(3, "48", 1));
	addToOptab("LDA", make_info(3, "00", 1));
	addToOptab("LDB", make_info(3, "68", 1));
	addToOptab("LDCH", make_info(3, "50", 1));
	addToOptab("LDF", make_info(3, "70", 1));
	addToOptab("LDL", make_info(3, "08", 1));
	addToOptab("LDS", make_info(3, "6C", 1));
	addToOptab("LDT", make_info(3, "74", 1));
	addToOptab("LDX", make_info(3, "04", 1));
	addToOptab("LPS", make_info(3, "D0", 1));
	addToOptab("MUL", make_info(3, "20", 1));
	addToOptab("MULF", make_info(3, "60", 1));
	addToOptab("MULR", make_info(2, "98", 2));
	addToOptab("OR", make_info(3, "44", 1));
	addToOptab("RD", make_info(3, "D8", 1));
	addToOptab("RMO", make_info(2, "AC", 2));
	addToOptab("RSUB", make_info(3, "4C", 0));
	addToOptab("STA", make_info(3, "0C", 1));
	addToOptab("STB", make_info(3, "78", 1));
	addToOptab("STCH", make_info(3, "54", 1));
	addToOptab("STF", make_info(3, "80", 1));
	addToOptab("STI", make_info(3, "D4", 1));
	addToOptab("STL", make_info(3, "14", 1));
	addToOptab("STS", make_info(3, "7C", 1));
	addToOptab("STSW", make_info(3, "E8", 1));
	addToOptab("STT", make_info(3, "84", 1));
	addToOptab("STX", make_info(3, "10", 1));
	addToOptab("SUB", make_info(3, "1C", 1));
	addToOptab("SUBF", make_info(3, "5C", 1));
	addToOptab("SUBR", make_info(2, "94", 2));
	addToOptab("SVC", make_info(2, "B0", 2));
	addToOptab("TD", make_info(3, "E0", 1));
	addToOptab("TIO", make_info(1, "F8", 0));
	addToOptab("TIX", make_info(3, "2C", 1));
	addToOptab("TIXR", make_info(2, "B8", 1));
	addToOptab("WD", make_info(3, "DC", 1));

	addToOptab("SHIFTL", make_info(2, "A4", 2));
	addToOptab("SHIFTR", make_info(2, "A8", 2));
	addToOptab("SIO", make_info(1, "F0", 0));
	addToOptab("SSK", make_info(3, "EC", 1));
	addToOptab("NORM", make_info(1, "C8", 0));
	addToOptab("HIO", make_info(1, "F4", 0));
	addToOptab("FLOAT", make_info(1, "C0", 0));
	addToOptab("FIX", make_info(1, "C4", 0));

}

void addToIntrtmediate(X& item){
	intermediate.push_back(item);
}

void addToLittab(Literal& item){
	littab.push_back(item);
}

void load_tabels() {
	load_derictve();
	load_optab();
	load_register_tab();
}

/*
 * convert litral value to hex value
 */
string valueOfLitral(string s,string& type){
	s = trim(s);
	string value = "";
	smatch sm;
	if(regex_match(s, sm,word)){
		type = "W";
		value = adjustString(to_int(sm[2].str()),6);
		value = value.substr(value.size()-6,6);
	}else if(regex_match(s, sm, chars)){
		type ="C";
		value = strToHex(sm[2].str());
	}else if(regex_match(s, sm, rlhex)){
		type="X";
		value =sm[2].str();
	}
	return value;
}
/**
 * convert string to equivalent  hex value
 */
string strToHex(string s){
	string value = "";
	for(char c : s){
		int x = c;
		value += dec_to_hex(x);
	}
	return value;
}
string adjustString(ll value, ll adj) {
	stringstream s;
	s << setfill('0') << setw(adj) << ::hex << value;
	return s.str();
}
