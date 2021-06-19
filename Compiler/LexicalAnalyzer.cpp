#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "publicClass.h"
#include "LexicalAnalyzer.h"

using namespace std;

#ifdef  debugMode
#include <fstream>
extern ifstream in;
#endif //  debugMode

//关键字
extern string KeyWordTable[];
//运算符
extern string OperatorTable[];
//分隔符
extern string SeparatorTable[];
//标识符符号表
extern vector<string> symbolTable;
//常数表
extern vector<long long> constNumberTable;
//错误处理
extern void ProcError(ErrorType errorType);

int curRow = 0;							//当前读到的行
int curCol = 0;							//当前读到的列
int lastCol = 0;						//上一行的列
char ch;							//当前读到的字符
string strToken;					//当前读到的字符串

//尝试匹配关键字
WordType MatchKeyWord(const string& str)
{
	int index = -1;
	for (int i = 0; i < 13; ++i)
	{
		if (str == KeyWordTable[i])
		{
			index = i;
			break;
		}
	}
	if (index == -1) return WordType::IDENTIFIER;		//普通的标识符

	return (WordType)(index + 1);		//关键字
}

//换行
void LineChange()
{
	curRow++;
	lastCol = curCol;
	curCol = 0;
}

//拼接字符串
void Concat()
{
	strToken += ch;
}

//将小写字母转换为大写字母
void ToUpper(char& cha)
{
	if (cha >= 'a' && cha <= 'z')
	{
		cha -= 32;
	}
}

bool IsSpace(char& ch)
{
	return ch == 0x20 || ch == 0x09 || ch == 0x0a || ch == 0x0b || ch == 0x0c || ch == 0x0d;
}


//持续读取空白符
void GetSpace()
{
	while (IsSpace(ch))
	{
#ifdef debugMode
		ch = in.get();
#else
		ch = cin.get();
#endif // debugMode
		if (ch == '\n') LineChange();
		else curCol++;
	}
	ToUpper(ch);
}


//从输入流中读取输入字符
void GetChar()
{
#ifdef debugMode
	ch = in.get();
#else
	ch = cin.get();
#endif	
	if (ch == '\n') LineChange();
	else curCol++;
	ToUpper(ch);
}

//回调输入流
void Retract()
{
#ifdef debugMode
	in.putback(ch);
#else
	cin.putback(ch);
#endif
	if (ch == '\n')
	{
		curCol = lastCol;
		curRow--;
	}
	else curCol--;
	ch = 0;
}

//是否为字母
bool IsLetter()
{
	return ch >= 'A' && ch <= 'Z';
}

//是否为数字
bool IsNumber()
{
	return ch >= '0' && ch <= '9';
}

//将字符串转换为长整型数字
long long ConvertToLL(const string& str)
{
	long long num = 0;
	stringstream stm(str);
	stm >> num;
	return num;
}

//将长整型数字转换为字符串
string ConvertToStr(const long long& num)
{
	string str;
	stringstream stm;
	stm << num;
	stm >> str;
	return str;
}

//塞入到符号表中
int InsertIntoSymbolTable(const string& str)
{
	int index = symbolTable.size();
	symbolTable.emplace_back(str);
	return index;
}

//塞入到常数表中
int InsertIntoConstNumberTable(const long long& num)
{
	int index = constNumberTable.size();
	constNumberTable.emplace_back(num);
	return index;
}

//词法分析器
WordInf GetSym()
{
	strToken = "";
	GetChar();
	//if (ch == EOF || ch == 26) return WordInf(curRow, curCol, WordType::UNKNOW);
	GetSpace();
	if (ch == EOF || ch == 26) return WordInf(curRow, curCol, WordType::UNKNOW);
	if (IsLetter())	//首字符是字母
	{
		int _row = curRow;
		int _col = curCol;
		while (IsLetter() || IsNumber())
		{
			Concat();
			GetChar();
		}
		Retract();
		if (strToken.length() > 10) ProcError(ErrorType::LexicalError);		//超过了10个字符长度
		auto type = MatchKeyWord(strToken);
		if (type == WordType::IDENTIFIER)
		{
			int index = InsertIntoSymbolTable(strToken);
			return WordInf(_row, _col, type, strToken, 0, index);
		}
		else return WordInf(_row, _col, type, strToken);
	}
	else if (IsNumber()) //首字符是数字
	{
		int _row = curRow;
		int _col = curCol;
		while (IsNumber())
		{
			Concat();
			GetChar();
		}
		if (IsLetter()) ProcError(ErrorType::LexicalError);
		Retract();
		long long num = ConvertToLL(strToken);
		int index = InsertIntoConstNumberTable(num);
		return WordInf(_row, _col, WordType::NUMBER, strToken, num, index);
	}
	else if (ch == '=') return WordInf(curRow, curCol, WordType::EQUALSYM, "=");
	else if (ch == ':')
	{
		int _row = curRow;
		int _col = curCol;
		GetChar();
		if (ch == '=') return WordInf(_row, _col, WordType::ASSIGNSYM, ":=");
		else ProcError(ErrorType::LexicalError);			//冒号后面必须跟等号
	}
	else if (ch == '+') return WordInf(curRow, curCol, WordType::ADDSYM, "+");
	else if (ch == '-') return WordInf(curRow, curCol, WordType::SUBSYM, "-");
	/*{
		int _row = curRow;
		int _col = curCol;
		GetChar();
		if (IsNumber())
		{
			_row = curRow;
			_col = curCol;
			while (IsNumber())
			{
				Concat();
				GetChar();
			}
			if (IsLetter()) ProcError(ErrorType::LexicalError);
			Retract();
			long long num = (long long)-1 * ConvertToLL(strToken);
			int index = InsertIntoConstNumberTable(num);
			return WordInf(_row, _col, WordType::NUMBER, "", num, index);
		}
		else
		{
			Retract();
			return WordInf(_row, _col, WordType::SUBSYM);
		}
	}*/
	else if (ch == '*') return WordInf(curRow, curCol, WordType::MULSYM, "*");
	else if (ch == '/') return WordInf(curRow, curCol, WordType::DIVSYM, "/");
	else if (ch == '#') return WordInf(curRow, curCol, WordType::SHARPSYM, "#");
	else if (ch == '<')
	{
		int _row = curRow;
		int _col = curCol;
		GetChar();
		if (ch == '=') return WordInf(_row, _col, WordType::LESSEQUALSYM, "<=");
		Retract(); return WordInf(_row, _col, WordType::LESSSYM, "<");
	}
	else if (ch == '>')
	{
		int _row = curRow;
		int _col = curCol;
		GetChar();
		if (ch == '=') return WordInf(_row, _col, WordType::GREATEQUALSYM, ">=");
		Retract(); return WordInf(_row, _col, WordType::GREATSYM, ">");
	}
	else if (ch == ';') return WordInf(curRow, curCol, WordType::SEMISYM, ";");
	else if (ch == ',') return WordInf(curRow, curCol, WordType::COMMASYM, ",");
	else if (ch == '.') return WordInf(curRow, curCol, WordType::DOTSYM, ".");
	else if (ch == '(') return WordInf(curRow, curCol, WordType::LPARSYM, "(");
	else if (ch == ')') return WordInf(curRow, curCol, WordType::RPARSYM, ")");
	else ProcError(ErrorType::LexicalError);
	return WordInf(curRow, curCol, WordType::UNKNOW);
}