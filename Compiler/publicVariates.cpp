/*
	这个cpp文件声明了一些全局共享变量
	记得extern！
*/
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "SyntaxAnalyzer.h"
#include "LexicalAnalyzer.h"
#include "publicClass.h"
using namespace std;

#ifdef  debugMode
ifstream in("E:\\☆学习\\编译原理\\spfa\\program.txt");
#endif //  debugMode

//输出机器码
ofstream outfile("programCodes.dat", ios::out | ios::trunc);
//输出人看的汇编代码
ofstream outAssembly("assembly.dat", ios::out | ios::trunc);

//关键字
string KeyWordTable[] = {
	"CONST",
	"VAR",
	"PROCEDURE",
	"BEGIN",
	"END",
	"ODD",
	"IF",
	"THEN",
	"CALL",
	"WHILE",
	"DO",
	"READ",
	"WRITE",
};

//运算符
string OperatorTable[] = { "=",":=","+","-","*","/","#","<","<=",">",">=" };
//分隔符
string SeparatorTable[] = { ";",",",".","(",")" };
//标识符符号表
vector<string> symbolTable;
//常数表
vector<long long> constNumberTable;

//词法分析器的处理结果————词语序列
vector<WordInf> wordsBuffer;

//根符号表
extern SymbolTable* rootSymbolTable;

//四元式序列
//vector<Quaternary*> quaternaryList;
//机器码序列
vector<MachineCode> machineCodes;


//错误处理
void ProcError(ErrorType errorType)
{
	if (rootSymbolTable != nullptr) delete rootSymbolTable;
	throw MyException(errorType);
}

string OperTypeString[] = {
	"JEQ", "JNE", "JL", "JLE",
	"JG", "JGE", "JMP", "ADD",
	"SUB", "MUL", "DIV", "MOD",
	"UMI", "ASG", "READ", "WRITE",
	"CALL", "RET" };

string CodeTypeString[] = {
	"INT", "CAL", "LIT", "LOD", "STO", "JMP", "JPC", "OPR"
};

//打印四元式序列
/*void PrintQuadList()
{
	for (int i = 0; i < quaternaryList.size(); ++i)
	{
		//printf("%d\t%s\t", i, OperTypeString[(int)quaternaryList[i]->op].c_str());
		cout << i << '\t' << OperTypeString[(int)quaternaryList[i]->op] << '\t';
		if (quaternaryList[i]->arg1)
		{
			switch (quaternaryList[i]->arg1->type)
			{
			case 0:
				cout << quaternaryList[i]->arg1->value;
				break;
			case 1:
				cout << quaternaryList[i]->arg1->item->name;
				break;
			case 2:
				cout << 'T' << quaternaryList[i]->arg1->value;
				break;
			}
		}
		else cout << '-';
		cout << '\t';

		if (quaternaryList[i]->arg2)
		{
			switch (quaternaryList[i]->arg2->type)
			{
			case 0:
				cout << quaternaryList[i]->arg2->value;
				break;
			case 1:
				cout << quaternaryList[i]->arg2->item->name;
				break;
			case 2:
				cout << 'T' << quaternaryList[i]->arg2->value;
				break;
			}
		}
		else cout << '-';
		cout << '\t';

		if (quaternaryList[i]->result)
		{
			switch (quaternaryList[i]->result->type)
			{
			case 0:
				cout << quaternaryList[i]->result->value;
				break;
			case 1:
				cout << quaternaryList[i]->result->item->name;
				break;
			case 2:
				cout << 'T' << quaternaryList[i]->result->value;
				break;
			}
		}
		else cout << '-';
		cout << endl;
	}
}*/

//打印机器码序列
void PrintMachineCodes()
{
	for (int i = 0; i < machineCodes.size(); ++i)
	{
		outAssembly << i << '\t';
		outAssembly << CodeTypeString[(int)machineCodes[i].codeType] << '\t';
		outfile << (int)machineCodes[i].codeType << '\t';
		outAssembly << machineCodes[i].levelDis << '\t';
		outfile << machineCodes[i].levelDis << '\t';
		outAssembly << machineCodes[i].arg << endl;
		outfile << machineCodes[i].arg << endl;
	}
	outfile.close();
}

//垃圾清理
void RubbishClear()
{
	delete rootSymbolTable;
}