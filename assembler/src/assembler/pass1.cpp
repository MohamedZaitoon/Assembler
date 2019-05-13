#include "headers/pass1.h"

#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "headers/common.h"
#include "headers/parser.h"
#include "headers/statement.h"

#define ll int

using namespace std;
void handleDerictive(statement& ins, string op, parser& p, string& error);
string read_file(string path);
void writeOutputFile(ofstream& w, int line_no, ll address, statement& x);
void writeError(ofstream& w, string error);
void wirteInitialLine(ofstream& write);
void printSymbolTab(ofstream& w);
bool validLitral(string& literal, string& error);
void setLitral(statement& ins, ofstream& write);
Literal searchLiteral(string lit);
vector<string> code_lines;

ll locctr;
ll startaddrs;

bool foundError = false;
bool useBase;
string baseLabel;
const ll jf = 10;
symbol temp(0, 0, 0, 0);

string pass1(string path) {
	load_tabels();
	symtab.clear();
	intermediate.clear();
	useBase = false;
	cout << "reading file :" + path << ": " + read_file(path) << endl;
	ofstream write;
	write.open(reportfile, ios::trunc);
	if (write.is_open()) {

		int lineno = 0;
		wirteInitialLine(write);
		if (!code_lines.empty()) {
			int siz = code_lines.size();
			parser p;
			int i = 0;
			while (i < siz && regex_match(code_lines[i], rwhite))
				i++;
			if (i < siz) {
				statement ins = p.parse(code_lines[i++]);
				string op = to_upper(ins.getMnemonic());
				if (!ins.is_comment() && !op.compare("START")) { //must check label also
					regex r("^[\\da-fA-F]+$");
					if (regex_match(ins.getOperand(), r)) {

						locctr = hex_to_dec(ins.getOperand());
						writeOutputFile(write, ++lineno, locctr, ins);
						if (ins.has_error())
							writeError(write, ins.getError());
					} else {
						locctr = 0;
						writeOutputFile(write, ++lineno, locctr, ins);
						writeError(write, "invalid operand");
						foundError = true;
					}

					X item(locctr, ins, "");
					//addToIntermediate(item);
					intermediate.push_back(item);
				} else if (ins.is_comment()) {
					locctr = 0;
					write << std::left << setw(jf) << "" << setw(jf) << ""
							<< setw(jf) << ins.getComment() << endl;

				} else {
					locctr = 0;
					writeOutputFile(write, ++lineno, locctr, ins);
					if (ins.has_error())
						writeError(write, ins.getError());

					X item(locctr, ins, "");
					//addToIntermediate(item);
					intermediate.push_back(item);
				}
				bool isltorg = false;
				for (; i < siz; i++) {
					if (regex_match(code_lines[i], rwhite))
						continue;
					ins = p.parse(code_lines[i]);
					string error = "";
					op = to_upper(ins.getMnemonic());
					// handel when the end of operation
					ll l = locctr;
					if (!ins.is_comment()) {
						// search for length of format to add to locctr

						auto it = optab.find(op);
						if (it != optab.end()) {
							info inf = it->second;
							if (inf.formate == 2) {
								locctr += 2;
								temp.address = l;
								temp.length = 2;
								temp.type = temp.byte;
								temp.addressType = temp.reloc;
							} else if (inf.formate == 3) {
								locctr += 3 + ins.isFormate4();
								if (ins.isFormate4()) {
									temp.address = l;
									temp.length = 4;
									temp.type = temp.byte;
									temp.addressType = temp.reloc;
								} else {
									temp.address = l;
									temp.length = 1;
									temp.type = temp.word;
									temp.addressType = temp.reloc;
								}
								//if operand is literal
								if (ins.isLitral()) {
									string lit = ins.getOperand();
									if (validLitral(lit, error)) {
										if (searchLiteral(lit).literal.empty()) {
											string type;
											int length;
											string value = valueOfLitral(
													ins.getOperand(), type);
											if (type == "W") {
												length = 3;
											} else {
												length = value.size() / 2;
											}
											Literal l(lit, value, length, -1);
											addToLittab(l);
										}
									}
								}
							}
						} else {
							it = derctivetab.find(op);
							if (it != derctivetab.end()) {
								if (!op.compare("LTORG")) {
									isltorg = true;
								} else {
									handleDerictive(ins, op, p, error);
								}
							} else { //handle existence during parsing
								foundError = true;
								locctr += 3;
							}
						}

						if (ins.has_label()) {
							//search in symbol table for error
							auto it = symtab.find(to_upper(ins.getLabel()));
							if (it == symtab.end()) {
								//******* handle symbol arguments
								//add to smbol table (label , locct)
								symbol s(temp.address, temp.length, temp.type,
										temp.addressType);
								symtab.insert(
										make_pair(to_upper(ins.getLabel()), s)); // @suppress("Invalid arguments")

							} else {
								error += "Symbol '" + ins.getLabel()
										+ "' is already defined";
							}
						}
						writeOutputFile(write, ++lineno, l, ins);
						if (ins.has_error() || !error.empty()) {
							foundError = true;
							writeError(write, ins.getError() + ", " + error);
						}
						X item(l, ins, "");
						//addToIntermediate(item);
						intermediate.push_back(item);
						//set litrals on output file
						if (isltorg) {
							setLitral(ins, write);
							isltorg = false;
						}
					} //end if not a comment
					else {
						stringstream s;
						s << setfill('0') << setw(6) << dec_to_hex(locctr);
						write << std::left << setw(jf) << lineno << setw(jf)
								<< s.str() << setw(jf) << ins.getComment()
								<< endl;
					}

				}
				//write last line in intermediate file
				//save locctr - starting adrs as prog len
				statement dumy;
				dumy.setMnemonic("LTORG");
				setLitral(dumy, write);
				len = locctr - startaddrs;
			} else {
				write.close();
				return "This file is Empty";
			}
		} else {
			write.close();
			return "This file is Empty";
		}
		if (foundError) {
			write.close();
			write << "pass1 Failed" << endl;
			return "Failed pass1";
		}
	} else {
		return "Failed pass1";
	}
	completePass1 = true;
	printSymbolTab(write);
	write.close();
	write << "pass1 Successful" << endl;
	return "Successful Pass1";
}

string read_file(string path) {
	ifstream in;
	string line;
	code_lines.clear();
	in.open(path);
	if (in.is_open()) {
		while (getline(in, line)) {
			code_lines.push_back(line);
		}
		in.close();
		return "Done";
	} else {
		return "Can't open file";
	}
}

void handleDerictive(statement& ins, string op, parser& p, string& error) {
	string oprnd = to_upper(trim(ins.getOperand()));
	if (!op.compare("START")) {
		temp.address = locctr;
		temp.length = 0;
		temp.type = temp.byte;
		temp.addressType = temp.reloc;
		if (locctr == 0) {
			regex r("^[\\da-fA-F]+$");
			if (regex_match(oprnd, r) && oprnd.size() <= 4) {
				locctr = hex_to_dec(ins.getOperand());
			} else {
				error += " Invalid operand";
				locctr = 0;
			}
		} else {
			error += "duplicate or misplaced START statement, ";
		}
	} else if (!op.compare("END")) {
		temp.address = locctr;
		temp.length = 0;
		temp.type = temp.byte;
		temp.addressType = temp.reloc;
		if (p.assertRegex(oprnd, rlabel)) {
			if (!ins.has_error()) {
				if (symtab.find(oprnd) == symtab.end()) {
					error += "operand must be relocatable address";
				}
			}
		} else if (p.assertRegex(oprnd, rwhite)) {
		} else {
			if (!ins.has_error())
				error += "Invalid operand,";
		}
		if (!trim(ins.getLabel()).empty()) {
			error += "label must be blank in this statement , ";
		}
	} else if (!op.compare("WORD")) {	//handle range
		temp.address = locctr;
		temp.length = 1;
		temp.type = temp.word;
		temp.addressType = temp.reloc;
		locctr += 3;
		if (p.assertRegex(oprnd, rdig)) {
			if (oprnd.size() > 5)
				error += "4 digits at most";
		} else if (p.assertRegex(oprnd, rlabel)) {
			if (symtab.find(oprnd) == symtab.end())
				error += "undefined label operand";
		} else {
			if (!ins.has_error())
				error += "Invalid operand,";
		}

	} else if (!op.compare("BYTE")) {
		temp.address = locctr;
		temp.length = 0;
		temp.type = temp.byte;
		temp.addressType = temp.reloc;
		if (p.assertRegex(ins.getOperand(), rconstant)) {
			smatch sm;
			regex_match(ins.getOperand(), sm, rconstant);
			string c = to_upper(sm[1].str());
			string b = sm[2].str();
			int len = b.length();
			if (len > 15)
				error += "Exceeding the allowed number of constant";
			else if (c.compare("C") == 0) {
				locctr += len;
				temp.length = len;
			} else {
				if (len % 2 == 0) {
					locctr += len / 2;
					temp.length = len / 2;
				} else {
					error += "Hex character must be even,";
				}
			}
		} else {
			if (!ins.has_error())
				error += "Invalid operand,";
		}
	} else if (!op.compare("RESW")) {
		temp.address = locctr;
		temp.length = 0;
		temp.type = temp.word;
		temp.addressType = temp.reloc;
		if (p.assertRegex(ins.getOperand(), rdig)) {
			ll k = ::to_int(ins.getOperand());
			if (k / 10000 == 0) {
				locctr += k * 3;
				temp.length = k;

			} else {
				locctr += 3;
				temp.length = 1;
				error += "operand exceeding the limited rang,";
			}
		} else {
			if (!ins.has_error())
				error += "Invalid operand,";
		}
	} else if (!op.compare("RESB")) {
		temp.address = locctr;
		temp.length = 0;
		temp.type = temp.byte;
		temp.addressType = temp.reloc;
		if (p.assertRegex(ins.getOperand(), rdig)) {
			ll k = ::to_int(ins.getOperand());
			if (k / 10000 == 0) {
				locctr += k;
				temp.length = k;
			} else {
				locctr += 1;
				temp.length = 1;
				error += "operand exceeding the limited rang,";
			}
		} else {
			if (!ins.has_error())
				error += "Invalid operand,";
		}
	} else if (!op.compare("EQU")) {
		temp.address = locctr;
		temp.length = 0;
		temp.type = temp.byte;
		temp.addressType = temp.reloc;
		if (!ins.has_label()) {
			error += "this statement requires a label,";
		}
		if (p.assertRegex(oprnd, rdig)) {
			temp.address = ::to_int(oprnd);
			temp.addressType = temp.absol;
			if (oprnd.size() > 4)
				error += "4 digits at most for operand,";
		} else if (p.assertRegex(oprnd, rlabel)) {
			auto it = symtab.find(oprnd);
			if (it == symtab.end())
				error += "undefined label operand,";
			else {
				temp.address = it->second.address; // @suppress("Field cannot be resolved")
				temp.addressType = it->second.addressType; // @suppress("Field cannot be resolved")
			}
		} else if (p.assertRegex(oprnd, rexp)) {
			smatch sm;
			regex_match(oprnd, sm, rexp);
			string first = sm[1].str();
			string o = sm[2].str();
			string second = sm[3].str();
			bool type = 0;
			int result = calculate(first, second, o, error, temp.address, type);
			if (error.empty()) {
				temp.address = result;
				temp.addressType = type;
			}
		} else {
			if (!ins.has_error())
				error += "Invalid operand,";
		}

	} else if (!op.compare("ORG")) {
		temp.address = locctr;
		temp.length = 0;
		temp.type = temp.byte;
		temp.addressType = temp.reloc;
		if (ins.has_label()) {
			error += "this statement can not have a label, ";
		}
		if (p.assertRegex(oprnd, rdig)) {
			error += "address expression is not relocatable, ";
		} else if (p.assertRegex(oprnd, rlabel)) {
			auto it = symtab.find(oprnd);
			if (it == symtab.end())
				error += "undefined label operand,";
			else {
				locctr = it->second.address; // @suppress("Field cannot be resolved")
			}
		} else if (oprnd.compare("*")) {
		} else if (p.assertRegex(oprnd, rexp)) {
			smatch sm;
			regex_match(oprnd, sm, rexp);
			string first = sm[1].str();
			string o = sm[2].str();
			string second = sm[3].str();
			bool type = 0;
			int result = calculate(first, second, o, error, temp.address, type);
			if (error.empty()) {
				temp.address = result;
				if (type == temp.reloc)
					temp.addressType = type;
				else {
					error +=
							"result address of expression is not relocatable, ";
				}
			}
		} else {
			if (!ins.has_error())
				error += "Invalid operand,";
		}

	} else if (!op.compare("BASE")) {
		useBase = true;
		temp.address = locctr;
		temp.length = 0;
		temp.type = temp.byte;
		temp.addressType = temp.reloc;
		if (ins.has_label()) {
			error += "this statement can not have a label, ";
		}
		if (p.assertRegex(oprnd, rdig)) {
			error += "address expression is not relocatable, ";
		} else if (p.assertRegex(oprnd, rlabel)) {
			baseLabel = oprnd;
		} else if (oprnd.compare("*")) {
			baseLabel = "*";
		} else {

			if (!ins.has_error())
				error += "Invalid operand,";
		}
	} else if (!op.compare("NOBASE")) {
		useBase = false;
		if (ins.has_label()) {
			error += "this statement can not have a label, ";
		}
		if (!trim(oprnd).empty()) {
			error += "this statement can not have an operand, ";
		}
	} else {

	}

}

void writeOutputFile(ofstream& w, int line_no, ll address, statement& x) {
	string plus = "";
	if (x.isFormate4())
		plus = "+";
	stringstream s;
	s << setfill('0') << setw(6) << dec_to_hex(address);

	if (w.is_open()) {
		w << std::left << setw(jf) << line_no << setw(jf) << s.str() << setw(jf)
				<< x.getLabel() << setw(jf) << plus + x.getMnemonic()
				<< setw(jf) << trim(x.getOperand()) << setw(jf)
				<< x.getComment() << endl;
	}
}

void writeError(ofstream& w, string error) {

	if (w.is_open())
		w << "***Error: " << error << endl;
}
void wirteInitialLine(ofstream& write) {
	write << std::left << setw(jf) << "line no." << setw(jf) << "address"
			<< setw(jf) << "label" << setw(jf) << "op-code" << setw(jf)
			<< "operand" << setw(jf) << "comment" << endl;
}

void printSymbolTab(ofstream& w) {
	if (symtab.empty())
		return;
	w << std::left << setw(jf * 2) << "" << setw(5)
			<< "Symbol Table (value in decimal)" << endl << endl;
	w << std::left << setw(jf) << "" << setw(jf) << "Name" << setw(jf)
			<< "Value" << setw(jf) << "Reloc/Absol" << endl;
	w << std::left << setw(jf) << "" << setfill('-') << setw(jf * 3) << endl;
	w << setfill(' ');
	string s;
	for (auto entry : symtab) {
		s = ((entry.second.addressType == entry.second.absol) ? // @suppress("Field cannot be resolved")
		"Absolute" : "Relocatable");
		w << std::left << setw(jf) << "" << setw(jf) << entry.first << setw(jf)
				<< entry.second.address << setw(jf) << s << endl; // @suppress("Invalid overload") // @suppress("Field cannot be resolved")
	}
}

bool validLitral(string& literal, string& error) {
	smatch sm;
	string value = "";
	bool flag = false;
	if (regex_match(literal, sm, word)) {
		string c = sm[2].str();
		literal = sm[1].str() + "'" + c + "'";
		regex_match(c, sm, number);
		string value = sm[1].str();
		if (value.size() > 4) {
			error += "word must consists of 4 digit,";
			flag = false;
		} else
			flag = true;
	} else if (regex_match(literal, sm, chars)) {
		literal = sm[1].str() + sm[2].str();
		if (sm[2].str().size() > 3) {
			error += "incorrect length for literal,";
			flag = false;
		} else
			flag = true;
	} else if (regex_match(literal, sm, rlhex)) {
		literal = sm[1].str() + sm[2].str();
		if (sm[2].str().size() > 6) {
			error += "incorrect length for literal ,";
			flag = false;
		} else
			flag = true;
	} else {
		error += "Invalid literal, ";
	}
	return flag;
}
void setLitral(statement& ins, ofstream& write) {
	if (ins.has_label()) {
		writeError(write, "label field must be blank for this statement ");
		foundError = true;
	}
	if (!ins.getOperand().empty()) {
		writeError(write, "operand field must be blank for this statement ");
		foundError = true;
	}
	int sz = littab.size();
	int i = sz;
	for (i = sz - 1; i >= 0; i--) {
		if (littab[i].address == -1)
			continue;
		break;
	}
	i++;

	while (i < sz) {
		stringstream s;
		littab[i].address = locctr;
		statement st("Literal", "=>", littab[i].literal, "");
		X item(locctr, st, littab[i].value);
		//addToIntermediate(item);
		intermediate.push_back(item);
		s << setfill('0') << setw(6) << dec_to_hex(locctr);
		write << std::left << setw(jf) << "" << setw(jf) << s.str() << setw(jf)
				<< littab[i].value << setw(jf) << "literal" << setw(jf)
				<< littab[i].literal << endl;
		locctr += littab[i].length;
		i++;
	}
}

ll calculate(string a, string b, string op, string& error, ll curloc,
		bool& type) {
	bool ta, tb;
	ll valueA, valueB;
	getValue(a, valueA, ta, error, curloc);
	getValue(b, valueB, tb, error, curloc);
	//cout<<a<<" "<<valueA<<" "<<ta<<endl;
	//cout<<b<<" "<<valueB<<" "<<tb<<endl;
	if (error.empty()) {
		if (!op.compare("+")) {
			if ((ta == temp.absol && (tb == temp.reloc || tb == temp.absol))
					|| (tb == temp.absol
							&& (ta == temp.reloc || ta == temp.absol))) {
				type = ta & tb;
				return valueA + valueB;
			} else {
				error += "Adding 2 relocatable is not allowed";
				return -1;
			}
		} else if (!op.compare("-")) {
			if (ta == tb) {
				type = temp.absol;
				return valueA - valueB;
			} else {
				error += "Illegal expression";
				return -1;
			}
		} else if (!op.compare("*")) {
			if (ta == tb && ta == temp.absol) {
				type = temp.absol;
				return valueA * valueB;
			} else {
				error += "Illegal expression";
				return -1;
			}
		} else if (!op.compare("/")) {
			if (ta == tb && ta == temp.absol && valueB != 0) {
				type = temp.absol;
				return valueA / valueB;
			} else {
				error += "Illegal expression";
				return -1;
			}
		}
	}
	return -1;
}
void getValue(string x, ll& value, bool& ta, string& error, ll curloc) {
	if (regex_match(x, rdig)) {
		ta = temp.absol;
		value = to_int(x);
	} else if (!x.compare("*")) {
		ta = temp.reloc;
		value = curloc;
	} else {
		x = to_upper(x);
		auto it = symtab.find(x);
		if (it != symtab.end()) {
			symbol sy = it->second;
			ta = sy.addressType;
			value = sy.address;
			//cout<<x<<" "<<value<<endl;
		} else {
			value = -1;
			error += x + " undefined symbol, ";
		}
	}
}
template<typename T>
ll to_int(T h) {
	ll l;
	stringstream ss;
	ss << h;
	ss >> l;
	return l;
}
template<typename T>
string dec_to_hex(T d) {
	stringstream ss;
	ss << std::hex << d;
	return ss.str();
}

template<typename T>
ll hex_to_dec(T h) {
	ll l;
	stringstream ss;
	ss << h;
	ss >> std::hex >> l;
	return l;
}

