//======================================================================
//
//        Copyright (C) 2021 张锦伦    
//        All rights reserved
//
//        filename :condConstProgagation.hpp
//        description :
//
//        created by 张锦伦 at  03/07/2021 16:02:31
//        
//
//======================================================================

/**
 *                             _ooOoo_
 *                            o8888888o
 *                            88" . "88
 *                            (| -_- |)
 *                            O\  =  /O
 *                         ____/`---'\____
 *                       .'  \\|     |//  `.
 *                      /  \\|||  :  |||//  \
 *                     /  _||||| -:- |||||-  \
 *                     |   | \\\  -  /// |   |
 *                     | \_|  ''\---/''  |   |
 *                     \  .-\__  `-`  ___/-. /
 *                   ___`. .'  /--.--\  `. . __
 *                ."" '<  `.___\_<|>_/___.'  >'"".
 *               | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *               \  \ `-.   \_ __\ /__ _/   .-` /  /
 *          ======`-.____`-.___\_____/___.-`____.-'======
 *                             `=---='
 *          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *                     佛祖保佑        永无BUG
 *            佛曰:
 *                   写字楼里写字间，写字间里程序员；
 *                   程序人员写程序，又拿程序换酒钱。
 *                   酒醒只在网上坐，酒醉还来网下眠；
 *                   酒醉酒醒日复日，网上网下年复年。
 *                   但愿老死电脑间，不愿鞠躬老板前；
 *                   奔驰宝马贵者趣，公交自行程序员。
 *                   别人笑我忒疯癫，我笑自己命太贱；
 *                   不见满街漂亮妹，哪个归得程序员？
*/

#ifndef CONDCONSTPROPAGATION_HPP
#define CONDCONSTPROPAGATION_HPP
#include <queue>
#include "globalPolicyExecutor.hpp"

#define UNKNOW 0 	//不知道：未被赋值
#define ASKNOW 1 	//知道：赋值为确定的值
#define UPKNOW 2 	//超出知道范围：赋值不能够确定


class CondConstPropagation:
public GlobalPolicyExecutor
{
private:
    vector<bool> m_isBlockUseful;             //基本块是否有可能会被执行
    vector<uint> m_isVarValueKnow;            //变量赋值是否确定
    vector<int> m_assignVarValue;             //每个变量确定的赋值
    queue<uint> m_blockWorkList;              //基本块工作表
    unordered_map<string,uint> m_var2id;      //变量名名字映射到Id
    unordered_map<uint,string> m_id2var;      //变量名id映射到名字
    unordered_set<uint> m_varIdWorkList;      //变量名工作表 
private:
    void clear();
    void addToBlockWorkList(uint blockId);    //加入一个节点到节点工作表
    void addToVarIdWorkListA(string&          //加入一个变量到变量工作表
        varName,int value);
    void addToVarIdWorkListU(string& varName);//
    void actionOfTwoOperands(SsaTac* tacNode);
    void actionOfOneOperand(SsaTac* tacNode);
    void actionOfInsertOperand(SsaTac* tacNode);
    void actionOfMemOrCall(SsaTac* tacNode);
    void actionOfIfz(SsaTac* tacNode);
    void actionToOneTac(SsaTac* tacNode);
    void VarPropagation(void);          
    void ReplaceVarUseConst(SsaSymb* varSymb,int value); 
    void ReplaceVarAndCleanUseTac(SsaSymb* &deleteSymb,
        SsaSymb* &varSymb,UseSsaTac* &useTac,int value);      
    void deleteUnuseBlock(void);
public:
    void printInfoOfOptimizer(void);
    bool runOptimizer(FunctionBlock* block,
        FuncPropertyGetter* funcPropertyGetter);//运行优化器
    CondConstPropagation();
    ~CondConstPropagation();
};


void CondConstPropagation::clear()
{
    m_isBlockUseful.clear();
    m_isVarValueKnow.clear();
    m_assignVarValue.clear();
    m_block = NULL;
    while(!m_blockWorkList.empty())
        m_blockWorkList.pop();
    m_var2id.clear();
    m_id2var.clear();
    m_varIdWorkList.clear();
}

CondConstPropagation::CondConstPropagation()
{
    m_name = "常数传播删除器";
    clear();
}

CondConstPropagation::~CondConstPropagation()
{
}

bool CondConstPropagation::runOptimizer(FunctionBlock* block,
    FuncPropertyGetter* funcPropertyGetter)
{
    clear();
    m_block = block;
    uint varCnt = 0;
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    m_isBlockUseful.resize(basicBlocks.size());
    for(uint i = 0;i < m_isBlockUseful.size();i++)m_isBlockUseful[i] = false;
    //u和t就是本次的目标
    unordered_map<string,SsaSymb*>& uName2SsaSymbs = m_block->getUName2SsaSymbs();
    unordered_map<string,SsaSymb*>& tName2SsaSymbs = m_block->getTName2SsaSymbs();
    if(uName2SsaSymbs.size() + tName2SsaSymbs.size() == 0)return false;
    m_assignVarValue.resize(uName2SsaSymbs.size() + tName2SsaSymbs.size());
    for(uint i = 0;i < m_assignVarValue.size();i++)m_isVarValueKnow.push_back(UNKNOW);
    unordered_map<string,SsaSymb*>::iterator it;
    unordered_map<string,uint>::iterator iter;
    for(it = uName2SsaSymbs.begin();it != uName2SsaSymbs.end();it++)m_var2id[it->first]=varCnt++;
    for(it = tName2SsaSymbs.begin();it != tName2SsaSymbs.end();it++)m_var2id[it->first]=varCnt++;
    for(iter = m_var2id.begin();iter!=m_var2id.end();iter++)m_id2var[iter->second] = iter->first;

    //开始工作表算法
    addToBlockWorkList(0);
    while(!m_blockWorkList.empty() || !m_varIdWorkList.empty())
    {
        while(!m_blockWorkList.empty())
        {
            uint blockId = m_blockWorkList.front();
            m_blockWorkList.pop();
            SsaTac* tacHead = basicBlocks[blockId]->getTacHead();
            SsaTac* tacTail = basicBlocks[blockId]->getTacTail();
            if(tacHead == NULL)continue;
            SsaTac* tacHeadHead = new SsaTac();
            tacHeadHead->next = tacHead;
            tacHead->prev = tacHeadHead;
            SsaTac* curTac = tacHeadHead;
            do
            {
                curTac = curTac->next;
                actionToOneTac(curTac);
            } while (curTac != tacTail);
        }
        
        while(!m_varIdWorkList.empty())
        {
            unordered_set<uint>::iterator it = 
                m_varIdWorkList.begin();
            uint curIdOfVar = *it;
            m_varIdWorkList.erase(*it);
            SsaSymb* curVar;
            if(m_id2var[curIdOfVar].c_str()[0] == 't')
                curVar = tName2SsaSymbs[m_id2var[curIdOfVar]];
            else curVar = uName2SsaSymbs[m_id2var[curIdOfVar]];
            UseSsaTac* curUseTac = curVar->useList;
            while(curUseTac->next != NULL)
            {
                actionToOneTac(curUseTac->next->code);
                curUseTac = curUseTac->next;
            }
        }
    }
    //是否优化
    bool isOptimize = false;
    for(uint i = 0;i < m_isVarValueKnow.size();i++)
    {
        if(m_isVarValueKnow[i] != ASKNOW)continue;
        isOptimize = true;
    }
    for(uint i = 0;i < m_isBlockUseful.size();i++)
    {
        if(m_isBlockUseful[i] == true)continue;
        isOptimize = true;
    }    
    
    //根据得到的变量进行常数传播和删除基本块
    VarPropagation();//常数传播
    //清除无用变量

    for(uint i = 0;i < basicBlocks.size();i++)
        basicBlocks[i]->cleanDirtyTac();

    //删掉符号表中变量
    for(uint i = 0;i < m_isVarValueKnow.size();i++)
    {
        if(m_isVarValueKnow[i] != ASKNOW)continue;
        string varName = m_id2var[i];
        if(varName.c_str()[0] == 't')tName2SsaSymbs.erase(varName);
        else uName2SsaSymbs.erase(varName);
    }
    //删除基本块
    deleteUnuseBlock();
    return isOptimize;
}

void CondConstPropagation::printInfoOfOptimizer(void)
{
    //打印出删掉的基本块和值用于检查
    cout << "删掉的基本块有：" << endl;
    for(uint i = 0;i < m_isBlockUseful.size();i++)
    {
        if(m_isBlockUseful[i] == false)
        {
            cout << "B" << i << "\t";
        }   
    }
    cout << endl;
    cout << "常数传播后：" << endl;
    for(uint i = 0;i < m_isVarValueKnow.size();i++)
    {
        if(m_isVarValueKnow[i] == ASKNOW)
        {
            cout << "变量 " << m_id2var[i] << " : " << 
                m_assignVarValue[i] << endl;
        }
    }
}

void CondConstPropagation::actionToOneTac(SsaTac* tacNode)
{
    switch (tacNode->type)
    {
    case TAC_INSERT:
        actionOfInsertOperand(tacNode);
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
        actionOfTwoOperands(tacNode);
        break;
    //一个操作数
    case TAC_NEG:
    case TAC_POSI:
    case TAC_NOT:
    case TAC_COPY:
        actionOfOneOperand(tacNode);
        break;
    case TAC_FORMAL:
        actionOfMemOrCall(tacNode);
        break;
    case TAC_CALL:
        if(tacNode->first != NULL)
            actionOfMemOrCall(tacNode);
        break;
    //MEM
    case TAC_ARR_R:
        actionOfMemOrCall(tacNode);
        break;
    case TAC_IFZ:
        actionOfIfz(tacNode);
        break;
    default:
        break;
    }    
}

void CondConstPropagation::addToBlockWorkList(uint blockId)
{
    //如果一个节点被加入过，它的唯一后继也一定已经加入
    vector<vector<uint>>& forwardGraph = m_block->getForwardGraph();
    if(m_isBlockUseful[blockId] == true)return;
    uint uniqueSuccBlockId = blockId;
    while(forwardGraph[uniqueSuccBlockId].size() == 1)
    {
        m_isBlockUseful[uniqueSuccBlockId] = true;
        m_blockWorkList.push(uniqueSuccBlockId);
        uniqueSuccBlockId = forwardGraph[uniqueSuccBlockId][0];
    }
    m_isBlockUseful[uniqueSuccBlockId] = true;
    m_blockWorkList.push(uniqueSuccBlockId);
}


void CondConstPropagation::addToVarIdWorkListA(string& varName,int value)
{
    uint idOfVar = m_var2id[varName];
    m_assignVarValue[idOfVar] = value;
    if(m_isVarValueKnow[idOfVar] == UNKNOW)
        m_varIdWorkList.insert(idOfVar);
    m_isVarValueKnow[idOfVar] = ASKNOW;
}

void CondConstPropagation::addToVarIdWorkListU(string& varName)
{
    uint idOfVar = m_var2id[varName];
    if(m_isVarValueKnow[idOfVar] == ASKNOW)
        m_varIdWorkList.insert(idOfVar);
    m_isVarValueKnow[idOfVar] = UPKNOW;
        
}

void CondConstPropagation::actionOfTwoOperands(SsaTac* tacNode)
{
    //首先判断它是否在符号表里
    string firstOperand(tacNode->first->name);
    unordered_map<string,uint>::iterator it;
    it = m_var2id.find(firstOperand);
    if(it == m_var2id.end())return;
    //说明在符号表里
    //对第二个操作数操作
    if(tacNode->second->type == SYM_VAR)
    {
        string secondOperand(tacNode->second->name);
        it = m_var2id.find(secondOperand);
        if(it == m_var2id.end())
        {
            //说明没找到，第一个变量加入变量工作表 UPKNOW
            addToVarIdWorkListU(firstOperand);
            return;
        }
        //说明第二个变量在符号表里
        uint idOfSecondVar = m_var2id[secondOperand];
        if(m_isVarValueKnow[idOfSecondVar] != ASKNOW)
        {
            //第一个变量加入变量工作表
            addToVarIdWorkListU(firstOperand);
            return;       
        }
    }
    //此时第二个变量不是常量就是 ASKONW

    //对第二个操作数操作
    if(tacNode->third->type == SYM_VAR)
    {
        string thirdOperand(tacNode->third->name);
        it = m_var2id.find(thirdOperand);
        if(it == m_var2id.end())
        {
            //说明没找到，第一个变量加入变量工作表 UPKNOW
            addToVarIdWorkListU(firstOperand);
            return;
        }
        //说明第三个变量在符号表里
        uint idOfThirdVar = m_var2id[thirdOperand];
        if(m_isVarValueKnow[idOfThirdVar] != ASKNOW)
        {
            //第一个变量加入变量工作表
            addToVarIdWorkListU(firstOperand);
            return;       
        }
    }
    //此时第三个变量不是常量就是 ASKONW
    //故求出这两个常量再进行计算
    int secondValue,thirdValue,firstValue;
    string secondOperand(tacNode->second->name);
    string thirdOperand(tacNode->third->name);
    if(tacNode->second->type == SYM_INT)
        secondValue = tacNode->second->value;
    else 
    {
        uint idOfSecondVar = m_var2id[secondOperand];
        secondValue = m_assignVarValue[idOfSecondVar];
    }
    if(tacNode->third->type == SYM_INT)
        thirdValue = tacNode->third->value;
    else 
    {
        uint idOfThirdVar = m_var2id[thirdOperand];
        thirdValue = m_assignVarValue[idOfThirdVar];
    }
    switch (tacNode->type)
    {
    case TAC_ADD:
        firstValue = secondValue + thirdValue;
        break;
    case TAC_SUB:
        firstValue = secondValue - thirdValue;
        break;
    case TAC_MUL:
        firstValue = secondValue * thirdValue;
        break;
    case TAC_DIV:
        firstValue = secondValue / thirdValue;
        break;
    case TAC_EQ:
        firstValue = secondValue == thirdValue;
        break;
    case TAC_MOD:
        firstValue = secondValue % thirdValue;
        break;
    case TAC_NE:
        firstValue = secondValue != thirdValue;
        break;
    case TAC_LT:
        firstValue = secondValue < thirdValue;
        break;
    case TAC_LE:
        firstValue = secondValue <= thirdValue;
        break;
    case TAC_GT:
        firstValue = secondValue > thirdValue;
        break;
    case TAC_GE:
        firstValue = secondValue >= thirdValue;
        break;
    case TAC_OR:
        firstValue = secondValue || thirdValue;
        break;
    case TAC_AND:
        firstValue = secondValue && thirdValue;
        break;
    default:
        exit(-1);
        break;
    }
    addToVarIdWorkListA(firstOperand,firstValue);
    // string thirdOperand(tacNode->third->name);

}

void CondConstPropagation::actionOfOneOperand(SsaTac* tacNode)
{
    //首先判断它是否在符号表里
    string firstOperand(tacNode->first->name);
    unordered_map<string,uint>::iterator it;
    it = m_var2id.find(firstOperand);
    if(it == m_var2id.end())return;
    //说明在符号表里
    //对第二个操作数操作
    if(tacNode->second->type == SYM_VAR)
    {
        string secondOperand(tacNode->second->name);
        it = m_var2id.find(secondOperand);
        if(it == m_var2id.end())
        {
            //说明没找到，第一个变量加入变量工作表 UPKNOW
            addToVarIdWorkListU(firstOperand);
            return;
        }
        //说明第二个变量在符号表里
        uint idOfSecondVar = m_var2id[secondOperand];
        if(m_isVarValueKnow[idOfSecondVar] != ASKNOW)
        {
            //第一个变量加入变量工作表
            addToVarIdWorkListU(firstOperand);
            return;       
        }
    }
    //此时第二个变量不是常量就是 ASKONW   
    int secondValue,firstValue;
    string secondOperand(tacNode->second->name);
    if(tacNode->second->type == SYM_INT)
        secondValue = tacNode->second->value;
    else 
    {
        uint idOfSecondVar = m_var2id[secondOperand];
        secondValue = m_assignVarValue[idOfSecondVar];
    } 
    switch (tacNode->type)
    {
    case TAC_NEG:
        firstValue = - secondValue;
        break;
    case TAC_POSI:
    case TAC_COPY:
        firstValue = secondValue;
        break;
    case TAC_NOT:
        firstValue = !secondValue;
        break;
    default:
        exit(-1);
        break;
    }
    addToVarIdWorkListA(firstOperand,firstValue);
}

void CondConstPropagation::actionOfInsertOperand(SsaTac* tacNode)
{
    //insert的变量一定在符号表里，不用检测了
    //insert变量的来源一定也是符号表
    string firstOperand(tacNode->first->name);
    vector<vector<uint>>& backwardGraph = m_block->getBackwardGraph();
    vector<SsaSymb*>& insertVar = tacNode->functionSymb;
    unordered_set<string> insertEffectVar;
    unordered_set<string> insertEffectNoZeroVar;
    vector<int> constRecordor;
    constRecordor.clear();
    insertEffectVar.clear();
    insertEffectNoZeroVar.clear();
    uint blockId = tacNode->blockId;
    for(uint i = 0;i < backwardGraph[blockId].size();i++)
    {
        if(!m_isBlockUseful[backwardGraph[blockId][i]])continue;
        if(insertVar[i]->type == SYM_INT)constRecordor.push_back(insertVar[i]->value);
        if(insertVar[i]->type != SYM_VAR)continue;
            insertEffectVar.insert(insertVar[i]->name);
    }
    //拿到有效的SsaSymb，后续操作
    //接下来去掉d0的情况，也就是不统计
    unordered_set<string>::iterator it;;
    for(it = insertEffectVar.begin();it != insertEffectVar.end();++it)
    {
        if((*it).c_str()[0] != 'u')
        {
            insertEffectNoZeroVar.insert(*it);
            continue;
        }
        uint filePathLen = (*it).size();
        uint dPosition = (*it).find('d');
        const char* cFilepath = (*it).c_str()+dPosition+1;
        uint needTestId = atoi(cFilepath);
        if(needTestId != 0)insertEffectNoZeroVar.insert(*it);
    }
    //如果都不可执行。。。不可能，但还是检查一下
    if(insertEffectNoZeroVar.size() == 0)return;
    //剩下看它们是否都是常数
    bool isEqualAndConst = true;
    for(it = insertEffectNoZeroVar.begin();it != insertEffectNoZeroVar.end();it++)
    {
        uint idOfVar = m_var2id[*it];
        if(m_isVarValueKnow[idOfVar] != ASKNOW)
        {
            isEqualAndConst = false;
            break;
        }
    }  
    if(isEqualAndConst == false)
    {
        addToVarIdWorkListU(firstOperand);
        return;
    }
    it = insertEffectNoZeroVar.begin();
    uint idOfVar = m_var2id[*it];
    //常数是否相等
    int firstvalue = m_assignVarValue[idOfVar];
    for(it = insertEffectNoZeroVar.begin();it != insertEffectNoZeroVar.end();it++)
    {
        uint idOfVar = m_var2id[*it];
        if(m_assignVarValue[idOfVar] != firstvalue)
        {
            isEqualAndConst = false;
            break;
        }
    }
    for(uint i = 0;i < constRecordor.size();i++)   
    {
        if(constRecordor[i] != firstvalue)
        {
            isEqualAndConst = false;
            break;            
        }
    } 

    if(isEqualAndConst == false)
    {
        addToVarIdWorkListU(firstOperand);
        return;
    }
    addToVarIdWorkListA(firstOperand,firstvalue);
}

void CondConstPropagation::actionOfMemOrCall(SsaTac* tacNode)
{
    //首先判断它是否在符号表里
    string firstOperand(tacNode->first->name);
    unordered_map<string,uint>::iterator it;
    it = m_var2id.find(firstOperand);
    if(it == m_var2id.end())return;
    //如果在符号表里，直接UPKONW
    addToVarIdWorkListU(firstOperand);
}

void CondConstPropagation::actionOfIfz(SsaTac* tacNode)
{
    vector<vector<uint>>& forwardGraph = m_block->getForwardGraph();
    uint blockId = tacNode->blockId;
    uint firstBid = forwardGraph[blockId][0];
    uint secondBid = forwardGraph[blockId][1];
    uint gotoBid = atoi(tacNode->second->name+1);
    uint succBid = firstBid == gotoBid?secondBid:firstBid;
    if(tacNode->first->type == SYM_INT)
    {
        int value = tacNode->first->value;
        if(value != 0)addToBlockWorkList(succBid);
        else addToBlockWorkList(gotoBid);
        return;
    }
    else if(tacNode->first->type == SYM_VAR)
    {
        string firstOperand(tacNode->first->name);
        unordered_map<string,uint>::iterator it;
        it = m_var2id.find(firstOperand);
        if(it == m_var2id.end())
        {
            addToBlockWorkList(succBid);
            addToBlockWorkList(gotoBid);
            return;
        }
        uint idOfFirstVar = m_var2id[firstOperand];
        if(m_isVarValueKnow[idOfFirstVar] == ASKNOW)
        {
            int value = m_assignVarValue[idOfFirstVar];
            if(value != 0)addToBlockWorkList(succBid);
            else addToBlockWorkList(gotoBid);
            return;            
        }
        addToBlockWorkList(succBid);
        addToBlockWorkList(gotoBid);
    }
    else
    {
        exit(-1);
    }
}


void CondConstPropagation::VarPropagation(void)
{
    //搜集待删变量
    vector<SsaSymb*> needDeleteVarList;
    vector<int> needReplaceValueList;
    needDeleteVarList.clear();
    unordered_map<string,SsaSymb*>& uName2SsaSymbs = m_block->getUName2SsaSymbs();
    unordered_map<string,SsaSymb*>& tName2SsaSymbs = m_block->getTName2SsaSymbs();
    for(uint i = 0;i < m_isVarValueKnow.size();i++)
    {
        if(m_isVarValueKnow[i] != ASKNOW)continue;
        string varName = m_id2var[i];
        SsaSymb* varSymb;
        if(varName.c_str()[0] == 't')varSymb = tName2SsaSymbs[varName];
        else varSymb = uName2SsaSymbs[varName];
        needDeleteVarList.push_back(varSymb);
        needReplaceValueList.push_back(m_assignVarValue[i]);
    }    
    //变量传播，首先到使用链中在各个地方替换掉自己
    for(uint i = 0;i < needDeleteVarList.size();i++)
    {
        SsaSymb* needDeleteVar = needDeleteVarList[i];
        ReplaceVarUseConst(needDeleteVar,needReplaceValueList[i]);
    }
    for(uint i = 0;i < needDeleteVarList.size();i++)
    {
        SsaSymb* needDeleteVar = needDeleteVarList[i];
        needDeleteVar->defPoint->type = TAC_UNDEF;//删掉该条语句
    }
}   

void CondConstPropagation::ReplaceVarUseConst(SsaSymb* varSymb,int value)
{
    UseSsaTac* varUseListHead = varSymb->useList;
    while(varUseListHead->next != NULL)
    {
        SsaTac* needReplaceTac = varUseListHead->next->code;
        switch (needReplaceTac->type)
        {
        case TAC_INSERT:
            for(uint i = 0;i < needReplaceTac->functionSymb.size();i++)
            {
                ReplaceVarAndCleanUseTac(varSymb,needReplaceTac->functionSymb[i],
                    needReplaceTac->functionSymb2Tac[i],value);                
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
            ReplaceVarAndCleanUseTac(varSymb,needReplaceTac->second,
                needReplaceTac->secondPoint,value);
            ReplaceVarAndCleanUseTac(varSymb,needReplaceTac->third,
                needReplaceTac->thirdPoint,value);            
            break;
        case TAC_NEG:
        case TAC_POSI:
        case TAC_NOT:
        case TAC_COPY:
            ReplaceVarAndCleanUseTac(varSymb,needReplaceTac->second,
                needReplaceTac->secondPoint,value);            
            break;
        case TAC_IFZ:
        case TAC_ACTUAL:
        case TAC_RETURN:
            ReplaceVarAndCleanUseTac(varSymb,needReplaceTac->first,
                needReplaceTac->firstPoint,value);
            break;
        case TAC_ARR_L:
            ReplaceVarAndCleanUseTac(varSymb,needReplaceTac->second,
                needReplaceTac->secondPoint,value);
            ReplaceVarAndCleanUseTac(varSymb,needReplaceTac->third,
                needReplaceTac->thirdPoint,value);             
            break;
        case TAC_ARR_R:
            ReplaceVarAndCleanUseTac(varSymb,needReplaceTac->first,
                needReplaceTac->firstPoint,value);
            ReplaceVarAndCleanUseTac(varSymb,needReplaceTac->third,
                needReplaceTac->thirdPoint,value); 
            break;
        case TAC_LEA:
            ReplaceVarAndCleanUseTac(varSymb,needReplaceTac->third,
                needReplaceTac->thirdPoint,value); 
            break;
        default:
            break;
        }
        varUseListHead = varUseListHead->next;
    }
}

void CondConstPropagation::ReplaceVarAndCleanUseTac(
SsaSymb* &deleteSymb,SsaSymb* &varSymb,UseSsaTac* &useTac,int value)
{
    string deleteVarName(deleteSymb->name);
    if(varSymb->type == SYM_INT)return;
    if(varSymb->type == SYM_VAR)
    {
        string varSumbName(varSymb->name);
        if(!deleteVarName.compare(varSumbName))
        {
            varSymb = new SsaSymb();
            varSymb->type = SYM_INT;
            varSymb->value = value;
            useTac = NULL;
        }
    }
}

void CondConstPropagation::deleteUnuseBlock(void)
{
    vector<uint> needDeleteBidList;
    needDeleteBidList.clear();
    for(uint i = 0;i < m_isBlockUseful.size();i++)
    {
        if(m_isBlockUseful[i] == true)continue;
        needDeleteBidList.push_back(i);
    }
    if(needDeleteBidList.size() == 0)return;
    //删除基本块之前，需要将里面所有变量的引用和定值全部删除
    //定值语句直接在符号表里删，其他的现在符号表里找，找得到就删引用
    // vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    // for(uint i = 0;i < needDeleteBidList.size();i++)
    // {
    //     uint needDeleteBid = needDeleteBidList[i];
    //     SsaTac* tacHead = basicBlocks[needDeleteBid]->getTacHead();
    //     SsaTac* tacTail = basicBlocks[needDeleteBid]->getTacTail();
    //     if(tacHead == NULL)continue;
    //     SsaTac* tacHeadHead = new SsaTac();
    //     tacHeadHead->next = tacHead;
    //     SsaTac* curTac = tacHeadHead;         
    //     do
    //     {
    //         curTac = curTac->next;
    //         switch (curTac->type)
    //         {
    //         case :
                
    //             break;
            
    //         default:
    //             break;
    //         }
    //     } while (curTac != tacTail);

    // }
    m_block->deleteBasicBlock(needDeleteBidList);    
}
#endif