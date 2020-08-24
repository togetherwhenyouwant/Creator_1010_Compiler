#ifndef FUNCSYMTALE_HPP
#define FUNCSYMTALE_HPP
#include <iostream>
#include <vector>
#include <unordered_map>
#include "ssaCodeStruct.hpp"
using namespace std;
typedef unsigned int uint;
class FuncSymTable
{
private:
    uint m_varNum;
public:
    unordered_map<uint,uint> m_varId2numId;
    unordered_map<uint,SsaSymb*> m_varId2symb;
    vector<uint> m_numId2varId;
    vector<SsaSymb*> m_numId2symb;

    //装数组的声明，用于后续输出用
    vector<SsaTac*> m_arrVar;
public:
    FuncSymTable();
    ~FuncSymTable();
    void addVar(SsaSymb* temp);
    unordered_map<uint,uint>& getVarId2numIdMap(void){return m_varId2numId;};
    vector<uint>& getNumId2varIdMap(void){return m_numId2varId;}
    vector<SsaSymb*>& getNumId2symbMap(void){return m_numId2symb;}
    unordered_map<uint,uint>& getVarId2numIdbMap(void){return m_varId2numId;}
    void deleteVars(vector<uint> varIdList);

    void addArrTac(SsaTac* tacNode);
    vector<SsaTac*>& getArrVar(void){return m_arrVar;};
};

FuncSymTable::FuncSymTable()
{
    m_varNum = 0;
    m_arrVar.clear();
    m_varId2numId.clear();
    m_numId2varId.clear();
    m_numId2symb.clear();
    m_varId2symb.clear();
}

FuncSymTable::~FuncSymTable()
{
}
void FuncSymTable::addVar(SsaSymb* temp)
{
    //数组也包含
    //只记录程序局部变量 u
    if(temp->type == SYM_VAR || temp->type == SYM_ARRAY)
    {
        uint varId = atoi(temp->name+1);
        m_varId2numId[varId] = m_varNum++;
        m_varId2symb[varId] = temp;
        m_numId2varId.push_back(varId);
        m_numId2symb.push_back(temp);
        //cout << "this is functional var : " << temp->name << endl;
    }
        
}

void FuncSymTable::deleteVars(vector<uint> varIdList)
{
    if(varIdList.size() == 0)return;
    for(uint i = 0;i < varIdList.size();i++)
    {
        uint varId = varIdList[i];
        m_varId2numId.erase(varId);
        m_varId2symb.erase(varId);
    }
    m_varNum = 0;
    m_numId2varId.clear();
    m_numId2symb.clear();
    unordered_map<uint,uint>::iterator it;
    for(it = m_varId2numId.begin();it!=m_varId2numId.end();it++)
    {
        m_numId2varId.push_back(it->first);
        m_numId2symb.push_back(m_varId2symb[it->first]);
        m_varId2numId[it->first] = m_varNum++;
    }
}

void FuncSymTable::addArrTac(SsaTac* tacNode)
{
    m_arrVar.push_back(tacNode);
}
#endif