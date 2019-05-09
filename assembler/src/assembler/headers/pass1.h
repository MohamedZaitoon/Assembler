#ifndef ASSEMBLER_HEADERS_PASS1_H_
#define ASSEMBLER_HEADERS_PASS1_H_
using namespace std;
#include <string>
#define ll int
string pass1(string);

ll calculate(string a, string b, string op, string& error, ll curloc,
		bool type);
void getValue(string x, ll& value, bool& ta, string& error, ll curloc);

template<typename T>
ll to_int(T h);

template<typename T>
string dec_to_hex(T d);

template<typename T>
ll hex_to_dec(T h);
#endif /* ASSEMBLER_ASSEMBLER_H_ */
