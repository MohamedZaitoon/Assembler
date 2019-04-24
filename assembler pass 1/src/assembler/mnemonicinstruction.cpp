#include "mnemonicinstruction.h"
#include <string>

mnemonic_instruction::mnemonic_instruction(string label,string mnemonic,string operand,string comment) {
	this->label = label;
	this->mnemonic = mnemonic;
	this->operand = operand;
	this->comment = comment;
	this->formate4 = false;
}

mnemonic_instruction::mnemonic_instruction(){
	this->formate4 = false;
}

mnemonic_instruction::~mnemonic_instruction() {
}

bool mnemonic_instruction::has_label(){
	return !this->label.empty();
}

bool mnemonic_instruction::is_comment(){
	if(this->mnemonic.empty() && !this->comment.empty()) return true;
	return false;
}

bool mnemonic_instruction::has_error(){
	return !this->error.empty();
}

