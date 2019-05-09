#include "headers/pass2.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

#include "headers/parser.h"
#include "headers/common.h"
#include "headers/statement.h"
#include "headers/pass1.h"

using namespace std;
void writeHeader(ofstream& w, X item);
void initTextRec(ofstream& w, ll locctr);

void wirteInitialLinep2(ofstream& w);
void writeReport(ofstream& w, X& item);
void writeErrorP2(ofstream& w, string error);

void calculateObcode(X& item, string& error, string& op);

string toHexValue(string c);
string toBinValue(char c);
string valueOfConstat(string s);

Literal searchLiteral(string lit);

ll startadrs = 0;
const ll jf = 10;
const ll maxLen = 60;
const string obfile("OBJECTFILE.txt");
bool enableBase = false;
string labelBase;

bool ero = false;

ll cnt = 0;

ll pc;
ll br;
ll curloc;
bool pass2() {
	if (completePass1) {
		ofstream wreport, wof;
		wreport.open(reportfile, ios::app);
		wof.open(obfile);
		wirteInitialLinep2(wreport);
		ll sz = intermediate.size();
		int i = 0;
		X item = intermediate[i];
		curloc = item.locctr;
		pc = intermediate[i + 1].locctr;
		string op = to_upper(item.stat.getMnemonic());
		if (!op.compare("START")) {
			startadrs = item.locctr;
			writeReport(wreport, item);
			i++;
		}
		writeHeader(wof, item);
		initTextRec(wof, item.locctr);
		string textRec = "";
		for (; i < sz; i++) {
			string error;
			item = intermediate[i];
			op = to_upper(item.stat.getMnemonic());
			if (!op.compare("END"))
				break; //need handle
			curloc = item.locctr;
			pc = intermediate[i + 1].locctr;
			cout << "operand in pass2()  =>" << op << endl;
			if (optab.find(op) != optab.end()) {
				calculateObcode(item, error, op);
			} else if (!op.compare("WORD")) {
				item.obcode = adjustString(to_int(item.stat.getOperand()), 6);
			} else if (!op.compare("BYTE")) {
				item.obcode = valueOfConstat(item.stat.getOperand());
			} else if (!op.compare("BASE")) {
				string operand = to_upper(item.stat.getOperand());
				auto it = symtab.find(operand);
				if (it != symtab.end()) {
					enableBase = true;
					symbol sym = it->second;
					br = sym.address;
				} else {
					br = 0;
					error += "this operand is not existing";
				}
			} else if (!op.compare("NOBASE")) {
				enableBase = false;
			}
			if (item.obcode.size() != 0) {
				if (!(textRec.size() + item.obcode.size() - cnt <= maxLen)) {
					//handle last text record in file later
					cout << "test record length: " << textRec.size() / 2
							<< endl;
					textRec = adjustString((textRec.size() - cnt) / 2, 2)
							+ textRec;
					wof << textRec << endl;
					initTextRec(wof, curloc);
					textRec = "";
					cnt = 0;
					cout << "text record => " << i << " =>" << textRec << endl;
				}
				cnt++;
				textRec += "^" + item.obcode;
			}
			writeReport(wreport, item);
			if (!error.empty()) {
				ero = true;
				writeErrorP2(wreport, error);
			}

		}
		//write last text record
		textRec = adjustString((textRec.size() - cnt) / 2, 2) + textRec;
		wof << textRec << endl;
		//write end record
		wof << "E" << adjustString(startadrs, 6) << endl;
		//write last report(list file) line
		writeReport(wreport, item);
		if (i < sz - 1) {
			//extera statement
			ero = true;
			writeErrorP2(wreport, "extera statement after end statement");
		} else if (i == sz
				&& to_upper(item.stat.getMnemonic()).compare("END")) {
			//write error in report error
			writeErrorP2(wreport, "missing end statement");
			ero = true;
		}
		if (ero)
			return false;
		return true;
	}
	return false;
}
void calculateObcode(X& item, string& error, string& op) {
	string operand = to_upper(item.stat.getOperand());
	cout << "operand in calculate()  =>" << operand << endl;
	info opinfo = optab.find(op)->second;
	string opcode = opinfo.opcode;
	cout << "opcode in calculate()  =>" << opcode << endl;
	string obcode = opcode.substr(0, 1);
	bool n, i, x, b, p, e;
	string dis;
	ll adrs;
	smatch sm;
	if (opinfo.formate == 2) {
		obcode += opcode.substr(1, 1);
		if (opinfo.operands == 2) {
			if (regex_match(operand, sm, r2operand)) {
				string first = sm[1].str();
				string second = sm[2].str();
				auto fr = register_tab.find(first);
				auto sc = register_tab.find(second);
				if (fr != register_tab.end() && sc != register_tab.end()) {
					obcode += dec_to_hex(fr->second) + ""
							+ dec_to_hex(sc->second);
					item.obcode = obcode;
				} else {
					error += "Invalid operand";
				}
			} else {
				error +=
						"this instruction must have two operand or invalid operand,";
			}
		} else if (opinfo.operands == 1) {
			if (regex_match(operand, sm, rlabel)) {
				string first = sm[1].str();
				auto fr = register_tab.find(first);
				if (fr != register_tab.end()) {
					obcode += dec_to_hex(fr->second) + "0";
					item.obcode = obcode;
				}
			} else {
				error += "Invalid operand";
			}
		}
	} else if (opinfo.formate == 3 && opinfo.operands == 1) {

		if (regex_match(operand, sm, rdig)) {
			adrs = to_int(sm[1].str());
			n = i = 1;
			x = 0;
		} else if (regex_match(operand, sm, rexp)) {
			string first = sm[1].str();
			string o = sm[2].str();
			string second = sm[3].str();
			bool type = 0;
			int result = calculate(first, second, o, error, curloc, type);
			adrs = result;
			n = i = 1;
			x = 0;
			if (type == symbol().absol) {
				b = p = 0;
				dis = dec_to_hex(adrs);
			}
		} else if (trim(operand).compare("*") == 0) {

			adrs = curloc;
			n = i = 1;
			x = 0;
		} else if (regex_match(operand, sm, rlabel)) {
			auto sy = symtab.find(operand);
			n = i = 1;
			x = 0;
			if (sy != symtab.end()) {
				symbol sym = sy->second;
				adrs = sym.address;
			} else {
				adrs = 0;
				error += "undefined symbol in operand";
			}
		} else if (regex_match(operand, sm, rimmd_ind)) {
			string perfix = sm[1].str();
			string oprnd = sm[2].str();
			if (!perfix.compare("#")) {
				i = 1;
				x = n = 0;
			} else {
				n = 1;
				i = x = 0;
			}
			if (!trim(oprnd).compare("*")) {
				adrs = curloc;
			} else if (regex_match(oprnd, rdig)) {
				adrs = to_int(oprnd);
				b = p = 0;
				dis = dec_to_hex(adrs);
			} else {
				auto sy = symtab.find(oprnd);
				if (sy != symtab.end()) {
					symbol sym = sy->second;
					adrs = sym.address;
				} else {
					adrs = 0;
					error += "undefined symbol in operand";
				}
			}
		} else if (regex_match(operand, sm, r2operand)) {
			string first = sm[1].str();
			string second = sm[2].str();
			i = n = x = 1;
			auto sy = symtab.find(first);
			if (sy != symtab.end()) {
				symbol sym = sy->second;
				adrs = sym.address;
			} else {
				adrs = 0;
				error += "undefined symbol in operand, ";
			}
			if (second.compare("X")) {
				error += "second operand must be  register X, ";
			}
		} else if (item.stat.isLitral()) {
			regex_match(item.stat.getOperand(), sm, rlitral);
			string c = sm[2].str();
			string literal = sm[1].str() + "'" + c + "'";
			Literal l = searchLiteral(literal);
			if (!l.literal.empty()) {
				adrs = l.address;
				cout << adrs << endl;
			} else {
				cout << "failed to find literal" << endl;
			}
		}
		cout << "target address " << adrs << endl;

		if (item.stat.isFormate4()) {
			e = 1;
			p = b = 0;
			dis = dec_to_hex(adrs);
		} else if (((i & !n) || (!i & n)) && !dis.empty()) {
			e = 0;
			p = b = 0;
		} else {
			e = 0;
			ll disp = adrs - pc;
			cout << "Displacement " << disp << endl;
			if (disp < 2048 && disp >= -2048) {
				p = 1;
				b = 0;
				dis = dec_to_hex(disp);
				cout << "displacement in hex " << dis << endl;
			} else if (enableBase) {
				p = 0;
				b = 1;
				disp = adrs - br;
				if (disp > 0 && disp < 4096) {
					dis = dec_to_hex(disp);
				} else {
					dis = "0";
					error += "overflow in Displacement field";
				}
			} else {
				error += "overflow in Displacement field";
			}
		}
	} else if (opinfo.formate == 3 && opinfo.operands == 0) {
		i = n = x = b = p = 0;
		e = item.stat.isFormate4();
		dis = "0";
	} else { // handle this case later
		i = n = x = b = p = 0;
		e = item.stat.isFormate4();
		dis = "0";
	}

	if (opinfo.formate == 3) {
		stringstream ss;
		string temp;
		if (e) {
			ss << setfill('0') << setw(5) << dis;
			temp = ss.str();
			temp = temp.substr(temp.size()-5,5);
		} else {
			ss << setfill('0') << setw(6) << dis;
			temp = ss.str();
			int sz = temp.size();
			temp = temp.substr(sz - 3, 3);
		}
		string c2 = toBinValue(opcode[1]);
		c2[2] = ('0' + n);
		c2[3] = ('0' + i);
		cout << "c2 => " << c2 << endl;
		c2 = toHexValue(c2);
		cout << "c2 hex => " << c2 << endl;
		string c3 = "0000";
		c3[0] = ('0' + x);
		c3[1] = ('0' + b);
		c3[2] = ('0' + p);
		c3[3] = ('0' + e);
		cout << "c3 => " << c3 << endl;
		c3 = toHexValue(c3);
		cout << "c3 hex=> " << c3 << endl;
		obcode += c2 + c3 + temp;
		cout << "temp =>" << temp << endl;
		cout << "object code  => " << obcode << endl;
		item.obcode = obcode;
	}
}

void writeHeader(ofstream& w, X item) {
	string name = "";
	if (to_upper(item.stat.getMnemonic()) == "START")
		name = item.stat.getLabel();
	w << ::left << "H" << setw(6) << name << "^" << adjustString(item.locctr, 6)
			<< "^" << adjustString(len, 6) << endl;
}
void initTextRec(ofstream& w, ll locctr) {
	w << ::left << "T" << adjustString(locctr, 6) << "^";
}

/**
 * convert value to hex with 6 digits
 *  ex. 15 =>   00000f
 */

void wirteInitialLinep2(ofstream& w) {
	w << endl << endl << setw(40) << "# Pass 2 #" << endl;
	w << ::left << setw(jf) << "LC" << setw(jf) << "obcode" << setw(jf)
			<< "Label" << setw(jf) << "Mnemonic" << setw(jf) << "operand"
			<< endl;
}
void writeReport(ofstream& w, X& item) {
	string plus = "";
	if (item.stat.isFormate4())
		plus = "+";
	stringstream s;
	s << setfill('0') << setw(6) << ::hex << item.locctr;

	w << ::left << setw(jf) << s.str() << setw(jf) << item.obcode << setw(jf)
			<< item.stat.getLabel() << setw(jf)
			<< plus + item.stat.getMnemonic() << setw(jf)
			<< item.stat.getOperand() << endl;
}
void writeErrorP2(ofstream& w, string error) {

	if (w.is_open())
		w << "***Error: " << error << endl;
}

Literal searchLiteral(string lit) {
	for (Literal l : littab) {
		if (!lit.compare(l.literal)) {
			return l;
		}
	}
	Literal dummy("", "", 0, 0);
	return dummy;
}

const regex rcchars("^\\s*([cC])\\s*'([\\s\\S]+)'$");
const regex rchex("^\\s*([xX])\\s*'([\\da-fA-F]+)'$");
string valueOfConstat(string s) {
	s = trim(s);
	string value = "";
	smatch sm;
	if (regex_match(s, sm, rcchars)) {
		value = strToHex(sm[2].str());
	} else if (regex_match(s, sm, rchex)) {
		value = sm[2].str();
	}
	return value;
}

string toBinValue(char c) {
	switch (c) {
	case '0':
		return "0000";
	case '1':
		return "0001";
	case '2':
		return "0010";
	case '3':
		return "0011";
	case '4':
		return "0100";
	case '5':
		return "0101";
	case '6':
		return "0110";
	case '7':
		return "0111";
	case '8':
		return "1000";
	case '9':
		return "1001";
	case 'A':
		return "1010";
	case 'B':
		return "1011";
	case 'C':
		return "1100";
	case 'D':
		return "1101";
	case 'E':
		return "1110";
	case 'F':
		return "1111";
	default:
		return "0000";
	}
}
string toHexValue(string c) {

	if (!c.compare("0000"))
		return "0";
	if (!c.compare("0001"))
		return "1";
	if (!c.compare("0010"))
		return "2";
	if (!c.compare("0011"))
		return "3";
	if (!c.compare("0100"))
		return "4";
	if (!c.compare("0101"))
		return "5";
	if (!c.compare("0110"))
		return "6";
	if (!c.compare("0111"))
		return "7";
	if (!c.compare("1000"))
		return "8";
	if (!c.compare("1001"))
		return "9";
	if (!c.compare("1010"))
		return "A";
	if (!c.compare("1011"))
		return "B";
	if (!c.compare("1100"))
		return "C";
	if (!c.compare("1101"))
		return "D";
	if (!c.compare("1110"))
		return "E";
	if (!c.compare("1111"))
		return "F";

	return "0";

}
