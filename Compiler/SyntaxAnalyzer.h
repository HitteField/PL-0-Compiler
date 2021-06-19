#pragma once
/*
	这个头文件定义了一些语法制导翻译会用到的类
*/

#include <iostream>
#include <string>
#include <vector>
#include "publicClass.h"

using namespace std;

//语法分析节点关键字
enum class SyntaxTreeNodeType
{
	UNKNOW = -1,								//未知				-1											
	PROGRAM,									//程序				0
	SUBPROG,									//分程序				1
	CONSTANTDECLARE,							//常量说明部分		2
	CONSTANTDEFINE,								//常量定义			3
	VARIABLEDECLARE,							//变量说明部分		4
	PROCEDUREDECLARE,							//过程说明部分		5
	PROCEDUREHEAD,								//过程首部			6
	SENTENCE,									//语句				7
	ASSIGNMENT,									//赋值语句			8
	COMBINED,									//复合语句			9
	CONDITION,									//条件				10
	EXPRESSION,									//表达式				11
	ITEM,										//项					12
	FACTOR,										//因子				13
	IFSENTENCE,									//条件语句			14
	CALLSENTENCE,								//过程调用语句		15
	WHILESENTENCE,								//当型循环语句		16
	READSENTENCE,								//读语句				17
	WRITESENTENCE,								//写语句				18
	EMPTY,										//空语句				19
	//下面三个是叶子节点
	LP,											//左括号				20
	RP,											//右括号				21
	COMMA,										//逗号				22
	NUMBER,										//数字				23
	LETTER										//字母				24
};

class SymbolTable;
class SymbolTableItem;

/*class Place
{
public:
	int type;		//0:数字		1:标识符		2:临时变量
	int value;		//存储数字或临时变量名
	SymbolTableItem* item;	//存储标识符

	Place() :type(0), value(0), item(nullptr) {}
	Place(int value) :type(0), value(value), item(nullptr) {}
	Place(int tempNum, bool isTemp) :type(2), value(tempNum), item(nullptr) {}
	Place(SymbolTableItem* item) :type(1), value(0), item(item) {}
};*/

/*//四元式中的操作类型
enum class OperType {
	JEQ, JNE, JL, JLE,
	JG, JGE, JMP, ADD,
	SUB, MUL, DIV, MOD,
	UMI, ASG, READ, WRITE,
	CALL, RET
};*/

/*//四元式
class Quaternary
{
public:
	//	op的类型：
	//	0：等于跳转		1：不等于跳转		2：小于跳转		3：小于等于跳转		4：大于跳转		5：大于等于跳转	6: 无条件跳转
	//	7：加法			8：减法			9：乘法			10：除法				11：取余			12：取负			13：赋值
	//	14：输入READ		15：输出WRITE
	//	16：调用			17：返回
	OperType op;
	Place* arg1;
	Place* arg2;
	Place* result;
	int index;		//在四元式翻译为机器码后此四元式对应的机器码的起始地址

	Quaternary(OperType op, Place* arg1, Place* arg2, Place* result) :op(op), arg1(arg1), arg2(arg2), result(result),index(0) {}
	~Quaternary()
	{
		if (arg1) delete arg1;
		if (arg2) delete arg2;
		if (result) delete result;
	}
};*/

//语法树节点:综合属性
class TreeNode
{
public:
	SyntaxTreeNodeType nodeType;				//节点类型(最后看来这玩意其实没什么用)
	vector<int>* truelist;
	vector<int>* falselist;
	vector<int>* nextlist;

	//Place* place;

	TreeNode(SyntaxTreeNodeType nodeType = SyntaxTreeNodeType::UNKNOW) :nodeType(nodeType)
	{
		truelist = nullptr;
		falselist = nullptr;
		nextlist = nullptr;
		//place = nullptr;
	}

	~TreeNode()
	{
		if(truelist != nullptr)
			delete truelist;
		if (falselist != nullptr)
			delete falselist;
		if (nextlist != nullptr)
			delete nextlist;
		//if (place != nullptr)
		//	delete place;
	}
};

//符号表表项的类型
enum class SymbolTableItemType
{
	CONSTANT,
	VARIABLE,
	PROCEDURE
};

//符号表表项
class SymbolTableItem
{
public:
	SymbolTableItemType itemType;			//此项的类型
	int parameter1;							//参数一：如果是CONSTANT则为常数的值，否则为所在的层次
	int parameter2;							//参数二：VARIABLE和PROCEDURE的相对地址
	SymbolTable* childTable;				//参数三：PROCEDURE的符号表

	string name;							//符号表项的名字

	SymbolTableItem(SymbolTableItemType itemType, int para1, int para2, SymbolTable* childTable, string name) :
		itemType(itemType), parameter1(para1), parameter2(para2), childTable(childTable), name(name) {}
};

//符号表
class SymbolTable
{
public:

	SymbolTable(SymbolTable* father): DX(3) 
	{
		fatherTable = father;
		if (father != nullptr)
			level = fatherTable->level + 1;
		else
			level = 0;
	}
	~SymbolTable()
	{
		//嵌套处理所有子级符号表的清理tableItem操作
		for (auto& i : table)
		{
			if (i->itemType == SymbolTableItemType::PROCEDURE && i->childTable != nullptr)
			{
				delete i->childTable;
				i->childTable = nullptr;
			}
		}

		/*//删除所有的SymbolTableItem
		for (auto& i : table)
		{
			if (i)	delete i;
			
		}*/
	}

	SymbolTable* GetFather() const { return fatherTable; }						//获取父符号表
	
	void AddItem(string name, SymbolTableItemType type, int para1, int para2, SymbolTable* childTable)
	{
		if (type == SymbolTableItemType::VARIABLE) para2 = DX++;
		if (type != SymbolTableItemType::CONSTANT) para1 = level;
		//table.push_back({ type,para1,para2,childTable,name });
		table.push_back(new SymbolTableItem(type, para1, para2, childTable, name));
	}
	void AddConstant(string name, int value)
	{
		table.push_back(new SymbolTableItem(SymbolTableItemType::CONSTANT, value, 0, nullptr, name));
	}
	void AddVariable(string name)
	{
		table.push_back(new SymbolTableItem(SymbolTableItemType::VARIABLE, level, DX++, nullptr, name));
	}
	SymbolTableItem* AddProcedure(string name, SymbolTable* childTable,int addr)
	{
		//addr是相对地址
		SymbolTableItem* ret = new SymbolTableItem(SymbolTableItemType::PROCEDURE, level, addr, childTable, name);
		table.push_back(ret);
		return ret;
	}

	static SymbolTable* MakeTable(SymbolTable* previous)						//创建一个新表，其父亲为previous
	{
		return new SymbolTable(previous);
	}

	bool FindItem(string name, SymbolTableItem*& ret)
	{
		for (auto& i : table)
		{
			if (i->name == name)
			{
				ret = i;
				return true;
			}
		}
		return false;
	}
	//这个是错的，会嵌套找到不应该有的外层重复定义
	//比如用外层的变量而不是内层的同名常量（应该报语法错误）
	bool FindItem(string name, SymbolTableItemType type, SymbolTableItem*& ret)	
	{
		for (auto& i : table)
		{
			if (i->name == name && i->itemType == type)
			{
				ret = i;
				return true;
			}
		}
		return false;
	}
	int GetLevel() const { return level; }				//获取此符号表的层次
	int GetDX() const { return DX; }					//获取此符号表的DX，用于在函数开始的时候INT开辟空间	

private:
	vector<SymbolTableItem*> table;			//此符号表的表项
	SymbolTable* fatherTable;				//父符号表
	int level;								//深度,层次
	int DX;									//变量的相对地址
};