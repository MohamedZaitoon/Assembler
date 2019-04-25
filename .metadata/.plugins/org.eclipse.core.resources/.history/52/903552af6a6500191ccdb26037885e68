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

string mnemonic_instruction::get_operation(){
	return this->mnemonic;
}
bool mnemonic_instruction::has_label(){
	return !this->label.empty();
}

bool mnemonic_instruction::is_comment(){
	if(this->mnemonic.empty() && !this->comment.empty()) return true;
	return false;
}

void mnemonic_instruction::seterror(string e){
	this->error = e;
}

string mnemonic_instruction::geterror(){
	return this->error;
}
bool mnemonic_instruction::has_error(){
	return !this->error.empty();
}
void mnemonic_instruction::setformat4(bool f){
	this->formate4 = f;
}
bool mnemonic_instruction::is_formate4(){
	return this->formate4;
}
