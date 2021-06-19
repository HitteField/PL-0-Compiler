/*
	���cpp�ļ�������һЩȫ�ֹ������
	�ǵ�extern��
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
ifstream in("E:\\��ѧϰ\\����ԭ��\\spfa\\program.txt");
#endif //  debugMode

//���������
ofstream outfile("programCodes.dat", ios::out | ios::trunc);
//����˿��Ļ�����
ofstream outAssembly("assembly.dat", ios::out | ios::trunc);

//�ؼ���
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

//�����
string OperatorTable[] = { "=",":=","+","-","*","/","#","<","<=",">",">=" };
//�ָ���
string SeparatorTable[] = { ";",",",".","(",")" };
//��ʶ�����ű�
vector<string> symbolTable;
//������
vector<long long> constNumberTable;

//�ʷ��������Ĵ���������������������
vector<WordInf> wordsBuffer;

//�����ű�
extern SymbolTable* rootSymbolTable;

//��Ԫʽ����
//vector<Quaternary*> quaternaryList;
//����������
vector<MachineCode> machineCodes;


//������
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

//��ӡ��Ԫʽ����
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

//��ӡ����������
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

//��������
void RubbishClear()
{
	delete rootSymbolTable;
}