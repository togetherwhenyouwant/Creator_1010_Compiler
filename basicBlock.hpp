//======================================================================
//
//        Copyright (C) 2021 张锦伦    
//        All rights reserved
//
//        filename :basicBlock.hpp
//        description :
//
//        created by 张锦伦 at  03/07/2021 16:02:31
//        
//
//======================================================================

/**
 * ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┐
 * │Esc│   │ F1│ F2│ F3│ F4│ │ F5│ F6│ F7│ F8│ │ F9│F10│F11│F12│ │P/S│S L│P/B│  ┌┐    ┌┐    ┌┐
 * └───┘   └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┘  └┘    └┘    └┘
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐ ┌───┬───┬───┐ ┌───┬───┬───┬───┐
 * │~ `│! 1│@ 2│# 3│$ 4│% 5│^ 6│& 7│* 8│( 9│) 0│_ -│+ =│ BacSp │ │Ins│Hom│PUp│ │N L│ / │ * │ - │
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤ ├───┼───┼───┤ ├───┼───┼───┼───┤
 * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │{ [│} ]│ | \ │ │Del│End│PDn│ │ 7 │ 8 │ 9 │   │
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤ └───┴───┴───┘ ├───┼───┼───┤ + │
 * │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │: ;│" '│ Enter  │               │ 4 │ 5 │ 6 │   │
 * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────────┤     ┌───┐     ├───┼───┼───┼───┤
 * │ Shift  │ Z │ X │ C │ V │ B │ N │ M │< ,│> .│? /│  Shift   │     │ ↑ │     │ 1 │ 2 │ 3 │   │
 * ├─────┬──┴─┬─┴──┬┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤ ┌───┼───┼───┐ ├───┴───┼───┤ E││
 * │ Ctrl│    │Alt │         Space         │ Alt│    │    │Ctrl│ │ ← │ ↓ │ → │ │   0   │ . │←─┘│
 * └─────┴────┴────┴───────────────────────┴────┴────┴────┴────┘ └───┴───┴───┘ └───────┴───┴───┘
 */

#ifndef BASICBLOCK_HPP
#define BASICBLOCK_HPP
#include "frontEndInput.hpp"
#include "ssaCodeStruct.hpp"
#include <iostream>
#include <vector>
using namespace std;
typedef unsigned int uint;
class BasicBlock
{
private:
public:
    uint m_id;                                              //基本块的ID
    uint m_instNum;                                         //指令数
    SsaTac* m_tacHead;                                      //头指针
    SsaTac* m_tacTail;                                      //尾指针
private:
    void cleanDirtyTac(bool isNeedDelete);                  //删除脏语句
    void placeInsertTac(SsaTac* newTac);                    //放置insert语句
public:
    BasicBlock();
    ~BasicBlock();
    void printBasicBlock(void);                             //打印基本块               
    void cleanDirtyTac(void);                               //清理脏指令
    void raiseUnUseTac(void);                               //移除不使用的节点
    void setId(uint id);                                    //设置基本块ID
    void setinstNum(uint instNum){m_instNum=instNum;}       //设置块指令数
    void setTacHead(SsaTac* tacHead){m_tacHead=tacHead;}    //设置基本块头指针
    void setTacTail(SsaTac* tacTail){m_tacTail=tacTail;}    //设置基本块尾指针
    void insertPsiFunction(SsaSymb* temp,uint size);        //插入Psi函数
    SsaTac* getTacTail(void){return m_tacTail;};            //得到头指针
    SsaTac* getTacHead(void){return m_tacHead;};            //得到尾指针
    uint getInstNum(void){return m_instNum;};               //得到指令条数
    void insertTacToTail(SsaTac* newTac);                   //在尾部插入语句
};

BasicBlock::BasicBlock()
{
    m_instNum = 0;
    m_tacHead = m_tacTail = NULL;
}

BasicBlock::~BasicBlock()
{

}
void BasicBlock::setId(uint id)
{
    m_id=id;
    if(m_tacHead == NULL)return;
    SsaTac* tacHeadHead = new SsaTac();
    tacHeadHead->next = m_tacHead;
    SsaTac* curTac = tacHeadHead;
    do
    {
        curTac = curTac->next;
        curTac->blockId = id;
    } while (curTac != m_tacTail);
    delete tacHeadHead;
        
}

void BasicBlock::printBasicBlock(void)
{
    cout << endl << "blockId: B" <<  m_id << endl;
    if(m_tacHead == NULL)return;
    //如果有插入函数，则优先显示插入函数
    SsaTac* tacHeadHead = new SsaTac();
    tacHeadHead->next = m_tacHead;
    SsaTac* curTac = tacHeadHead;
    do
    {
        curTac = curTac->next;
        printOneSsaTac(curTac);
        printf("\n");
    } while (curTac != m_tacTail);
    delete tacHeadHead;
}

//加入插入函数语句
void BasicBlock::insertPsiFunction(SsaSymb* temp,uint size)
{
    //插在头节点上面
    SsaTac* addNode = new SsaTac();
    addNode->type = TAC_INSERT;
    addNode->blockId = m_id;
    addNode->first = createSsaSymbFromSsaSymb(temp);
    addNode->functionSymb.resize(size);
    for(uint i = 0;i < size;i++)
        addNode->functionSymb[i] = 
        createSsaSymbFromSsaSymb(temp);

    addNode->functionSymb2Tac.resize(size);
    if(m_tacHead == NULL)
        m_tacHead = m_tacTail = addNode;
    else 
    {
        addNode->next = m_tacHead;
        m_tacHead->prev = addNode;
        m_tacHead = addNode;
    }
    m_instNum++;
}

void BasicBlock::cleanDirtyTac(void)
{
    cleanDirtyTac(true);
} 

void BasicBlock::cleanDirtyTac(bool isNeedDelete)
{
    if(m_tacHead == NULL)return;
    //首先要保证拿到一个正确的头
    vector<SsaTac*> needToSaveList;
    vector<SsaTac*> needToDeleteList;
    needToSaveList.clear();
    needToDeleteList.clear();

    SsaTac* tacHeadHead = new SsaTac();
    tacHeadHead->next = m_tacHead;
    SsaTac* curTac = tacHeadHead;
    do
    {
        curTac = curTac->next;
        if(curTac->type == TAC_UNDEF)
            needToDeleteList.push_back(curTac);
        else needToSaveList.push_back(curTac);
    } while (curTac!=m_tacTail);
    delete tacHeadHead;
    //删掉无用的头节点
    m_instNum = m_instNum - needToDeleteList.size();
    if(isNeedDelete == true)
    {
        for(uint i = 0;i < needToDeleteList.size();i++)
            delete needToDeleteList[i];
    }

    //剩下有用的语句相连
    if(needToSaveList.size() == 0)
    {
        m_tacHead = m_tacTail = NULL;
        return;
    }
    m_tacHead = needToSaveList[0];
    m_tacTail = needToSaveList[0];
    for(uint i = 1;i < needToSaveList.size();i++)
    {
        m_tacTail->next = needToSaveList[i];
        needToSaveList[i]->prev = m_tacTail;
        m_tacTail = needToSaveList[i];
    }    
}

void BasicBlock::raiseUnUseTac(void)
{
    cleanDirtyTac(false);
}

void BasicBlock::insertTacToTail(SsaTac* newTac)
{
    m_instNum++;
    newTac->prev = NULL;
    newTac->next = NULL;
    if(newTac->type == TAC_INSERT)
    {
        placeInsertTac(newTac);
        return;
    }
    if(m_tacHead == NULL)
    {
        m_tacHead = m_tacTail = newTac;
        return;
    }
    //假如不为空
    //查看尾指针是否是goto语句
    if(m_tacTail->type != TAC_GOTO)
    {
        //直接插入即可
        m_tacTail->next = newTac;
        newTac->prev = m_tacTail;
        m_tacTail = newTac;
        return;
    }
    //是goto语句则要判断是否只有一条语句
    if(m_tacTail == m_tacHead)
    {
        m_tacHead = newTac;
        m_tacHead->next = m_tacTail;
        m_tacTail->prev = m_tacHead;
        return;
    }
    //往goto语句前面插语句
    m_tacTail->prev->next = newTac;
    newTac->prev = m_tacTail->prev;
    m_tacTail->prev = newTac;
    newTac->next = m_tacTail;
}

void BasicBlock::placeInsertTac(SsaTac* newTac)
{
    if(m_tacHead == NULL)
    {
        m_tacHead = newTac;
        m_tacTail = newTac;
        return;
    }
    if(m_tacTail->type == TAC_INSERT)
    {
        m_tacTail->next = newTac;
        newTac->prev = m_tacTail;
        m_tacTail = newTac;
        return;
    }
    SsaTac* tacTailTail = new SsaTac();
    tacTailTail->prev = m_tacTail;
    SsaTac* curTac = tacTailTail;  
    do
    {
        curTac = curTac->prev;
        if(curTac->type != TAC_INSERT)continue;
        //如果遇到了insert语句，就往它后面加
        newTac->prev = curTac;
        newTac->next = curTac->next;
        curTac->next->prev = newTac;
        curTac->next = newTac;
        return;
    } while (curTac != m_tacHead);
    delete tacTailTail;
    //如果都没有
    //就只能在头节点插入了
    m_tacHead->prev = newTac;
    newTac->next = m_tacHead;
    m_tacHead = newTac;
}

#endif