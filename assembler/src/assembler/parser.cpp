#include "headers/parser.h"

#include <iterator>
#include <unordered_map>
#include <vector>

using namespace std;
parser::parser() {
}

parser::~parser() {
}

statement parser::parse(string ins) {
	statement x;
	smatch result;
	if (regex_match(ins, result, rcomment)) {
		x.setComment(*result.begin());
		return x;
	} else if (regex_search(ins, result, rinstruction)) {
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
		x.setComment(ins);
		invalid: x.setError("Invalid instruction syntax");
		return x;
	}
	return x;
}
void parser::one_field(statement* x, string field) {
	smatch m;
	if (regex_search(field, m, ropcode)) {
		vector<string> groups = get_groups(m);
		if (groups.size() == 2) {
			x->setFormate4(true);
			string op = groups[1];
			x->setMnemonic(op);
		} else if (groups.size() == 1) {
			string op = groups[0];
			x->setMnemonic(op);
		}
	}

}

void parser::two_field(statement* x, vector<string>& fields) {
	smatch m;
	if (regex_match(fields[0], m, ropcode)) { // either label or opcode
		vector<string> v = get_groups(m);
		if (v.size() == 2) { //absolutely  opcde format 4
			x->setFormate4(true);
			x->setMnemonic(v[1]);
			smatch sm;
			if (regex_match(fields[1], sm, rcomment)) {
				x->setComment(sm[1].str());
			} else {
				x->setOperand(fields[1]);
			}
		} else if (v.size() == 1) { // either label or opcode
			if (!assertReservedWord(v[0])) { //case first field label
				x->setLabel(v[0]);
				one_field(x, fields[1]);
			} else {
				one_field(x, fields[0]);
				smatch sm;
				if (regex_match(fields[1], sm, rcomment)) {
					x->setComment(sm[1].str());
				} else {
					x->setOperand(fields[1]);
				}
			}
		}
	}

}
void parser::three_field(statement* x, vector<string>& fields) {
	vector<string> y(2);
	y[0] = fields[0];
	y[1] = fields[1];
	two_field(x, y);
	smatch sm;
	if (regex_match(fields[2], sm, rcomment)) {
		x->setComment(sm.str());
	} else {
		if (x->getOperand().empty())
			x->setOperand(fields[2]);
		else
			x->setOperand(x->getOperand() + " " + fields[2]);
	}
}
void parser::four_field(statement* x, vector<string>& fields) {
	x->setLabel(fields[0]);
	one_field(x, fields[1]);
	x->setOperand(fields[2]);
	x->setComment(fields[3]);
}

void parser::valid_syntax(statement& x) {
	string oprnd = to_upper(x.getOperand());
	string op = to_upper(x.getMnemonic());
	if (x.has_label()) {
		if (!regex_match(x.getLabel(), rlabel)) { //label must begin with character
			x.setError(
					"label must begin with alphabetic character, "
							+ x.getError());
		}
		if (assertReservedWord(x.getLabel())) { //label is not reserved word
			x.setError("label must not be a reserved word, " + x.getError());
		}
	}
	check_mnemonic(x, op);

	auto it = optab.find(op);
	if (it != optab.end()) {
		if (!trim(x.getOperand()).empty()) { //operand validation
			smatch sm;
			string operand = trim(x.getOperand());
			if (regex_match(operand, sm, roperand)) {
				string perfix = trim(sm[1].str());
				string first = trim(to_upper(sm[2].str()));
				string comma = trim(sm[3].str());
				string second = trim(to_upper(sm[4].str()));
				int oprs = it->second.operands;
				if (assertRegex(operand, rdig)
						|| assertRegex(operand, rlabel)) {
				} else if (assertRegex(operand, rimmd_ind)) {
				} else if (assertRegex(operand, r2operand)) {
					if (oprs == 2) {
						if (!(assertRegister(first) && assertRegister(second))) {
							x.setError(
									x.getError()
											+ " operands must be a register,");
						}
					} else if (assertReservedWord(first)) {
						x.setError(
								x.getError()
										+ " operand must be not a reserved word,");
					} else if (second.compare("X") != 0) {
						x.setError(
								x.getError()
										+ " operand must be a register X not a "
										+ second + ",");
					}
				} else if (trim(operand).compare("*") == 0) {
					if (!first.empty() || !comma.empty() || second.empty()) {
						x.setError(
								x.getError()
										+ " extra characters at end of statement,");
					}
				} else if (comma.empty() && !second.empty()) {
					x.setError(x.getError() + "Invalid operand, ");
				} else {
					x.setError(
							x.getError()
									+ " extra characters at end of statement,");
				}
			} else if (trim(operand).compare("*") == 0) {
			} else if (assertRegex(operand, rlitral)) {
				x.setLitral(true);
			} else if (assertRegex(operand, rconstant)) {
			}else if (assertRegex(operand, rexp)) {
			} else {
				x.setError(x.getError() + " invalid operand,");
			}
		} else if (it->second.operands >= 1) { //check missed operand of this opcode
			x.setError(x.getError() + "Missed operand,");
		}
	} else {
		it = derctivetab.find(op);
		if (it != derctivetab.end()) {
			if (!(assertRegex(oprnd, rdig) || assertRegex(oprnd, rlabel)
					|| assertRegex(oprnd, rexp) || assertRegex(oprnd, rconstant)
					|| assertRegex(oprnd, rwhite) || assertRegex(oprnd, rhex))) {
				x.setError("Invalid operand ," + x.getError());
			}
		}
	}
}
bool parser::assertRegex(string s, regex r) {
	return regex_match(s, r);
}
bool parser::assertReservedWord(string op) {
	string s = to_upper(op);
	auto it1 = optab.find(s);
	auto it2 = derctivetab.find(to_upper(s));
	if (it1 != optab.end() || it2 != derctivetab.end()) {
		return true;
	}
	return false;
}
bool parser::assertRegister(string s) {
	string o = to_upper(s);
	return register_tab.find(o) != register_tab.end();

}
/**
 * check that:
 * 		is op in optab
 * 		format 2 does not come with format 4
 *
 */
void parser::check_mnemonic(statement&x, string op) {
	auto entry = optab.find(to_upper(op));
	if (entry != optab.end()) {
		info t = entry->second;
		if (x.isFormate4() && t.formate == 2)
			x.setError(op + " cant't be format 4");

	} else {
		entry = derctivetab.find(to_upper(op));
		if (entry != derctivetab.end()) {
			info t = entry->second;
			if (x.isFormate4())
				x.setError(op + " cant't be format 4,");

		} else
			x.setError("invalid mnemonic instruction, ");
	}
}

vector<string> parser::get_groups(smatch m) {
	vector<string> v;
	int sz = m.size();
	for (int i = 1; i < sz; i++) {
		string re = m[i].str();
		if (!re.empty()) {
			v.push_back(re);
		}
	}
	return v;
}

