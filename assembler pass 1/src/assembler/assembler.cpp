#include "assembler.h"
#include <fstream>
#include <vector>
#include "parser.h"
#include <iostream>
#include "mnemonicinstruction.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <map>

using namespace std;
typedef mnemonic_instruction instruct;
const string pass1_path = "pass1.c";

vector<string> code_lines;

map<string,string> symtab;

long long locctr;

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

void write_pass1_output(string line) {
	ofstream write;
	write.open(pass1_path, ios::trunc);
	if (write.is_open()) {
//			for (string s : code_lines) {
//				cout << s << endl;
//				write << s << endl;
//			}
		write.close();
	}
}

string pass1(string path) {
	read_file(path);
	if (!code_lines.empty()) {
		int siz = code_lines.size();
		parser p;
		instruct ins = p.parse(code_lines[0]);
		string op = to_upper(ins.getMnemonic());

		if (!ins.has_error() && !ins.is_comment() && op == "START") {

		} else if (ins.has_error()) {

		} else {
			locctr = 0;
		}
		for(int i = 1; i < siz;i++){
			ins = p.parse(code_lines[i]);
			if(ins.has_error()){
				//handel when there is an error
				continue;
			}
			op  = to_upper(ins.getMnemonic());
			if(op == "END") {break;}// handel when the end of operation
			if(!ins.is_comment()){
				if(ins.has_label()){
					//search in symbol table for error
					bool founed ;
					if(founed){

					}else{
						//add to smbol table (label , locctr)
					}
				}
				// search for length of formate to add to locctr
				int l = 0;
				bool found = false;
				if(found){continue;}
				else if(op == "WORD"){}//add 3 to locctr
				else if(op == "BYTE"){}
				else if (op == "RESW"){}
				else if(op == "RESB"){}
				else {}//error invalid operation code
			}//end if not a comment
			//write line to intermediate file
		}
		//write last line in intermediate file
		//save locctr - starting adrs as prog len
	} else {
		return "Can't assemble this file";
	}
	return "done";
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

