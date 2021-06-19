#pragma once
/*
	���ͷ�ļ�������һЩ�﷨�Ƶ�������õ�����
*/

#include <iostream>
#include <string>
#include <vector>
#include "publicClass.h"

using namespace std;

//�﷨�����ڵ�ؼ���
enum class SyntaxTreeNodeType
{
	UNKNOW = -1,								//δ֪				-1											
	PROGRAM,									//����				0
	SUBPROG,									//�ֳ���				1
	CONSTANTDECLARE,							//����˵������		2
	CONSTANTDEFINE,								//��������			3
	VARIABLEDECLARE,							//����˵������		4
	PROCEDUREDECLARE,							//����˵������		5
	PROCEDUREHEAD,								//�����ײ�			6
	SENTENCE,									//���				7
	ASSIGNMENT,									//��ֵ���			8
	COMBINED,									//�������			9
	CONDITION,									//����				10
	EXPRESSION,									//���ʽ				11
	ITEM,										//��					12
	FACTOR,										//����				13
	IFSENTENCE,									//�������			14
	CALLSENTENCE,								//���̵������		15
	WHILESENTENCE,								//����ѭ�����		16
	READSENTENCE,								//�����				17
	WRITESENTENCE,								//д���				18
	EMPTY,										//�����				19
	//����������Ҷ�ӽڵ�
	LP,											//������				20
	RP,											//������				21
	COMMA,										//����				22
	NUMBER,										//����				23
	LETTER										//��ĸ				24
};

class SymbolTable;
class SymbolTableItem;

/*class Place
{
public:
	int type;		//0:����		1:��ʶ��		2:��ʱ����
	int value;		//�洢���ֻ���ʱ������
	SymbolTableItem* item;	//�洢��ʶ��

	Place() :type(0), value(0), item(nullptr) {}
	Place(int value) :type(0), value(value), item(nullptr) {}
	Place(int tempNum, bool isTemp) :type(2), value(tempNum), item(nullptr) {}
	Place(SymbolTableItem* item) :type(1), value(0), item(item) {}
};*/

/*//��Ԫʽ�еĲ�������
enum class OperType {
	JEQ, JNE, JL, JLE,
	JG, JGE, JMP, ADD,
	SUB, MUL, DIV, MOD,
	UMI, ASG, READ, WRITE,
	CALL, RET
};*/

/*//��Ԫʽ
class Quaternary
{
public:
	//	op�����ͣ�
	//	0��������ת		1����������ת		2��С����ת		3��С�ڵ�����ת		4��������ת		5�����ڵ�����ת	6: ��������ת
	//	7���ӷ�			8������			9���˷�			10������				11��ȡ��			12��ȡ��			13����ֵ
	//	14������READ		15�����WRITE
	//	16������			17������
	OperType op;
	Place* arg1;
	Place* arg2;
	Place* result;
	int index;		//����Ԫʽ����Ϊ����������Ԫʽ��Ӧ�Ļ��������ʼ��ַ

	Quaternary(OperType op, Place* arg1, Place* arg2, Place* result) :op(op), arg1(arg1), arg2(arg2), result(result),index(0) {}
	~Quaternary()
	{
		if (arg1) delete arg1;
		if (arg2) delete arg2;
		if (result) delete result;
	}
};*/

//�﷨���ڵ�:�ۺ�����
class TreeNode
{
public:
	SyntaxTreeNodeType nodeType;				//�ڵ�����(�������������ʵûʲô��)
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

//���ű���������
enum class SymbolTableItemType
{
	CONSTANT,
	VARIABLE,
	PROCEDURE
};

//���ű����
class SymbolTableItem
{
public:
	SymbolTableItemType itemType;			//���������
	int parameter1;							//����һ�������CONSTANT��Ϊ������ֵ������Ϊ���ڵĲ��
	int parameter2;							//��������VARIABLE��PROCEDURE����Ե�ַ
	SymbolTable* childTable;				//��������PROCEDURE�ķ��ű�

	string name;							//���ű��������

	SymbolTableItem(SymbolTableItemType itemType, int para1, int para2, SymbolTable* childTable, string name) :
		itemType(itemType), parameter1(para1), parameter2(para2), childTable(childTable), name(name) {}
};

//���ű�
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
		//Ƕ�״��������Ӽ����ű������tableItem����
		for (auto& i : table)
		{
			if (i->itemType == SymbolTableItemType::PROCEDURE && i->childTable != nullptr)
			{
				delete i->childTable;
				i->childTable = nullptr;
			}
		}

		/*//ɾ�����е�SymbolTableItem
		for (auto& i : table)
		{
			if (i)	delete i;
			
		}*/
	}

	SymbolTable* GetFather() const { return fatherTable; }						//��ȡ�����ű�
	
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
		//addr����Ե�ַ
		SymbolTableItem* ret = new SymbolTableItem(SymbolTableItemType::PROCEDURE, level, addr, childTable, name);
		table.push_back(ret);
		return ret;
	}

	static SymbolTable* MakeTable(SymbolTable* previous)						//����һ���±��丸��Ϊprevious
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
	//����Ǵ�ģ���Ƕ���ҵ���Ӧ���е�����ظ�����
	//���������ı����������ڲ��ͬ��������Ӧ�ñ��﷨����
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
	int GetLevel() const { return level; }				//��ȡ�˷��ű�Ĳ��
	int GetDX() const { return DX; }					//��ȡ�˷��ű��DX�������ں�����ʼ��ʱ��INT���ٿռ�	

private:
	vector<SymbolTableItem*> table;			//�˷��ű�ı���
	SymbolTable* fatherTable;				//�����ű�
	int level;								//���,���
	int DX;									//��������Ե�ַ
};