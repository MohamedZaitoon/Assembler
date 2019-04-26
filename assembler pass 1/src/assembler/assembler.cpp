#include "assembler.h"
#include <fstream>
#include <vector>
#include "parser.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <map>
#include <iomanip>

#include "statement.h"
#define ll int

using namespace std;
void handleDerictive(statement& ins, string op, parser& p, string& error);

template<typename T>
long long hex_to_dec(T h);

template<typename T>
ll to_long(T h);

template<typename T>
string dec_to_hex(T d);

string to_string(int d);

string to_upper(string s);
string read_file(string path);
void writeListFile(ofstream& w, int line_no, ll address, statement& x) ;
void writeCommentLine(ofstream& w, int line_no, ll address,statement& x);
void writeError(ofstream& w, string error);
struct symbol {
	const bool byte = false;
	const bool word = true;
	const bool reloc = false;
	const bool absol = true;ll address;
	int length;
	bool type;
	bool flag;
	symbol(ll adrs, int len, bool type, bool flag) {
		this->address = adrs;
		this->length = len;
		this->type = type;
		this->flag = flag;
	}
};


vector<string> code_lines;

map<string, symbol> symtab;

ll locctr;
ll startaddrs;
ll len;
const int jf = 10;



string pass1(string path) {
	cout << "reading file :" + path << ": " + read_file(path) << endl;
	ofstream write;
	write.open("LISTFILE.txt", ios::trunc);
	if (write.is_open()) {
		bool foundError = false;
		int lineno = 0;
		write << setw(3 * jf + 10) << "pass1" << endl;
		write << std::left << setw(jf) << "line no." << setw(jf) << "address"
				<< setw(jf) << "label" << setw(jf) << "op-code" << setw(jf)
				<< "operand" << setw(jf) << "comment" << endl;
		if (!code_lines.empty()) {
			int siz = code_lines.size();
			parser p;
			statement ins = p.parse(code_lines[0]);
			string op = to_upper(ins.getMnemonic());
			cout << op << endl;
			if (!ins.is_comment() && !op.compare("START")) { //must check label also
				regex r("^[\\da-fA-F]+$");
				if (regex_match(ins.getOperand(), r)) {
					locctr = hex_to_dec(ins.getOperand());
					writeListFile(write, ++lineno, locctr, ins);
				} else {
					writeError(write, "invalid operand");
					foundError = true;
				}
			} else if (ins.is_comment()) {
				write << std::left << setw(jf) << "" << setw(jf) << ""
						<< setw(jf) << ins.getComment() << endl;
				locctr = 0;
			} else {
				locctr = 0;
			}
			for (int i = 1; i < siz; i++) {
				ins = p.parse(code_lines[i]);
				string error = "";
				op = to_upper(ins.getMnemonic());
				if (!op.compare("END")) {
					writeListFile(write, i + 1, locctr, ins);
					if (ins.has_error()) {
						writeError(write, ins.getError());
						foundError = true;
					}
					break;
				} // handel when the end of operation
				if (!ins.is_comment()) {
					if (ins.has_label()) {
						//search in symbol table for error
						auto it = symtab.find(ins.getLabel());
						if (it != symtab.end()) {
							error += "Symbol '" + ins.getLabel()
									+ "' is already defined";
						} else { //******* handle symbol arguments
							//add to smbol table (label , locct)
							symbol s(locctr, 1, s.word, s.reloc);
							symtab.insert(make_pair(ins.getLabel(), s));
						}
					}
					// search for length of format to add to locctr
					ll l = locctr;
					auto it = p.optab.find(op);
					if (it != p.optab.end()) {
						info inf = it->second;
						if (inf.formate == 2) {
							locctr += 2;
						} else if (inf.formate == 3) {
							locctr += 3 + ins.isFormate4();
						}
					} else {
						it = p.derctivetab.find(op);
						if (it != p.derctivetab.end()) {
								handleDerictive(ins,op,p,error);
						} else {
							foundError = true;
						}
					}
					writeListFile(write, ++lineno, l, ins);
					if (ins.has_error() || !error.empty())
						writeError(write, ins.getError() + ", " + error);
				} //end if not a comment
				else {
					write << std::left << setw(jf) << "" << setw(jf) << ""
							<< setw(jf) << ins.getComment() << endl;
				}

			}
			//write last line in intermediate file
			//save locctr - starting adrs as prog len
			len = locctr - startaddrs;
		} else {
			write.close();
			return "Can't assemble this file";
		}
	}
	write.close();

	return "done";
}
string to_upper(string s) {
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
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
	if (!op.compare("WORD")) {			//handle range
		locctr += 3;
	} else if (!op.compare("BYTE")) {
		if (p.assertConstant(ins.getOperand())) {
			smatch sm;
			regex_match(ins.getOperand(), sm, rconstant);
			string c = to_upper(sm[1].str());
			string b = sm[2].str();
			int len = b.length();
			if (len > 15)
				error += "Exceeding the allowed number of constant";
			else if (c.compare("C") == 0) {
				locctr += len;
			} else {
				if (len % 2 == 0)
					locctr += len / 2;
				else {
					error += "Hex character must be even,";
				}
			}
		}
	} else if (!op.compare("RESW")) {
		if (p.assertDigits(ins.getOperand())) {
			ll k = to_long(ins.getOperand());
			locctr += k * 3;
		}
	} else if (!op.compare("RESB")) {
		if (p.assertDigits(ins.getOperand())) {
			ll k = to_long(ins.getOperand());
			locctr += k;
		}
	} else if (!op.compare("EQU")) {

	} else if (!op.compare("ORG")) {

	} else if (!op.compare("LTORG")) {

	}

}

void writeListFile(ofstream& w, int line_no, ll address, statement& x) {
	string plus = "";
	if (x.isFormate4())
		plus = "+";

	stringstream s;
	s << setfill('0') << setw(6) << dec_to_hex(address);

	if (w.is_open()) {
		w << std::left << setw(jf) << line_no << setw(jf) << s.str() << setw(jf)
				<< x.getLabel() << setw(jf) << plus + x.getMnemonic()
				<< setw(jf) << x.getOperand() << setw(jf) << x.getComment()
				<< endl;
	}
}
void writeCommentLine(ofstream& w, int line_no, ll address,
		statement& x) {

	if (w.is_open())
		w << std::left << setw(jf) << line_no << setw(jf) << dec_to_hex(address)
				<< setw(jf) << x.getComment() << endl;
}

void writeError(ofstream& w, string error) {

	if (w.is_open())
		w << "***Error: " << error << endl;
}


template<typename T>
long long hex_to_dec(T h) {
	long long l;
	stringstream ss;
	ss << h;
	ss >> std::hex >> l;
	return l;
}

template<typename T>
ll to_long(T h) {
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
string to_string(int d) {
	stringstream ss;
	ss << d;
	return ss.str();
}
