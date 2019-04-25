#ifndef ASSEMBLER_MNEMONICINSTRUCTION_H_
#define ASSEMBLER_MNEMONICINSTRUCTION_H_
#include <string>
using namespace std;
class mnemonic_instruction {
	//length of fields
	const static int LENLABEL = 7;
	const static int LENMNEMONIC = 6;
	const static int LENOPERAND = 18;
	const static int LENCOMMENT = 30;

	private:
	string label;
	string mnemonic;
	string operand;
	string comment;
	string error;
	bool formate4;
public:
	mnemonic_instruction(string label,string mnemonic,string operand,string comment);
	mnemonic_instruction();
	virtual ~mnemonic_instruction();

	bool is_comment();
	bool has_label();

	string get_operation();

	void seterror(string e);
	string geterror();
	bool has_error();

	bool is_formate4();
	void setformat4(bool f);
};

#endif
