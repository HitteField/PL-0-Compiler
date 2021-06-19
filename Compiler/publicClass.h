#pragma once
/*
	���ͷ�ļ�������һЩ���й��̶����ܻ��õ�����
	�����쳣�ͻ�����
*/

#include <iostream>
#include <vector>
#include "SyntaxAnalyzer.h"

//#define debugMode

using namespace std;

//��������
enum class ErrorType
{
	Unknow,
	LexicalError,
	SyntaxError
};

//���쳣��
class MyException
{
public:
	MyException(ErrorType _errorType = ErrorType::Unknow) :errorType(_errorType) {}

	void what() const
	{
		switch (errorType)
		{
		case ErrorType::LexicalError:
			cout << "Lexical Error" << endl;
			break;
		case ErrorType::SyntaxError:
			cout << "Syntax Error" << endl;
			break;
		default:
			cout << "Unknow Error" << endl;
			break;
		}
	}

private:
	ErrorType errorType;
};

/*PL/0�����룺������PL/0�����õĻ�����

����		���		����			����
INT		0		N			��ջ������N���洢��Ԫ��ͨ���ں������ú���Ҫ����ִ��
CAL		L		A			���õ�ַΪA�Ĺ��̣����ù����뱻���ù��̵Ĳ��ΪL
LIT		0		A			������A����ջ����t��1
LOD		L		A			�����ΪL��ƫ����ΪA�Ĵ洢��Ԫ��ֵȡ��ջ����t��1
STO		L		A			��ջ����ֵ������ΪL��ƫ����ΪA�ĵ�Ԫ��t��1
JMP		0		A			������ת������ַA
JPC		0		A			��ջ��Ϊ0��ת������ַA��t��1
OPR		0		0			���������ù��̣����ص��õ㲢��ջ			��������		RET
OPR		0		1			ջ�� := -ջ��							ȡ����		UML
OPR		0		2			ջ�� := !ջ��							ȡ��			NOT
OPR		0		3			��ջ�� := ��ջ�� + ջ����t��1				�ӷ�			ADD
OPR		0		4			��ջ�� := ��ջ�� - ջ����t��1				����			SUB
OPR		0		5			��ջ�� := ��ջ�� * ջ����t��1				�˷�			MUL
OPR		0		6			��ջ�� := ��ջ�� / ջ����t��1				����			DIV
OPR		0		7			��ջ�� := ��ջ�� % ջ����t��1				ȡ��			MOD
OPR		0		8			��ջ�� := (��ջ�� = ջ��)��t��1			����			EQUAL
OPR		0		9			��ջ�� := (��ջ�� != ջ��)��t��1			������		NOT EQUAL
OPR		0		10			��ջ�� := (��ջ�� < ջ��)��t��1			С��			LESS
OPR		0		11			��ջ�� := (��ջ�� <= ջ��)��t��1			С�ڵ���		LESS OR EQUAR
OPR		0		12			��ջ�� := (��ջ�� > ջ��)��t��1			����			GREATER
OPR		0		13			��ջ�� := (��ջ�� >= ջ��)��t��1			���ڵ���		GREATER OR EQUAR
OPR		0		14			ջ����ֵ���������̨��Ļ��t��1				���			WRITE
OPR		0		15			����̨��Ļ���һ������					����			ENDL
OPR		0		16			�ӿ���̨����һ�����룬ֲ��ջ����t��1		����			READ

*/

//����������
enum class CodeType { INT, CAL, LIT, LOD, STO, JMP, JPC, OPR };

class MachineCode
{
public:
	CodeType codeType;	//������
	int levelDis;		//���
	int arg;			//����
	MachineCode(CodeType type, int levelDis, int arg) :codeType(type), levelDis(levelDis), arg(arg) {}
};

