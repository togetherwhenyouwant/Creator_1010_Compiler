/*code is far away from bug with the animal protecting
*  ┏┓　　　┏┓
*┏┛┻━━━┛┻┓
*┃　　　　　　　┃ 　
*┃　　　━　　　┃
*┃　┳┛　┗┳　┃
*┃　　　　　　　┃
*┃　　　┻　　　┃
*┃　　　　　　　┃
*┗━┓　　　┏━┛
*　　┃　　　┃神兽保佑
*　　┃　　　┃代码无BUG！
*　　┃　　　┗━━━┓
*　　┃　　　　　　　┣┓
*　　┃　　　　　　　┏┛
*　　┗┓┓┏━┳┓┏┛
*　　　┃┫┫　┃┫┫
*　　　┗┻┛　┗┻┛ 
*　　　
*/
//created by 张锦伦 at  08/08/2021 16:02:31
#ifndef LOOPOPTIMIZER_HPP
#define LOOPOPTIMIZER_HPP
#include <queue>
#include "globalPolicyExecutor.hpp"
//代码审查ing : 1遍
class Loop
{
private:
    int m_id;               //id
    int m_head;             //循环头
    int m_newNode;          //新增节点
    int m_fatherId;         //嵌套的爸爸
	vector<uint> m_sonLoop; //嵌套的子循环
    vector<uint> m_nodeList;//循环的节点序号
public:
    Loop();
    ~Loop(){}
    void setId(int id){m_id = id;}
    void setHeadNode(int head){m_head = head;}
    void setNewNode(int newNode){m_newNode = newNode;}
    void setFatherLoop(int fatherId){m_fatherId = fatherId;}
    void addSonLoop(uint sonId){m_sonLoop.push_back(sonId);}
    void setNodeList(vector<uint>& nodeList);
    int getId(void){return m_id;}
    int getHeadNode(void){return m_head;}
    int getNewNode(void){return m_newNode;}
    int getFatherLoop(void){return m_fatherId;}
    vector<uint>& getSonLoop(void){return m_sonLoop;}
    vector<uint>& getNodeList(void){return m_nodeList;}
};
Loop::Loop()
{
    m_id = -1;               
    m_head = -1;             
    m_newNode = -1;          
    m_fatherId = -1;         
	m_sonLoop.clear();  
    m_nodeList.clear();
}
void Loop::setNodeList(vector<uint>& nodeList)
{
    m_nodeList.clear();
    for(uint i = 0;i < nodeList.size();i++)
        m_nodeList.push_back(nodeList[i]);
}

class ArrayDirtyUnit
{
private:
    unordered_set<uint> m_dirtyIndex;
    bool m_dirtyFlag;               //true为脏 false为干净
public:
    void makeDirty(void){m_dirtyFlag = true;}//直接弄脏
    void makeDirty(SsaSymb* varSymb)
    {
        if(m_dirtyFlag == true)return;
        if(varSymb == NULL)return;
        if(varSymb->type != SYM_INT)//不是int，就全脏了
            m_dirtyFlag = true;
        else
        {
            uint dirtyIndex = varSymb->value;
            m_dirtyIndex.insert(dirtyIndex);
        }
        
    }
    bool isDirty(SsaSymb* varSymb)
    {   
        if(varSymb == NULL)exit(-1);//不可能传入空值
        if(m_dirtyFlag == true)return true;
        if(varSymb->type == SYM_INT)//看看这个坐标是否脏了
            return isDirty(varSymb->value);
        return m_dirtyFlag;         //说明是a[t]或者a[g]
    }
    bool isDirty(uint index)
    {
        if(m_dirtyFlag == true)return true;//全脏了
        else
        {
            unordered_set<uint>::iterator it;
            it = m_dirtyIndex.find(index);
            if(it != m_dirtyIndex.end())return true;
            return false;                    //找不到说明没脏
        }
    }
    ArrayDirtyUnit(void):m_dirtyFlag(false){m_dirtyIndex.clear();};
    ~ArrayDirtyUnit(void){};
};

//每次将循环不变量放在新增节点的基本块内即可

//循环优化分为三步
//1.确定循环
//2.新增节点
//3.外提变量

class LoopOptimizer:
public GlobalPolicyExecutor
{
private:
    bool m_isOptimize;                      //是否有优化的机会
    FunctionBlock* m_block;                 
    vector<pair<uint,uint>> m_extraEdgeList;//边集
    vector<vector<uint>> m_mustPassNodeTree;//必经树
    vector<uint> m_idomVector;              //直接必经节点
    vector<Loop*> m_loopList;               //循环集合
    vector<bool> m_isAddNewNodeForId;       //循环是否已添加节点
    unordered_map<string,ArrayDirtyUnit*> m_arrAssignUnit;  //数组脏标志
    bool m_globalArrDirtyFlag;
private: 
    void clear();
    void calMustPassNodeTree(void);         //得到必经节点数
    void calNeedCheckEdgeList(void);        //得到{控制流边集 - 必经树边集}
    void lookForLoop(void);                 //寻找所有循环
    void buildLoopTreeBaseOnVector
        (vector<vector<uint>> &loopList);   //根据数组构建循环嵌套树
    void addNewNodeForALoop(uint loopId);   //为一个循环找到一个头节点
    void fixTheNodeHeadForLoop(void);       //为每个循环确定新增节点
    void extraVarForALoop(uint loopId);     //为一个循环外提变量
    void extraVarForLoop(void);             //为每个循环外提变量
    bool isLoopInvariant(SsaSymb* varSymb,
    unordered_set<uint>& loopNodeSet);      //是否循环不变量
    bool isLegitDefVar(SsaSymb* varSymb);   //是否合法的定值变量
    bool ifarr_rCodeDirty(SsaTac* codeTac); //该左值在循环内是否合法
    bool ifArrParameter(SsaTac* codeTac,string& varName);//判断actual里的参数是否是数组
    void generateArrayDirtyUnit(vector<uint>& nodeList);//生成DirtyUnit
    void getVarImportances(void);//得到每个变量重要性
public:
    LoopOptimizer();
    ~LoopOptimizer();
    void printInfoOfOptimizer(void);
    bool runOptimizer(FunctionBlock* block,
        FuncPropertyGetter* funcPropertyGetter);//运行优化器
    
};

LoopOptimizer::LoopOptimizer()
{
    clear();
    m_name = "循环优化器";
}

LoopOptimizer::~LoopOptimizer()
{
}
void LoopOptimizer::clear()
{
    m_block = NULL;
    m_isOptimize = false;
    m_globalArrDirtyFlag = false;
    m_idomVector.clear();
    m_mustPassNodeTree.clear();
    m_extraEdgeList.clear();
    m_loopList.clear();
    m_isAddNewNodeForId.clear();
    m_arrAssignUnit.clear();
}
void LoopOptimizer::printInfoOfOptimizer(void)
{
    
}
bool LoopOptimizer::runOptimizer(FunctionBlock* block,
    FuncPropertyGetter* funcPropertyGetter)//运行优化器
{
    clear();
    m_block = block;
    lookForLoop();              //寻找循环
    fixTheNodeHeadForLoop();    //为每个循环找到新增节点
    bool isOptimize = false;
    m_isOptimize = true;
    while(m_isOptimize)
    {
        m_isOptimize = false;
        extraVarForLoop();          //为每个循环提取循环不变量
        if(m_isOptimize)isOptimize = true;
    }
    getVarImportances();
    return isOptimize;
}

void LoopOptimizer::calMustPassNodeTree(void)     //得到必经节点数
{
    //得到必经节点数树
    vector<uint> startVector;
    vector<uint> endVector;
    startVector.clear();endVector.clear();
    m_extraEdgeList.clear();
    m_mustPassNodeTree.clear();
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    vector<vector<uint>>& forwardGraph = m_block->getForwardGraph();
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
    m_idomVector.resize(dominantTreeIdom.size()/2);
    for(uint i = 0;i < m_mustPassNodeTree.size();i++)m_mustPassNodeTree[i].clear();
    for(uint i = 0;i < m_mustPassNodeTree.size();i++)
    {
        uint idomNode = dominantTreeIdom[i*2+1];
        uint sonNode = dominantTreeIdom[i*2];
        m_idomVector[sonNode] = idomNode;//不可能访问到 m_idomVector[0]
        if(idomNode==sonNode)continue;
        m_mustPassNodeTree[idomNode].push_back(sonNode);
    }
    
}


void LoopOptimizer::calNeedCheckEdgeList(void)
{
    m_extraEdgeList.clear();
    vector<vector<uint>>& forwardGraph = m_block->getForwardGraph();
    vector<pair<uint,uint>> graghEdgeList;//图的边集
    vector<pair<uint,uint>> mustPassEdgeList;//必经节点树的边集
    graghEdgeList.clear();
    mustPassEdgeList.clear();
    //拿到图的所有边
    for(uint i = 0;i < forwardGraph.size();i++)
    {
        for(uint j = 0;j < forwardGraph[i].size();j++)
            graghEdgeList.push_back(make_pair(i,forwardGraph[i][j]));
    }
    m_extraEdgeList.resize(graghEdgeList.size());
    //拿到必经树的所有边
    for(uint i = 1;i < m_idomVector.size();i++)
    {
        uint endPoint = i;
        uint startPoint = m_idomVector[i];
        mustPassEdgeList.push_back(make_pair(startPoint,endPoint));
    }
    //做一个差集
    sort(graghEdgeList.begin(),graghEdgeList.end());
    sort(mustPassEdgeList.begin(),mustPassEdgeList.end());
    vector<pair<uint,uint>>::iterator it = 
    set_difference(graghEdgeList.begin(), graghEdgeList.end(), 
    mustPassEdgeList.begin(), mustPassEdgeList.end(), m_extraEdgeList.begin());
    m_extraEdgeList.resize(it - m_extraEdgeList.begin());//得到差集 
}

void LoopOptimizer::lookForLoop(void)             //寻找循环
{
    //计算必经节点数
    calMustPassNodeTree();
    //得到除必经节点数的边集
    calNeedCheckEdgeList();
    //通过边集计算得到循环
    vector<vector<uint>> loopList;				    //循环集合
    loopList.clear();
    for(uint i = 0;i < m_extraEdgeList.size();i++)
    {
        vector<uint> newVector;
        newVector.clear();
        uint startPoint = m_extraEdgeList[i].first;
        uint endPoint = m_extraEdgeList[i].second;
        do
        {
            newVector.push_back(startPoint);
            if(m_idomVector[startPoint] == 0 ||
            m_idomVector[startPoint] == endPoint)break;
            startPoint = m_idomVector[startPoint];
        }while(1);
        if(m_idomVector[startPoint] == 0)continue;  //说明没找到循环
        //如果找到循环了
        newVector.clear();
        newVector.push_back(m_extraEdgeList[i].second);
        newVector.push_back(m_extraEdgeList[i].first);
        loopList.push_back(newVector);
    }
    //
    vector<vector<uint>>& backwardGraph = m_block->getBackwardGraph();
    vector<unordered_set<uint>> loopNodeListSet;
    loopNodeListSet.clear();
    loopNodeListSet.resize(loopList.size());
    for(uint i = 0;i < loopList.size();i++)
    {
        uint headNode = loopList[i][0];
        uint startPoint = loopList[i][1];
        loopNodeListSet[i].insert(headNode);
        loopNodeListSet[i].insert(startPoint);
        //开始找前驱
        queue<uint> prevList;
        prevList.push(startPoint);
        while(!prevList.empty())
        {
            uint needToLookNode = prevList.front();
            prevList.pop();       
            for(uint j = 0;j < backwardGraph[needToLookNode].size();j++)
            {
                uint prevBid = backwardGraph[needToLookNode][j];
                if(prevBid != headNode && loopNodeListSet[i].find(prevBid)
                     == loopNodeListSet[i].end())
                {
                    prevList.push(prevBid);
                    loopNodeListSet[i].insert(prevBid);
                    loopList[i].push_back(prevBid);
                }
            }
        }
    }
    buildLoopTreeBaseOnVector(loopList);
}

void LoopOptimizer::buildLoopTreeBaseOnVector(vector<vector<uint>> &loopList)
{
    m_loopList.clear();
    for(uint i = 0;i < loopList.size();i++)
    {
        Loop* newLoop = new Loop();
        newLoop->setId(i);
        newLoop->setHeadNode(loopList[i][0]);
        newLoop->setNodeList(loopList[i]);
        m_loopList.push_back(newLoop);
    }
    //最重要的便是找出儿子和父亲的关系
    //也就是循环嵌套树，最内层循环等
    //仔细想想，这是一个拓扑问题，拓扑排序即可
    //核心思想：若祖辈只有一个，那一定就是父亲
    //构建反向图
    uint loopNum = m_loopList.size();
    vector<unordered_set<uint>> loopNodeSetList;
    loopNodeSetList.resize(loopNum);
    //用集合进行比较时间复杂度较低
    for(uint i = 0;i < loopNum;i++)
    {
        vector<uint>& nodeListTemp = m_loopList[i]->getNodeList();
        for(uint j = 0;j < nodeListTemp.size();j++)
            loopNodeSetList[i].insert(nodeListTemp[j]);
    }
    vector<vector<uint>> youngerOfId;//儿子孙子。。集合
    vector<uint> inDegreeOfId;
    youngerOfId.resize(loopNum);
    inDegreeOfId.resize(loopNum);
    for(uint i = 0;i < loopNum;i++)youngerOfId[i].clear();
    for(uint i = 0;i < loopNum;i++)inDegreeOfId[i] = 0;
    for(uint i = 0;i < loopNum;i++)
    {
        int headNode = m_loopList[i]->getHeadNode();
        for(uint j = 0;j < loopNum;j++)
        {
            if(j == i)continue;//自己与自己不比较
            if(loopNodeSetList[j].find(headNode)
            == loopNodeSetList[j].end())continue;
            //如果确实找到了两个循环图的节点关系
            youngerOfId[j].push_back(i);//第i个循环图是第j个图的儿孙
            inDegreeOfId[i]++;         //第i个节点的出度加一，也就是祖辈又多一个
        }
    }
    //进行拓扑算法求得族谱，思想：祖辈只有一个，说明一定是父亲
    //一定能拓扑完，族谱满足无环结构
    queue<uint> canFindFatherNode;
    while(!canFindFatherNode.empty())canFindFatherNode.pop();
    for(uint i = 0;i < inDegreeOfId.size();i++)
    {//先找到每个族谱的老祖宗
        if(inDegreeOfId[i] != 0)continue;
        canFindFatherNode.push(i);
    }
    while(!canFindFatherNode.empty())
    {
        uint headNode = canFindFatherNode.front();//这里的节点意思是一个循环图
        canFindFatherNode.pop();
        for(uint i = 0;i < youngerOfId[headNode].size();i++)
        {
            uint youngerOfNode = youngerOfId[headNode][i];
            inDegreeOfId[youngerOfNode]--;
            if(inDegreeOfId[youngerOfNode] != 0)continue;
            //如果出度为0，说明是真父亲啊
            canFindFatherNode.push(youngerOfNode);
            //可以建立关系了
            m_loopList[youngerOfNode]->setFatherLoop(headNode);
            m_loopList[headNode]->addSonLoop(youngerOfNode);
        }
    }
}

void LoopOptimizer::fixTheNodeHeadForLoop(void)   //为每个循环确定新增节点
{
    //因为确定新增节点涉及到循环序列的改动
    //所以需要从最内层循环开始寻找或建立
    //这里采用dfs来进行循环确定新节点
    m_isAddNewNodeForId.resize(m_loopList.size());
    for(uint i = 0;i < m_isAddNewNodeForId.size();i++)
        m_isAddNewNodeForId[i] = false;
    for(uint i = 0;i < m_loopList.size();i++)
    {
        if(m_isAddNewNodeForId[i])continue;//确定过就算了
        addNewNodeForALoop(i);             //为一个循环找到一个头节点
    }
}

void LoopOptimizer::addNewNodeForALoop(uint loopId)
{
    m_isAddNewNodeForId[loopId] = true;
    vector<uint> &sonLoop = m_loopList[loopId]->getSonLoop();
    for(uint i = 0;i < sonLoop.size();i++)
        addNewNodeForALoop(sonLoop[i]);
    /***给每个循环确认新节点***/
    //得到循环头除循环外的前驱
    unordered_set<uint> loopNodeSet;
    vector<uint> extraPrevBidList;
    loopNodeSet.clear();
    extraPrevBidList.clear();
    vector<uint>& nodeList = m_loopList[loopId]->getNodeList();
    for(uint i = 0;i < nodeList.size();i++)loopNodeSet.insert(nodeList[i]);
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    vector<vector<uint>>& forwardGraph = m_block->getForwardGraph();
    vector<vector<uint>>& backwardGraph = m_block->getBackwardGraph();
    uint headNode = m_loopList[loopId]->getHeadNode();
    int FatherLoopId = m_loopList[loopId]->getFatherLoop();
    for(uint i = 0;i < backwardGraph[headNode].size();i++)
    {
        uint prevBid = backwardGraph[headNode][i];
        if(loopNodeSet.find(prevBid) != loopNodeSet.end())continue;
        extraPrevBidList.push_back(prevBid);
    }
    //检查是否不用新增节点，可以复用节点
    if(extraPrevBidList.size() == 0)exit(-1);//不可能为0，预防一下
    do
    {
        int headNodeOfFatherLoop; 
        if(FatherLoopId == -1)headNodeOfFatherLoop = -1;
        else headNodeOfFatherLoop = m_loopList[FatherLoopId]->getHeadNode();
        if(extraPrevBidList.size() != 1)continue;
        uint node = extraPrevBidList[0];
        if(forwardGraph[node].size() != 1)continue;
        if(node == headNodeOfFatherLoop)continue;
        //满足以上条件，那么可以作为复用节点
        //满足以上条件的肯定在父亲循环的循环内。
        m_loopList[loopId]->setNewNode(node);
        return;
    } while (0);
    
    //剩下的必须要自己建一个节点
    uint newNode = m_block->addNewNodeForLoop(headNode,extraPrevBidList);
    m_loopList[loopId]->setNewNode(newNode);
    while(FatherLoopId != -1)
    {
        //在父循环中加入该新节点
        vector<uint>& fatherLoopNode = m_loopList[FatherLoopId]->getNodeList();
        vector<uint>::iterator it = find(fatherLoopNode.begin(),fatherLoopNode.end(),headNode);
        if(it == fatherLoopNode.end())break;
        fatherLoopNode.insert(it,newNode);//插入新增节点
        FatherLoopId = m_loopList[FatherLoopId]->getFatherLoop();
    }
}

void LoopOptimizer::extraVarForLoop(void)         //为每个循环外提变量
{
    //循环提取变量的时候，新增节点步骤已经结束
    m_isAddNewNodeForId.clear();
    m_isAddNewNodeForId.resize(m_loopList.size());
    for(uint i = 0;i < m_isAddNewNodeForId.size();i++)
        m_isAddNewNodeForId[i] = false;
    for(uint i = 0;i < m_loopList.size();i++)
    {
        if(m_isAddNewNodeForId[i])continue;//确定过就算了
        extraVarForALoop(i);               //为一个循环提取变量到新增节点
    }
}

void LoopOptimizer::extraVarForALoop(uint loopId)
{
    m_isAddNewNodeForId[loopId] = true;
    vector<uint> &sonLoop = m_loopList[loopId]->getSonLoop();
    for(uint i = 0;i < sonLoop.size();i++)
        extraVarForALoop(sonLoop[i]);
    //为每个循环提取变量，不需要考虑太多
    
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    vector<SsaTac*> needExtractTacs;
    vector<uint> needExtractTacsType;
    unordered_set<uint> loopNodeSet;
    loopNodeSet.clear();
    needExtractTacs.clear();
    needExtractTacsType.clear();
    vector<uint>& nodeList = m_loopList[loopId]->getNodeList();
    for(uint i = 0;i < nodeList.size();i++)loopNodeSet.insert(nodeList[i]);
    uint newNode = m_loopList[loopId]->getNewNode();
    generateArrayDirtyUnit(nodeList);
    for(uint i = 0;i < nodeList.size();i++)  
    {
        uint blockId = nodeList[i];
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
            switch (curTac->type)
            {
            //Insert不需要考虑，可以仔细思考一下，很简单
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
                //先不管全局变量
                if(!isLegitDefVar(curTac->first))break;
                if(!isLoopInvariant(curTac->second,loopNodeSet))break;
                if(!isLoopInvariant(curTac->third,loopNodeSet))break;
                //说明可以提取到循环外
                m_isOptimize = true;
                curTac->blockId = newNode;
                needExtractTacsType.push_back(curTac->type);
                curTac->type = TAC_UNDEF;
                needExtractTacs.push_back(curTac);
                break;
            case TAC_NEG:
            case TAC_POSI:
            case TAC_NOT:
            case TAC_COPY:
                //先不管全局变量
                if(!isLegitDefVar(curTac->first))break;
                if(!isLoopInvariant(curTac->second,loopNodeSet))break;
                //说明可以提取到循环外
                m_isOptimize = true;
                curTac->blockId = newNode;
                needExtractTacsType.push_back(curTac->type);
                curTac->type = TAC_UNDEF;
                needExtractTacs.push_back(curTac);                
                break;
            case TAC_LEA:
                if(!isLegitDefVar(curTac->first))break;
                if(!isLoopInvariant(curTac->third,loopNodeSet))break;
                m_isOptimize = true;
                curTac->blockId = newNode;
                needExtractTacsType.push_back(curTac->type);
                curTac->type = TAC_UNDEF;
                needExtractTacs.push_back(curTac);   
                break;
            //数组可以考虑，但是比较麻烦一点
            case TAC_ARR_R:
                if(!isLegitDefVar(curTac->first))break;
                if(!isLoopInvariant(curTac->third,loopNodeSet))break;
                //循环内是否有脏数组
                if(m_globalArrDirtyFlag == true &&
                    curTac->second->name[0] == 'g')break;
                //printOneSsaTac(curTac);
                if(ifarr_rCodeDirty(curTac))break;
                //说明可以提取到循环外
                m_isOptimize = true;
                curTac->blockId = newNode;
                needExtractTacsType.push_back(curTac->type);
                
                curTac->type = TAC_UNDEF;
                needExtractTacs.push_back(curTac);  
                
                break;
            // case TAC_ARR_L:
            //     if(!isLoopInvariant(curTac->second,loopNodeSet))break;
            //     if(!isLoopInvariant(curTac->third,loopNodeSet))break;
            //     m_isOptimize = true;
            //     curTac->blockId = newNode;
            //     needExtractTacsType.push_back(curTac->type);
            //     curTac->type = TAC_UNDEF;
            //     needExtractTacs.push_back(curTac);  
            //     break;
            default:
                break;
            }
        } while (curTac != tacTail);
        delete tacHeadHead;
    }
    //先将基本块内的需要提取的语句抹掉
    for(uint i = 0;i < nodeList.size();i++)
    {
        uint blockId = nodeList[i];
        basicBlocks[blockId]->raiseUnUseTac();
    }
    //将语句加入到新节点中
    for(uint i = 0;i < needExtractTacs.size();i++)
    {
        //添加语句到基本块
        needExtractTacs[i]->type = needExtractTacsType[i];
        basicBlocks[newNode]->insertTacToTail(needExtractTacs[i]);
    }
}

bool LoopOptimizer::isLoopInvariant(SsaSymb* 
    varSymb,unordered_set<uint>& loopNodeSet)
{
    if(varSymb->type == SYM_INT)return true;
    unordered_map<string,SsaSymb*>& uName2SsaSymbs = m_block->getUName2SsaSymbs();
    unordered_map<string,SsaSymb*>& tName2SsaSymbs = m_block->getTName2SsaSymbs();
    //目前采用的策略
    if(uName2SsaSymbs.find(varSymb->name) == uName2SsaSymbs.end() &&
    tName2SsaSymbs.find(varSymb->name) == tName2SsaSymbs.end())return false;
    uint defBlockId = varSymb->defPoint->blockId;
    if(loopNodeSet.find(defBlockId) == loopNodeSet.end())return true;
    return false;
}

bool LoopOptimizer::isLegitDefVar(SsaSymb* varSymb)   //是否合法的定值变量
{
    unordered_map<string,SsaSymb*>& uName2SsaSymbs = m_block->getUName2SsaSymbs();
    unordered_map<string,SsaSymb*>& tName2SsaSymbs = m_block->getTName2SsaSymbs();
    //目前采用的策略
    if(uName2SsaSymbs.find(varSymb->name) == uName2SsaSymbs.end() &&
    tName2SsaSymbs.find(varSymb->name) == tName2SsaSymbs.end())return false;
    return true;
}


void LoopOptimizer::generateArrayDirtyUnit(vector<uint>& nodeList)
{
    m_arrAssignUnit.clear();
    m_globalArrDirtyFlag = false;
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    for(uint i = 0;i < nodeList.size();i++)
    {
        uint blockId = nodeList[i];
        SsaTac* tacHead = basicBlocks[blockId]->getTacHead();
        SsaTac* tacTail = basicBlocks[blockId]->getTacTail();
        if(tacHead == NULL)continue;
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        SsaTac* curTac = tacHeadHead; 
        do
        {
            curTac = curTac->next;
            switch(curTac->type)
            {
                case TAC_ARR_L:
                    {
                        ArrayDirtyUnit* curArr;
                        unordered_map<string,ArrayDirtyUnit*>::iterator it;
                        it = m_arrAssignUnit.find(curTac->first->name);
                        if(it == m_arrAssignUnit.end())
                        {
                            curArr = new ArrayDirtyUnit();
                            m_arrAssignUnit[curTac->first->name] = curArr;
                        }
                        else curArr = it->second;
                        curArr->makeDirty(curTac->second);
                    }
                    break;
                case TAC_ACTUAL:
                    {
                        string parameterName;
                        if(!ifArrParameter(curTac,parameterName))break;
                        ArrayDirtyUnit* curArr;
                        unordered_map<string,ArrayDirtyUnit*>::iterator it;
                        it = m_arrAssignUnit.find(parameterName);
                        if(it == m_arrAssignUnit.end())
                        {
                            curArr = new ArrayDirtyUnit();
                            m_arrAssignUnit[parameterName] = curArr;
                        }  
                        else curArr = it->second;   
                        curArr->makeDirty();                   
                    }
                case TAC_CALL:
                    {
                        m_globalArrDirtyFlag = true;
                    }
                    break;
            }
        }while(curTac != tacTail);
    }
}

bool LoopOptimizer::ifarr_rCodeDirty(SsaTac* codeTac) //该左值在循环内是否合法
{
    if(m_arrAssignUnit.empty())return false;
    unordered_map<string,ArrayDirtyUnit*>::iterator it;
    it = m_arrAssignUnit.find(codeTac->second->name);
    if(it == m_arrAssignUnit.end())return false;//没找到
    else
    {
        return m_arrAssignUnit[codeTac->second->name]
            ->isDirty(codeTac->third);
    }
}

bool LoopOptimizer::ifArrParameter(SsaTac* codeTac,string& varName)
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
}

void LoopOptimizer::getVarImportances(void)//得到每个变量重要性
{
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    //数组初始化
    for(uint i = 0;i < basicBlocks.size();i++)
    {
        SsaTac* tacHead = basicBlocks[i]->getTacHead();
        SsaTac* tacTail = basicBlocks[i]->getTacTail();
        if(tacHead == NULL)continue;  
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        SsaTac* curTac = tacHeadHead; 
        do
        {
            curTac = curTac->next;
            // if(curTac->type == TAC_LABEL)continue;
            // else if(curTac->type == TAC_FORMAL)continue;
            // else if(curTac->type == TAC_GOTO)continue;
            curTac->priority = 0;
        }while(curTac != tacTail);
        delete tacHeadHead;
    }
    for(uint i = 0;i < m_loopList.size();i++)
    {
        vector<uint>& nodeList = m_loopList[i]->getNodeList();
        for(uint j = 0;j < nodeList.size();j++)
        {
            uint blockId = nodeList[j];
            SsaTac* tacHead = basicBlocks[blockId]->getTacHead();
            SsaTac* tacTail = basicBlocks[blockId]->getTacTail();
            if(tacHead == NULL)continue;  
            SsaTac* tacHeadHead = new SsaTac();
            tacHeadHead->next = tacHead;
            SsaTac* curTac = tacHeadHead; 
            do
            {
                curTac = curTac->next;
                if(curTac->type == TAC_LABEL)continue;
                else if(curTac->type == TAC_FORMAL)continue;
                else if(curTac->type == TAC_GOTO)continue;
                curTac->priority++;
            }while(curTac != tacTail);
            delete tacHeadHead;
        }
    }
}

#endif