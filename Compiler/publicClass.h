#pragma once
/*
	这个头文件定义了一些所有过程都可能会用到的类
	比如异常和机器码
*/

#include <iostream>
#include <vector>
#include "SyntaxAnalyzer.h"

//#define debugMode

using namespace std;

//错误类型
enum class ErrorType
{
	Unknow,
	LexicalError,
	SyntaxError
};

//报异常的
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

/*PL/0机器码：真正的PL/0语言用的机器码

功能		层差		参数			含义
INT		0		N			在栈顶开辟N个存储单元，通常在函数调用后需要立即执行
CAL		L		A			调用地址为A的过程，调用过程与被调用过程的层差为L
LIT		0		A			立即数A存入栈顶，t加1
LOD		L		A			将层差为L，偏移量为A的存储单元的值取到栈顶，t加1
STO		L		A			将栈顶的值存入层差为L、偏移量为A的单元，t减1
JMP		0		A			无条件转移至地址A
JPC		0		A			若栈顶为0，转移至地址A，t减1
OPR		0		0			结束被调用过程，返回调用点并退栈			函数返回		RET
OPR		0		1			栈顶 := -栈顶							取负数		UML
OPR		0		2			栈顶 := !栈顶							取逆			NOT
OPR		0		3			次栈顶 := 次栈顶 + 栈顶，t减1				加法			ADD
OPR		0		4			次栈顶 := 次栈顶 - 栈顶，t减1				减法			SUB
OPR		0		5			次栈顶 := 次栈顶 * 栈顶，t减1				乘法			MUL
OPR		0		6			次栈顶 := 次栈顶 / 栈顶，t减1				除法			DIV
OPR		0		7			次栈顶 := 次栈顶 % 栈顶，t减1				取余			MOD
OPR		0		8			次栈顶 := (次栈顶 = 栈顶)，t减1			等于			EQUAL
OPR		0		9			次栈顶 := (次栈顶 != 栈顶)，t减1			不等于		NOT EQUAL
OPR		0		10			次栈顶 := (次栈顶 < 栈顶)，t减1			小于			LESS
OPR		0		11			次栈顶 := (次栈顶 <= 栈顶)，t减1			小于等于		LESS OR EQUAR
OPR		0		12			次栈顶 := (次栈顶 > 栈顶)，t减1			大于			GREATER
OPR		0		13			次栈顶 := (次栈顶 >= 栈顶)，t减1			大于等于		GREATER OR EQUAR
OPR		0		14			栈顶的值输出至控制台屏幕，t减1				输出			WRITE
OPR		0		15			控制台屏幕输出一个换行					换行			ENDL
OPR		0		16			从控制台读入一行输入，植入栈顶，t加1		输入			READ

*/

//操作码类型
enum class CodeType { INT, CAL, LIT, LOD, STO, JMP, JPC, OPR };

class MachineCode
{
public:
	CodeType codeType;	//操作码
	int levelDis;		//层差
	int arg;			//参数
	MachineCode(CodeType type, int levelDis, int arg) :codeType(type), levelDis(levelDis), arg(arg) {}
};

