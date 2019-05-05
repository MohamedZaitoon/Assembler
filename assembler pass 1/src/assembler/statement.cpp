#include "headers/statement.h"

#include <string>

statement::statement(string label,string mnemonic,string operand,string comment) {
	this->label = label;
	this->mnemonic = mnemonic;
	this->operand = operand;
	this->comment = comment;
	this->formate4 = false;

}

statement::statement(){
	this->formate4 = false;
}

statement::~statement() {
}

bool statement::has_label(){
	return !this->label.empty();
}

bool statement::is_comment(){
	if(this->mnemonic.empty() && !this->comment.empty()) return true;
	return false;
}

bool statement::has_error(){
	return !this->error.empty();
}

