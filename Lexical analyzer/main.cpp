#define _CRT_SECURE_NO_WARNINGS
/*
* letter_ : [a-zA-Z_];
* digit : [0-9];
*
* identifier : letter_ (letter_ | digit)*;
*
* keyword:
* auto, bool, break, case, char, const, constexpr, continue, default,
* do, double, else, enum, extern, false, float, for, goto, if, inline,
* int, long, namespace, nullptr,register, restrict, return, short, signed,
* sizeof, static, static_assert, struct,switch, true, typedef, typeof,
* union, unsigned, using, void, volatile, while.
*
* boolean-literal: true, false;
* pointer-literal: nullptr;
* character-literal
* string-literal
* integer_literal : digit+
* floating_literal : digit* '.' digit+ ([eE] [+-]? digit+)?
*
* op_or_punc : '{'|'}'|'['|']'|'('|')'|';'|':'|'.'|'+'|'-'|'*'|'/'|'%'|'^'|'~'|','
			  |'&'|'|'|'!'|'='|'<'|'>'|'=='|'!='|'<='|'>='|'&&'|'||';
*/
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const string keyWord[] = {
	"auto", "bool", "break", "case", "char", "const", "constexpr", "continue", "default",
	"do", "double", "else", "enum", "extern", "false", "float", "for", "goto", "if", "inline",
	"int", "long", "namespace", "nullptr", "register", "restrict", "return", "short", "signed",
	"sizeof", "static", "static_assert", "struct", "switch", "true", "typedef", "typeof",
	"union", "unsigned", "using", "void", "volatile", "while"
};

enum TokenName {
	identifier,
	keyword,
	boolean_literal,
	pointer_literal,
	character_literal,
	string_literal,
	integer_literal,
	floating_literal,
	relop,
	op_or_punc
};

enum RelOp {
	LE, LT, GE, GT, EQ, NE
};

union Attribute {
	int int_val;
	double double_val;
	string* string_ptr;
	char char_val;
	RelOp relop;
	bool bool_val;
};

typedef pair<TokenName, Attribute> Token;

class LexicalAnalyzer {
	int isum;//整型数值
	double fsum;//浮点数值
	string strsum;//字符串

	int state;//状态
	int index;//扫描位置

	vector<Token> tokens;//词法单元序列	
	vector<string> identifiers;//符号表


	void scan(const string& str) {
		char thischar = str[index];

		switch (state) {
		case 0:
			if (thischar == ' ' || thischar == '\n' || thischar == '\t') { state = 0; }
			else if (IsLetter_(thischar)) { strsum = ""; strsum += thischar; state = 1; }
			else if (IsDigit(thischar)) { isum = thischar - '0';   state = 3; }
			else if (thischar == '.') { fsum = 0.0; isum = 0; state = 5; }
			else if (thischar == '\'') { state = 11; }
			else if (thischar == '"') { state = 14; }
			else if (thischar == '+') { state17op_or_punc(thischar); }
			else if (thischar == '-') { state17op_or_punc(thischar); }
			else if (thischar == '*') { state17op_or_punc(thischar); }
			else if (thischar == '/') { state17op_or_punc(thischar); }
			else if (thischar == '%') { state17op_or_punc(thischar); }
			else if (thischar == '^') { state17op_or_punc(thischar); }
			else if (thischar == '~') { state17op_or_punc(thischar); }
			else if (thischar == ';') { state17op_or_punc(thischar); }
			else if (thischar == ':') { state17op_or_punc(thischar); }
			else if (thischar == ',') { state17op_or_punc(thischar); }
			else if (thischar == '{') { state17op_or_punc(thischar); }
			else if (thischar == '}') { state17op_or_punc(thischar); }
			else if (thischar == '[') { state17op_or_punc(thischar); }
			else if (thischar == ']') { state17op_or_punc(thischar); }
			else if (thischar == '(') { state17op_or_punc(thischar); }
			else if (thischar == ')') { state17op_or_punc(thischar); }
			else if (thischar == '&') { strsum = ""; strsum += thischar; state = 18; }
			else if (thischar == '|') { strsum = ""; strsum += thischar; state = 18; }
			else if (thischar == '!') { strsum = ""; strsum += thischar; state = 18; }
			else if (thischar == '=') { strsum = ""; strsum += thischar; state = 18; }
			else if (thischar == '<') { strsum = ""; strsum += thischar; state = 18; }
			else if (thischar == '>') { strsum = ""; strsum += thischar; state = 18; }
			else
				state = -1;
			break;
		case 1:
			if (IsLetter_(thischar) || IsDigit(thischar)) { strsum += thischar; state = 1; }
			else state = 2;
			break;
		case 2:
			if (strsum == "true") { tokens.push_back(Token(boolean_literal, Attribute{ true })); state = 0; index--; break; }
			if (strsum == "false") { tokens.push_back(Token(boolean_literal, Attribute{ false })); state = 0; index--; break; }
			if (strsum == "nullptr") { tokens.push_back(Token(pointer_literal, Attribute{ NULL })); state = 0; index--; break; }
			for (int i = 0; i < 43; i++) {
				if (strsum == keyWord[i]) {
					tokens.push_back(Token(keyword, Attribute{ i }));
					state = 0; index--; break;
				}
			}
			tokens.push_back(Token(identifier, Attribute{ static_cast<int>(identifiers.size()) }));
			identifiers.push_back(strsum);
			state = 0; index--; break;
		case 3:
			if (IsDigit(thischar)) { isum = isum * 10 + thischar - '0'; state = 3; }
			else if (thischar == '.') { fsum = isum; isum = 0; state = 5; }
			else { state = 4; }
			break;
		case 4:
			tokens.push_back(Token(integer_literal, Attribute{ isum }));
			state = 0;
			index--;
			break;
		case 5:
			if (IsDigit(thischar)) { fsum = fsum + (thischar - '0') / pow(10.0, ++isum); state = 6; }
			else state = -1;
			break;
		case 6:
			if (IsDigit(thischar)) { fsum = fsum + (thischar - '0') / pow(10.0, ++isum); state = 6; }
			else if (thischar == 'e' || thischar == 'E') { isum = 0; state = 7; }
			else { state = 10; }
			break;
		case 7:
			if (thischar == '+' || thischar == '-') { strsum = ""; strsum += thischar; state = 8; }
			else if (IsDigit(thischar)) { strsum = "+"; isum += thischar - '0'; state = 9; }
			else state = -1;
			break;
		case 8:
			if (IsDigit(thischar)) { isum = isum * 10 + thischar - '0'; state = 9; }
			else state = -1;
			break;
		case 9:
			if (IsDigit(thischar)) { isum = isum * 10 + thischar - '0'; state = 9; }
			else { state = 10; }
			break;
		case 10:
			int tempdoublenum = 0.0;
			if (strsum == "-")
				tempdoublenum = static_cast<double>(fsum / pow(10, isum));
			else
				tempdoublenum = static_cast<double>(fsum * pow(10, isum));
			tokens.push_back(Token(floating_literal, Attribute{ tempdoublenum }));
			state = 0;
			index--;
			break;
		default:
			throw "error";
			break;
		}





		index++;
	}

	inline void state17op_or_punc(char ch) {
		tokens.push_back(Token(op_or_punc, Attribute{ ch }));
		state = 0;
	}

	/// @brief 判断是否为字母或下划线
	/// @param ch 字符
	/// @return 是否为字母或下划线
	inline bool IsLetter_(char ch) {
		if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
			return true;
		return false;
	}
	/// @brief 判断是否为数字
	/// @param ch 字符
	/// @return 是否为数字
	inline bool IsDigit(char ch) {
		if (ch >= '0' && ch <= '9')
			return true;
		return false;
	}
};








int main()
{



	system("pause");
	return 0;
}