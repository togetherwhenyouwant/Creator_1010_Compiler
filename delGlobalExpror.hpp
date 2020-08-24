#ifndef DELGLOBALEXPROR_HPP
#define DELGLOBALEXPROR_HPP
#include "globalPolicyExecutor.hpp"
#include <string>
using namespace std;
//TODO
//还有工作没有做完
//其实插入函数也算是一个表达式，也可以删除，但目前还没想到啥办法
class DelGlobalExpror:
public GlobalPolicyExecutor
{
private:
    bool m_isOptimize;
    unordered_map<uint,string> m_tacTypeToString;
    unordered_map<string,string> m_expressTable;
    vector<vector<uint>> m_mustPassNodeTree;//必经树
    FuncPropertyGetter* m_funcPropertyGetter;
private:
    void clear();
    bool weakeningTac(SsaTac* curTac);
    bool checkTheSymbVar(SsaSymb* varSymb);
    void deleteCommonExpression(uint blockId);
    string getStringOfSymb(SsaSymb* varSymb);
    void addExpressionIntoTable(SsaTac* curTac,vector<string>& needDeleteList);
public:
    DelGlobalExpror();
    ~DelGlobalExpror();
    void printInfoOfOptimizer(void);
    bool runOptimizer(FunctionBlock* block,
        FuncPropertyGetter* funcPropertyGetter);//运行优化器
};

DelGlobalExpror::DelGlobalExpror()
{
    m_name = "表达式删除器";
    clear();
}

void DelGlobalExpror::clear()
{
    m_block = NULL;
    m_isOptimize = false;
    m_expressTable.clear();
    m_tacTypeToString.clear();
    m_tacTypeToString[TAC_ADD] = "+";
    m_tacTypeToString[TAC_SUB] = "-";
    m_tacTypeToString[TAC_MUL] = "*";
    m_tacTypeToString[TAC_DIV] = "/";
    m_tacTypeToString[TAC_EQ] = "==";
    m_tacTypeToString[TAC_MOD] = "%%";
    m_tacTypeToString[TAC_NE] = "!=";
    m_tacTypeToString[TAC_LT] = "<";
    m_tacTypeToString[TAC_LE] = "<=";
    m_tacTypeToString[TAC_GT] = ">";
    m_tacTypeToString[TAC_GE] = ">=";
    m_tacTypeToString[TAC_OR] = "||";
    m_tacTypeToString[TAC_AND] = "&&";
    m_tacTypeToString[TAC_NEG] = "-";
    m_tacTypeToString[TAC_NOT] = "!";
    m_tacTypeToString[TAC_LEA] = "&[]";
    m_tacTypeToString[TAC_CALL] = "call";
}

DelGlobalExpror::~DelGlobalExpror()
{
}

void DelGlobalExpror::printInfoOfOptimizer(void)
{

}

bool DelGlobalExpror::runOptimizer(FunctionBlock* block,
    FuncPropertyGetter* funcPropertyGetter)//运行优化器
{
    clear();
    m_block = block;
    m_funcPropertyGetter = funcPropertyGetter;
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    vector<vector<uint>>& forwardGraph = m_block->getForwardGraph();
    //构建必经树
    vector<uint> startVector;
    vector<uint> endVector;
    startVector.clear();
    endVector.clear();
    for(uint i = 0;i < forwardGraph.size();i++)
    {
        for(uint j = 0;j < forwardGraph[i].size();j++)
        {
            startVector.push_back(i);
            endVector.push_back(forwardGraph[i][j]);
        }
    }
    vector<uint> dominantTreeIdom = s_algorithmExecutor->
        getDominantTreeIdom(startVector,endVector);
    m_mustPassNodeTree.clear();
    m_mustPassNodeTree.resize(dominantTreeIdom.size()/2);
    for(uint i = 0;i < m_mustPassNodeTree.size();i++)m_mustPassNodeTree[i].clear();
    for(uint i = 0;i < m_mustPassNodeTree.size();i++)
    {
        uint idomNode = dominantTreeIdom[i*2+1];
        uint sonNode = dominantTreeIdom[i*2];
        if(idomNode==sonNode)continue;
        m_mustPassNodeTree[idomNode].push_back(sonNode);
    }
    //得到必经树之后，进行dfs
    deleteCommonExpression(0);
    return m_isOptimize;
}

void DelGlobalExpror::deleteCommonExpression(uint blockId)
{
    vector<string> needToDelete;
    needToDelete.clear();
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    unordered_map<string,SsaSymb*>& uName2SsaSymbs = m_block->getUName2SsaSymbs();
    unordered_map<string,SsaSymb*>& tName2SsaSymbs = m_block->getTName2SsaSymbs();    
    vector<vector<uint>> backwardGraph = m_block->getBackwardGraph();
    //如果找到了就不加，如果是新的就加
    //同时将新的加入到待删列表
    SsaTac* tacHead = basicBlocks[blockId]->getTacHead();
    SsaTac* tacTail = basicBlocks[blockId]->getTacTail();
    if(tacHead != NULL)
    {
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        SsaTac* curTac = tacHeadHead;
        do
        {
            string secondOperands,thirdOperands;
            string needSymbol,expression;
            unordered_map<string,string>::iterator it;
            curTac = curTac->next;
            switch(curTac->type)
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
                if(weakeningTac(curTac))break;//删掉一些无用表达式
                if(!checkTheSymbVar(curTac->second))break;
                if(!checkTheSymbVar(curTac->third))break;       
                //得到表达式，再去寻找
                secondOperands = getStringOfSymb(curTac->second);
                thirdOperands = getStringOfSymb(curTac->third);
                needSymbol = m_tacTypeToString[curTac->type];
                expression = secondOperands + " " + needSymbol + " " + thirdOperands;
                it = m_expressTable.find(expression);
                if(it == m_expressTable.end())//说明没有，要加入该表
                {//如果是全局变量，不能加入表
                    if(!checkTheSymbVar(curTac->first))break;
                    addExpressionIntoTable(curTac,needToDelete);
                }
                else//说明有，则可以用来替换该变量
                {
                    string needToReplace = m_expressTable[expression];
                    SsaSymb* copySymb;
                    if(needToReplace.c_str()[0] == 't')
                        copySymb = tName2SsaSymbs[needToReplace];
                    else copySymb = uName2SsaSymbs[needToReplace];
                    curTac->second->useTimes--;
                    curTac->third->useTimes--;
                    deleteUseSsaTac(curTac->secondPoint);
                    deleteUseSsaTac(curTac->thirdPoint);
                    curTac->second = copySymb;
                    curTac->third = NULL;
                    addTacToUseListForSsaSymb(curTac,copySymb,curTac->secondPoint);
                    curTac->thirdPoint = NULL;
                    curTac->type = TAC_COPY;
                    m_isOptimize = true;
                }
                
                break;
            case TAC_NEG:
            case TAC_NOT:
                if(!checkTheSymbVar(curTac->second))break;
                secondOperands = getStringOfSymb(curTac->second);
                needSymbol = m_tacTypeToString[curTac->type];
                expression = needSymbol + " " + secondOperands;
                it = m_expressTable.find(expression);
                if(it == m_expressTable.end())//说明没有，要加入该表
                {//如果是全局变量，不能加入表
                    if(!checkTheSymbVar(curTac->first))break;
                    addExpressionIntoTable(curTac,needToDelete);
                }
                else//说明有，则可以用来替换该变量
                {
                    string needToReplace = m_expressTable[expression];
                    SsaSymb* copySymb;
                    if(needToReplace.c_str()[0] == 't')
                        copySymb = tName2SsaSymbs[needToReplace];
                    else copySymb = uName2SsaSymbs[needToReplace];
                    curTac->second->useTimes--;
                    deleteUseSsaTac(curTac->secondPoint);
                    curTac->second = copySymb;
                    addTacToUseListForSsaSymb(curTac,copySymb,curTac->secondPoint);
                    curTac->type = TAC_COPY;
                    m_isOptimize = true;
                }
                break;
            case TAC_LEA:
                //%s=&%s[%s]
                //如果第三个数字不是g即可
                if(curTac->third->type != SYM_INT &&
                curTac->third->name[0] == 'g')break;
                secondOperands = getStringOfSymb(curTac->second);
                thirdOperands =  getStringOfSymb(curTac->third);
                needSymbol = m_tacTypeToString[curTac->type];
                expression = secondOperands + " " + needSymbol + " " + thirdOperands;
                it = m_expressTable.find(expression);
                if(it == m_expressTable.end())//说明没有，要加入该表
                {//如果是全局变量，不能加入表
                    if(curTac->first->name[0] == 'g')break;
                    addExpressionIntoTable(curTac,needToDelete);
                }
                else
                {
                    string needToReplace = m_expressTable[expression];
                    SsaSymb* copySymb;
                    if(needToReplace.c_str()[0] == 't')
                        copySymb = tName2SsaSymbs[needToReplace];
                    else copySymb = uName2SsaSymbs[needToReplace];
                    curTac->third->useTimes--;
                    deleteUseSsaTac(curTac->thirdPoint);
                    curTac->second = copySymb;
                    curTac->third = NULL;
                    addTacToUseListForSsaSymb(curTac,copySymb,curTac->secondPoint);
                    curTac->thirdPoint = NULL;
                    curTac->type = TAC_COPY;
                    m_isOptimize = true;
                }
                break;
            case TAC_CALL:
                {
                    
                    if(curTac->first == NULL)break;//不是一个表达式
                    if(!m_funcPropertyGetter->isFuncASimpleExpression(curTac->second->name))break;
                    //简单函数可以看成一个表达式
                    secondOperands = curTac->second->name;//函数名
                    //前驱基本块
                    uint prevBlock = backwardGraph[blockId][0];
                    SsaTac* prevTacTail = basicBlocks[prevBlock]->getTacTail();
                    SsaTac* prevTacHead = basicBlocks[prevBlock]->getTacHead();
                    expression = secondOperands + " call";
                    //actual里不能有全局变量
                    SsaTac* tacTailTail = new SsaTac();
                    tacTailTail->prev = prevTacTail;
                    SsaTac* prevCurTac = tacTailTail;
                    if(prevTacHead != NULL)
                    {
                        do
                        {
                            prevCurTac = prevCurTac->prev;
                            if(prevCurTac->type != TAC_ACTUAL)break;
                            if(prevCurTac->first->type != SYM_INT &&
                                prevCurTac->first->name[0] == 'g')break;
                            //其余时候，添加字符串表达式
                            expression += " "+ getStringOfSymb(prevCurTac->first);
                        }while(prevCurTac != prevTacHead);
                        if(prevCurTac->type == TAC_ACTUAL &&
                            prevCurTac->first->type != SYM_INT &&
                            prevCurTac->first->name[0] == 'g')break;
                    }
                    
                    it = m_expressTable.find(expression);
                    if(it == m_expressTable.end())//说明没有，要加入该表
                    {//如果是全局变量，不能加入表
                        if(!checkTheSymbVar(curTac->first))break;
                        //addExpressionIntoTable(curTac,needToDelete);
                        m_expressTable[expression] = curTac->first->name;
                        needToDelete.push_back(expression);
                    }
                    else
                    {
                        string needToReplace = m_expressTable[expression];
                        //cout << "这个表达式是：" << expression << endl;
                        SsaSymb* copySymb;
                        if(needToReplace.c_str()[0] == 't')
                            copySymb = tName2SsaSymbs[needToReplace];
                        else copySymb = uName2SsaSymbs[needToReplace];
                        curTac->second = copySymb;
                        curTac->third = NULL;
                        addTacToUseListForSsaSymb(curTac,copySymb,curTac->secondPoint);
                        curTac->thirdPoint = NULL;
                        curTac->type = TAC_COPY;
                        m_isOptimize = true;
                        //删除所有的actual
                        tacTailTail->prev = prevTacTail;
                        SsaTac* prevCurTac = tacTailTail;
                        if(prevTacHead == NULL)break;  
                        do
                        {
                            prevCurTac = prevCurTac->prev;
                            if(prevCurTac->type != TAC_ACTUAL)break;
                            prevCurTac->type = TAC_UNDEF;
                            if(prevCurTac->first->type != SYM_INT &&
                                prevCurTac->first->name[0] != 'g')
                            {
                                prevCurTac->first->useTimes--;
                                deleteUseSsaTac(prevCurTac->firstPoint);
                            }
                        }while(prevCurTac != prevTacHead);
                        basicBlocks[prevBlock]->cleanDirtyTac();
                    }
                break;
                }
                break;
            }
        } while (curTac != tacTail);
        delete tacHeadHead;
    }

        

    for(uint i = 0;i < m_mustPassNodeTree[blockId].size();i++)
    {
        uint sonBlockId = m_mustPassNodeTree[blockId][i];
        deleteCommonExpression(sonBlockId);
    }
    
    //删掉插入的内容
    for(uint i = 0; i < needToDelete.size();i++)
        m_expressTable.erase(needToDelete[i]);
}

bool DelGlobalExpror::checkTheSymbVar(SsaSymb* varSymb)
{
    if(varSymb->type == SYM_INT)return true;
    unordered_map<string,SsaSymb*>& uName2SsaSymbs = m_block->getUName2SsaSymbs();
    unordered_map<string,SsaSymb*>& tName2SsaSymbs = m_block->getTName2SsaSymbs();
    unordered_map<string,SsaSymb*>::iterator it;
    if(varSymb->type == SYM_VAR && varSymb->name[0] == 't')
    {
        it = tName2SsaSymbs.find(varSymb->name);
        if(it != tName2SsaSymbs.end())return true;
    }
    if(varSymb->type == SYM_VAR && varSymb->name[0] == 'u')
    {
        it = uName2SsaSymbs.find(varSymb->name);
        if(it != uName2SsaSymbs.end())return true;        
    }
    return false;
}

string DelGlobalExpror::getStringOfSymb(SsaSymb* varSymb)
{
    string varSymbName;
    if(varSymb->type == SYM_INT)
        varSymbName = to_string(varSymb->value);//itoa(curTac->second->value);
    else varSymbName = varSymb->name;
    return varSymbName;
}

void DelGlobalExpror::addExpressionIntoTable(SsaTac* curTac,vector<string>& needDeleteList)
{
    string secondOperands,thirdOperands;
    string firstOperands,needSymbol;
    switch (curTac->type)
    {
    //可以左右调换顺序的
    case TAC_ADD:
    case TAC_MUL:
    case TAC_EQ:
    case TAC_NE:
    case TAC_OR:
    case TAC_AND:
        firstOperands = getStringOfSymb(curTac->first);
        secondOperands = getStringOfSymb(curTac->second);
        thirdOperands = getStringOfSymb(curTac->third);
        needSymbol = m_tacTypeToString[curTac->type];
        m_expressTable[secondOperands + " " + needSymbol
        + " " + thirdOperands] = firstOperands;
        m_expressTable[thirdOperands + " " + needSymbol
        + " " + secondOperands] = firstOperands;
        needDeleteList.push_back(secondOperands + " " 
        + needSymbol + " " + thirdOperands);
        needDeleteList.push_back(thirdOperands + " " 
        + needSymbol + " " + secondOperands);
        break;

    case TAC_SUB:
    case TAC_DIV:
    case TAC_MOD:
    case TAC_LT:
    case TAC_LE:
    case TAC_GT:
    case TAC_GE:
        firstOperands = getStringOfSymb(curTac->first);
        secondOperands = getStringOfSymb(curTac->second);
        thirdOperands = getStringOfSymb(curTac->third);
        needSymbol = m_tacTypeToString[curTac->type];
        m_expressTable[secondOperands + " " + needSymbol
        + " " + thirdOperands] = firstOperands;  
        needDeleteList.push_back(secondOperands + " " 
        + needSymbol + " " + thirdOperands);  
        break;
    case TAC_NEG:
    case TAC_NOT:
        firstOperands = getStringOfSymb(curTac->first);
        secondOperands = getStringOfSymb(curTac->second);
        needSymbol = m_tacTypeToString[curTac->type];
        m_expressTable[secondOperands + " " + needSymbol] = firstOperands; 
        needDeleteList.push_back(secondOperands + " " + needSymbol);
        break;
    case TAC_LEA:
        firstOperands = getStringOfSymb(curTac->first);
        secondOperands = getStringOfSymb(curTac->second);
        thirdOperands = getStringOfSymb(curTac->third);
        needSymbol = m_tacTypeToString[curTac->type];
        m_expressTable[secondOperands + " " + needSymbol
             + " " + thirdOperands] = firstOperands; 
        needDeleteList.push_back(secondOperands + " " + needSymbol
             + " " + thirdOperands);
        break;
    }
}

//在公共表达式中
//删除诸如：a*0,a-0,a+0,0/a

bool DelGlobalExpror::weakeningTac(SsaTac* curTac)
{
    SsaSymb* firstOperands = curTac->first;
    SsaSymb* secondOperands = curTac->second;
    SsaSymb* thirdOperands = curTac->third;
    switch (curTac->type)
    {
    case TAC_ADD:
        //0+a
        if(secondOperands->type == SYM_INT && 
            secondOperands->value == 0)
        {
            curTac->type = TAC_COPY;
            curTac->second = curTac->third;
            curTac->secondPoint = curTac->thirdPoint;
            curTac->thirdPoint = NULL; 
            curTac->third = NULL;
            break;
        }
        //a+0
        if(thirdOperands->type == SYM_INT && 
            thirdOperands->value == 0)
        {
            curTac->type = TAC_COPY;
            curTac->third = NULL;
            break;
        }
        break;
    case TAC_SUB:
        //a-0
        if(thirdOperands->type == SYM_INT && 
            thirdOperands->value == 0)
        {
            curTac->type = TAC_COPY;
            curTac->third = NULL;
            break;
        }
        break;
    case TAC_MUL:
        //a*1 a*0 0*a 1*a
        //0*a
        if(secondOperands->type == SYM_INT && 
            secondOperands->value == 0)
        {
            curTac->type = TAC_COPY;
            curTac->third->useTimes--;
            deleteUseSsaTac(curTac->thirdPoint);
            curTac->third = NULL;
            break;
        }
        //a*0
        if(thirdOperands->type == SYM_INT && 
            thirdOperands->value == 0)
        {
            curTac->type = TAC_COPY;
            curTac->second->useTimes--;
            deleteUseSsaTac(curTac->secondPoint);
            curTac->second = NULL;
            break;
        }
        //1*a
       if(secondOperands->type == SYM_INT && 
            secondOperands->value == 1)
        {
            curTac->type = TAC_COPY;
            curTac->second = curTac->third;
            curTac->secondPoint = curTac->thirdPoint;
            curTac->third = NULL;           
            curTac->thirdPoint = NULL; 
            break;
        }
        //a*1
        if(thirdOperands->type == SYM_INT && 
            thirdOperands->value == 1)
        {
            curTac->type = TAC_COPY;
            curTac->third = NULL;
            break;
        }
        break;
    case TAC_DIV:
        //a/a 0/a 
        if(thirdOperands->type == SYM_VAR && 
            thirdOperands == secondOperands)
        {
            curTac->type = TAC_COPY;
            curTac->second->useTimes--;
            curTac->second = new SsaSymb();
            curTac->second->type = SYM_INT;
            curTac->second->value = 1;
            curTac->third = NULL;
            deleteUseSsaTac(curTac->secondPoint);
            curTac->secondPoint = NULL;
            curTac->thirdPoint = NULL;
            break;
        }
        //0/a
        if(secondOperands->type == SYM_INT &&
            secondOperands->value == 0)
        {
            curTac->type = TAC_COPY;
            curTac->third->useTimes--;
            deleteUseSsaTac(curTac->thirdPoint);
            curTac->third = NULL;
            break;
        }
        break;
    case TAC_MOD:
        // a%1 = 0 a%a = 0  
        if((thirdOperands->type == SYM_VAR && 
            thirdOperands == secondOperands) || 
            (thirdOperands->type == SYM_INT &&
            thirdOperands->value == 1))
        {
            curTac->type = TAC_COPY;
            curTac->second->useTimes--;
            curTac->second = new SsaSymb();
            curTac->second->type = SYM_INT;
            curTac->second->value = 0;
            curTac->third = NULL;
            deleteUseSsaTac(curTac->secondPoint);
            if(thirdOperands->type == SYM_VAR)
                deleteUseSsaTac(curTac->thirdPoint);
            curTac->secondPoint = NULL;
            curTac->thirdPoint = NULL;
            break;    
        }
        //0%a = 0
        if(secondOperands->type == SYM_INT &&
            secondOperands->value == 0)
        {
            curTac->type = TAC_COPY;
            curTac->third->useTimes--;
            curTac->third = NULL;
            deleteUseSsaTac(curTac->thirdPoint);
            curTac->secondPoint = NULL;
            curTac->thirdPoint = NULL;
            break;               
        }
        break;
    case TAC_NE:
    case TAC_LT:
    case TAC_GT:
        // a != a
        if(thirdOperands->type == SYM_VAR && 
            thirdOperands == secondOperands)
        {
            curTac->type = TAC_COPY;
            curTac->second->useTimes--;
            curTac->second = new SsaSymb();
            curTac->second->type = SYM_INT;
            curTac->second->value = 0;
            curTac->third = NULL;
            deleteUseSsaTac(curTac->secondPoint);
            deleteUseSsaTac(curTac->thirdPoint);
            curTac->secondPoint = NULL;
            curTac->thirdPoint = NULL;
            break;            
        }
        break;
    case TAC_LE:
    case TAC_GE:
    case TAC_EQ:
        // a <= a
        // a >= a
        // a == a
        if(thirdOperands->type == SYM_VAR && 
            thirdOperands == secondOperands)
        {
            curTac->type = TAC_COPY;
            curTac->second->useTimes--;
            curTac->second = new SsaSymb();
            curTac->second->type = SYM_INT;
            curTac->second->value = 1;
            curTac->third = NULL;
            deleteUseSsaTac(curTac->secondPoint);
            deleteUseSsaTac(curTac->thirdPoint);
            curTac->secondPoint = NULL;
            curTac->thirdPoint = NULL;
            break;            
        }
        break;
    case TAC_OR:
        //a || 1
        if(thirdOperands->type == SYM_INT && 
            thirdOperands->value == 1)
        {
            curTac->type = TAC_COPY;
            curTac->second->useTimes--;
            curTac->second = curTac->third;
            curTac->third = NULL;
            deleteUseSsaTac(curTac->secondPoint);
            curTac->secondPoint = NULL;
            curTac->thirdPoint = NULL;
            break;            
        }
        //1 || a
        if(secondOperands->type == SYM_INT && 
            secondOperands->value == 1)
        {
            curTac->type = TAC_COPY;
            curTac->third->useTimes--;
            curTac->third = NULL;
            deleteUseSsaTac(curTac->thirdPoint);
            curTac->thirdPoint = NULL;
            break;            
        }
        break;
    case TAC_AND:
        //a && 0
        if(thirdOperands->type == SYM_INT && 
            thirdOperands->value == 0)
        {
            curTac->type = TAC_COPY;
            curTac->second->useTimes--;
            curTac->second = curTac->third;
            curTac->third = NULL;
            deleteUseSsaTac(curTac->secondPoint);
            curTac->secondPoint = NULL;
            curTac->thirdPoint = NULL;
            break;            
        }
        //0 && a
        if(secondOperands->type == SYM_INT && 
            secondOperands->value == 0)
        {
            curTac->type = TAC_COPY;
            curTac->third->useTimes--;
            curTac->third = NULL;
            deleteUseSsaTac(curTac->thirdPoint);
            curTac->thirdPoint = NULL;
            break;            
        }
        break;
    default:
        break;
    }
    if(curTac->type == TAC_COPY)
    {
        m_isOptimize = true;
        return true;
    }
    return false;
}

#endif
