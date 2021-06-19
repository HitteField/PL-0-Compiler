#pragma once
/*
	这个头文件定义了一些词法分析器会用到的类
*/

#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class WordType		//单词类型
{
	UNKNOW,				//未知
	//关键字
	CONSTSYM, VARSYM, PROCEDURESYM, BEGINSYM, ENDSYM, ODDSYM, IFSYM, THENSYM, CALLSYM, WHILESYM, DOSYM, READSYM, WRITESYM,
	//相等，赋值，加，减，乘，除，不等于，小于，小于等于，大于，大于等于
	EQUALSYM, ASSIGNSYM, ADDSYM, SUBSYM, MULSYM, DIVSYM, SHARPSYM, LESSSYM, LESSEQUALSYM, GREATSYM, GREATEQUALSYM,
	//分号，逗号，点，左括号，右括号
	SEMISYM, COMMASYM, DOTSYM, LPARSYM, RPARSYM,
	NUMBER,				//数字
	IDENTIFIER			//标识符
};

struct WordInf			//单词信息
{
	int row;				//所在行数
	int col;				//所在列数
	WordType wordType;		//单词类型
	string ID;				//标识符的机内表示串
	long long NUM;			//数字的机内表示值
	int tableIndex;			//在符号表/常数表中的下标，若为保留字则为-1
	WordInf() :row(0), col(0), wordType(WordType::UNKNOW), ID(), NUM(0), tableIndex(-1) {}
	WordInf(int _row, int _col, WordType _type, string _id = "", long long _num = 0, int _sti = -1) :row(_row), col(_col), wordType(_type), ID(_id), NUM(_num), tableIndex(_sti) {}
};