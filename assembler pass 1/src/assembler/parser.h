#ifndef ASSEMBLER_PARSER_H_
#define ASSEMBLER_PARSER_H_
#include "mnemonicinstruction.h"
#include <string>
#include <map>
#include <regex>

class info {

public:
	unsigned int formate :4;
	string opcode;
	unsigned int operands :2;
	info(unsigned int f, string opc, unsigned int opr) {
		this->formate = f;
		this->opcode = opc;
		this->operands = opr;
	}
};
class parser {

public:
	map<string, info> optab;
	parser();
	virtual ~parser();
	mnemonic_instruction parse(string ins);
private:
	void load_optab();
	void add(string name, info i);
	info make_info(unsigned int f, string s, unsigned int opr);
	void check_mnemonic(mnemonic_instruction*x,string op,int noOperand);
	void one_field(mnemonic_instruction*x, string field);
	void two_field(mnemonic_instruction*x,smatch field);
	void three_field(mnemonic_instruction*x,smatch field);
	void four_field(mnemonic_instruction*x,smatch field);
};

#endif /* ASSEMBLER_PARSER_H_ */
