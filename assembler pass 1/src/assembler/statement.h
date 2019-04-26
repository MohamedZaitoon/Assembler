#ifndef ASSEMBLER_STATEMENT_H_
#define ASSEMBLER_STATEMENT_H_
#include <string>
using namespace std;
class statement {
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
	string litral;

public:
	statement(string label,string mnemonic,string operand,string comment);
	statement();
	virtual ~statement();

	bool is_comment();
	bool has_label();
	bool has_error();

	bool operand_is_litral();
	const string& getComment() const {
		return comment;
	}

	void setComment(const string& comment) {
		this->comment = comment;
	}

	const string& getError() const {
		return error;
	}

	void setError(const string& error) {
		this->error = error;
	}

	bool isFormate4() const {
		return formate4;
	}

	void setFormate4(bool formate4) {
		this->formate4 = formate4;
	}

	const string& getLabel() const {
		return label;
	}

	void setLabel(const string& label) {
		this->label = label;
	}

	const string& getMnemonic() const {
		return mnemonic;
	}

	void setMnemonic(const string& mnemonic) {
		this->mnemonic = mnemonic;
	}

	const string& getOperand() const {
		return operand;
	}

	void setOperand(const string& operand) {
		this->operand = operand;
	}

	const string& getLitral() const {
		return litral;
	}

	void setLitral(const string& litral) {
		this->litral = litral;
	}
};

#endif
