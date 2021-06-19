#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

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

//机器码序列
vector<MachineCode> machineCodes;
//读文件
ifstream infile("programCodes.dat");

//#define Debug

#ifdef  Debug
ifstream inputfile("E:\\☆学习\\编译原理\\spfa\\test5.in");
//ofstream outfile("output.txt");
#endif //  Debug


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

//运行时栈
vector<int> theStack;
//几个寄存器
int IP;     //当前运行的指令的下标
int PC;     //下一条要执行的目标指令
int SP;     //栈顶寄存器
int BP;     //基址寄存器
//在每个过程开始时，初始分配三个空间：
//SL:静态链，指向定义该过程的直接外过程运行时的数据段的基地址
//DL:动态链，指向调用该过程前正在运行过程的数据段的基地址
//RA:返回地址，记录调用该过程时的目标程序断点，即调用CAL时PC的值
//程序是否结束了
bool isEnd = false;

void LoadProgramCode()
{
    while (!infile.eof())
    {
        int opr, level, arg;
        infile >> opr >> level >> arg;
        machineCodes.push_back(MachineCode((CodeType)opr, level, arg));
    }
    infile.close();

    IP = 0;
    PC = 1;
    SP = -1;         
    BP = 0;         //初始基址为0

    for (int i = 0; i < 3; ++i)
        theStack.push_back(-1);      //main的三个空间全是-1
}

void ProcessProgram()
{
    MachineCode& instruction = machineCodes[IP];
    CodeType codeType = instruction.codeType;
    int levelDis = instruction.levelDis;
    int arg = instruction.arg;
    switch (codeType)
    {
    case CodeType::INT:
    {
        //压栈
        SP += arg;
        //压入除了三个链以外的变量，初始化全都是0
        arg -= 3;        
        while (arg--) theStack.push_back(0);

        PC++;
        break;
    }
    case CodeType::CAL:
    {
        //填写静态链
        int staticLink = BP;
        while (levelDis--) staticLink = theStack[staticLink];
        theStack.push_back(staticLink);
        //填写动态链
        theStack.push_back(BP);
        //填写返回地址
        theStack.push_back(PC + 1);
        //设置下一条指令为过程入口
        PC = arg;
        //设置新的基址
        BP = SP + 1;
        break;
    }
    case CodeType::LIT:
    {
        theStack.push_back(arg);
        SP++;
        PC++;
        break;
    }
    case CodeType::LOD:
    {
        int targetBase = BP;
        while (levelDis--) targetBase = theStack[targetBase];
        theStack.push_back(theStack[targetBase + arg]);
        SP++;
        PC++;
        break;
    }
    case CodeType::STO:
    {
        int targetBase = BP;
        while (levelDis--) targetBase = theStack[targetBase];
        theStack[targetBase + arg] = theStack[SP];
        theStack.pop_back();
        SP--;
        PC++;
        break;
    }
    case CodeType::JMP:
    {
        PC = arg;
        break;
    }
    case CodeType::JPC:
    {
        int value = theStack[SP];
        SP--;
        theStack.pop_back();
        if (!value) PC = arg;
        else PC++;
        break;
    }
    case CodeType::OPR:
    {
        PC++;
        switch (arg)
        {
        case 0:         //程序返回
        {
            int nums = SP - BP + 1;
            SP = BP - 1;
            PC = theStack[BP + 2];
            BP = theStack[BP + 1];            
            while (nums--) theStack.pop_back();  
            if (SP == BP && BP == PC && PC == -1) isEnd = true;
            break;
        }
        case 1:         //取负数
        {
            theStack[SP] = -theStack[SP];
            break;
        }
        case 2:         //取反
        {
            if (theStack[SP]) theStack[SP] = 0;
            else theStack[SP] = 1;
            break;
        }
        case 3:         //加法
        {
            theStack[SP - 1] = theStack[SP - 1] + theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 4:         //减法
        {
            theStack[SP - 1] = theStack[SP - 1] - theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 5:         //乘法
        {
            theStack[SP - 1] = theStack[SP - 1] * theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 6:         //除法
        {
            theStack[SP - 1] = theStack[SP - 1] / theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 7:         //取余
        {
            theStack[SP - 1] = theStack[SP - 1] % theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 8:         //相等
        {
            theStack[SP - 1] = theStack[SP - 1] == theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }   
        case 9:         //不等于
        {
            theStack[SP - 1] = theStack[SP - 1] != theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }   
        case 10:        //小于
        {
            theStack[SP - 1] = theStack[SP - 1] < theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 11:        //小于等于
        {
            theStack[SP - 1] = theStack[SP - 1] <= theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 12:        //大于
        {
            theStack[SP - 1] = theStack[SP - 1] > theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 13:        //大于等于
        {
            theStack[SP - 1] = theStack[SP - 1] >= theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 14:
        {
            cout << theStack[SP];       
            theStack.pop_back();
            SP--;
            break;
        }
        case 15:
        {
            cout << endl;
            break;
        }
        case 16:
        {
            int value;
#ifdef Debug
            inputfile >> value;
#else
            cin >> value;
#endif // Debug   
            theStack.push_back(value);
            SP++;
            break;
        }
        }
        break;
    }
    }

    IP = PC;
}

int main()
{
    LoadProgramCode();
    while (!isEnd)
        ProcessProgram();

    int i;   
#ifdef Debug
    while (inputfile >> i);
#else
    while (cin >> i);
#endif // Debug   

    return 0;
}

