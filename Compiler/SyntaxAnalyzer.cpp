#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stack>
#include "publicClass.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"

using namespace std;

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

//——————————————————————————————————————————//

//当前函数声明的层数
int curLevel = 0;
//当前的SYMBOL
string curSymbol;
//当前的SYMBOL维于wordsBuffer中的下标
int curSymbolIndex;

//——————————————————————————————————————————//

//EMPTY <空语句>
TreeNode* ProcEmpty();
//WRITESENTENCE <写语句>
TreeNode* ProcWriteSentence();
//READSENTENCE <读语句>
TreeNode* ProcReadSentence();
//WHILESENTENCE <当型循环语句>
TreeNode* ProcWhileSentence();
//CALLSENTENCE <过程调用语句>
TreeNode* ProcCallSentence();
//IFSENTENCE <条件语句>
TreeNode* ProcIfSentence();
//FACTOR <因子>
TreeNode* ProcFactor();
//ITEM <项>
TreeNode* ProcItem();
//EXPRESSION <表达式>
TreeNode* ProcExpression();
//CONDITION <条件>
TreeNode* ProcCondition();
//COMBINED <复合语句>
TreeNode* ProcCombined();
//ASSIGNMENT <赋值语句>
TreeNode* ProcAssignment();
//STNTENCE <语句>
TreeNode* ProcSentence();
//PROCEDUREHEAD <过程首部>
TreeNode* ProcProcedureHead();
//PROCEDUREDECLARE <过程说明部分>
TreeNode* ProcProcedureDeclare();
//VARIABLEDECLARE <变量说明部分>
TreeNode* ProcVariableDeclare();
//CONSTANTDEFINE <常量定义>
TreeNode* ProcConstantDefine();
//CONSTANTDECLARE <常量说明部分>
TreeNode* ProcConstantDeclare();
//SUBPROG <分程序>
TreeNode* ProcSubprog(vector<int>* beginPos);
//PROGRAM <程序>
TreeNode* ProcProgram();

//——————————————————————————————————————————//

//错误处理
extern void ProcError(ErrorType errorType);

//匹配
string Match(const string token)
{
	if (token == curSymbol)
	{
		curSymbolIndex++;
		if (curSymbolIndex == wordsBuffer.size()) curSymbol = "";
		else curSymbol = wordsBuffer[curSymbolIndex].ID;
	}
	else ProcError(ErrorType::SyntaxError);

	return token;
}

//——————————————————————————————————————————//

// 嵌套外层符号表指针，栈顶是当前的符号表
stack<SymbolTable*> Tblptr;	
//根符号表
SymbolTable* rootSymbolTable;	
//四元式序列
//extern vector<Quaternary*> quaternaryList;	
//临时值的序号
//int TemporaryValueNumber;
//下一个四元式的序号，始终等于quaternaryList的size()
//int nextQuad;
//main程序入口(四元式)
//int startpos;

//机器码序列
extern vector<MachineCode> machineCodes;
//下一条机器码指令的绝对地址，始终等于machineCodes的size()
int nextCode;
//main程序入口(机器码)
int startAddr;

/*Place* NewTemp()
{
	Place* ret = new Place(TemporaryValueNumber, true);
	TemporaryValueNumber++;
	return ret;
}*/

/*//写入一个四元式
void Emit(OperType operType, Place* arg1, Place* arg2, Place* result)
{
	Place* a1 = arg1 ? new Place(*arg1) : nullptr;
	Place* a2 = arg2 ? new Place(*arg2) : nullptr;
	Place* re = result ? new Place(*result) : nullptr;
	quaternaryList.emplace_back(new Quaternary(operType, a1, a2, re));
	nextQuad++;
}*/

//写入一条机器码指令
void Gen(CodeType codeType, int levelDis, int arg)
{
	machineCodes.emplace_back(MachineCode(codeType, levelDis, arg));
	nextCode++;
}

//向上寻找对应的符号,level是与当前层的层差
bool LookUp(string name, SymbolTableItemType type, SymbolTableItem*& ret,int& level)
{
	SymbolTable* table = Tblptr.top();
	level = 0;
	while (table != nullptr)
	{
		if (table->FindItem(name, ret))
		{
			return type == ret->itemType;
		}
		else
		{
			table = table->GetFather();
			level++;
		}
	}
	return false;
}

//构造一个仅含有i的<int>链表
vector<int>* MakeList(const int& i)
{
	vector<int>* ret = new vector<int>();
	ret->emplace_back(i);
	return ret;
}

//构造一个<int>链表
vector<int>* MakeList()
{
	return new vector<int>();
}

//复制一个链表
vector<int>* CloneList(vector<int>*& list)
{
	if (!list) return nullptr;
	return new vector<int>(*list);
}

//将两条链表合二为一，返回的是一个新的独立的链表
vector<int>* Merge(vector<int>*& p1,vector<int>*& p2)
{
	vector<int>* ret = new vector<int>();
	for (const auto& i : *p1)
	{
		ret->emplace_back(i);
	}

	for (const auto& i : *p2)
	{
		ret->emplace_back(i);
	}

	return ret;
}

//回填，把p所连接的所有的arg填为t
void backpatch(vector<int>*& p, int& t)
{
	/*int size = quaternaryList.size();
	for (const auto& i : *p)
	{
		if (i >= size) ProcError(ErrorType::SyntaxError);
		else
		{
			if (quaternaryList[i]->result == nullptr)
			{
				quaternaryList[i]->result = new Place(t);
			}
			else
			{
				quaternaryList[i]->result->type = 0;
				quaternaryList[i]->result->value = t;
			}
		}
	}*/

	int size = machineCodes.size();
	for (auto& i : *p)
	{
		if (i >= size) ProcError(ErrorType::SyntaxError);
		else
		{
			if (machineCodes[i].codeType == CodeType::JMP || machineCodes[i].codeType == CodeType::JPC)
			{
				machineCodes[i].arg = t;
			}
			else ProcError(ErrorType::SyntaxError);
		}
	}
}

//——————————————————————————————————————————//

//EMPTY <空语句>
TreeNode* ProcEmpty()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::EMPTY);
	ret->nextlist = MakeList();
	return ret;
}

//WRITESENTENCE <写语句>
TreeNode* ProcWriteSentence()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::WRITESENTENCE);
	Match("WRITE");
	Match("(");

	TreeNode* E = ProcExpression();
	//WRITE
	Gen(CodeType::OPR, 0, 14);
	//回车
	Gen(CodeType::OPR, 0, 15);
	delete E;
	while (curSymbol == ",")
	{
		Match(",");
		E = ProcExpression();
		//WRITE
		Gen(CodeType::OPR, 0, 14);
		//回车
		Gen(CodeType::OPR, 0, 15);
		delete E;
	}

	/*if (wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
	{
		string name = Match(curSymbol);
		SymbolTableItem* item = nullptr;
		int targetLevel = -1;
		if (LookUp(name, SymbolTableItemType::VARIABLE, item, targetLevel))
		{
			//Emit(OperType::WRITE, new Place(item), nullptr, nullptr);
			//从对应的变量处加载值
			Gen(CodeType::LOD, targetLevel, item->parameter2);
			//WRITE
			Gen(CodeType::OPR, 0, 14);
			//回车
			Gen(CodeType::OPR, 0, 15);
		}
		else ProcError(ErrorType::SyntaxError);
	}
	else ProcError(ErrorType::SyntaxError);
	while (curSymbol == ",")
	{
		Match(",");
		if (wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
		{
			string name = Match(curSymbol);
			SymbolTableItem* item = nullptr;
			int targetLevel = -1;
			if (LookUp(name, SymbolTableItemType::VARIABLE, item, targetLevel))
			{
				//Emit(OperType::WRITE, new Place(item), nullptr, nullptr);
				//从对应的变量处加载值
				Gen(CodeType::LOD, targetLevel, item->parameter2);
				//WRITE
				Gen(CodeType::OPR, 0, 14);
				//回车
				Gen(CodeType::OPR, 0, 15);
			}
			else ProcError(ErrorType::SyntaxError);
		}
		else ProcError(ErrorType::SyntaxError);
	}*/
	Match(")");

	ret->nextlist = MakeList();
	return ret;
}

//READSENTENCE <读语句>
TreeNode* ProcReadSentence()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::READSENTENCE);
	Match("READ");
	Match("(");

	if (wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
	{
		string name = Match(curSymbol);
		SymbolTableItem* item = nullptr;
		int targetLevel = -1;
		if (LookUp(name, SymbolTableItemType::VARIABLE, item, targetLevel))
		{
			//Emit(OperType::READ, new Place(item), nullptr, nullptr);
			//READ
			Gen(CodeType::OPR, 0, 16);
			//存储到对应的变量里
			Gen(CodeType::STO, targetLevel, item->parameter2);
		}
		else ProcError(ErrorType::SyntaxError);
	}
	else ProcError(ErrorType::SyntaxError);
	while (curSymbol == ",")
	{
		Match(",");
		if (wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
		{
			string name = Match(curSymbol);
			SymbolTableItem* item = nullptr;
			int targetLevel = -1;
			if (LookUp(name, SymbolTableItemType::VARIABLE, item, targetLevel))
			{
				//Emit(OperType::READ, new Place(item), nullptr, nullptr);
				//READ
				Gen(CodeType::OPR, 0, 16);
				//存储到对应的变量里
				Gen(CodeType::STO, targetLevel, item->parameter2);
			}
			else ProcError(ErrorType::SyntaxError);
		}
		else ProcError(ErrorType::SyntaxError);
	}
	Match(")");

	ret->nextlist = MakeList();
	return ret;
}

//WHILESENTENCE <当型循环语句>
TreeNode* ProcWhileSentence()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::WHILESENTENCE);

	Match("WHILE");
	int M1_Addr = nextCode;
	TreeNode* E = ProcCondition();

	Match("DO");
	int M2_Addr = nextCode;
	TreeNode* S = ProcSentence();

	backpatch(S->nextlist, M1_Addr);
	backpatch(E->truelist, M2_Addr);
	ret->nextlist = CloneList(E->falselist);
	//Emit(OperType::JMP, nullptr, nullptr, new Place(M1_Quad));
	Gen(CodeType::JMP, 0, M1_Addr);

	delete E, S;
	return ret;
}

//CALLSENTENCE <过程调用语句>
TreeNode* ProcCallSentence()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::CALLSENTENCE);
	Match("CALL");
	if (wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
	{
		string name = Match(curSymbol);
		SymbolTableItem* item =nullptr;
		int targetLevel = -1;
		if (LookUp(name, SymbolTableItemType::PROCEDURE, item, targetLevel))
		{
			Gen(CodeType::CAL, targetLevel, item->parameter2);
		}
		else ProcError(ErrorType::SyntaxError);
	}
	else ProcError(ErrorType::SyntaxError);

	ret->nextlist = MakeList();

	return ret;
}

//IFSENTENCE <条件语句>
TreeNode* ProcIfSentence()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::IFSENTENCE);
	Match("IF");
	TreeNode* E = ProcCondition();
	Match("THEN");
	int M_Addr = nextCode;
	TreeNode* S =  ProcSentence();

	backpatch(E->truelist, M_Addr);
	ret->nextlist = Merge(E->falselist, S->nextlist);

	delete E, S;
	return ret;
}

//FACTOR <因子>
TreeNode* ProcFactor()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::FACTOR);

	if (curSymbolIndex < wordsBuffer.size())
	{
		if (wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
		{
			string name = Match(curSymbol);
			//设置综合属性
			SymbolTableItem* item = nullptr;
			int targetLevel = -1;
			if (LookUp(name, SymbolTableItemType::VARIABLE, item, targetLevel))	//变量
			{
				Gen(CodeType::LOD, targetLevel, item->parameter2);
			}
			else if (LookUp(name, SymbolTableItemType::CONSTANT, item, targetLevel))	//常量
			{
				if (item)	Gen(CodeType::LIT, 0, item->parameter1);		
				else ProcError(ErrorType::SyntaxError);
			}
			else	ProcError(ErrorType::SyntaxError);
		}
		else if (wordsBuffer[curSymbolIndex].wordType == WordType::NUMBER)
		{
			int num = wordsBuffer[curSymbolIndex].NUM;
			Match(curSymbol);
			Gen(CodeType::LIT, 0, num);

		}
		else if (curSymbol == "(")
		{
			Match("(");
			TreeNode* E = ProcExpression();
			delete E;
			Match(")");
		}
		else ProcError(ErrorType::SyntaxError);
	}
	else ProcError(ErrorType::SyntaxError);

	return ret;
}

//ITEM <项>
TreeNode* ProcItem()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::ITEM);
	
	TreeNode* F1 = ProcFactor();

	bool isMul = false;				//是否是乘法
	while (curSymbol == "*" || curSymbol == "/")
	{
		if (curSymbol == "*") isMul = true;
		else if (curSymbol == "/") isMul = false;

		Match(curSymbol);
		TreeNode* F2 = ProcFactor();

		if (isMul) //Emit(OperType::MUL, ret->place, F2->place, ret->place);
			Gen(CodeType::OPR, 0, 5);
		else //Emit(OperType::DIV, ret->place, F2->place, ret->place);
			Gen(CodeType::OPR, 0, 6);

		delete F2;
	}

	delete F1;
	return ret;
}

//EXPRESSION <表达式>
TreeNode* ProcExpression()
{
	//生成的计算结果屯在栈顶
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::EXPRESSION);

	bool negative = false;
	if (curSymbol == "+" || curSymbol == "-")
	{
		if (curSymbol == "-") negative = true;
		Match(curSymbol);
	}

	TreeNode* I1 = ProcItem();
	
	if (negative) Gen(CodeType::OPR, 0, 1);

	bool isAdd = false;
	while (curSymbol == "+" || curSymbol == "-")
	{
		if (curSymbol == "+") isAdd = true;
		else if(curSymbol == "-") isAdd = false;

		Match(curSymbol);
		TreeNode* I2 = ProcItem();

		if (isAdd) //Emit(OperType::ADD, I1->place, I2->place, ret->place);
			Gen(CodeType::OPR, 0, 3);
		else //Emit(OperType::SUB, I1->place, I2->place, ret->place);
			Gen(CodeType::OPR, 0, 4);

		delete I2;
	}

	delete I1;
	return ret;
}

//CONDITION <条件>
TreeNode* ProcCondition()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::CONDITION);

	if (curSymbol == "ODD")	//是否为奇数？
	{
		Match("ODD");
		TreeNode* E = ProcExpression();
		Gen(CodeType::LIT, 0, 2);
		Gen(CodeType::OPR, 0, 7);
		Gen(CodeType::OPR, 0, 2);		//取反

		ret->truelist = MakeList(nextCode);
		ret->falselist = MakeList(nextCode + 1);

		Gen(CodeType::JPC, 0, 0);		//需要反填
		Gen(CodeType::JMP, 0, 0);		//需要反填
		delete E;
	}
	else
	{
		TreeNode* E1 = ProcExpression();

		int opType = -1;
		if (curSymbol == "=")
		{
			Match("=");
			opType = 0;
		}
		else if (curSymbol == "#")
		{
			Match("#");
			opType = 1;
		}
		else if (curSymbol == "<")
		{
			Match("<");
			opType = 2;
		}
		else if (curSymbol == "<=")
		{
			Match("<=");
			opType = 3;
		}
		else if (curSymbol == ">")
		{
			Match(">");
			opType = 4;
		}
		else if (curSymbol == ">=")
		{
			Match(">=");
			opType = 5;
		}
		else ProcError(ErrorType::SyntaxError);
		TreeNode* E2 =  ProcExpression();

		
		switch (opType)
		{
		case 0:
			//Emit(OperType::JEQ, E1->place, E2->place, nullptr);
			Gen(CodeType::OPR, 0, 8);
			break;
		case 1:
			//Emit(OperType::JNE, E1->place, E2->place, nullptr);
			Gen(CodeType::OPR, 0, 9);
			break;
		case 2:
			//Emit(OperType::JL, E1->place, E2->place, nullptr);
			Gen(CodeType::OPR, 0, 10);
			break;
		case 3:
			//Emit(OperType::JLE, E1->place, E2->place, nullptr);
			Gen(CodeType::OPR, 0, 11);
			break;
		case 4:
			//Emit(OperType::JG, E1->place, E2->place, nullptr);
			Gen(CodeType::OPR, 0, 12);
			break;
		case 5:
			//Emit(OperType::JGE, E1->place, E2->place, nullptr);
			Gen(CodeType::OPR, 0, 13);
			break;
		default:
			ProcError(ErrorType::SyntaxError);
			break;
		}
		Gen(CodeType::OPR, 0, 2);
		ret->truelist = MakeList(nextCode);
		ret->falselist = MakeList(nextCode + 1);
		//Emit(OperType::JMP, nullptr, nullptr, nullptr);
		Gen(CodeType::JPC, 0, 0);
		Gen(CodeType::JMP, 0, 0);

		delete E1, E2;
	}

	return ret;
}

//COMBINED <复合语句>
TreeNode* ProcCombined()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::COMBINED);

	Match("BEGIN");
	TreeNode* S1 = ProcSentence();

	//backpatch(S1->nextlist, nextCode);

	while (curSymbol == ";")
	{
		Match(";");
		//一直处理下去，S1永远是最新的一个的综合属性
		delete S1;
		S1 = ProcSentence();

		//backpatch(S1->nextlist, nextCode);
	}
	
	Match("END");

	if (ret->nextlist) delete ret->nextlist;
	ret->nextlist = CloneList(S1->nextlist);
	delete S1;
	return ret;
}

//ASSIGNMENT <赋值语句>
TreeNode* ProcAssignment()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::ASSIGNMENT);

	SymbolTableItem* item = nullptr;
	int targetLevel = -1;
	if (curSymbolIndex < wordsBuffer.size() && wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
	{
		string name = Match(curSymbol);
		if (LookUp(name, SymbolTableItemType::VARIABLE, item, targetLevel))
		{
			//ret->place = new Place(item);
		}
		else ProcError(ErrorType::SyntaxError);
	}
	else ProcError(ErrorType::SyntaxError);

	Match(":=");

	TreeNode* E = ProcExpression();

	//Emit(OperType::ASG, E->place, nullptr, ret->place);
	if (item)	Gen(CodeType::STO, targetLevel, item->parameter2);
	else ProcError(ErrorType::SyntaxError);
	ret->nextlist = MakeList();

	delete E;
	return ret;
}

//STNTENCE <语句>
TreeNode* ProcSentence()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::SENTENCE);
	TreeNode* S = nullptr;

	if (curSymbolIndex < wordsBuffer.size() && wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
	{
		S = ProcAssignment();
	}
	else if (curSymbol == "IF")
	{
		S = ProcIfSentence();
	}
	else if (curSymbol == "WHILE")
	{
		S = ProcWhileSentence();
	}
	else if (curSymbol == "CALL")
	{
		S = ProcCallSentence();
	}
	else if (curSymbol == "READ")
	{
		S = ProcReadSentence();
	}
	else if (curSymbol == "WRITE")
	{
		S = ProcWriteSentence();
	}
	else if (curSymbol == "BEGIN")
	{
		S = ProcCombined();
	}
	else
	{
		S = ProcEmpty();
	}

	ret->nextlist = CloneList(S->nextlist);
	backpatch(S->nextlist, nextCode);
	delete S;
	return ret;
}

//PROCEDUREHEAD <过程首部>
TreeNode* ProcProcedureHead()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::PROCEDUREHEAD);
	Match("PROCEDURE");

	string name;
	if (curSymbolIndex < wordsBuffer.size() && wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
	{
		name = curSymbol;
		Match(curSymbol);
	}
	else ProcError(ErrorType::SyntaxError);

	Match(";");

	SymbolTable* childTable = new SymbolTable(Tblptr.top());
	SymbolTableItem* item = Tblptr.top()->AddProcedure(name, childTable, nextCode);		
	Tblptr.push(childTable);

	ret->nextlist = MakeList(nextCode);
	Gen(CodeType::JMP, 0, 0); //需要回填过程的起始地址

	return ret;
}

//PROCEDUREDECLARE <过程说明部分>
TreeNode* ProcProcedureDeclare()
{
	curLevel++;
	if (curLevel >= 4) ProcError(ErrorType::SyntaxError);

	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::PROCEDUREDECLARE);
	TreeNode* head = ProcProcedureHead();

	TreeNode* subProg = ProcSubprog(head->nextlist);
	Match(";");
	//Emit(OperType::RET, nullptr, nullptr, nullptr);

	Tblptr.pop();

	while (curSymbol == "PROCEDURE")
	{
		curLevel--;
		TreeNode* procedureDeclare = ProcProcedureDeclare();

		delete procedureDeclare;
		curLevel++;
	}

	curLevel--;

	delete head, subProg;
	return ret;
}

//VARIABLEDECLARE <变量说明部分>
TreeNode* ProcVariableDeclare()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::VARIABLEDECLARE);
	Match("VAR");

	string name;
	if (curSymbolIndex < wordsBuffer.size() && wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
	{
		name = curSymbol;
		Match(curSymbol);
	}
	else ProcError(ErrorType::SyntaxError);
	SymbolTableItem* item = nullptr;
	if (Tblptr.top()->FindItem(name, item))
	{
		ProcError(ErrorType::SyntaxError);
	}
	else Tblptr.top()->AddVariable(name);

	while (curSymbol == ",")
	{
		Match(",");
		if (curSymbolIndex < wordsBuffer.size() && wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
		{
			name = curSymbol;
			Match(curSymbol);
		}
		else ProcError(ErrorType::SyntaxError);
		SymbolTableItem* item = nullptr;
		if (Tblptr.top()->FindItem(name, item))
		{
			ProcError(ErrorType::SyntaxError);
		}
		else Tblptr.top()->AddVariable(name);
	}

	Match(";");
	return ret;
}

//CONSTANTDEFINE <常量定义>
TreeNode* ProcConstantDefine()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::CONSTANTDEFINE);

	string name;
	int value = 0;
	if (curSymbolIndex < wordsBuffer.size() && wordsBuffer[curSymbolIndex].wordType == WordType::IDENTIFIER)
	{
		name = curSymbol;
		Match(curSymbol);
	}
	else ProcError(ErrorType::SyntaxError);

	Match("=");

	if (curSymbolIndex < wordsBuffer.size() && wordsBuffer[curSymbolIndex].wordType == WordType::NUMBER)
	{
		value = wordsBuffer[curSymbolIndex].NUM;
		Match(curSymbol);
	}
	else ProcError(ErrorType::SyntaxError);
	Tblptr.top()->AddConstant(name, value);

	return ret;
}

//CONSTANTDECLARE <常量说明部分>
TreeNode* ProcConstantDeclare()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::CONSTANTDECLARE);
	Match("CONST");

	TreeNode* constantDefine = ProcConstantDefine();
	while (curSymbol == ",")
	{
		Match(",");
		TreeNode* constantDefine2 = ProcConstantDefine();


		delete constantDefine2;
	}
	Match(";");


	delete constantDefine;
	return ret;
}

//SUBPROG <分程序>
TreeNode* ProcSubprog(vector<int>* beginPos)
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::SUBPROG);
	TreeNode* constantDeclare = nullptr, * variableDeclare = nullptr, * procedureDeclare = nullptr, * S = nullptr;

	if (curSymbol == "CONST")
	{
		constantDeclare = ProcConstantDeclare();
	}
	if (curSymbol == "VAR")
	{
		variableDeclare = ProcVariableDeclare();
	}
	if (curSymbol == "PROCEDURE")
	{
		procedureDeclare = ProcProcedureDeclare();
	}

	if (beginPos)
	{
		backpatch(beginPos, nextCode);
	}

	//记录main入口
	if (curLevel == 0)
	{
		//startpos = nextQuad;		//四元式的
		startAddr = nextCode;		//机器码的
	}

	//根据变量的数量申请DX个空间
	Gen(CodeType::INT, 0, Tblptr.top()->GetDX());

	S = ProcSentence();

	//RET
	Gen(CodeType::OPR, 0, 0);

	delete constantDeclare, variableDeclare, procedureDeclare, S;
	return ret;
}

//PROGRAM <程序>
TreeNode* ProcProgram()
{
	TreeNode* ret = new TreeNode(SyntaxTreeNodeType::PROGRAM);

	//初始跳转到main入口
	ret->nextlist = MakeList(nextCode);
	Gen(CodeType::JMP, 0, 0);	

	TreeNode* subProg = ProcSubprog(ret->nextlist);

	Match(".");

	delete subProg;
	return ret;
}

void Init()
{
	//TemporaryValueNumber = 0;
	//nextQuad = 0;

	rootSymbolTable = new SymbolTable(NULL);
	Tblptr.push(rootSymbolTable);

	//startpos = 0;
	startAddr = 0;
}

//主处理程序
void SyntaxAnalyze()
{
	Init();

	if (wordsBuffer.empty())
	{
		curSymbol = "";
	}
	else curSymbol = wordsBuffer.front().ID;
	curSymbolIndex = 0;
	curLevel = 0;
	TreeNode* program = ProcProgram();
	delete program;
	//如果分析完了发现栈里还有东西，就判失败
	if (curSymbolIndex != wordsBuffer.size()) ProcError(ErrorType::SyntaxError);

	//delete rootSymbolTable;
	//rootSymbolTable = nullptr;
}