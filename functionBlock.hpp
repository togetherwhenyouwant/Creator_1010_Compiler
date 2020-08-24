//======================================================================
//
//        Copyright (C) 2021 张锦伦    
//        All rights reserved
//
//        filename :functionBlock.hpp
//        description :
//
//        created by 张锦伦 at  03/07/2021 16:02:31
//        
//
//======================================================================

#ifndef FUNCTIONBLOCK_HPP
#define FUNCTIONBLOCK_HPP
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include "basicBlock.hpp"
#include "frontEndInput.hpp"
#include "ssaCodeStruct.hpp"
#include "globalSymTable.hpp"
#include "funcSymTable.hpp"
#include "blockSymTable.hpp"
#include "algorithmExecutor.hpp"
using namespace std;
typedef unsigned int uint;
#define SSAMODE true
#define COMMODE false
//函数块用于保留信息
//同时完成编写函数块的操作集
//1.完成基本块的初步划分
//2.能够删除基本块
//3.能够添加基本块
//第二遍审查
class FunctionBlock
{
private:
public:
    //基本块
    string m_name;                                              //函数块名称
    vector<BasicBlock*> m_basicBlocks;                          //基本块集合
    vector<vector<uint>> m_forwardGraph;                        //控制流正向图
    vector<vector<uint>> m_backwardGraph;                       //控制流反向图
    FuncSymTable* m_funcSymTable;                               //函数局部变量
    static GlobalSymTable* s_globalSymTable;                    //全局符号标量
    vector<BlockSymTable*> m_blockSymTables;                    //基本块内局部变量
    unordered_map<string,SsaSymb*> m_uName2SsaSymbs;            //函数局部定义集合
    unordered_map<string,SsaSymb*> m_tName2SsaSymbs;            //块内局部定义集合
private:
    vector<uint> calcBasicBlockPartPoints
        (SsaTac* beginTac,map<uint,uint>& tableOfLid2Bid);      //确定基本块切分点
    void divideBasicBlockBaseOnPoints(SsaTac* beginTac,
    vector<uint>& dividePoints,map<uint,uint>& tableOfLid2Bid); //根据切分点切分基本块
    // void deleteUnusedBlockAndBuildGraph(void);
    void buildControlFlowGraph(void);                           //建立控制流图
    void deleteUnreachableBlock(void);                          //删除无用基本块
    void directDeleteBasicBlock(vector<uint> BidList);          //不考虑后果直接删除基本块并改变控制流
    void deleteBasicBlockPrevWork(vector<uint> BidList);        //删除基本块前做的工作
    void cleanVarInfluence(vector<uint> BidList);               //删除基本块前清除变量
    uint getBlockPrintSucc(uint blockId);                       //基本块的直接输出后继
    uint getBlockPrintPrev(uint blockId);                       //基本块的直接输出前驱
    void addBasicBlockPreWork(void);                            //增加基本块前做的工作
    void edgeSegmentation(void);                                //对控制流图做边分割
    void getEdgeNeedToSegmentation(vector<uint>& 
        needSegStartPoints,vector<uint>& needSegEndPoints);     //得到需要做边分割的边集
    uint getMaxDNumOfUVar(string varName);                      //得到某个变量的d的最大值，用于新建变量
public:
    FunctionBlock();
    ~FunctionBlock(){};
    static GlobalSymTable* createGlobalSymTable(void);          //用于创建全局符号表
    void buildBlockFromTacs(SsaTac* tacHead);                   //根据输入Tac构建基本块

    /******* 给外界提供的对基本块的操作集 *******/
    void cleanDirtySsaTac(void);                                //用于清理优化器优化后的垃圾
    void deleteBasicBlock(vector<uint> BidList);                //删除某些基本块
    uint addNewNodeForLoop(uint loopHead,vector<uint> prevBids);//为循环添加新节点（用于提取循环不变量）
    void deleteExtraGotoBlocks(void);                           //删除多余的goto基本块（会破坏SSA结构[insert消除后做]）

    /******* 给外界提供的信息显示工具 *******/
    void printFunctionBlock(void);                              
    void printFunctionBlockDirct(void);
    void printControlFlowGraph(void);
    void printBasicBlockNum(void);
    vector<vector<uint>> getPrintList(void);                    //得到程序流图的后继输出链集合
    
    /******* 给外界提供的类内数据 *******/
    uint getTheLineOfFunction(void);            
    vector<BasicBlock*>& getBasicBlocks(void){return m_basicBlocks;}
    FuncSymTable*& getFuncSymTable(void){return m_funcSymTable;}
    unordered_map<string,SsaSymb*>& getUName2SsaSymbs(void){return m_uName2SsaSymbs;}
    unordered_map<string,SsaSymb*>& getTName2SsaSymbs(void){return m_tName2SsaSymbs;}
    vector<vector<uint>>& getForwardGraph(void){return m_forwardGraph;};
    vector<vector<uint>>& getBackwardGraph(void){return m_backwardGraph;};
    
};

GlobalSymTable* FunctionBlock::s_globalSymTable = FunctionBlock::createGlobalSymTable();

/*********************************
 * 第一阶段：根据代码初步构建基本块
 * 该阶段较为简单：根据代码切分基本块
 * 并建立简单的控制流图（正反）
 *********************************/

FunctionBlock::FunctionBlock()
{
    m_name.clear();
    m_funcSymTable = NULL;
    m_blockSymTables.clear();
    m_basicBlocks.clear();
    m_forwardGraph.clear();
    m_backwardGraph.clear();
    m_uName2SsaSymbs.clear();
    m_tName2SsaSymbs.clear(); 
}
uint FunctionBlock::getTheLineOfFunction(void)
{
    uint lineNum = 0;
    for(uint i = 0;i < m_basicBlocks.size();i++)
        lineNum += m_basicBlocks[i]->m_instNum;
    return lineNum;
}

GlobalSymTable* FunctionBlock::createGlobalSymTable(void)
{
    GlobalSymTable* m_globalSymTable = new GlobalSymTable();
    return m_globalSymTable;
}

void FunctionBlock::printFunctionBlockDirct(void)
{
    for(uint i = 0;i < m_basicBlocks.size();i++)
    {
        m_basicBlocks[i]->printBasicBlock();
    }
}

vector<uint> FunctionBlock::calcBasicBlockPartPoints
    (SsaTac* beginTac,map<uint,uint>& tableOfLid2Bid)
{
    vector<uint> dividePoints;                                  //基本块切分点
    uint curDividePoint = 0;
    tableOfLid2Bid.clear();
    dividePoints.clear();
    dividePoints.push_back(curDividePoint);
    for(SsaTac* curTac = beginTac;curTac->next != NULL;
            curTac = curTac->next)
    {
        curDividePoint++;
        
        switch(curTac->next->type)
        {
        case TAC_GOTO:
            dividePoints.push_back(curDividePoint);
            break;
        case TAC_IFZ:
            dividePoints.push_back(curDividePoint);
            break;
        case TAC_RETURN:
            dividePoints.push_back(curDividePoint);
            break;
        case TAC_ENDFUNC:
            if(dividePoints.back() != curDividePoint-1)
                dividePoints.push_back(curDividePoint-1);
            break;
        case TAC_LABEL:
            {
                if(dividePoints.back() != curDividePoint-1)
                    dividePoints.push_back(curDividePoint-1);
                char* name = curTac->next->first->name;
                int idLNumber = atoi(name+1);
                int idBNumber = dividePoints.size();                //因为B0为entry
                tableOfLid2Bid[idLNumber] = idBNumber;
            }
            break;
        case TAC_CALL:
            {
                if(dividePoints.back() != curDividePoint-1)
                    dividePoints.push_back(curDividePoint-1); 
                dividePoints.push_back(curDividePoint);
            }
            break;
        default:
            break; 
        }
        if(curTac->next->type == TAC_ENDFUNC)break;
    }
    return dividePoints;    
}

void FunctionBlock::divideBasicBlockBaseOnPoints(SsaTac* beginTac,
    vector<uint>& dividePoints,map<uint,uint>& tableOfLid2Bid)
{
    SsaTac* curTac = beginTac;
    for(uint idOfB = 0;idOfB < dividePoints.size()-1;idOfB++)
    {
        int instNum = dividePoints[idOfB+1]-dividePoints[idOfB];
        SsaTac* blockTacHead = curTac->next;
        SsaTac* blockTacTail = blockTacHead;
        BasicBlock* curbBlock = new BasicBlock();
        curbBlock->setinstNum(instNum);
        while(instNum--)
        {
            if(blockTacTail->type == TAC_IFZ)
            {
                char* name = blockTacTail->second->name;
                int Bid = tableOfLid2Bid[atoi(name+1)];
                sprintf(blockTacTail->second->name,"B%d",Bid);
            }
            else if(blockTacTail->type == TAC_GOTO
                || blockTacTail->type == TAC_LABEL)
            {
                char* name = blockTacTail->first->name;
                int Bid = tableOfLid2Bid[atoi(name+1)];
                sprintf(blockTacTail->first->name,"B%d",Bid);
            }
            if(instNum != 0)
            blockTacTail = blockTacTail->next;
        }
        curbBlock->setTacHead(blockTacHead);
        curbBlock->setTacTail(blockTacTail);
        curbBlock->setId(idOfB+1);//第0块是extry基本块
        m_basicBlocks.push_back(curbBlock);
        curTac = blockTacTail;
    }
}

void FunctionBlock::buildBlockFromTacs(SsaTac* tacHead)
{
    //函数块名字
    m_name.clear();
    m_name.append(tacHead->next->first->name);
    SsaTac* curTac = tacHead->next->next;
    SsaTac* beginTac = curTac;//begin
    //建立函数变量符号表
    m_funcSymTable = new FuncSymTable();
    for(;curTac->next != NULL;curTac = curTac->next)
    {//添加变量
        if(curTac->next->type != TAC_VAR &&
            curTac->next->type != TAC_ARR &&
            curTac->next->type != TAC_FORMAL)break;
        if(curTac->next->type == TAC_ARR)
            m_funcSymTable->addArrTac(curTac->next);
        if(curTac->next->first->name[0] == 'u')
            m_funcSymTable->addVar(curTac->next->first);
    }
    //判断是否空函数
    if(curTac->next->type == TAC_ENDFUNC)return;
    //将begin和非变量语句连起来，其他的删掉
    //注意，不能删掉formal的语句
    //在内存中还没释放，但问题应该不大
    SsaTac* formalEndTac = beginTac;
    while(formalEndTac->next->type == TAC_FORMAL)
        formalEndTac = formalEndTac->next;
    //将formal和下面语句粘合起来
    SsaTac* defHead = curTac->next;
    formalEndTac->next = defHead;
    defHead->prev = formalEndTac;
    map<uint,uint> tableOfLid2Bid;//L+id替换成B+id
    vector<uint> dividePoints = 
        calcBasicBlockPartPoints(beginTac,tableOfLid2Bid);
    //创建extry基本块
    BasicBlock *extryBlock = new BasicBlock();
    extryBlock->setId(0);
    extryBlock->setinstNum(0);
    extryBlock->setTacHead(NULL);
    extryBlock->setTacTail(NULL);
    m_basicBlocks.push_back(extryBlock);
    //第二遍扫描
    divideBasicBlockBaseOnPoints
    (beginTac,dividePoints,tableOfLid2Bid);
    //最后加一个exit基本块
    BasicBlock *exitBlock = new BasicBlock();
    exitBlock->setId(dividePoints.size());
    exitBlock->setinstNum(0);
    exitBlock->setTacHead(NULL);
    exitBlock->setTacTail(NULL);
    m_basicBlocks.push_back(exitBlock);
    buildControlFlowGraph();
    deleteUnreachableBlock();
}

//在一开始构建的时候建立的控制流（不可用于后续）
void FunctionBlock::buildControlFlowGraph(void)
{
    m_forwardGraph.clear();
    m_backwardGraph.clear();
    m_forwardGraph.resize(m_basicBlocks.size());
    m_backwardGraph.resize(m_basicBlocks.size());
    //entry基本块后继一定可达
    m_forwardGraph[0].push_back(1);
    m_backwardGraph[1].push_back(0);
    for(uint i = 1;i < m_basicBlocks.size()-1;i++)
    {
        //对每个基本块
        SsaTac* tacTail = m_basicBlocks[i]->getTacTail();
        if(tacTail == NULL)continue;
        switch(tacTail->type)
        {
            case TAC_IFZ:

                {
                    //添加下一个
                    m_forwardGraph[i].push_back(i+1);
                    m_backwardGraph[i+1].push_back(i);
                    //添加跳转
                    char *name = tacTail->second->name;
                    uint Bid = atoi(name+1);
                    if(Bid != i+1)
                    {
                        m_forwardGraph[i].push_back(Bid);
                        m_backwardGraph[Bid].push_back(i);
                    }
                    else
                    {
                        tacTail->type = TAC_UNDEF;
                        m_basicBlocks[i]->cleanDirtyTac();
                    }
                    break;
                }
            case TAC_GOTO:
                {                
                    //只需要添加跳转
                    char *name = tacTail->first->name;
                    uint Bid = atoi(name+1);
                    m_forwardGraph[i].push_back(Bid);
                    m_backwardGraph[Bid].push_back(i); 
                    break;
                }               
            case TAC_RETURN:
                //只需要连接exit基本块
                m_forwardGraph[i].push_back(m_basicBlocks.size()-1);
                m_backwardGraph[m_basicBlocks.size()-1].push_back(i);
                break;
            default:
                //其他只需要连接下一块即可
                m_forwardGraph[i].push_back(i+1);
                m_backwardGraph[i+1].push_back(i);
                break;
        }
    }
}

/*********************************
 * 第二阶段：对不可达基本块做删除
 * 该阶段逻辑较为复杂，重点解决删除
 * 基本块后产生的各种不良影响
 * 并且因为引入了输出后继链的概念
 *********************************/

void FunctionBlock::deleteUnreachableBlock(void)
{
    //得到不可达节点
    vector<uint> startVector;
    vector<uint> endVector;
    vector<uint> deleteBidList;
    startVector.clear();
    endVector.clear();
    for(uint i = 0;i < m_forwardGraph.size();i++)
    {
        for(uint j = 0;j < m_forwardGraph[i].size();j++)
        {
            startVector.push_back(i);
            endVector.push_back(m_forwardGraph[i][j]);
        }
    }
    deleteBidList = s_algorithmExecutor
    ->getUnreachableNodeList(startVector,endVector,0);
    if(deleteBidList.size() != 0)deleteBasicBlock(deleteBidList);
    edgeSegmentation();
}

void FunctionBlock::printFunctionBlock(void)
{
    //确定输出顺序
    vector<uint> printOrder;
    printOrder.clear();
    vector<vector<uint>> PrintList = getPrintList();
    for(uint i = 0; i < PrintList.size();i++)
    {
        for(uint j = 0;j < PrintList[i].size();j++)
            printOrder.push_back(PrintList[i][j]);
    }
    printOrder.push_back(m_basicBlocks.size()-1);
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    cout << "label " <<  m_name << endl;
    cout << "begin"  << endl;
    for(uint i = 0;i < m_basicBlocks.size();i++)
    {
        m_basicBlocks[printOrder[i]]->printBasicBlock();
    }
    cout << "end"  << endl;
}

void FunctionBlock::printBasicBlockNum(void)
{
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    cout << "there have "<< m_basicBlocks.size()-2 << " blocks!" << endl;
}

void FunctionBlock::printControlFlowGraph(void)
{
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    cout << "start block: start->B" << m_forwardGraph[0][0] << endl;
    for(uint i = 1;i < m_forwardGraph.size()-1;i++)
    {
        cout << "B" << i << " can reach list: ";
        for(uint j = 0;j < m_forwardGraph[i].size();j++)
        {
            if(m_forwardGraph[i][j] == m_forwardGraph.size()-1)
                cout << "end" << "\t";
            else cout << "B" << m_forwardGraph[i][j] << "\t";
        }
        cout << endl;
    }
}

//TODO
//变量删除
//删除节点块的代码编写
//边分割算法的代码编写

void FunctionBlock::deleteBasicBlockPrevWork(vector<uint> BidList)
{
    unordered_set<uint> needCleanBid;
    needCleanBid.clear();
    //删基本块之前先检查被删除的基本块的前驱
    for(uint i = 0;i < BidList.size();i++)
    {
        uint needDeleteBid = BidList[i];
        //如果无前驱，则不需要做什么
        if(m_backwardGraph[needDeleteBid].size() == 0)continue;

        for(uint j = 0;j < m_backwardGraph[needDeleteBid].size();j++)
        {
            //对每个前驱进行检查
            uint prevBid = m_backwardGraph[needDeleteBid][j];
            SsaTac* tacTail = m_basicBlocks[prevBid]->getTacTail();
            //说明删除的基本块是前驱的直接输出后继
            if(tacTail == NULL)continue;
            //如果是goto说明只有一个语句，删掉goto语句即可
            if(tacTail->type == TAC_GOTO)
            {
                tacTail->type = TAC_UNDEF;
                needCleanBid.insert(prevBid);
                continue;
            }
            if(tacTail->type == TAC_IFZ)
            {
                uint succOfIfzBid = getBlockPrintSucc(prevBid);
                if(succOfIfzBid != needDeleteBid)//是goto部分
                {
                    tacTail->type = TAC_UNDEF;
                    needCleanBid.insert(prevBid);
                    continue;
                }
                //
                uint gotoBid = atoi(tacTail->second->name+1);
                //自己跳到自己，这就只能用goto了
                if(gotoBid == prevBid)
                {
                    tacTail->type = TAC_GOTO;
                    tacTail->first = tacTail->second;
                    tacTail->second = NULL;
                    continue;
                }
                uint prevOfGotoBid = getBlockPrintPrev(gotoBid);
                if(prevOfGotoBid == gotoBid)//没有前驱
                {
                    //控制流图不用改变，只需要删除即可
                    tacTail->type = TAC_UNDEF;
                    needCleanBid.insert(prevBid);
                    continue;
                }
                //有前驱
                //改成goto即可
                tacTail->type = TAC_GOTO;
                tacTail->first = tacTail->second;
                tacTail->second = NULL;
                continue;
            }
            //如果是ifz，说明该基本块有两个跳转
            //假如被删的基本块是ifz的goto部分，直接将该句删掉即可
            //假如被删的基本块是ifz的输出后继，那么需要看另一个
            //goto的基本块是否能成为ifz的输出后继，如果可以，那么
            //删掉ifz语句，否则，改ifz为goto语句
        }
    }
    unordered_set<uint>::iterator it = needCleanBid.begin();
    for(;it != needCleanBid.end();it++)
        m_basicBlocks[*it]->cleanDirtyTac();
    //检查后继，将后继中的insert语句相应的位置变量删除
    //每个insert变量都需要从后遍历删除避免出错，所以采用这样的方法
    unordered_set<uint> needDeleteSet;
    needDeleteSet.clear();
    for(uint i = 0;i < BidList.size();i++)
        needDeleteSet.insert(BidList[i]);
    for(uint i = 0;i < m_basicBlocks.size();i++)
    {
        //无前驱直接跳过
        if(m_backwardGraph[i].size() == 0)continue;
        if(needDeleteSet.find(i) != needDeleteSet.end())continue;
        //被删集合直接跳过

        //记录被删除的前驱id集合，从大到小
        vector<uint> needDeletePrev;
        needDeletePrev.clear();
        for(int j = m_backwardGraph[i].size()-1;j >= 0;j--)
        {
            uint prevBid = m_backwardGraph[i][j];
            if(needDeleteSet.find(prevBid) != needDeleteSet.end())
                needDeletePrev.push_back(j);
        }
        if(needDeletePrev.size() == 0)continue;
        SsaTac* tacHead = m_basicBlocks[i]->getTacHead();
        SsaTac* tacTail = m_basicBlocks[i]->getTacTail();
        if(tacHead == NULL)continue;
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        SsaTac* curTac = tacHeadHead;
        do
        {
            curTac = curTac->next;
            //TODO：这里真的能这样删除嘛，恐怕要想一想（还需要检查一遍）
            if(curTac->type != TAC_INSERT)break;
            for(uint j = 0;j < curTac->functionSymb.size();j++)
            {
                if(curTac->functionSymb2Tac[j] == NULL)continue;
                curTac->functionSymb[j]->useTimes--;
                deleteUseSsaTac(curTac->functionSymb2Tac[j]);
            }
            for(uint j  = 0;j < needDeletePrev.size();j++)
            {
                curTac->functionSymb.erase(curTac->functionSymb.begin()+needDeletePrev[j]);
                curTac->functionSymb2Tac.erase(curTac->functionSymb2Tac.begin()+needDeletePrev[j]);
            }
            for(uint j = 0;j < curTac->functionSymb.size();j++)
            {
                if(curTac->functionSymb[j]->type == SYM_INT)continue;
                if(m_tName2SsaSymbs.find(curTac->functionSymb[j]->name) == m_tName2SsaSymbs.end() &&
                m_uName2SsaSymbs.find(curTac->functionSymb[j]->name) == m_uName2SsaSymbs.end())continue;
                addTacToUseListForSsaSymb(curTac,
                curTac->functionSymb[j],curTac->functionSymb2Tac[j]);
            }
        } while (curTac != tacTail);
        delete tacHeadHead;
    }
}

uint FunctionBlock::getBlockPrintSucc(uint blockId)
{
    //输出后继一定不能是EXIT基本块
    uint exitBlockId = m_basicBlocks.size()-1;
    SsaTac* tacTail = m_basicBlocks[blockId]->getTacTail();
    if(tacTail != NULL && tacTail->type == TAC_GOTO)return blockId;
    if(tacTail != NULL && (tacTail->type == TAC_IFZ ||
    tacTail->type == TAC_IFEQ || tacTail->type == TAC_IFNE
    || tacTail->type == TAC_IFLT || tacTail->type == TAC_IFLE
    || tacTail->type == TAC_IFGT || tacTail->type == TAC_IFGE))
    {//它会有两个后继，如果第一个不是就是第二个
        uint gotoBid = atoi(tacTail->second->name+1);
        uint firstBid = m_forwardGraph[blockId][0];
        if(firstBid != gotoBid && firstBid == exitBlockId)return blockId;
        if(firstBid != gotoBid)return firstBid;
        if(m_forwardGraph[blockId][1] == exitBlockId)return blockId;
        return m_forwardGraph[blockId][1];
    }
    //如果是其他的情况，它们只会有一个后继,或者没有
    uint succNum = m_forwardGraph[blockId].size();
    if(succNum == 0)return blockId;
    if(m_forwardGraph[blockId][0] == exitBlockId)return blockId;
    return m_forwardGraph[blockId][0];
}

uint FunctionBlock::getBlockPrintPrev(uint blockId)
{   //前驱不可能是 EXIT 基本块
    //ENTRY 的前驱只能是他自己
    //无前驱，说明它无输出前驱
    if(m_backwardGraph[blockId].size() == 0)return blockId;
    //有前驱，只要不是goto的基本块，就是它的输出前驱
    for(uint i = 0;i < m_backwardGraph[blockId].size();i++)
    {
        uint succBid = m_backwardGraph[blockId][i];
        SsaTac* tacTail = m_basicBlocks[succBid]->getTacTail();
        //空基本块，说明是输出前驱
        if(tacTail == NULL)return succBid;
        if(tacTail->type == TAC_GOTO)continue;
        if(tacTail->type == TAC_IFZ||
        tacTail->type == TAC_IFEQ || tacTail->type == TAC_IFNE
        || tacTail->type == TAC_IFLT || tacTail->type == TAC_IFLE
        || tacTail->type == TAC_IFGT || tacTail->type == TAC_IFGE)
        {
            uint gotoBid = atoi(tacTail->second->name+1);
            if(gotoBid == blockId)continue;
            return succBid;
        }
        //如果不是上诉几种情况，则是直接输出前驱
        return succBid;
    }
    //这都找不到，说明无输出前驱
    return blockId;
}

vector<vector<uint>> FunctionBlock::getPrintList(void)
{
    vector<vector<uint>> pointRoadList;
    uint blockNum = m_basicBlocks.size();
    bool *pointPrintFlag = new bool[blockNum];
    for(uint i=0;i<blockNum;i++)pointPrintFlag[i] = false;
    pointPrintFlag[blockNum-1] = true;
    pointRoadList.clear();
    for(uint i = 0;i < blockNum-1;i++)
    {
        //找到输出后继链的头节点
        uint prevNode = i;
        if(pointPrintFlag[prevNode] == true)continue;
        while (prevNode != getBlockPrintPrev(prevNode))
            prevNode = getBlockPrintPrev(prevNode);
        //找到输出后继链头节点后
        uint succNode = prevNode;
        vector<uint> pointRoad;
        pointRoad.clear();
        while(succNode != getBlockPrintSucc(succNode))
        {
            pointRoad.push_back(succNode);
            pointPrintFlag[succNode] = true;
            succNode = getBlockPrintSucc(succNode);
        }
        pointRoad.push_back(succNode);
        pointPrintFlag[succNode] = true;
        pointRoadList.push_back(pointRoad);
    }
    return pointRoadList;
}
void FunctionBlock::directDeleteBasicBlock(vector<uint> BidList)
{
    uint blockNum = m_basicBlocks.size();
    //直接删除那些基本块并先改名
    uint deleteNum = BidList.size();
    //建立 oldBid 和 newBid 之间的映射
    bool* BidFlag = new bool[blockNum];
    uint* old2new = new uint[blockNum];
    vector<uint> BidSortList(0);
    for(uint i = 0; i < blockNum;i++)BidFlag[i] = true;
    for(uint i = 0; i < BidList.size();i++)BidFlag[BidList[i]] =false;
    for(uint i = 0,cnt = 0;i < blockNum;i++)
    {
        old2new[i] = cnt;
        if(BidFlag[i] == true)cnt++;
        else BidSortList.push_back(i);
    }    
    //删除对应的基本块
    for(int i = BidSortList.size()-1;i>=0;i--)
        m_basicBlocks.erase(m_basicBlocks.begin()+BidSortList[i]);
    //更改id名
    for(uint i = 0;i < m_basicBlocks.size();i++)
        m_basicBlocks[i]->setId(i);
    //将每一个基本块内的oldBid修改成newBid
    for(uint i = 0;i < m_basicBlocks.size();i++)
    {
        SsaTac* tacHead = m_basicBlocks[i]->getTacHead();
        SsaTac* tacTail = m_basicBlocks[i]->getTacTail();
        if(tacHead == NULL)continue;
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        SsaTac* curTac = tacHeadHead;
        do
        {
            curTac = curTac->next;
           if(curTac->type == TAC_IFZ ||
            curTac->type == TAC_IFEQ || curTac->type == TAC_IFNE
            || curTac->type == TAC_IFLT || curTac->type == TAC_IFLE
            || curTac->type == TAC_IFGT || curTac->type == TAC_IFGE)
            {
                char* name = curTac->second->name;
                sprintf(curTac->second->name,"B%d",old2new[atoi(name+1)]);
            }
            else if(curTac->type == TAC_GOTO
                || curTac->type == TAC_LABEL)
            {
                char* name = curTac->first->name;
                sprintf(curTac->first->name,"B%d",old2new[atoi(name+1)]);
            }            
        }while(curTac != tacTail);
        delete tacHeadHead;
    }
    //考虑修改控制流信息
    //TODO : 分开修改forward和backward （还需要检查一次）
    //backward不能由forward生成，因为insert的原因
    //先修改forward
    BidFlag[blockNum-1] = false;
    for(int i = blockNum-2;i >= 0;i--)//blockNum-2是因为不删 EXIT 基本块
    {
        if(BidFlag[i] == false)//假如该基本块需要被删除
        {
            m_forwardGraph.erase(m_forwardGraph.begin()+i);
            m_backwardGraph.erase(m_backwardGraph.begin()+i);
            continue;
        } 
        //否则，更改forward和backward的信息
        vector<uint> &succInfo = m_forwardGraph[i];
        for(int j = succInfo.size()-1;j >= 0;j--)
        {
            uint oldBid = succInfo[j];
            if(BidFlag[oldBid] == false)
                succInfo.erase(succInfo.begin()+j);
            else succInfo[j] = old2new[oldBid];
        }
        vector<uint> &prevInfo = m_backwardGraph[i];
        for(int j = prevInfo.size()-1;j >= 0;j--)
        {
            uint oldBid = prevInfo[j];
            if(BidFlag[oldBid] == false)
                prevInfo.erase(prevInfo.begin()+j);
            else prevInfo[j] = old2new[oldBid];
        }
    }
    //补上连接到EXIT的边
    uint exitBlockId = m_basicBlocks.size()-1;
    m_backwardGraph[exitBlockId].clear();
    vector<vector<uint>> printList = getPrintList();
    for(uint i = 0;i < printList.size();i++)
    {
        uint listLen = printList[i].size();
        uint tailNode = printList[i][listLen-1];
        SsaTac* tacTail = m_basicBlocks[tailNode]->getTacTail();
        if(tacTail != NULL && tacTail->type == TAC_GOTO)continue;
        //补上连接到EXIT的边
        m_backwardGraph[exitBlockId].push_back(tailNode);
        m_forwardGraph[tailNode].push_back(exitBlockId);
    }
    //EXIT节点的INSERT顺序并不重要，因为insert始终会被删除
    //forward中的节点顺序更不重要
}
void FunctionBlock::deleteBasicBlock(vector<uint> BidList)
{
    deleteBasicBlockPrevWork(BidList);//去除删基本块对语句的影响
    if(!m_uName2SsaSymbs.empty() || !m_tName2SsaSymbs.empty())
    cleanVarInfluence(BidList);       //清除变量代码的影响
    directDeleteBasicBlock(BidList);  //直接删除基本块改变控制流
    vector<uint> startVector;
    vector<uint> endVector;
    vector<uint> deleteBidList;
    startVector.clear();
    endVector.clear();
    for(uint i = 0;i < m_forwardGraph.size();i++)
    {
        for(uint j = 0;j < m_forwardGraph[i].size();j++)
        {
            startVector.push_back(i);
            endVector.push_back(m_forwardGraph[i][j]);
        }
    }
    deleteBidList = s_algorithmExecutor
    ->getUnreachableNodeList(startVector,endVector,0);
    if(deleteBidList.size() != 0)deleteBasicBlock(deleteBidList);
}

void FunctionBlock::cleanVarInfluence(vector<uint> BidList)
{
    for(uint i = 0;i < BidList.size();i++)
    {
        uint needDeleteBid = BidList[i];
        SsaTac* tacHead = m_basicBlocks[needDeleteBid]->getTacHead();
        SsaTac* tacTail = m_basicBlocks[needDeleteBid]->getTacTail();
        if(tacHead == NULL)continue;
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        SsaTac* curTac = tacHeadHead;
        //删除基本块之前，需要将里面所有变量的引用和定值全部删除
        //定值语句直接在符号表里删，其他的现在符号表里找，找得到就删引用
        do
        {
            curTac = curTac->next;
            switch (curTac->type)
            {
            case TAC_INSERT:
                for(uint i = 0;i < curTac->functionSymb.size();i++)
                {
                    if(curTac->functionSymb2Tac[i] == NULL)continue;
                    curTac->functionSymb[i]->useTimes--;
                    deleteUseSsaTac(curTac->functionSymb2Tac[i]);
                }
                break;
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
            case TAC_ARR_L:
                if(curTac->secondPoint != NULL)
                {
                    curTac->second->useTimes--;
                    deleteUseSsaTac(curTac->secondPoint);
                }
                if(curTac->thirdPoint != NULL)
                {
                    curTac->third->useTimes--;
                    deleteUseSsaTac(curTac->thirdPoint);
                }
                break;
            case TAC_NEG:
            case TAC_POSI:
            case TAC_NOT:
            case TAC_COPY: 
            case TAC_IFZ:
                if(curTac->secondPoint != NULL)
                {
                    curTac->second->useTimes--;
                    deleteUseSsaTac(curTac->secondPoint);
                }
                break;
            case TAC_ACTUAL:
            case TAC_RETURN:
                if(curTac->firstPoint != NULL)
                {
                    curTac->first->useTimes--;
                    deleteUseSsaTac(curTac->firstPoint);
                }
                break;
            case TAC_ARR_R:
            case TAC_LEA:
                if(curTac->thirdPoint != NULL)
                {
                    curTac->third->useTimes--;
                    deleteUseSsaTac(curTac->thirdPoint);
                }               
                break;
            default:
                break;
            }
            switch (curTac->type)
            {
            case TAC_INSERT:
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
            case TAC_NEG:
            case TAC_POSI:
            case TAC_NOT:
            case TAC_COPY: 
            case TAC_ARR_R:
            case TAC_LEA:  
            case TAC_CALL:
                if(curTac->first == NULL)break;
                if(m_uName2SsaSymbs.find(curTac->first->name) != m_uName2SsaSymbs.end()) 
                   m_uName2SsaSymbs.erase(curTac->first->name);
                if(m_tName2SsaSymbs.find(curTac->first->name) != m_tName2SsaSymbs.end()) 
                   m_tName2SsaSymbs.erase(curTac->first->name);                           
                break;
            default:
                break;
            }
        
        } while (curTac != tacTail);
        
    }
}

void FunctionBlock::cleanDirtySsaTac(void)
{
    for(uint i = 0; i <m_basicBlocks.size();i++)
        m_basicBlocks[i]->cleanDirtyTac();
}


void FunctionBlock::getEdgeNeedToSegmentation(
vector<uint>& needSegStartPoints,vector<uint>& needSegEndPoints)
{
    //对控制流图做边分割
    vector<uint> startPoints;
    vector<uint> endPoints;
    needSegStartPoints.clear();
    needSegEndPoints.clear();
    startPoints.clear();
    endPoints.clear();
    for(uint i = 0;i < m_forwardGraph.size();i++)
    {
        for(uint j = 0;j < m_forwardGraph[i].size();j++)
        {
            startPoints.push_back(i);
            endPoints.push_back(m_forwardGraph[i][j]);
        }
    }
    for(uint i = 0;i < startPoints.size();i++)
    {
        uint startPoint = startPoints[i];//多个后继
        uint endPoint = endPoints[i];    //多个前驱
        if(m_forwardGraph[startPoint].size() > 1 &&
            m_backwardGraph[endPoint].size() > 1)
        {
            needSegStartPoints.push_back(startPoint);
            needSegEndPoints.push_back(endPoint);
        }
    }    
}

void FunctionBlock::addBasicBlockPreWork(void)
{
    uint origExitBlockId = m_basicBlocks.size()-1;
    //查看end的直接前驱
    int prevOfEnd = -1;
    for(uint i = 0;i < m_backwardGraph[origExitBlockId].size();i++)
    {
        uint prevBlockId = m_backwardGraph[origExitBlockId][i];
        SsaTac* tacTail = m_basicBlocks[prevBlockId]->getTacTail();
        if(tacTail == NULL)
        {
            prevOfEnd = prevBlockId;
            break;
        }
        if(tacTail->type != TAC_GOTO)
        {
            if(tacTail->type == TAC_RETURN)continue;
            prevOfEnd = prevBlockId;
            break;
        }
    }
    if(prevOfEnd != -1)
    {
        //说明有直接前驱，在直接前驱上把
        BasicBlock* newBlock = new BasicBlock();
        SsaTac* returnTac = new SsaTac();
        returnTac->type = TAC_RETURN;
        newBlock->setTacHead(returnTac);
        newBlock->setTacTail(returnTac);
        newBlock->setId(origExitBlockId);
        //先改变除了prevOfEnd之外的所有控制流图
        for(uint i = 0;i < m_backwardGraph[origExitBlockId].size();i++)
        {
            uint prevBlockId = m_backwardGraph[origExitBlockId][i];
            if(prevBlockId == prevOfEnd)continue;
            for(uint j = 0;j < m_forwardGraph[prevBlockId].size();i++)
            {
                if(m_forwardGraph[prevBlockId][j] != origExitBlockId)continue;
                m_forwardGraph[prevBlockId][j] = origExitBlockId + 1;
                break;
            }
        }
        //TODO : 分开修改forward和backward （需要检查一遍）
        //backward不能由forward生成，因为insert的原因
        //修改正向图
        vector<uint> newVector;
        newVector.clear();
        newVector.push_back(origExitBlockId + 1);
        m_forwardGraph.insert(m_forwardGraph.begin()+m_forwardGraph.size()-1,newVector);
        //修改反向图
        for(uint i = 0;i < m_backwardGraph[origExitBlockId].size();i++)
        {
            if(m_backwardGraph[origExitBlockId][i] != prevOfEnd)continue;
            m_backwardGraph[origExitBlockId][i] = origExitBlockId;
            break;
        }
        vector<uint> newVectorCopy;
        newVectorCopy.clear();
        newVector.push_back(prevOfEnd);
        m_backwardGraph.insert(m_backwardGraph.begin()+m_backwardGraph.size()-1,newVectorCopy);
        //修改基本块
        m_basicBlocks[origExitBlockId]->setId(origExitBlockId+1);
        m_basicBlocks.insert(m_basicBlocks.begin()+origExitBlockId,newBlock);
    }

}

//边分割
void FunctionBlock::edgeSegmentation(void)
{
    //对控制流图做边分割
    vector<uint> needSegStartPoints;
    vector<uint> needSegEndPoints;
    needSegStartPoints.clear();
    needSegEndPoints.clear();
    getEdgeNeedToSegmentation(needSegStartPoints,needSegEndPoints);
    //得到边集了,需要注意，在needSegEndPoints中可能会有EXIT节点
    if(needSegStartPoints.size() == 0)return;
    //在新加节点之前，首先在exit的直接前驱后面添加一个return基本块并改变控制流
    addBasicBlockPreWork();
    uint exitBlockId = m_forwardGraph.size()-1;
    uint addBlockNum = needSegStartPoints.size();
    //建立新的 newForwardGraph
    vector<vector<uint>> newForwardGraph;
    newForwardGraph.clear();
    newForwardGraph.resize(exitBlockId + addBlockNum + 1);

    for(uint i = 0;i < m_forwardGraph.size()-1;i++)
    {
        for(uint j = 0;j < m_forwardGraph[i].size();j++)
        {
            if(m_forwardGraph[i][j] == exitBlockId)
                newForwardGraph[i].push_back(exitBlockId+addBlockNum);
            else newForwardGraph[i].push_back(m_forwardGraph[i][j]);
        }
    }
    
    vector<BasicBlock*> newBlockList(addBlockNum);
    for(uint i = 0;i < needSegStartPoints.size();i++)
    {
        uint startPoint = needSegStartPoints[i];//多个后继
        uint endPoint = needSegEndPoints[i];    //多个前驱
        //如果这个endPoint是exit节点，那么可想而知
        //与exit相连的节点一定是每条输出后继链的尾节点
        //假如 startPoint的输出后继就是endPoint，那么插入即可
        uint newBlockId = exitBlockId + i;
        uint printSuccOfstartPoint = getBlockPrintSucc(startPoint);
        if(endPoint == exitBlockId || printSuccOfstartPoint == endPoint)
        {
            //直接创建空节点并改变边即可
            newBlockList[i] = new BasicBlock();
            newBlockList[i]->setId(newBlockId);
            newBlockList[i]->setinstNum(0);
            newBlockList[i]->setTacHead(NULL);
            newBlockList[i]->setTacTail(NULL);
            if(endPoint == exitBlockId)endPoint = exitBlockId + addBlockNum;
            vector<uint>::iterator it = find(newForwardGraph[startPoint].begin(),
                newForwardGraph[startPoint].end(),endPoint);
            if(it == newForwardGraph[startPoint].end()){exit(-1);}
            *it = newBlockId;
            newForwardGraph[newBlockId].push_back(endPoint);
            continue;
        }
        //如果不是exitBlock
        //也不是直接后继关系，直接创建节点并改变
        uint oldGotoBid;
        SsaTac* tacTail = m_basicBlocks[startPoint]->getTacTail();
        if(tacTail->type == TAC_GOTO)
        {
            oldGotoBid = atoi(tacTail->first->name+1);
            sprintf(tacTail->first->name,"B%d",newBlockId);
        }
        else if(tacTail->type == TAC_IFZ)
        {
            oldGotoBid = atoi(tacTail->second->name+1);
            sprintf(tacTail->second->name,"B%d",newBlockId);
        }
        else exit(-1);
        //创建新节点
        SsaTac* newLabel = new SsaTac();
        newLabel->type = TAC_LABEL;
        newLabel->first = new SsaSymb();
        newLabel->first->type = SYM_LABEL;
        sprintf(newLabel->first->name,"B%d",newBlockId);

        SsaTac* newGoto = new SsaTac();
        newGoto->type = TAC_GOTO;
        newGoto->first = new SsaSymb();
        newGoto->first->type = SYM_LABEL;
        sprintf(newGoto->first->name,"B%d",oldGotoBid);

        newLabel->next = newGoto;
        newGoto->prev = newLabel;

        newBlockList[i] = new BasicBlock();
        newBlockList[i]->setinstNum(2);
        newBlockList[i]->setTacHead(newLabel);
        newBlockList[i]->setTacTail(newGoto);
        newBlockList[i]->setId(newBlockId);
        vector<uint>::iterator it = find(newForwardGraph[startPoint].begin(),
            newForwardGraph[startPoint].end(),endPoint);
        if(it == newForwardGraph[startPoint].end()){exit(-1);}
        *it = newBlockId;
        newForwardGraph[newBlockId].push_back(endPoint);        
    }
    //改变m_basicBlock
    m_basicBlocks.insert(m_basicBlocks.begin()+m_basicBlocks.size() - 1,
        newBlockList.begin(),newBlockList.end());
    //改变forwardGraph和backwardGraph
    m_forwardGraph.clear();
    m_backwardGraph.clear();
    m_forwardGraph.resize(newForwardGraph.size());
    m_backwardGraph.resize(newForwardGraph.size());
    for(uint i = 0;i < newForwardGraph.size();i++)
    {
        for(uint j = 0; j < newForwardGraph[i].size();j++)
        {
            m_forwardGraph[i].push_back(newForwardGraph[i][j]);
            m_backwardGraph[newForwardGraph[i][j]].push_back(i);
        }
    }
    for(uint i = 0;i < m_basicBlocks.size();i++)
    {
        m_basicBlocks[i]->setId(i);
    }
}  

//prevBids为不在循环内的loopHead的前驱基本块Id集合（内含顺序）
//TODO : 这里有大问题
uint FunctionBlock::addNewNodeForLoop(uint loopHead,vector<uint> prevBids)
{
    //做好增加基本块前置准备
    addBasicBlockPreWork();
    uint newNodeId = m_basicBlocks.size() - 1;
    //做好一个block
    BasicBlock* newBlock = new BasicBlock();

    SsaTac* labelTac = new SsaTac();
    labelTac->type = TAC_LABEL;
    SsaSymb* newLabelSymb = new SsaSymb();
    newLabelSymb->type = SYM_LABEL;
    sprintf(newLabelSymb->name,"B%d",newNodeId);
    labelTac->first = newLabelSymb;

    SsaTac* gotoTac = new SsaTac();
    gotoTac->type = TAC_GOTO;
    SsaSymb* newGotoSymb = new SsaSymb();
    newGotoSymb->type = SYM_LABEL;
    sprintf(newGotoSymb->name,"B%d",loopHead);
    gotoTac->first = newGotoSymb;

    labelTac->next = gotoTac;
    gotoTac->prev = labelTac;
    newBlock->setTacHead(labelTac);
    newBlock->setTacTail(gotoTac);
    newBlock->setinstNum(2);
    newBlock->setId(newNodeId);

    //替换掉前驱可能产生的goto loopHead 或者 ifz ?? goto loopHead
    for(uint i = 0;i < prevBids.size();i++)
    {
        uint prevBid = prevBids[i];
        SsaTac* tacTail = m_basicBlocks[prevBid]->getTacTail();
        if(tacTail == NULL)continue;
        if(tacTail->type == TAC_GOTO)
            sprintf(tacTail->first->name,"B%d",newNodeId);
        if(tacTail->type == TAC_IFZ)
        {
            //查看goto是否和loopHead一致
            uint oldBid = atoi(tacTail->second->name+1);
            if(oldBid != loopHead)continue;
            sprintf(tacTail->second->name,"B%d",newNodeId);
        }
    }
    //修改insert语句
    unordered_set<uint> prevBidSet;
    prevBidSet.clear();
    for(uint i = 0;i < prevBids.size();i++)prevBidSet.insert(prevBids[i]);
    //在连接控制图之前，首先需要解决insert语句
    //需要考虑 prevBids 只有一个的情况，这时候不需要解决insert语句
    do
    {
        if(prevBids.size() != 1)break;//不等于1，考虑一下insert语句
        SsaTac* tacHead = m_basicBlocks[loopHead]->getTacHead();
        SsaTac* tacTail = m_basicBlocks[loopHead]->getTacTail();
        if(tacHead == NULL)break;
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        tacHead->prev = tacHeadHead;
        SsaTac* curTac = tacHeadHead;
        do
        {
            curTac = curTac->next;
            if(curTac->type != TAC_INSERT)break;
            //为insert的左值变量找到一个新的变量
            vector<SsaSymb*> needMoveVar;   //需要移动的变量
            vector<uint> needDeletePos;     //需要删除的位置
            needMoveVar.clear();
            string varName = curTac->first->name;
            int positionOfD = varName.find('d');
            string uVarName = varName.substr(0, positionOfD);
            uint dNumOfNewVar = getMaxDNumOfUVar(varName);
            //just 提取个变量
            for(uint i = 0;i < m_backwardGraph[loopHead].size();i++)
            {
                uint prevBid = m_backwardGraph[loopHead][i];
                if(prevBidSet.find(prevBid) == prevBidSet.end())continue;
                //如果找得到说明这个地方的insert变量就得删掉
                needMoveVar.push_back(curTac->functionSymb[i]);
                needDeletePos.push_back(i);
            }
            //对定义链做大清洗
            for(uint i = 0;i < curTac->functionSymb.size();i++)
            {
                if(curTac->functionSymb2Tac[i] == NULL)continue;
                curTac->functionSymb[i]->useTimes--;
                deleteUseSsaTac(curTac->functionSymb2Tac[i]);
            }
            //删除变量，从后往前删
            for(int i = needDeletePos.size()-1;i >=0;i--)
            {
                curTac->functionSymb2Tac.erase(
                curTac->functionSymb2Tac.begin()+needDeletePos[i]);
                curTac->functionSymb.erase(
                curTac->functionSymb.begin()+needDeletePos[i]);
            }
            //提取的时候还得注意，因为在我的代码里
            //假如有insert(u1d1,u1d1,u1d1,u1d2)
            //那么对应的useTac为 有值 NULL NULL 有值
            //新做一条insert语句加入新基本块中
            SsaTac* insertTac = new SsaTac();
            insertTac->type = TAC_INSERT;
            insertTac->blockId = newNodeId;
            SsaSymb* newSymb = new SsaSymb();
            newSymb->type = SYM_VAR;
            newSymb->defPoint = insertTac;
            sprintf(newSymb->name,"%sd%d",uVarName.c_str(),dNumOfNewVar+1);
            insertTac->first = newSymb;
            insertTac->functionSymb.resize(needMoveVar.size());
            insertTac->functionSymb2Tac.resize(needMoveVar.size());
            for(uint i = 0;i < needMoveVar.size();i++)
            {
                insertTac->functionSymb[i] = needMoveVar[i];
                insertTac->functionSymb2Tac[i] = NULL;
                if(insertTac->functionSymb[i]->type == SYM_INT)continue;
                if(m_tName2SsaSymbs.find(insertTac->functionSymb[i]->name) == m_tName2SsaSymbs.end() &&
                m_uName2SsaSymbs.find(insertTac->functionSymb[i]->name) == m_uName2SsaSymbs.end())continue;
                addTacToUseListForSsaSymb(insertTac,
                insertTac->functionSymb[i],insertTac->functionSymb2Tac[i]);
            }
            newBlock->insertTacToTail(insertTac);
            //将这个变量加入变量表中
            m_uName2SsaSymbs[newSymb->name] = newSymb;
            //修改原来语句的insert语句
            curTac->functionSymb.push_back(newSymb);
            curTac->functionSymb2Tac.push_back(NULL);
            for(uint i = 0;i < curTac->functionSymb.size();i++)
            {
                curTac->functionSymb2Tac[i] = NULL;
                if(curTac->functionSymb[i]->type == SYM_INT)continue;
                if(m_tName2SsaSymbs.find(curTac->functionSymb[i]->name) == m_tName2SsaSymbs.end() &&
                m_uName2SsaSymbs.find(curTac->functionSymb[i]->name) == m_uName2SsaSymbs.end())continue;
                addTacToUseListForSsaSymb(curTac,
                curTac->functionSymb[i],curTac->functionSymb2Tac[i]);
            }                
            
        } while (curTac != tacTail);
        delete tacHeadHead;
    } while (0);
    //接下来就是连接控制图
    //修改正向图和反向图
    //TODO : 分开修改forward和backward（还需要检查一遍）
    for(uint i = 0;i < prevBids.size();i++)
    {
        uint prevBid = prevBids[i];
        vector<uint>::iterator it;
        it = find(m_forwardGraph[prevBid].begin(),m_forwardGraph[prevBid].end(),loopHead);
        if(it == m_forwardGraph[prevBid].end())exit(-1);
        *it = newNodeId;
    }
    m_forwardGraph[newNodeId].clear();
    m_forwardGraph[newNodeId].push_back(loopHead);
    vector<uint> newVector;
    newVector.clear();
    m_forwardGraph.push_back(newVector);
    //修改反向图
    for(int i = m_backwardGraph[loopHead].size()-1;i>=0;i--)
    {
        uint prevBid = m_backwardGraph[loopHead][i];
        vector<uint>::iterator it = find(m_backwardGraph[loopHead].begin(),
                m_backwardGraph[loopHead].end(),prevBid);
        if(it == m_backwardGraph[loopHead].end())continue;
        m_backwardGraph[loopHead].erase(it);
    }
    m_backwardGraph[loopHead].push_back(newNodeId);
    m_backwardGraph.insert(m_backwardGraph.begin()+m_backwardGraph.size()-1,prevBids);
    //插入基本块
    m_basicBlocks.insert(m_basicBlocks.begin() +
        m_basicBlocks.size() - 1,newBlock);
    m_basicBlocks[m_basicBlocks.size()-1]->setId(m_basicBlocks.size()-1);
    return newNodeId;
}

uint FunctionBlock::getMaxDNumOfUVar(string varName)
{
    //输入的肯定是个重命名的u变量
    uint maxDNum = 0;
    int positionOfD = varName.find('d');
    if(positionOfD == -1)exit(-1);
    string uVarName = varName.substr(0, positionOfD);
    unordered_map<string,SsaSymb*>::iterator it;
    for(it = m_uName2SsaSymbs.begin();it != m_uName2SsaSymbs.end();it++)
    {
        int positionOfDTemp = (it->first).find('d');
        if(positionOfDTemp == -1)continue;
        string uVarNameTemp = varName.substr(0, positionOfDTemp);
        if(uVarName.compare(uVarNameTemp) == -1)continue;//不等于
        string dNumString = (it->first).substr(positionOfDTemp+1);
        uint dValue = atoi(dNumString.c_str());
        if(dValue > maxDNum)maxDNum = dValue;
    }
    return maxDNum;
}

//TODO
//消除多余的边分割基本块（最后的任务）
void FunctionBlock::deleteExtraGotoBlocks(void)
{
    vector<uint> BidList;
    BidList.clear();
    for(uint i = 0;i < m_basicBlocks.size();i++)
    {
        uint instNum = m_basicBlocks[i]->getInstNum();
        if(instNum != 2)continue;
        SsaTac* tacHead = m_basicBlocks[i]->getTacHead();
        SsaTac* tacTail = m_basicBlocks[i]->getTacTail();
        if(tacHead->type != TAC_LABEL)continue;
        if(tacTail->type != TAC_GOTO)continue;
        //前驱唯一,后继唯一
        if(m_backwardGraph[i].size() != 1)continue;
        if(m_forwardGraph[i].size() != 1)continue;
        //并且前驱通过goto到达它
        uint prevBid = m_backwardGraph[i][0];
        uint succBid = m_forwardGraph[i][0];
        SsaTac* prevTacHead = m_basicBlocks[prevBid]->getTacHead();
        SsaTac* prevTacTail = m_basicBlocks[prevBid]->getTacTail();
        if(prevTacHead == NULL)continue;
        uint continueFlag = true;
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = prevTacHead;
        SsaTac* curTac = tacHeadHead;        
        do
        {
            curTac = curTac->next;
            if(curTac->type == TAC_IFZ ||
        curTac->type == TAC_IFEQ || curTac->type == TAC_IFNE
        || curTac->type == TAC_IFLT || curTac->type == TAC_IFLE
        || curTac->type == TAC_IFGT || curTac->type == TAC_IFGE
            )
            {
                uint gotoBId = atoi(curTac->second->name+1);
                if(gotoBId != i)continue;
                sprintf(curTac->second->name,"B%d",succBid);
                continueFlag = false;
            }
            else if(curTac->type == TAC_GOTO)
            {
                uint gotoBId = atoi(curTac->first->name+1);
                if(gotoBId != i)continue;
                sprintf(curTac->first->name,"B%d",succBid);
                continueFlag = false;
            }
        } while (curTac != prevTacTail);
        delete tacHeadHead;
        if(continueFlag)continue;
        //好了，都改完了，那么只需要改变控制流即可
        vector<uint>::iterator it = find(m_backwardGraph[succBid].begin(),
            m_backwardGraph[succBid].end(),i);
        if(it == m_backwardGraph[succBid].end())exit(-1);//不可能找不到
        *it = prevBid;
        it = find(m_forwardGraph[prevBid].begin(),m_forwardGraph[prevBid].end(),i);
        if(it == m_forwardGraph[prevBid].end())exit(-1);//不可能找不到
        *it = succBid;//更改后继
        m_forwardGraph[i].clear();
        m_backwardGraph[i].clear();
        BidList.push_back(i);
    }
    if(BidList.size() == 0)return;
    directDeleteBasicBlock(BidList);
}

#endif