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
string read_file(string path);
void writeListFile(ofstream& w, int line_no, ll address, statement& x);
void writeError(ofstream& w, string error);
void wirteInitialLine(ofstream& write);
void printSymbolTab(ofstream& w);

string trim(const string& str);
template<typename T>
long long hex_to_dec(T h);
template<typename T>
ll to_int(T h);
template<typename T>
string dec_to_hex(T d);
string to_string(int d);
string to_upper(string s);

struct symbol {
	const bool byte = false;
	const bool word = true;
	const bool reloc = false;
	const bool absol = true;ll address;ll length;
	bool type;
	bool addressType;
	symbol(ll adrs, ll len, bool type, bool flag) {
		this->address = adrs;
		this->length = len;
		this->type = type;
		this->addressType = flag;
	}
};

vector<string> code_lines;
map<string, symbol> symtab;
ll locctr;
ll startaddrs;
ll len;
bool useBase;
string baseLabel;
const int jf = 10;
symbol temp(0, 0, 0, 0);

string pass1(string path) {
	cout << "reading file :" + path << ": " + read_file(path) << endl;
	ofstream write;
	write.open("LISTFILE.txt", ios::trunc);
	if (write.is_open()) {
		bool foundError = false;
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
					writeListFile(write, ++lineno, locctr, ins);
					if (regex_match(ins.getOperand(), r)) {
						locctr = hex_to_dec(ins.getOperand());
					} else {
						writeError(write, "invalid operand");
						locctr = 0;
						foundError = true;
					}
				} else if (ins.is_comment()) {
					write << std::left << setw(jf) << "" << setw(jf) << ""
							<< setw(jf) << ins.getComment() << endl;
					locctr = 0;
				} else {
					writeListFile(write, ++lineno, locctr, ins);
					if (!ins.has_error())
						writeError(write, ins.getError());
					locctr = 0;
				}
				for (; i < siz; i++) {
					if (regex_match(code_lines[i], rwhite))
						continue;
					ins = p.parse(code_lines[i]);
					string error = "";
					op = to_upper(ins.getMnemonic());
					// handel when the end of operation
					if (!ins.is_comment()) {
						// search for length of format to add to locctr
						ll l = locctr;
						auto it = p.optab.find(op);
						if (it != p.optab.end()) {
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
							}
						} else {
							it = p.derctivetab.find(op);
							if (it != p.derctivetab.end()) {
								handleDerictive(ins, op, p, error);
							} else { //handle existence during parsing
								foundError = true;
								locctr += 3;
							}
						}

						if (ins.has_label()) {
							//search in symbol table for error
							auto it = symtab.find(ins.getLabel());
							if (it != symtab.end()) {
								error += "Symbol '" + ins.getLabel()
										+ "' is already defined";
							} else { //******* handle symbol arguments
								//add to smbol table (label , locct)
								symbol s(temp.address, temp.length, temp.type,
										temp.addressType);
								symtab.insert(make_pair(ins.getLabel(), s));
							}
						}
						writeListFile(write, ++lineno, l, ins);
						if (ins.has_error() || !error.empty()){
							foundError = true;
							writeError(write, ins.getError() + ", " + error);
						}
					} //end if not a comment
					else {
						write << std::left << setw(jf) << "" << setw(jf) << ""
								<< setw(jf) << dec_to_hex(locctr) << setw(jf)
								<< ins.getComment() << endl;
					}

				}
				//write last line in intermediate file
				//save locctr - starting adrs as prog len
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
	}else{
		return "Failed pass1";
	}
	printSymbolTab(write);
	write.close();
	write << "pass1 Successful" << endl;
	return "Successful Pass1";
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
	string oprnd = ins.getOperand();
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
	} else if (!op.compare("WORD")) {	//handle range
		temp.address = locctr;
		temp.length = 1;
		temp.type = temp.word;
		temp.addressType = temp.reloc;
		locctr += 3;
		if (p.assertRegex(oprnd, rdig)) {
			if (oprnd.size() > 4)
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
			ll k = to_int(ins.getOperand());
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
			ll k = to_int(ins.getOperand());
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
			temp.address = to_int(oprnd);
			temp.addressType = temp.absol;
			if (oprnd.size() > 4)
				error += "4 digits at most for operand,";
		} else if (p.assertRegex(oprnd, rlabel)) {
			auto it = symtab.find(oprnd);
			if (it == symtab.end())
				error += "undefined label operand,";
			else {
				temp.address = it->second.address;
			}
		} /*else if (p.assertRegex(oprnd,rexp)){
		 smatch sm;
		 regex_match(oprnd,sm,rexp);
		 string first = sm[1].str();
		 string o = sm[2].str();
		 string second= sm[3].str();
		 int result = calculat(first,secod,o);
		 }*/else {
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
				locctr = it->second.address;
			}
		} else {
			if (!ins.has_error())
				error += "Invalid operand,";
		}

	} else if (!op.compare("LTORG")) {
		//litralSetup();
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
		} else {
			if (!ins.has_error())
				error += "Invalid operand,";
		}
	} else if (!op.compare("NOBASE")) {
		useBase = false;
		if (ins.has_label()) {
			error += "this statement can not have a label, ";
		}
		if(!trim(oprnd).empty()){
			error += "this statement can not have an operand, ";
		}
	}else{

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

void writeError(ofstream& w, string error) {

	if (w.is_open())
		w << "***Error: " << error << endl;
}
void wirteInitialLine(ofstream& write) {
	write << std::left << setw(jf) << "line no." << setw(jf) << "address"
			<< setw(jf) << "label" << setw(jf) << "op-code" << setw(jf)
			<< "operand" << setw(jf) << "comment" << endl;
}

void printSymbolTab(ofstream& w){
	if(symtab.empty()) return;
	w<<std::left<<setw(jf*2)<<""<<setw(5)<<"Symbol Table (value in decimal)"<<endl<<endl;
	w<<std::left<<setw(jf)<<""<<setw(jf)<<"Name"<<setw(jf)<<"Value"<<setw(jf)<<"Reloc/Absol"<<endl;
	w<<std::left<<setw(jf)<<""<<setfill('-')<<setw(jf*3)<<endl;
	w<<setfill(' ');
	string s;
	for(auto entry : symtab){
		s = ((entry.second.addressType == entry.second.absol)?"Absolute":"Relocatable");
		w<<std::left<<setw(jf)<<""<<setw(jf)<<entry.first<<setw(jf)<<entry.second.address<<setw(jf)<<"Reloc/Absol"<<endl;
	}
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
string to_string(int d) {
	stringstream ss;
	ss << d;
	return ss.str();
}
string trim(const string& str){
	const string whitespace = " \t";
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}
