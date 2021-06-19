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

//�ؼ���
extern string KeyWordTable[];
//�����
extern string OperatorTable[];
//�ָ���
extern string SeparatorTable[];
//��ʶ�����ű�
extern vector<string> symbolTable;
//������
extern vector<long long> constNumberTable;
//������
extern void ProcError(ErrorType errorType);

int curRow = 0;							//��ǰ��������
int curCol = 0;							//��ǰ��������
int lastCol = 0;						//��һ�е���
char ch;							//��ǰ�������ַ�
string strToken;					//��ǰ�������ַ���

//����ƥ��ؼ���
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
	if (index == -1) return WordType::IDENTIFIER;		//��ͨ�ı�ʶ��

	return (WordType)(index + 1);		//�ؼ���
}

//����
void LineChange()
{
	curRow++;
	lastCol = curCol;
	curCol = 0;
}

//ƴ���ַ���
void Concat()
{
	strToken += ch;
}

//��Сд��ĸת��Ϊ��д��ĸ
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


//������ȡ�հ׷�
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


//���������ж�ȡ�����ַ�
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

//�ص�������
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

//�Ƿ�Ϊ��ĸ
bool IsLetter()
{
	return ch >= 'A' && ch <= 'Z';
}

//�Ƿ�Ϊ����
bool IsNumber()
{
	return ch >= '0' && ch <= '9';
}

//���ַ���ת��Ϊ����������
long long ConvertToLL(const string& str)
{
	long long num = 0;
	stringstream stm(str);
	stm >> num;
	return num;
}

//������������ת��Ϊ�ַ���
string ConvertToStr(const long long& num)
{
	string str;
	stringstream stm;
	stm << num;
	stm >> str;
	return str;
}

//���뵽���ű���
int InsertIntoSymbolTable(const string& str)
{
	int index = symbolTable.size();
	symbolTable.emplace_back(str);
	return index;
}

//���뵽��������
int InsertIntoConstNumberTable(const long long& num)
{
	int index = constNumberTable.size();
	constNumberTable.emplace_back(num);
	return index;
}

//�ʷ�������
WordInf GetSym()
{
	strToken = "";
	GetChar();
	//if (ch == EOF || ch == 26) return WordInf(curRow, curCol, WordType::UNKNOW);
	GetSpace();
	if (ch == EOF || ch == 26) return WordInf(curRow, curCol, WordType::UNKNOW);
	if (IsLetter())	//���ַ�����ĸ
	{
		int _row = curRow;
		int _col = curCol;
		while (IsLetter() || IsNumber())
		{
			Concat();
			GetChar();
		}
		Retract();
		if (strToken.length() > 10) ProcError(ErrorType::LexicalError);		//������10���ַ�����
		auto type = MatchKeyWord(strToken);
		if (type == WordType::IDENTIFIER)
		{
			int index = InsertIntoSymbolTable(strToken);
			return WordInf(_row, _col, type, strToken, 0, index);
		}
		else return WordInf(_row, _col, type, strToken);
	}
	else if (IsNumber()) //���ַ�������
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
		else ProcError(ErrorType::LexicalError);			//ð�ź��������Ⱥ�
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