#ifndef GLOBALSYMTABLE_HPP
#define GLOBALSYMTABLE_HPP
#include "ssaCodeStruct.hpp"
#include <iostream>
#include <unordered_set>
#include <unordered_map>
using namespace std;
class ArrayValueUnit
{
private:
    unordered_map<uint,uint> m_var2Value;
    unordered_set<uint> m_indexList;
    void clear(void){m_var2Value.clear();m_indexList.clear();}
public:
    void addArrayUnit(uint index,uint value)
    {
        m_var2Value[index] = value;
        m_indexList.insert(index);
    }
    uint getValueOfUnit(uint index)
    {
        if(m_indexList.find(index) == m_indexList.end())return 0;
        return m_var2Value[index];
    }
    ArrayValueUnit(){clear();};
    ~ArrayValueUnit(){};
};
class GlobalSymTable
{
private:
    unordered_map<string,uint> m_var2value;
    unordered_map<string,ArrayValueUnit*> m_arr2Value;
public:
    GlobalSymTable();
    ~GlobalSymTable();
    SsaTac* m_tacHead;//全局变量初始化语句
    SsaTac* m_tacTail;//全局变量初始化语句
    void addVar(SsaSymb* var);
    void addTac(SsaTac* code);
    uint getValueOfArr(string arrName,uint index);
    SsaTac* getTacHead(void);
    SsaTac* getTacTail(void);
    void clear();
};

GlobalSymTable::GlobalSymTable()
{
    clear();
}

GlobalSymTable::~GlobalSymTable()
{
}

void GlobalSymTable::clear()
{
    m_tacHead = NULL;
    m_tacTail = NULL;
    m_var2value.clear();
}
void GlobalSymTable::addVar(SsaSymb* var)
{
    //cout << "this global var : " << var->name << endl;
}

void GlobalSymTable::addTac(SsaTac* code)
{
    if(m_tacHead == NULL)
    {
        m_tacHead = code;
        m_tacTail = code;
    }
    else 
    {
        m_tacTail->next = code;
        code->prev = m_tacTail;
        m_tacTail = code;
    }
    if(code->type == TAC_COPY)
        m_var2value[code->first->name] = code->second->value;
    else if(code->type == TAC_ARR_L)
    {
        ArrayValueUnit* curArr;
        unordered_map<string,ArrayValueUnit*>::iterator it;
        it = m_arr2Value.find(code->first->name);
        if(it == m_arr2Value.end())
        {
            curArr = new ArrayValueUnit();
            m_arr2Value[code->first->name] = curArr;
        }
        else curArr = it->second;
        curArr->addArrayUnit(code->second->value,code->third->value);
    }
}

SsaTac* GlobalSymTable::getTacHead(void)
{
    return m_tacHead;
}

SsaTac* GlobalSymTable::getTacTail(void)
{
    return m_tacTail;
}

uint GlobalSymTable::getValueOfArr(string arrName,uint index)
{
    ArrayValueUnit* curArr;
    unordered_map<string,ArrayValueUnit*>::iterator it;
    it = m_arr2Value.find(arrName);
    if(it == m_arr2Value.end())return 0;
    return m_arr2Value[arrName]->getValueOfUnit(index);
}

#endif 