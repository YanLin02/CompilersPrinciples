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
#include <fstream>
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
	op_or_punc//A表示&&，B表示||
};

enum RelOp {
	LE, LT, GE, GT, EQ, NE
};

union Attribute {
	int int_val;
	double double_val;
	char char_val;
	RelOp relop;
	bool bool_val;
};

typedef pair<TokenName, Attribute> Token;

class LexicalAnalyzer {
	int isum;//整型数值
	double fsum;//浮点数值
	string strsum;//字符串
	char chsum;//字符

	int state;//状态
	int index;//扫描位置

	vector<Token> tokens;//词法单元序列	
	vector<string> identifiers;//符号表
	vector<string> strings;//字符串表

	void scan(const string& str) {
		char thischar = str[index];

		switch (state) {
		case 0:
			if (thischar == ' ' || thischar == '\n' || thischar == '\t') { state = 0; }
			else if (IsLetter_(thischar)) { strsum = ""; strsum += thischar; state = 1; }
			else if (IsDigit(thischar)) { isum = thischar - '0';   state = 3; }
			else if (thischar == '.') { fsum = 0.0; isum = 0; state = 5; }
			else if (thischar == '\'') { state = 11; }
			else if (thischar == '"') { strsum = ""; state = 14; }
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
			else if (thischar == '&') { chsum = thischar; strsum = ""; strsum += thischar; state = 18; }
			else if (thischar == '|') { chsum = thischar; strsum = ""; strsum += thischar; state = 18; }
			else if (thischar == '!') { chsum = thischar; strsum = ""; strsum += thischar; state = 18; }
			else if (thischar == '=') { chsum = thischar; strsum = ""; strsum += thischar; state = 18; }
			else if (thischar == '<') { chsum = thischar; strsum = ""; strsum += thischar; state = 18; }
			else if (thischar == '>') { chsum = thischar; strsum = ""; strsum += thischar; state = 18; }
			else
				state = -1;
			break;
		case 1:
			if (IsLetter_(thischar) || IsDigit(thischar)) { strsum += thischar; state = 1; }
			else { state = 2; index--; }
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
			if (state != 0) {
				tokens.push_back(Token(identifier, Attribute{ static_cast<int>(identifiers.size()) }));
				identifiers.push_back(strsum);
				state = 0; index--;
			}
			break;
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
			else { state17op_or_punc('.'); index--; }
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
			if (strsum == "-")
				fsum = static_cast<double>(fsum / pow(10, isum));
			else
				fsum = static_cast<double>(fsum * pow(10, isum));
			Attribute temp;
			temp.double_val = fsum;
			tokens.push_back(Token(floating_literal, temp));
			state = 0;
			index--;
			break;
		case 11:
			if (thischar == '\\') { state = 12; }
			else { chsum = thischar;  state = 13; }
			break;
		case 12:
			if (thischar == 'n') { chsum = '\n'; state = 13; }
			else if (thischar == 't') { chsum = '\t'; state = 13; }
			else if (thischar == 'r') { chsum = '\r'; state = 13; }
			else if (thischar == '0') { chsum = '\0'; state = 13; }
			else if (thischar == '\'') { chsum = '\''; state = 13; }
			else if (thischar == '\\') { chsum = '\\'; state = 13; }
			else if (thischar == '"') { chsum = '\"'; state = 13; }
			else state = -1;
			break;
		case 13:
			if (thischar == '\'') { tokens.push_back(Token(character_literal, Attribute{ chsum })); state = 0; }
			else state = -1;
			break;
		case 14:
			if (thischar == '\\') { state = 15; }
			else if (thischar == '"') { state = 16; }
			else { strsum += thischar; state = 14; }
			break;
		case 15:
			if (thischar == 'n') { strsum += '\n'; state = 14; }
			else if (thischar == 't') { strsum += '\t'; state = 14; }
			else if (thischar == 'r') { strsum += '\r'; state = 14; }
			else if (thischar == '0') { strsum += '\0'; state = 14; }
			else if (thischar == '\'') { strsum += '\''; state = 14; }
			else if (thischar == '\\') { strsum += '\\'; state = 14; }
			else if (thischar == '"') { strsum += '\"'; state = 14; }
			else state = -1;
			break;
		case 16:
			tokens.push_back(Token(string_literal, Attribute{ static_cast<int>(strings.size()) }));
			strings.push_back(strsum);
			state = 0;
			index--;
			break;
		case 18:
			switch (chsum) {
			case '&':
				if (thischar == '&') { tokens.push_back(Token(op_or_punc, Attribute{ 'A' })); state = 0; }
				else state19();
				break;
			case '|':
				if (thischar == '|') { tokens.push_back(Token(op_or_punc, Attribute{ 'B' })); state = 0; }
				else state19();
				break;
			case '!':
				if (thischar == '=') { tokens.push_back(Token(relop, Attribute{ NE })); state = 0; }
				else state19();
				break;
			case '=':
				if (thischar == '=') { tokens.push_back(Token(relop, Attribute{ EQ })); state = 0; }
				else state19();
				break;
			case '<':
				if (thischar == '=') { tokens.push_back(Token(relop, Attribute{ LE })); state = 0; }
				else state19();
				break;
			case '>':
				if (thischar == '=') { tokens.push_back(Token(relop, Attribute{ GE })); state = 0; }
				else state19();
				break;
			default:
				throw "error";
			}
			break;
		default:
			throw index;
			break;
		}
		index++;
		//cout << thischar << "\t" << state << endl;
	}

	inline void state17op_or_punc(char ch) {
		tokens.push_back(Token(op_or_punc, Attribute{ ch }));
		state = 0;
	}

	inline void state19() {
		tokens.push_back(Token(op_or_punc, Attribute{ chsum }));
		index--;
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
public:
	LexicalAnalyzer() :isum(0), fsum(0.0), strsum(""), chsum('\0'), state(0), index(0) {}

	vector<Token> Analyze(const string& str) {
		tokens.clear();
		identifiers.clear();
		strings.clear();
		index = 0;
		while (index < str.size()) {
			scan(str);
		}
		return tokens;
	}

	vector<string> GetIdentifiers() {
		return identifiers;
	}

	vector<string> GetStrings() {
		return strings;
	}

	void print() {
		for (int i = 0; i < tokens.size(); i++) {
			switch (tokens[i].first) {
			case identifier:
				cout << "identifier" << "\t" << identifiers[tokens[i].second.int_val] << endl;
				break;
			case keyword:
				cout << "keyword" << "\t\t" << keyWord[tokens[i].second.int_val] << endl;
				break;
			case boolean_literal:
				cout << "boolean_literal" << "\t" << tokens[i].second.bool_val << endl;
				break;
			case pointer_literal:
				cout << "pointer_literal" << "\t" << tokens[i].second.int_val << endl;
				break;
			case character_literal:
				cout << "character_literal" << "\t" << tokens[i].second.char_val << endl;
				break;
			case string_literal:
				cout << "string_literal" << "\t" << strings[tokens[i].second.int_val] << endl;
				break;
			case integer_literal:
				cout << "integer_literal" << "\t" << tokens[i].second.int_val << endl;
				break;
			case floating_literal:
				cout << "floating_literal" << "\t" << tokens[i].second.double_val << endl;
				break;
			case relop:
				switch (tokens[i].second.relop) {
				case LE:
					cout << "relop" << "\t" << "<=" << endl;
					break;
				case LT:
					cout << "relop" << "\t" << "<" << endl;
					break;
				case GE:
					cout << "relop" << "\t" << ">=" << endl;
					break;
				case GT:
					cout << "relop" << "\t" << ">" << endl;
					break;
				case EQ:
					cout << "relop" << "\t" << "==" << endl;
					break;
				case NE:
					cout << "relop" << "\t" << "!=" << endl;
					break;
				default:
					break;
				}
				break;
			case op_or_punc:
				cout << "op_or_punc" << "\t" << tokens[i].second.char_val << endl;
				break;
			default:
				break;
			}
		}
	}

};



int main()
{
	ifstream fin("input.txt");
	string str;
	string temp;
	while (getline(fin, temp)) {
		str += temp;
		str += '\n';
	}
	fin.close();
	LexicalAnalyzer la;
	la.Analyze(str);
	la.print();
	return 0;
}