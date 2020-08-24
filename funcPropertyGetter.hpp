//函数特性获得器
//用于分析函数的特性，比如是否可以将函数当成一个表达式
//比如函数受什么变量的影响，是否会更改传进去的指针变量
#ifndef FUNCPROPERTYGETTER_HPP
#define FUNCPROPERTYGETTER_HPP
#include <unordered_set>
#include "functionBlock.hpp"

//函数信息容器
class FuncInfoContainer
{
private:
    string m_funcName;
    // vector<string> m_callFuncList;    //调用函数
    vector<string> m_callFuncParaList;//向调用函数传入的参数
    //第一个是调用了哪些函数名
    //第二个是传入的变量是自身函数formal的第几个变量（actual）
    //第三个是传入的变量是被调函数的第几个变量（formal）
    unordered_map<string,uint> m_arrFormalIndex;
    unordered_map<string,vector<pair<uint,uint>>> m_funcFormal2formaled;
    unordered_map<string,vector<pair<string,uint>>> m_funcGlobal2formaled;
    // unordered_map<string,vector<pair<uint,uint>>> m_funcFormal2formaled;
    // unordered_map<string,pair<string,uint>> m_funcGlobal2formaled;
    //调用的函数
    unordered_set<string> m_callFuncList;
    //函数会修改的全局变量名字
    unordered_set<string> m_modifyGlobalName;
    //函数会修改的参数指针名
    unordered_set<string> m_modifyFormalPointerName;
    //函数会修改的全局指针名
    unordered_set<string> m_modifyGlobalPointerName;
public:
    void clear(void);
    void addModifyGlobalName(string varName);
    void addModifyGlobalPointerName(string varName);
    void addCallFunc(string funcName);
    void addCallFuncOfLocalArr(string funcName,uint formalVar,uint formaledVar);//局部变量
    void addCallFuncOfGlobalArr(string funcName,string formalVar,uint formaledVar);//全局变量
    void printCallFun(void);
    string getFuncName(void){return m_funcName;}
    unordered_set<string>& getCallFuncList(void){return m_callFuncList;};
    unordered_set<string>& getModifyGlobalName(void);
    unordered_set<string>& getModifyGlobalPointerName(void);
    unordered_map<string,uint>& getArrFormalIndex(void){return m_arrFormalIndex;};
    unordered_map<string,vector<pair<uint,uint>>>& getFuncFormal2formaled(void){return m_funcFormal2formaled;};
    unordered_map<string,vector<pair<string,uint>>>& getFuncGlobal2formaled(void){return m_funcGlobal2formaled;};
    FuncInfoContainer(){};
    FuncInfoContainer(string funcName){m_funcName = funcName;};
    ~FuncInfoContainer(){};
};
void FuncInfoContainer::clear(void)
{
    m_callFuncList.clear();
    m_callFuncParaList.clear();
    m_modifyGlobalName.clear();
    m_arrFormalIndex.clear();
    m_funcFormal2formaled.clear();
    m_modifyFormalPointerName.clear();
    m_modifyGlobalPointerName.clear();
}
void FuncInfoContainer::addModifyGlobalName(string varName)
{
    m_modifyGlobalName.insert(varName);
}

void FuncInfoContainer::addModifyGlobalPointerName(string varName)
{
    m_modifyGlobalPointerName.insert(varName);
}

void FuncInfoContainer::addCallFunc(string funcName)
{
    m_callFuncList.insert(funcName);
}
unordered_set<string>& FuncInfoContainer::getModifyGlobalName(void)
{
    return m_modifyGlobalName;
}

unordered_set<string>& FuncInfoContainer::getModifyGlobalPointerName(void)
{
    return m_modifyGlobalPointerName;
}

void FuncInfoContainer::addCallFuncOfLocalArr(string funcName,uint formalVar,uint formaledVar)//局部变量
{
    pair<uint,uint> needAddVar = make_pair(formalVar,formaledVar);
    auto it = m_funcFormal2formaled.find(funcName);
    if(it != m_funcFormal2formaled.end())
    {
        //说明需要添加
        m_funcFormal2formaled[funcName].push_back(needAddVar);
        return;
    }
    vector<pair<uint,uint>> newVector;
    newVector.clear();
    newVector.push_back(needAddVar);
    m_funcFormal2formaled[funcName] = newVector;
    return;
}

void FuncInfoContainer::addCallFuncOfGlobalArr(string funcName,string formalVar,uint formaledVar)//全局变量
{
    pair<string,uint> needAddVar = make_pair(formalVar,formaledVar);
    auto it = m_funcGlobal2formaled.find(funcName);
    if(it != m_funcGlobal2formaled.end())
    {
        //说明需要添加
        m_funcGlobal2formaled[funcName].push_back(needAddVar);
        return;
    }
    vector<pair<string,uint>> newVector;
    newVector.clear();
    newVector.push_back(needAddVar);
    m_funcGlobal2formaled[funcName] = newVector;
    return;
}

void FuncInfoContainer::printCallFun(void)
{
    cout<< endl << "函数内参数传参：" << endl;
    for(auto it = m_funcFormal2formaled.begin();it != m_funcFormal2formaled.end();it++)
    {
        cout << it->first << "\t";
        vector<pair<uint,uint>> formal2formaled = it->second;
        for(uint i = 0;i < formal2formaled.size();i++)
        {
            cout << " (" << formal2formaled[i].first << "," << formal2formaled[i].second << ") ";
        }
        cout << endl;
    }
    cout<< endl << "全局数组参数传参：" << endl;
    for(auto it = m_funcGlobal2formaled.begin();it != m_funcGlobal2formaled.end();it++)
    {
        cout << it->first << "\t";
        vector<pair<string,uint>> global2formaled = it->second;
        for(uint i = 0;i < global2formaled.size();i++)
        {
            cout << " (" << global2formaled[i].first << "," << global2formaled[i].second << ") ";
        }
        cout << endl;
    }
}

//函数特性分析器
class FuncPropertyGetter
{
private:
    vector<vector<uint>> m_backwardGraph;
    unordered_map<string,FuncInfoContainer*> m_funcInfoContainers;
    unordered_set<string> m_funcNameSet;
    unordered_map<string,uint> m_funcName2index;
    unordered_map<uint,string> m_index2funcName;
    unordered_map<string,bool> m_funcSimpleExprFlag;
private:
    void getFuncInfo(FunctionBlock* funcBlock);
    void buildCallGraph(void);
    bool ifArrParameter(SsaTac* codeTac,string& varName);
    string lookForFuncName(BasicBlock* basicBlock);
    void lowWorksheetAlgorithm(void);
    void highWorksheetAlgorithm(void);
public:
    FuncPropertyGetter();
    ~FuncPropertyGetter(){};
    void clear(void);
    void addANewFunc(FunctionBlock* funcBlock);
    bool isFuncASimpleExpression(string funcName);
    unordered_set<string> getDirtyArrName(void);

};
//需要知道执行该函数会修改哪些变量
//需要知道该函数会依赖哪些全局变量
FuncPropertyGetter::FuncPropertyGetter()
{
    clear();
}
void FuncPropertyGetter::clear(void)
{
    m_funcNameSet.clear();
    m_index2funcName.clear();
    m_funcName2index.clear();
    m_funcSimpleExprFlag.clear();
    m_funcInfoContainers.clear();
    m_backwardGraph.clear();
}
//改变变量的情况
//1.全局变量改变值依赖于输入参数
//2.全局变量改变值不依赖于输入参数
//3.全局变量不改变，改变输入参数（数组）
//4.全局变量不改变，也不改变输入参数

//改变函数输出值的情况
//1.返回值可能依赖于全局变量
//2.返回值只依赖于输入（纯表达式）
void FuncPropertyGetter::addANewFunc(FunctionBlock* funcBlock)
{
    getFuncInfo(funcBlock);
    //得到这个函数的形参
    unordered_set<string> formalVarName;
    vector<BasicBlock*>& basicBlock = funcBlock->getBasicBlocks();
    formalVarName.clear();
    //判断一个函数是否可以当成一个单纯的表达式
    m_funcSimpleExprFlag[funcBlock->m_name] = true;
    for(uint blockId = 0;blockId < basicBlock.size();blockId++)
    {
        if(m_funcSimpleExprFlag[funcBlock->m_name] == false)break;
        SsaTac* tacHead = basicBlock[blockId]->getTacHead();
        SsaTac* tacTail = basicBlock[blockId]->getTacTail();
        if(tacHead == NULL)continue;
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        SsaTac* curTac = tacHeadHead;
        do
        {
            curTac = curTac->next;
            switch (curTac->type)
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
                if(curTac->first->name[0] == 'g')
                {
                    m_funcSimpleExprFlag[funcBlock->m_name] = false;
                    break;
                }
                if(curTac->second->type != SYM_INT && 
                    curTac->second->name[0] == 'g')
                {
                    m_funcSimpleExprFlag[funcBlock->m_name] = false;
                    break;
                }
                if(curTac->third->type != SYM_INT && 
                    curTac->third->name[0] == 'g')
                {
                    m_funcSimpleExprFlag[funcBlock->m_name] = false;
                    break;
                }
                
                break;
            case TAC_NEG:
            case TAC_POSI:
            case TAC_NOT:
            case TAC_COPY:
                if(curTac->first->name[0] == 'g')
                {
                    m_funcSimpleExprFlag[funcBlock->m_name] = false;
                    break;
                }
                if(curTac->second->type != SYM_INT && 
                    curTac->second->name[0] == 'g')
                {
                    m_funcSimpleExprFlag[funcBlock->m_name] = false;
                    break;
                }
                break;
            case TAC_CALL:
                m_funcSimpleExprFlag[funcBlock->m_name] == false;
                break;
            case TAC_IFZ:
                if(curTac->first->type != SYM_INT &&
                    curTac->first->name[0] == 'g')
                {
                    m_funcSimpleExprFlag[funcBlock->m_name] = false;
                    break;
                }
                break;
            case TAC_RETURN:
                if( curTac->first != NULL &&
                    curTac->first->type != SYM_INT &&
                    curTac->first->name[0] == 'g')
                {
                    m_funcSimpleExprFlag[funcBlock->m_name] = false;
                    break;
                }
                break;
            case TAC_ARR_L:
                //还需要修改，因为可能不是全局也可能不是formal中传来的
                m_funcSimpleExprFlag[funcBlock->m_name] = false;
                break;
            case TAC_ARR_R:
            //     if(curTac->second->name[0] == 'g')
            //     {
            //         m_funcSimpleExprFlag[funcBlock->m_name] = false;
            //         break;
            //     }
            //    if(curTac->third->type != SYM_INT && 
            //         curTac->third->name[0] == 'g')
            //     {
            //         m_funcSimpleExprFlag[funcBlock->m_name] = false;
            //         break;
            //     }
                m_funcSimpleExprFlag[funcBlock->m_name] = false;
                break;
            default:
                break;
            }
        } while (curTac != tacTail);
        
    }
} 
 
bool FuncPropertyGetter::isFuncASimpleExpression(string funcName)
{
    //如果是一个单纯的表达式
    //计算不能有全局变量值
    //不修改数组内容
    //不给全局变量赋值
    //意思就是不能出现全局变量并且不能给数组赋值
    auto it = m_funcSimpleExprFlag.find(funcName);
    if(it == m_funcSimpleExprFlag.end())return false;
    else return m_funcSimpleExprFlag[funcName];
}


//理清每一个函数是否修改了全局变量
//这个非常简单，不管call的函数，记录修改的全局变量
//然后构建函数调用图，最后通过工作表算法解决

void FuncPropertyGetter::getFuncInfo(FunctionBlock* funcBlock)
{
    string funcName = funcBlock->m_name;
    if(m_funcNameSet.find(funcName) == m_funcNameSet.end())
    {
        m_funcNameSet.insert(funcName);
        m_index2funcName[m_funcName2index.size()] = funcName;
        m_funcName2index[funcName] = m_funcName2index.size();
    }
    
    //获得函数信息容器
    auto funcIterator= m_funcInfoContainers.find(funcName);
    FuncInfoContainer *funcInfo;
    if(funcIterator == m_funcInfoContainers.end())
    {
        funcInfo = new FuncInfoContainer(funcName);
        m_funcInfoContainers[funcName] = funcInfo;
    }
    else funcInfo = funcIterator->second;
    funcInfo->clear();
    //遍历函数块
    vector<BasicBlock*>& basicBlock = funcBlock->getBasicBlocks();
    vector<vector<uint>>& forwardGraph = funcBlock->getForwardGraph();
    unordered_set<string>& modifyGlobalPointerName = funcInfo->getModifyGlobalPointerName();
    //搞到自己的formal形参
    unordered_map<string,uint>& arrFormalIndex = funcInfo->getArrFormalIndex();
    uint formalNum = 0;
    for(uint blockId = 0;blockId < basicBlock.size();blockId++)
    {
        SsaTac* tacHead = basicBlock[blockId]->getTacHead();
        SsaTac* tacTail = basicBlock[blockId]->getTacTail();
        if(tacHead == NULL)continue;
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        SsaTac* curTac = tacHeadHead;
        do
        {
            curTac = curTac->next;
            switch(curTac->type)
            {
            case TAC_FORMAL:
                if(curTac->first->type == SYM_ARRAY)
                    arrFormalIndex[curTac->first->name] = formalNum;
                formalNum++;
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
            case TAC_NEG:
            case TAC_POSI:
            case TAC_NOT:
            case TAC_COPY:
            case TAC_ARR_R:
            case TAC_LEA:
                if(curTac->first->name[0] != 'g')break;
                funcInfo->addModifyGlobalName(curTac->first->name);
                break;
            case TAC_CALL:
                if(curTac->first == NULL)break;
                if(curTac->first->name[0] != 'g')break;
                funcInfo->addModifyGlobalName(curTac->first->name);
                funcInfo->addCallFunc(curTac->second->name);
            case TAC_ACTUAL:
                {
                    string varName;
                    if(!ifArrParameter(curTac,varName))break;
                    if(varName.c_str()[0] == 'g')
                        modifyGlobalPointerName.insert(varName);
                }
            case TAC_ARR_L:
            case TAC_ZERO:
                if(curTac->first->name[0] == 'g')
                    modifyGlobalPointerName.insert(curTac->first->name);
                break;
            }
        }while(curTac != tacTail);
        delete tacHeadHead;
    }
    for(auto it = arrFormalIndex.begin();it != arrFormalIndex.end();it++)
        it->second = formalNum - 1 - it->second;
    //判断actual
    for(uint blockId = 0;blockId < basicBlock.size();blockId++)
    {
        SsaTac* tacHead = basicBlock[blockId]->getTacHead();
        SsaTac* tacTail = basicBlock[blockId]->getTacTail();
        if(tacHead == NULL)continue;      
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        SsaTac* curTac = tacHeadHead; 
        uint actualNum = 0;
        vector<pair<string,uint>> arrIndex;
        arrIndex.clear();
        do
        {
            curTac = curTac->next;
            string varName;
            if(curTac->type != TAC_ACTUAL)continue;//只找actual
            if(!ifArrParameter(curTac,varName)){actualNum++;continue;}
            arrIndex.push_back(make_pair(varName,actualNum));
            actualNum++;
        }while(curTac != tacTail);
        delete tacHeadHead;
        if(arrIndex.size() == 0)continue;
        string funcName = lookForFuncName(basicBlock[forwardGraph[blockId][0]]);
        for(uint i = 0;i < arrIndex.size();i++)
        {
            arrIndex[i].second = actualNum - 1 - arrIndex[i].second;
            string varName = arrIndex[i].first;
            uint index = arrIndex[i].second;
            if(varName.c_str()[0] == 'g')
                funcInfo->addCallFuncOfGlobalArr(funcName,varName,index);
            else if(arrFormalIndex.find(varName) != arrFormalIndex.end())
                funcInfo->addCallFuncOfLocalArr(funcName,arrFormalIndex[varName],index);
        }
    }
    cout << endl;
}

string FuncPropertyGetter::lookForFuncName(BasicBlock* basicBlock)
{
    SsaTac* tacHead = basicBlock->getTacHead();
    SsaTac* tacTail = basicBlock->getTacTail();
    if(tacHead == NULL)exit(-1);
    SsaTac* tacHeadHead = new SsaTac();
    tacHeadHead->next = tacHead;
    SsaTac* curTac = tacHeadHead; 
    do
    {
        curTac = curTac->next;
        if(curTac->type != TAC_CALL)continue;
        delete tacHeadHead;
        return curTac->second->name;
    }while(curTac != tacTail);
}

bool FuncPropertyGetter::ifArrParameter(SsaTac* codeTac,string& varName)
{
    if(codeTac->type != TAC_ACTUAL)exit(-1);
    if(codeTac->first->type == SYM_INT)return false;//参数不是数组
    if(codeTac->first->name[0] =='g')return false;
    SsaSymb* needToJudge = codeTac->first;
    do
    {
        SsaTac* defTac = needToJudge->defPoint;
        //判断是不是copy，如果是就break
        if(defTac->type == TAC_COPY)
        {
            if(defTac->second->type == SYM_INT)return false;
            if(defTac->second->type == SYM_VAR &&
                defTac->second->name[0] == 'g')return false;
            needToJudge = defTac->second;
        }
        else if(defTac->type == TAC_LEA)
        {
            varName = defTac->second->name;
            return true;
        }
        else return false;
    }while(1);
    return false;
}

void FuncPropertyGetter::buildCallGraph(void)
{
    m_backwardGraph.resize(m_funcNameSet.size());
    for(uint i = 0;i < m_backwardGraph.size();i++)
        m_backwardGraph[i].clear();
    for(auto it = m_funcInfoContainers.begin();it != 
                    m_funcInfoContainers.end();it++)
    {
        uint funcNameIndex = m_funcName2index[it->first];
        FuncInfoContainer* container = it->second;
        unordered_set<string>& callFuncList = container->getCallFuncList();
        for(auto iter = callFuncList.begin();iter != callFuncList.end();iter++)
        {
            if(m_funcNameSet.find(*iter) == m_funcNameSet.end())continue;
            uint connectFuncNameIndex = m_funcName2index[*iter];
            m_backwardGraph[connectFuncNameIndex].push_back(funcNameIndex);
        }
    }
}

void FuncPropertyGetter::lowWorksheetAlgorithm(void)
{
    //反向图已经构建成功，接下来就是跑低级的工作表算法
    unordered_set<uint> nodeWorkList;
    nodeWorkList.clear();
    //添加最初的工作表节点
    for(auto it = m_index2funcName.begin();it != m_index2funcName.end();it++)
        nodeWorkList.insert(it->first);

    while(!nodeWorkList.empty())
    {
        uint curNode = *(nodeWorkList.begin());//取第一个元素节点
        nodeWorkList.erase(curNode);
        //得到当前处理节点的全局变量修改集合
        string curFuncName = m_index2funcName[curNode];
        unordered_set<string>& curGlobalVar = 
        m_funcInfoContainers[curFuncName]->getModifyGlobalName();
        set<string> curGlobalVarTemp;
        curGlobalVarTemp.clear();
        for(auto it = curGlobalVar.begin();it != curGlobalVar.end();it++)
            curGlobalVarTemp.insert(*it);
        //根据反向图给调用该节点的节点求并集
        for(uint i = 0;i < m_backwardGraph[curNode].size();i++)
        {
            uint callingNode = m_backwardGraph[curNode][i];
            string callingFuncName = m_index2funcName[callingNode];
            unordered_set<string>& callingGlobalVar = 
            m_funcInfoContainers[callingFuncName]->getModifyGlobalName(); 
            //求并集，如果并集有改变，则加入工作表中
            uint formerSize = callingGlobalVar.size();//之前的个数
            //求个callingGlobalVar和curGlobalVar的并集
            set<string> callingGlobalVarTemp;
            set<string> callingGlobalVarResult;
            callingGlobalVarResult.clear();
            callingGlobalVarTemp.clear();
            for(auto it = callingGlobalVar.begin();it != callingGlobalVar.end();it++)
                callingGlobalVarTemp.insert(*it);
            set_union(curGlobalVarTemp.begin(),curGlobalVarTemp.end(),
            callingGlobalVarTemp.begin(),callingGlobalVarTemp.end(),
            inserter(callingGlobalVarResult,callingGlobalVarResult.begin()));
            for(auto it = callingGlobalVarResult.begin();it != callingGlobalVarResult.end();it++)
                callingGlobalVar.insert(*it);  
            //求并集结束，判断是否加入工作表          
            uint laterSize = callingGlobalVar.size();
            if(laterSize != formerSize)nodeWorkList.insert(callingNode);
        }
    }
    //全局数组也可以先这样显式的求得
}


void FuncPropertyGetter::highWorksheetAlgorithm(void)
{
    //第一步：先做一遍过滤
    for(auto it = m_funcInfoContainers.begin();
    it != m_funcInfoContainers.end();it++)
    {
        FuncInfoContainer *container = it->second;
        unordered_map<string,vector<pair<uint,uint>>>& 
            funcFormal2formaled = container->getFuncFormal2formaled();
        unordered_map<string,vector<pair<string,uint>>>& 
            funcGlobal2formaled = container->getFuncGlobal2formaled();
        for(auto iter = funcFormal2formaled.begin();
        iter != funcFormal2formaled.end();iter++)
        {
            string funcName = iter->first;
            if(m_funcNameSet.find(funcName) != m_funcNameSet.end())continue;
            funcFormal2formaled.erase(funcName);
        }
        for(auto iter = funcGlobal2formaled.begin();
        iter != funcGlobal2formaled.end();iter++)
        {
            string funcName = iter->first;
            if(m_funcNameSet.find(funcName) != m_funcNameSet.end())continue;
            funcGlobal2formaled.erase(funcName);
        }
    }
    //第二步：根据调用关系，得到被调用关系图
    //被调者名字，调用者名字，被调者formal位置，调用者formal位置
    unordered_map<string,unordered_map<string,vector<pair<uint,uint>>>> funcCalled;
    for(auto it = m_funcInfoContainers.begin();
    it != m_funcInfoContainers.end();it++)
    {
        string funcName = it->first;
        FuncInfoContainer *container = it->second;
        unordered_map<string,vector<pair<uint,uint>>>& 
            funcFormal2formaled = container->getFuncFormal2formaled();
        for(auto iter = funcFormal2formaled.begin();
        iter != funcFormal2formaled.end();iter++)
        {
            string callFuncName = iter->first;
            auto iterCall = funcCalled.find(callFuncName);
        }                
    }    
    //第一步：得到每一张图
    //假如可以显式求得的都求得了
    //接下来就是求指针的修改了
    //调用图不一定有用，要看是否传了参

}

//理清每一个函数是否修改了全局数组
//这个非常困难，首先肯定不管call的函数，
//记录被修改的全局数组变量和formal局部数组变量
//构建函数调用图，也是通过工作表算法解决
//但是和全局变量不一样的是：
//先通过工作表算法和全局变量一样解决


//1.函数内是否显式的对全局指针进行修改                   //工作表算法
//2.讨论清楚每一个函数是否对formal指针进行修改           //工作表算法
//3.假如全局变量是某个函数的形参，而函数会对改形参进行修改//遍历一遍即可
unordered_set<string> FuncPropertyGetter::getDirtyArrName(void)
{
    unordered_set<string> result;
    result.clear();
    for(auto it = m_funcInfoContainers.begin();
    it != m_funcInfoContainers.end();it++)
    {
        FuncInfoContainer* container = it->second;
        unordered_set<string>& dirtyArrName = container->getModifyGlobalPointerName();
        for(auto iter = dirtyArrName.begin();
        iter != dirtyArrName.end();iter++)
            result.insert(*iter);
    }
    return result;
}
#endif