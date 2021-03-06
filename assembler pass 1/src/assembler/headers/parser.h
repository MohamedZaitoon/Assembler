#ifndef ASSEMBLER_PARSER_H_
#define ASSEMBLER_PARSER_H_
#include <string>
#include <map>
#include <regex>
#include "statement.h"

	const regex rcomment("^\\s*[\\.]([\\s\\S]*)");

	const regex ropcode("^\\s*([+])?([\\S]+)\\s*$");

	const regex roperand("^([#@=*])?\\s*([\\w]+)\\s*([,])?\\s*(\\S+)?$");

	const regex rlitral("^[=]\\s*([cCxX])\\s*['](\\w+)[']\\s*$");

	const regex rconstant("^s*([cCxX])\\s*[']([\\S\\s]+)[']\\s*$");

	const regex rlabel("(^\\s*[a-zA-Z][\\w]*)\\s*$");

	const regex rexp("^\\s*([a-zA-Z][\\w]*|[\\d]+)\\s*([+*-\\/])"
			"\\s*([a-zA-Z][\\w]*|[\\d]+)\\s*$");

	const regex rdig("^\\s*\\d+\\s*$");

	const regex r2operand("^\\s*([a-zA-Z][\\w]*|[\\d]+)\\s*[,]\\s*([a-zA-Z][\\w]*|[\\d]+)\\s*$");

	const regex rimmd_ind("^\\s*[#@]\\s*([a-zA-Z][\\w]*|[\\d]+|[*])\\s*$");

	const regex rinstruction("^\\s*(?:(\\S+)?\\s+)?"
			"(\\s*[+]?[\\w]+)\\s*"
			"(?:\\s+([^.]+)?\\s*)?"
			"(?:\\s+(\\.[\\s\\S]+)?)?$");
	const regex rwhite("^[^\\S]*$");
	const regex rhex("^[\\da-fA-F]+$");

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
	map<string, info> derctivetab;
	map<string, int> register_tab;

	parser();
	virtual ~parser();
	statement parse(string ins);
	bool assertRegex(string s, regex r);
	bool assertReservedWord(string op);
	bool assertRegister(string s);

private:
	void load_optab();
	void add(string name, info i);
	info make_info(unsigned int f, string s, unsigned int opr);
	void check_mnemonic(statement&x, string op);
	void one_field(statement*x, string field);
	void two_field(statement*x, vector<string>& fields);
	void three_field(statement*x, vector<string>& fields);
	void four_field(statement*x, vector<string>& fields);
	void valid_syntax(statement& x);
	void load_register_tab();
	vector<string> get_groups(smatch m);
	void load_derictve();

};

#endif /* ASSEMBLER_PARSER_H_ */
