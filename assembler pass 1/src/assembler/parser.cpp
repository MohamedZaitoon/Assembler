#include "parser.h"
#include <map>
#include <string>
#include <iostream>
#include <regex>
#include "assembler.h"
#include <utility>
using namespace std;
typedef mnemonic_instruction instruct;

const regex comment("^\\s*[\\.]([\\s\\S]*)");

const regex opcode("^\\s*([+])?([\\S]+)\\s*$");

const regex operand("^([#*@])?\\s*(\\S+)\\s*([,])?\\s*(\\S+)?$");

const regex litral("^[=]\\s*([cCxX])\\s*['](\\w+)[']\\s*$");

const regex label("(^[a-zA-Z][\\w]+)$");

const regex instruction("^\\s*(?:(\\S+)?\\s+)?"
		"(\\s*[+]?[\\w]+)\\s*"
		"(?:\\s+([=#*@,'\\w\\s]+)?\\s*)?"
		"(?:\\s+(\\.[\\s\\S]+)?)?$");

parser::parser() {
	load_optab();
	load_register_tab();
	load_derictve();
}

parser::~parser() {
}

instruct parser::parse(string ins) {
	instruct x;
	smatch result;
	if (regex_match(ins, result, comment)) {
		x.setComment(*result.begin());
		return x;
	} else if (regex_search(ins, result, instruction)) {
		if (result.empty())
			goto invalid;
		vector<string> v = get_groups(result);
		switch (v.size()) {
		case 1:
			one_field(&x, v[0]);
			break;
		case 2:
			two_field(&x, v);
			break;
		case 3:
			three_field(&x, v);
			break;
		case 4:
			four_field(&x, v);
			break;
		default:
			goto invalid;
		}
		valid_syntax(x);

	} else {
		invalid: x.setError("Invalid instruction syntax");
		return x;
	}
	return x;
}
void parser::valid_syntax(mnemonic_instruction& x){
	//label must begin with character
	if(x.has_label()&& !regex_match(x.getLabel(),label)){
		x.setError("label must begin with alphabetic character, "+x.getError());
	}
	//operand
	string op = x.getMnemonic();
	auto it = optab.find(op);
	if(it != optab.end()){
		if(!x.getOperand().empty()){
			smatch sm;
			if(regex_match(x.getOperand(),sm,operand)){
				 string perfix = sm[1].str();
				 string first = sm[2].str();
				 string comma = sm[3].str();
				 string second = sm[4].str();
				 int oprs = it->second.operands;
				 if((perfix == "#" || perfix == "@")&&!comma.empty()&&!second.empty()){
					 x.setError(x.getError()+", illegal addressing mode");
				 }else if(perfix == "*"&& !first.empty()){
					 x.setError(x.getError()+", extra characters at end of statement");
				 }else if(comma.empty() && !second.empty()){
					 x.setError(x.getError()+", the must be comma ',' between two operands");
				 }else if(oprs == 2){
					 if(second.empty()){
						 x.setError(x.getError()+", second register is missed");
					 }else if (register_tab.find(first) == register_tab.end() ||
							 register_tab.find(second) == register_tab.end()){
						 x.setError(x.getError()+", operand must be a register");
					 }
				 }else if(!second.empty() && "X" != to_upper(second)){
					 x.setError(x.getError()+", operand must be a register X not a "+second);
				 }
			}else if(regex_match(x.getOperand(),sm,litral)){
				 //handle litral
			}else{
				x.setError(x.getError()+", invalid operand");
			}
		}
	}else {
		// handle derictrive
	}
}

void parser::one_field(mnemonic_instruction* x, string field) {
	smatch m;
	if (regex_search(field, m, opcode)) {
		vector<string> groups = get_groups(m);
		if (groups.size() == 2) {
			x->setFormate4(true);
			string op = groups[1];
			check_mnemonic(x, op);
			x->setMnemonic(op);
		} else if (groups.size() == 1) {
			string op = groups[0];
			check_mnemonic(x, op);
			x->setMnemonic(op);
		} else {
			x->setError(x->getError()+", Invalid instruction syntax");
		}
	}
}

void parser::two_field(mnemonic_instruction* x, vector<string>& fields) {
	smatch m;
	if (regex_match(fields[0], m, opcode)) { // either label or opcode
		vector<string> v = get_groups(m);
		if (v.size() == 2) { //absolutely  opcde format 4
			x->setFormate4(true);
			check_mnemonic(x, v[1]);
			x->setMnemonic(v[1]);
			smatch sm;
			if (regex_match(fields[1], sm, comment)) {
				x->setComment(sm[1].str());
			} else if (regex_match(v[1], sm, operand)) {
				x->setOperand(sm.str());
			}
		} else if (v.size() == 1) { // either label or opcode
			if (optab.find(v[0]) == optab.end()) { //case first field label
				x->setLabel(v[0]);
				one_field(x,fields[1]);
			} else {
				one_field(x,fields[0]);
				smatch sm;
				 if (regex_match(v[1], sm, comment)) {
					 x->setComment(sm[1].str());
				} else if (regex_match(v[1], sm, operand)) {
					x->setOperand(sm.str());
				}
			}
		}
	}

}
void parser::three_field(mnemonic_instruction* x, vector<string>& fields) {
	vector<string> y(2);
	y[0] = fields[0];
	y[1] = fields[1];
	two_field(x,y);
	smatch sm;
	if(regex_match(fields[2],sm,comment)){
		x->setComment(sm.str());
	}else {
		x->setError(x->getError()+", "+"syntax error");
	}
}
void parser::four_field(mnemonic_instruction* x, vector<string>& fields) {
	x->setLabel(fields[0]);
	one_field(x,fields[1]);
	x->setOperand(fields[2]);
	x->setComment(fields[3]);
}
vector<string> parser::get_groups(smatch m) {
	vector<string> v;
	for (int i = 1; i < m.size(); i++) {
		string re = m[i].str();
		if (!re.empty()) {
			v.push_back(re);
		}
	}
	return v;
}
/**
 * check that:
 * 		is op in optab
 * 		format 2 does not come with format 4
 *
 */
void parser::check_mnemonic(mnemonic_instruction*x, string op) {
	auto entry = optab.find(op);
	if (entry == optab.end()) {
		x->setError("this is invalid mnemonic instruction");
	} else {
		info t = entry->second;
		if (x->isFormate4() && t.formate == 2) {
			x->setError(op + " must be format 2");
		}
	}
}

void parser::load_derictve(){
	this->derctivetab.insert(make_pair("START",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("END",make_info(0,"",0)));
	this->derctivetab.insert(make_pair("BYTE",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("WORD",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("RESB",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("RESW",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("BASE",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("NOBASE",make_info(0,"",0)));
	this->derctivetab.insert(make_pair("EXTDEF",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("EXTREF",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("EQU",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("USE",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("CSECT",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("ORG",make_info(0,"",1)));
	this->derctivetab.insert(make_pair("LTORG",make_info(0,"",0)));

}

void parser::load_register_tab(){
	register_tab.insert(make_pair("A",0));
	register_tab.insert(make_pair("X",1));
	register_tab.insert(make_pair("L",2));
	register_tab.insert(make_pair("B",3));
	register_tab.insert(make_pair("S",4));
	register_tab.insert(make_pair("T",5));
	register_tab.insert(make_pair("F",6));
	register_tab.insert(make_pair("PC",8));
	register_tab.insert(make_pair("SW",9));
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
	//pair<string,info> p = make_pair(name,i);
}

info parser::make_info(unsigned int f, string s, unsigned int opr) {
	info i(f, s, opr);
	return i;
}
