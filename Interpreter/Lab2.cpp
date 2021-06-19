#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

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

//����������
vector<MachineCode> machineCodes;
//���ļ�
ifstream infile("programCodes.dat");

//#define Debug

#ifdef  Debug
ifstream inputfile("E:\\��ѧϰ\\����ԭ��\\spfa\\test5.in");
//ofstream outfile("output.txt");
#endif //  Debug


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

//����ʱջ
vector<int> theStack;
//�����Ĵ���
int IP;     //��ǰ���е�ָ����±�
int PC;     //��һ��Ҫִ�е�Ŀ��ָ��
int SP;     //ջ���Ĵ���
int BP;     //��ַ�Ĵ���
//��ÿ�����̿�ʼʱ����ʼ���������ռ䣺
//SL:��̬����ָ����ù��̵�ֱ�����������ʱ�����ݶεĻ���ַ
//DL:��̬����ָ����øù���ǰ�������й��̵����ݶεĻ���ַ
//RA:���ص�ַ����¼���øù���ʱ��Ŀ�����ϵ㣬������CALʱPC��ֵ
//�����Ƿ������
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
    BP = 0;         //��ʼ��ַΪ0

    for (int i = 0; i < 3; ++i)
        theStack.push_back(-1);      //main�������ռ�ȫ��-1
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
        //ѹջ
        SP += arg;
        //ѹ���������������ı�������ʼ��ȫ����0
        arg -= 3;        
        while (arg--) theStack.push_back(0);

        PC++;
        break;
    }
    case CodeType::CAL:
    {
        //��д��̬��
        int staticLink = BP;
        while (levelDis--) staticLink = theStack[staticLink];
        theStack.push_back(staticLink);
        //��д��̬��
        theStack.push_back(BP);
        //��д���ص�ַ
        theStack.push_back(PC + 1);
        //������һ��ָ��Ϊ�������
        PC = arg;
        //�����µĻ�ַ
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
        case 0:         //���򷵻�
        {
            int nums = SP - BP + 1;
            SP = BP - 1;
            PC = theStack[BP + 2];
            BP = theStack[BP + 1];            
            while (nums--) theStack.pop_back();  
            if (SP == BP && BP == PC && PC == -1) isEnd = true;
            break;
        }
        case 1:         //ȡ����
        {
            theStack[SP] = -theStack[SP];
            break;
        }
        case 2:         //ȡ��
        {
            if (theStack[SP]) theStack[SP] = 0;
            else theStack[SP] = 1;
            break;
        }
        case 3:         //�ӷ�
        {
            theStack[SP - 1] = theStack[SP - 1] + theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 4:         //����
        {
            theStack[SP - 1] = theStack[SP - 1] - theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 5:         //�˷�
        {
            theStack[SP - 1] = theStack[SP - 1] * theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 6:         //����
        {
            theStack[SP - 1] = theStack[SP - 1] / theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 7:         //ȡ��
        {
            theStack[SP - 1] = theStack[SP - 1] % theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 8:         //���
        {
            theStack[SP - 1] = theStack[SP - 1] == theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }   
        case 9:         //������
        {
            theStack[SP - 1] = theStack[SP - 1] != theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }   
        case 10:        //С��
        {
            theStack[SP - 1] = theStack[SP - 1] < theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 11:        //С�ڵ���
        {
            theStack[SP - 1] = theStack[SP - 1] <= theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 12:        //����
        {
            theStack[SP - 1] = theStack[SP - 1] > theStack[SP];
            theStack.pop_back();
            SP--;
            break;
        }
        case 13:        //���ڵ���
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

