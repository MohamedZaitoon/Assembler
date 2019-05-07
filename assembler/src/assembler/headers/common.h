#ifndef ASSEMBLER_HEADERS_COMMON_H_
#define ASSEMBLER_HEADERS_COMMON_H_
#define ll int
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>

using namespace std;
class Literal {
public:
	string literal;
	string value;
	int length;
	int address;
	Literal(string literal, string value, int length, int address) {
		this->literal = literal;
		this->value = value;
		this->length = length;
		this->address = address;
	}
};

class X {
public:
	ll locctr;
	string label;
	string mnemonic;
	string operand;
	X() :
			locctr(0), label(""), mnemonic(""), operand("") {

	}
	X(ll locctr, string label, string mnemonic, string operand) {
		this->locctr = locctr;
		this->label = label;
		this->mnemonic = mnemonic;
		this->operand = operand;
	}
};

class symbol {
public:
	const bool byte = false;
	const bool word = true;
	const bool reloc = false;
	const bool absol = true;ll address;ll length;
	bool type;
	bool addressType;
	symbol() :
			address(0), length(0), type(0), addressType(0) {
	}
	symbol(ll adrs, ll len, bool type, bool flag) {
		this->address = adrs;
		this->length = len;
		this->type = type;
		this->addressType = flag;
	}
};

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

extern unordered_map<string, info> optab;
extern unordered_map<string, info> derctivetab;
extern unordered_map<string, int> register_tab;
extern unordered_map<string, symbol> symtab;
extern vector<X> intermediate;
extern vector<Literal> littab;
extern bool completePass1;
extern const string reportfile;
extern ll len;

const regex word("^\\s*[=]\\s*([Ww])\\s*'([-]?[\\d]+)'\\s*$");
const regex number("^[-]?([\\d]+)$");
const regex chars("^\\s*[=]\\s*([cC])\\s*'([\\s\\S]+)'$");
const regex rlhex("^\\s*[=]\\s*([xX])\\s*'([\\da-fA-F]+)'$");

void addToIntermediate(X& item);
void addToLittab(Literal& item);
void load_tabels();
void load_optab();
void load_register_tab();
void load_derictve();
string valueOfLitral(string s);

string trim(const string& str);
string to_upper(string s);
string to_string(int d);

template<typename T>
ll hex_to_dec(T h);



#endif /* ASSEMBLER_HEADERS_COMMON_H_ */
