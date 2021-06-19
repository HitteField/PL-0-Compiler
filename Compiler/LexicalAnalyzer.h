#pragma once
/*
	���ͷ�ļ�������һЩ�ʷ����������õ�����
*/

#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class WordType		//��������
{
	UNKNOW,				//δ֪
	//�ؼ���
	CONSTSYM, VARSYM, PROCEDURESYM, BEGINSYM, ENDSYM, ODDSYM, IFSYM, THENSYM, CALLSYM, WHILESYM, DOSYM, READSYM, WRITESYM,
	//��ȣ���ֵ���ӣ������ˣ����������ڣ�С�ڣ�С�ڵ��ڣ����ڣ����ڵ���
	EQUALSYM, ASSIGNSYM, ADDSYM, SUBSYM, MULSYM, DIVSYM, SHARPSYM, LESSSYM, LESSEQUALSYM, GREATSYM, GREATEQUALSYM,
	//�ֺţ����ţ��㣬�����ţ�������
	SEMISYM, COMMASYM, DOTSYM, LPARSYM, RPARSYM,
	NUMBER,				//����
	IDENTIFIER			//��ʶ��
};

struct WordInf			//������Ϣ
{
	int row;				//��������
	int col;				//��������
	WordType wordType;		//��������
	string ID;				//��ʶ���Ļ��ڱ�ʾ��
	long long NUM;			//���ֵĻ��ڱ�ʾֵ
	int tableIndex;			//�ڷ��ű�/�������е��±꣬��Ϊ��������Ϊ-1
	WordInf() :row(0), col(0), wordType(WordType::UNKNOW), ID(), NUM(0), tableIndex(-1) {}
	WordInf(int _row, int _col, WordType _type, string _id = "", long long _num = 0, int _sti = -1) :row(_row), col(_col), wordType(_type), ID(_id), NUM(_num), tableIndex(_sti) {}
};