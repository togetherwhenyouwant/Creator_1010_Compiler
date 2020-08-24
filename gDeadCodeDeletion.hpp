#ifndef GDEADCODEDELETION_HPP
#define GDEADCODEDELETION_HPP
#include <queue>
#include "globalPolicyExecutor.hpp"
using namespace std;
class GDeadCodeDeletion:
public GlobalPolicyExecutor
{
public:
    GDeadCodeDeletion();
    ~GDeadCodeDeletion();
    void runOptimizer(void);
    void printInfoOfOptimizer(void);
    bool runOptimizer(FunctionBlock* block,
        FuncPropertyGetter* funcPropertyGetter);//运行优化器

};

GDeadCodeDeletion::GDeadCodeDeletion(/* args */)
{
    m_name = "死代码删除器";
}

GDeadCodeDeletion::~GDeadCodeDeletion()
{
}
void GDeadCodeDeletion::printInfoOfOptimizer(void)
{

}

//TODO
//删掉的变量名字也得抹去或者存起来
bool GDeadCodeDeletion::runOptimizer(FunctionBlock* block,
    FuncPropertyGetter* funcPropertyGetter)
{
    //得到没有使用的节点
    m_block = block;
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    unordered_map<string,SsaSymb*>& uName2SsaSymbs = m_block->getUName2SsaSymbs();
    unordered_map<string,SsaSymb*>& tName2SsaSymbs = m_block->getTName2SsaSymbs();
    queue<SsaSymb*> deadSymbList;
    while(!deadSymbList.empty())deadSymbList.pop();
    unordered_map<string,SsaSymb*>::iterator it = uName2SsaSymbs.begin();
    for(;it != uName2SsaSymbs.end();it++)
    {
        if(it->second->useTimes != 0)continue;
        deadSymbList.push(it->second);
    }
    it = tName2SsaSymbs.begin();
    for(;it != tName2SsaSymbs.end();it++)
    {
        if(it->second->useTimes != 0)continue;
        deadSymbList.push(it->second);        
    }
    //开始拓扑
    vector<string> needDeleteVarList;
    needDeleteVarList.clear();
    while(!deadSymbList.empty())
    {
        SsaSymb* needDelVar = deadSymbList.front();
        needDeleteVarList.push_back(needDelVar->name);
        deadSymbList.pop();
        SsaTac* needDelTac = needDelVar->defPoint;
        switch(needDelTac->type)
        {
        case TAC_ADD:
        case TAC_SUB:
        case TAC_MUL:
        case TAC_DIV:
        case TAC_EQ:
        case TAC_MOD:
        case TAC_NE:
        case TAC_LT:
        case TAC_LE:
        case TAC_GT:
        case TAC_GE:
        case TAC_OR:
        case TAC_AND:
            
            if((needDelTac->second->type == SYM_VAR && 
                needDelTac->second->name[0] == 'u') ||
                needDelTac->second->name[0] == 't')
            {
                needDelTac->second->useTimes--;
                if(needDelTac->second->useTimes == 0)
                    deadSymbList.push(needDelTac->second);
                //删掉 needDelTac->first 中对这个的使用链
                deleteUseSsaTac(needDelTac->secondPoint);
                needDelTac->secondPoint = NULL;
            }
            if((needDelTac->third->type == SYM_VAR && 
                needDelTac->third->name[0] == 'u') ||
                needDelTac->third->name[0] == 't')
            {
                needDelTac->third->useTimes--;
                if(needDelTac->third->useTimes == 0)
                    deadSymbList.push(needDelTac->third);
                //删掉 needDelTac->first 中对这个的使用链
                deleteUseSsaTac(needDelTac->thirdPoint);
                needDelTac->thirdPoint = NULL;
            }
            needDelTac->type = TAC_UNDEF;
            break;
        case TAC_NEG:
        case TAC_POSI:
        case TAC_NOT:
        case TAC_COPY:
            if((needDelTac->second->type == SYM_VAR && 
                needDelTac->second->name[0] == 'u') ||
                needDelTac->second->name[0] == 't')
            {
                needDelTac->second->useTimes--;
                if(needDelTac->second->useTimes == 0)
                    deadSymbList.push(needDelTac->second);
                //删掉 needDelTac->first 中对这个的使用链
                deleteUseSsaTac(needDelTac->secondPoint);
                needDelTac->secondPoint = NULL;
            }
            needDelTac->type = TAC_UNDEF;
            break;
        case TAC_FORMAL:
            //什么也不做
            break;
        case TAC_INSERT:
            {
                for(uint i = 0;i < needDelTac->functionSymb.size();i++)
                {
                    //u0d0问题，为什么他的functionSymb2Tac[i]不为空呢
                    if(needDelTac->functionSymb2Tac[i]==NULL)continue;
                    SsaSymb* varSymb = needDelTac->functionSymb[i];
                    if((varSymb->type == SYM_VAR && 
                        varSymb->name[0] == 'u') ||
                        varSymb->name[0] == 't')
                    {
                        varSymb->useTimes--;
                        if(varSymb->useTimes == 0)
                            deadSymbList.push(varSymb);
                        //删掉 needDelTac->first 中对这个的使用链
                        deleteUseSsaTac(needDelTac->functionSymb2Tac[i]);
                        needDelTac->functionSymb2Tac[i] = NULL;
                    }
                }
            }
            needDelTac->type = TAC_UNDEF;
            break;
        case TAC_CALL:
            needDelTac->first = NULL;
            break;
        }
    }
    bool isOptimize = false;
    //清理垃圾
    for(uint i = 0;i < basicBlocks.size();i++)
    {
        basicBlocks[i]->cleanDirtyTac();
    }
    for(uint i = 0;i < needDeleteVarList.size();i++)
    {
        isOptimize = true;
        if(needDeleteVarList[i].c_str()[0] == 't')
            tName2SsaSymbs.erase(needDeleteVarList[i]);
        else uName2SsaSymbs.erase(needDeleteVarList[i]);
    }
    return isOptimize;
}
#endif
