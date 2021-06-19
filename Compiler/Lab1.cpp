#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <ctype.h>
#include "publicClass.h"
#include "LexicalAnalyzer.h"
using namespace std;

#ifdef  debugMode
#include <fstream>
extern ifstream in;
#endif //  debugMode

//错误处理
extern void ProcError(ErrorType errorType);
//词法分析函数
extern WordInf GetSym();
//词法分析函数的工具函数————把长整型转换为字符串
extern string ConvertToStr(const long long& num);
//语法分析函数
extern void SyntaxAnalyze();
//美观化
extern void pretty(string exp);


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
//词法分析器的处理结果————词语序列
extern vector<WordInf> wordsBuffer;
//打印四元式序列
//extern void PrintQuadList();
//打印机器码序列
extern void PrintMachineCodes();
//垃圾清理
extern void RubbishClear();

int main()
{
	//第一部分：词法分析
	do
	{
		WordInf inf;
		try
		{
			 inf = GetSym();
		}
		catch(MyException& ex)
		{
			string tmp;
#ifdef  debugMode
			while (in >> tmp);
#else
			while (cin >> tmp);
#endif

			ex.what();
			return 1;
		}

		if (inf.wordType == WordType::UNKNOW) break;
		else wordsBuffer.emplace_back(inf);
		/*else if (inf.wordType == WordType::IDENTIFIER)
		{
			wordsBuffer.emplace_back("IDENTIFIER " + inf.ID);
		}
		else if (inf.wordType == WordType::NUMBER)
		{
			wordsBuffer.emplace_back("NUMBER " + ConvertToStr(inf.NUM));
		}
		else
		{
			int _int_type = (int)inf.wordType;
			if (_int_type >= 1 && _int_type <= 13) wordsBuffer.emplace_back(KeyWordTable[_int_type - 1]);
			else if (_int_type >= 14 && _int_type <= 24) wordsBuffer.emplace_back(OperatorTable[_int_type - 14]);
			else if (_int_type >= 25 && _int_type <= 29) wordsBuffer.emplace_back(SeparatorTable[_int_type - 25]);
		}*/
	} while (true);

	//第二部分：语法分析

	try
	{
		//pretty(SyntaxAnalyze());
		SyntaxAnalyze();
		PrintMachineCodes();
	}
	catch (MyException& ex)
	{
		ex.what();
		//RubbishClear();
		return 2;
	}

	RubbishClear();
	
	return 0;
}

