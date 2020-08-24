#ifndef PLURALPROPAGATION_HPP
#define PLURALPROPAGATION_HPP
#include "globalPolicyExecutor.hpp"

class PluralPropagation:
public GlobalPolicyExecutor
{
private:
    bool m_isOptimize;
private:
    void clear(void);
    void replaceSymb(SsaSymb* &replacer,SsaSymb* &replacee,SsaTac* &needReplaceTac);
    void testOneSymb(string originalName,SsaSymb* &replacer,
    SsaSymb* &replacee,UseSsaTac* &needReplaceUseTac);
public:
    PluralPropagation();
    ~PluralPropagation();
    void printInfoOfOptimizer(void);
    bool runOptimizer(FunctionBlock* block,
        FuncPropertyGetter* funcPropertyGetter);//运行优化器
};

PluralPropagation::PluralPropagation()
{
    m_name = "复数传播策略器";
}

PluralPropagation::~PluralPropagation()
{

}

void PluralPropagation::clear(void)
{
    m_isOptimize = false;
    m_block = NULL;
}

void PluralPropagation::printInfoOfOptimizer(void)
{

}

bool PluralPropagation::runOptimizer(FunctionBlock* block,
    FuncPropertyGetter* funcPropertyGetter)//运行优化器
{
    clear();
    m_block = block;
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    //u和t就是本次的目标
    unordered_map<string,SsaSymb*>& uName2SsaSymbs = m_block->getUName2SsaSymbs();
    unordered_map<string,SsaSymb*>& tName2SsaSymbs = m_block->getTName2SsaSymbs();
    if(uName2SsaSymbs.size() + tName2SsaSymbs.size() == 0)return m_isOptimize;
    //建立变量集合
    unordered_set<SsaSymb*> SsaSymbsSet;
    unordered_map<string,SsaSymb*>::iterator it;
    SsaSymbsSet.clear();
    for(it = uName2SsaSymbs.begin();it != uName2SsaSymbs.end();it++)SsaSymbsSet.insert(it->second);
    for(it = tName2SsaSymbs.begin();it != tName2SsaSymbs.end();it++)SsaSymbsSet.insert(it->second);
    //在这之前，先查看insert函数看看是否可以变成copy
    for(it = uName2SsaSymbs.begin();it != uName2SsaSymbs.end();it++)
    {
        SsaTac* curTac = (it->second)->defPoint;
        if(curTac->type != TAC_INSERT)continue;
        bool needTurnCopy = true;
        SsaSymb* curSymb = curTac->functionSymb[0];
        UseSsaTac* curSymbUse = curTac->functionSymb2Tac[0];
        for(uint i = 0;i < curTac->functionSymb.size();i++)
        {
            if(curSymb != curTac->functionSymb[i])
                needTurnCopy = false;
        }
        if(!needTurnCopy)continue;
        //就算长的都一样，还要看是不是都是 u0d0这样的符号，以防万一
        //如果出现，说明程序有未定义的变量，这不符合测试用例吧
        if(curSymb->type == SYM_VAR && curSymb->name[0] == 'u')
        {
            if(uName2SsaSymbs.find(curSymb->name) == uName2SsaSymbs.end())
            {   
                //没有什么办法，只能让其他优化器来删掉
                //先假装合法
                //cout << "这不符合测试用例" << endl;
                //exit(-1);
            }
        }
        for(uint i = 0;i < curTac->functionSymb.size();i++)
        {
            curTac->functionSymb[i] = NULL;
            curTac->functionSymb2Tac[i] = NULL;
        }
        curTac->type = TAC_COPY;
        curTac->second = curSymb;
        curTac->secondPoint = curSymbUse;
    }
    //查看每条定值语句，看看是否被copy
    vector<SsaSymb*> copyTacSsaSymbs;
    vector<string> needDeleteSymbs;
    copyTacSsaSymbs.clear();
    needDeleteSymbs.clear();
    it = uName2SsaSymbs.begin();
    for(;it != uName2SsaSymbs.end();it++)
    {
        SsaTac* curTac = (it->second)->defPoint;
        if(curTac->type != TAC_COPY)continue;
        if(curTac->second->type != SYM_VAR)continue;
        unordered_set<SsaSymb*>::iterator iter;
        iter = SsaSymbsSet.find(curTac->second);
        if(iter == SsaSymbsSet.end())continue;
        copyTacSsaSymbs.push_back(it->second);//在符号表里找得到
        needDeleteSymbs.push_back(it->first);
    }

    it = tName2SsaSymbs.begin();
    for(;it != tName2SsaSymbs.end();it++)
    {
        SsaTac* curTac = (it->second)->defPoint;
        if(curTac->type != TAC_COPY)continue;
        if(curTac->second->type != SYM_VAR)continue;
        unordered_set<SsaSymb*>::iterator iter;
        iter = SsaSymbsSet.find(curTac->second);
        if(iter == SsaSymbsSet.end())continue;
        copyTacSsaSymbs.push_back(it->second);
        needDeleteSymbs.push_back(it->first);
    }

    //对每个 copy 语句
    for(uint i = 0;i < copyTacSsaSymbs.size();i++)
    {
        SsaSymb* replaceeSymb = copyTacSsaSymbs[i];//被代替者
        SsaTac* curTac = replaceeSymb->defPoint;
        SsaSymb* replacerSymb = curTac->second;     //代替者
        //替换掉在其他地方使用的变量
        UseSsaTac* useTac = replaceeSymb->useList;
        while(useTac->next != NULL)
        {
            SsaTac* needReplaceTac = useTac->next->code;
            //在 needReplaceTac 指令中用 replacerSymb 代替 replaceeSymb
            replaceSymb(replacerSymb,replaceeSymb,needReplaceTac);
            useTac = useTac->next;
        }
    }
    for(uint i = 0;i < copyTacSsaSymbs.size();i++)
    {
        SsaSymb* replaceeSymb = copyTacSsaSymbs[i];//被代替者
        SsaTac* curTac = replaceeSymb->defPoint;
        curTac->second->useTimes--;
        deleteUseSsaTac(curTac->secondPoint);
        curTac->type = TAC_UNDEF;        
    }
    for(uint i = 0;i < basicBlocks.size();i++)
    {
        basicBlocks[i]->cleanDirtyTac();
    }
    for(uint i = 0;i < needDeleteSymbs.size();i++)
    {
        if(needDeleteSymbs[i].c_str()[0] == 't')
            tName2SsaSymbs.erase(needDeleteSymbs[i]);
        else uName2SsaSymbs.erase(needDeleteSymbs[i]);
    }
    return m_isOptimize;
}

//代替者、被代替者、指令
void PluralPropagation::replaceSymb(SsaSymb* &replacer,
        SsaSymb* &replacee,SsaTac* &needReplaceTac)
{
    //最暴力的做法即可
    m_isOptimize = true;
    switch (needReplaceTac->type)
    {
    case TAC_INSERT:
        for(uint i = 0;i < needReplaceTac->functionSymb.size();i++)
        {
            if(replacee != needReplaceTac->functionSymb[i])continue;
            needReplaceTac->functionSymb[i] = replacer;
            
            addTacToUseListForSsaSymb(needReplaceTac,replacer,
            needReplaceTac->functionSymb2Tac[i]);
        }
        break;
    //二个操作数
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
        if(replacee == needReplaceTac->second)//如果找到被替换变量
        {
            needReplaceTac->second = replacer;
            addTacToUseListForSsaSymb(needReplaceTac,replacer,
            needReplaceTac->secondPoint);            
        }
        if(replacee == needReplaceTac->third)
        {
            needReplaceTac->third = replacer;
            addTacToUseListForSsaSymb(needReplaceTac,replacer,
            needReplaceTac->thirdPoint);            
        }
        break;
    //一个操作数
    case TAC_NEG:
    case TAC_POSI:
    case TAC_NOT:
    case TAC_COPY:
        if(replacee == needReplaceTac->second)
        {
            needReplaceTac->second = replacer;
            addTacToUseListForSsaSymb(needReplaceTac,replacer,
            needReplaceTac->secondPoint);            
        }
        break;
    case TAC_ARR_L:

        if(replacee == needReplaceTac->second)
        {
            needReplaceTac->second = replacer;
            addTacToUseListForSsaSymb(needReplaceTac,replacer,
            needReplaceTac->secondPoint);            
        }        
        if(replacee == needReplaceTac->third)
        {
            needReplaceTac->third = replacer;
            addTacToUseListForSsaSymb(needReplaceTac,replacer,
            needReplaceTac->thirdPoint);            
        }        
        break;
    //MEM
    case TAC_ARR_R:
    case TAC_LEA:
        if(replacee == needReplaceTac->third)
        {
            needReplaceTac->third = replacer;
            addTacToUseListForSsaSymb(needReplaceTac,replacer,
            needReplaceTac->thirdPoint);            
        }
        break;        
    case TAC_IFZ:
    case TAC_ACTUAL:  
        if(replacee == needReplaceTac->first)
        {
            needReplaceTac->first = replacer;
            addTacToUseListForSsaSymb(needReplaceTac,replacer,
            needReplaceTac->firstPoint);            
        }      
        break;
    case TAC_RETURN:
        if(needReplaceTac->first == NULL)break;
        if(replacee == needReplaceTac->first)
        {
            needReplaceTac->first = replacer;
            addTacToUseListForSsaSymb(needReplaceTac,replacer,
            needReplaceTac->firstPoint);            
        }             
        break;
    default:
        break;   
    } 
}
#endif