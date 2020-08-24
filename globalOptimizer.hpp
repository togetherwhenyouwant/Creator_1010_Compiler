//======================================================================
//
//        Copyright (C) 2021 张锦伦    
//        All rights reserved
//
//        filename :globalOptimizr.hpp
//        description :
//
//        created by 张锦伦 at  03/07/2021 16:02:31
//        
//
//======================================================================

#ifndef GLOBALOPTIMIZER_HPP
#define GLOBALOPTIMIZER_HPP
#include <vector>
#include "functionBlock.hpp"
#include "frontEndInput.hpp"
#include "globalPolicyExecutor.hpp"
#include "condConstPropagation.hpp"
#include "pluralPropagation.hpp"
#include "gDeadCodeDeletion.hpp"
#include "delGlobalExpror.hpp"
#include "basicBlockExecutor.hpp"
#include "loopOptimizer.hpp"
#include "ssaTranslator.hpp"
#include"RegisterAlloc.hpp"
#include "funcPropertyGetter.hpp"
#include"ReplaceGlobalByLocal.hpp"
// #include"Inline_func.hpp"
// #include "gDeadCodeDeletion.hpp"
class GlobalOptimizer
{
private:
	vector<FunctionBlock*> m_functionBlocks;       //程序块
	SsaTranslator* m_ssaTranslator;                //SSA转换器
	vector<GlobalPolicyExecutor*> m_policyExecutor;//策略优化器
	FuncPropertyGetter* m_funcPropertyGetter;	   //函数特性获取器
private:
	void replaceIfTac(FunctionBlock* functionBlock);
public:
	GlobalOptimizer();
	~GlobalOptimizer();

	//读取中间代码到程序块
	void writeCodeIntoFunctionBlock(tac* tacHead);
	void makeSsaTranslate(void);
	tac* transSsaTacIntoTac(void);
	void runOptimizer(void);
};



GlobalOptimizer::GlobalOptimizer()
{
	m_functionBlocks.clear();
	m_policyExecutor.clear();
	m_ssaTranslator = new SsaTranslator();
	CondConstPropagation* condConstPropagation =
		new CondConstPropagation();
	PluralPropagation* pluralPropagation =
		new PluralPropagation();
	DelGlobalExpror* delGlobalExpror =
		new DelGlobalExpror();
	GDeadCodeDeletion* gDeadCodeDeletion =
		new GDeadCodeDeletion();
	BasicBlockExecutor* basicBlockExecutor =
		new BasicBlockExecutor();
	LoopOptimizer* loopOptimizer = new LoopOptimizer();
	#ifdef OPTIM
	m_funcPropertyGetter = new FuncPropertyGetter();
	m_policyExecutor.push_back(condConstPropagation); //条件常量传播
	m_policyExecutor.push_back(pluralPropagation);    //复数传播
	m_policyExecutor.push_back(delGlobalExpror);      //删除公共子表达式
	m_policyExecutor.push_back(gDeadCodeDeletion);    //死代码删除器
	m_policyExecutor.push_back(basicBlockExecutor);   //数组常复数传播
	m_policyExecutor.push_back(loopOptimizer);        //循环
	#endif
}

GlobalOptimizer::~GlobalOptimizer()
{
}


void GlobalOptimizer::writeCodeIntoFunctionBlock(tac* tacHead)
{
	FunctionBlock::s_globalSymTable->clear();
	SsaTac* sTacHead = transTacIntoSsaTac(tacHead);
	//首先添加全局符号表
	SsaTac* curTac = sTacHead;
	for (; curTac->next != NULL; curTac = curTac->next)
	{
		if (curTac->next->type == TAC_FUNC)break;
		//添加全局符号表
		FunctionBlock::s_globalSymTable->addTac(curTac->next);
		if (curTac->next->type == TAC_VAR)
			FunctionBlock::s_globalSymTable->addVar(curTac->next->first);
	}
	vector<SsaTac*> funtionBlockHead;
	funtionBlockHead.clear();
	for (; curTac->next != NULL; curTac = curTac->next)
	{
		if (curTac->next->type == TAC_FUNC)
			funtionBlockHead.push_back(curTac);
	}
	//cout << endl << "这是原始代码：" << endl;
	for (uint i = 0; i < funtionBlockHead.size(); i++)
	{
		FunctionBlock* functionBlock = new FunctionBlock();
		functionBlock->buildBlockFromTacs(funtionBlockHead[i]);
		// functionBlock->printBasicBlockNum();
		m_functionBlocks.push_back(functionBlock);
		// cout << "The line in the Function is : " << 
		// functionBlock->getTheLineOfFunction() << endl;
	}
}

void GlobalOptimizer::makeSsaTranslate(void)
{
	// cout << endl << "这是SSA代码：" << endl;
	for (uint i = 0; i < m_functionBlocks.size(); i++)
	{

		m_ssaTranslator->translateBlockIntoSsa(m_functionBlocks[i]);
		//m_functionBlocks[i]->printFunctionBlock();
	}
}

void GlobalOptimizer::runOptimizer(void)
{
	bool OptimizeContinue = true;
	vector<string> OptimizerRunOrder;
	vector<bool> OptimizerRunOrderEffict;
	//搞到每个函数值的特征
	//cout << endl << "这是SSA代码：" << endl;
	for(uint i = 0;i < m_functionBlocks.size();i++)
		m_funcPropertyGetter->addANewFunc(m_functionBlocks[i]);
	
	for (uint i = 0; i < m_functionBlocks.size(); i++)
	{
		//对每一个函数块做优化
		OptimizerRunOrder.clear();
		OptimizerRunOrderEffict.clear();
		// cout << "*************执行优化***************" << endl;
		// cout << "*************执行优化***************" << endl;
		// cout << "*************执行优化***************" << endl;
		// cout << "优化前：" << endl;
		// cout << "The line in the Function is : " << 
		// m_functionBlocks[i]->getTheLineOfFunction() << endl;
		// m_functionBlocks[i]->printFunctionBlock();
		// m_functionBlocks[i]->printControlFlowGraph();

		while (OptimizeContinue)
		{
			OptimizeContinue = false;
			for (uint j = 0; j < m_policyExecutor.size(); j++)
			{
				bool isEffict = m_policyExecutor[j]->runOptimizer(m_functionBlocks[i],m_funcPropertyGetter);
				//m_functionBlocks[i]->printFunctionBlock();
				if (isEffict) {
					OptimizeContinue = true;
					// cout << endl << m_policyExecutor[j]->getOptimizerName() << endl;
					// m_functionBlocks[i]->printFunctionBlock();
				}
				OptimizerRunOrder.push_back(m_policyExecutor[j]->getOptimizerName());
				OptimizerRunOrderEffict.push_back(isEffict);
			}
		}
		OptimizeContinue = true;
		// cout << "所用优化策略次序：" << endl;
		for (uint j = 0; j < OptimizerRunOrder.size(); j++)
		{
			//cout << OptimizerRunOrder[j] << " : ";
			// if(OptimizerRunOrderEffict[j])
			//     cout << "有效" << endl;
			// else cout << "无效" << endl;
		}
		// cout << "优化后：" << endl;
		// cout << "The line in the Function is : " << 
		// m_functionBlocks[i]->getTheLineOfFunction() << endl;
		// m_functionBlocks[i]->printFunctionBlock();
		// m_functionBlocks[i]->printControlFlowGraph();

		//消除insert函数之后
		// cout << "消除insert函数之后：" << endl;
		//m_ssaTranslator->eliminateInsertFunction(m_functionBlocks[i]);

		//m_functionBlocks[i]->printFunctionBlock();
			//lyj_label_0
		//replaceIfTac(m_functionBlocks[i]);//换成更高级的if语句
		m_ssaTranslator->solveInsertConflict(m_functionBlocks[i]);
		RegisterStrategy objRegMalloc;
		combineTwoMap(KeepRegisterMap, objRegMalloc.allocFuncblockRegister(*m_functionBlocks[i]));
		#ifndef NO_PRINT
		cout << "this is funct_tac" << endl;
		m_functionBlocks[i]->printFunctionBlock();
		#endif
		replaceIfTac(m_functionBlocks[i]);//换成更高级的if语句
		m_ssaTranslator->eliminateInsertFunction(m_functionBlocks[i]);
		//cout << "消除insert之后：" << endl;
		m_functionBlocks[i]->deleteExtraGotoBlocks();
		//m_functionBlocks[i]->printFunctionBlock();

		// cout << "The line in the Function is : " << 
		// m_functionBlocks[i]->getTheLineOfFunction() << endl;
		// cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
		// tac* Head = transSsaTacIntoTac();
		// tac_print(Head->next);

	}
	m_funcPropertyGetter->clear();
}

tac* GlobalOptimizer::transSsaTacIntoTac(void)
{
	//首先拿到函数外的代码
	vector<tac*> tacList;
	tacList.clear();
	clearFunc();
	clearLabel();
	clearVar();
	SsaTac* gTacHead = FunctionBlock::s_globalSymTable->getTacHead();
	SsaTac* gTacTail = FunctionBlock::s_globalSymTable->getTacTail();
	if (gTacHead != NULL)
	{
		SsaTac* tacHeadHead = new SsaTac();
		tacHeadHead->next = gTacHead;
		SsaTac* curTac = tacHeadHead;
		do
		{
			curTac = curTac->next;
			tacList.push_back(createTacFromSsaTac(curTac));
		} while (curTac != gTacTail);
	}

	//拿每个函数的代码

	// tacList.clear();
	for (uint i = 0; i < m_functionBlocks.size(); i++)
	{
		clearLabel();//每个函数有自己的label
		//函数开头需要生成func 函数名 和begin
		SsaTac* newTac = new SsaTac();
		newTac->type = TAC_FUNC;
		SsaSymb* newSymb = new SsaSymb();
		newSymb->type = SYM_FUNC;
		strcpy(newSymb->name, m_functionBlocks[i]->m_name.c_str());
		newTac->first = newSymb;
		tacList.push_back(createTacFromSsaTac(newTac));
		tac* beginTac = new tac();
		beginTac->op = TAC_BEGINFUNC;
		tacList.push_back(beginTac);
		//还需要变量声明
		//得到formal的变量名,在blockId=1的基本块里
		unordered_set<string> formalVarList;
		formalVarList.clear();
		if (m_functionBlocks[i]->getBasicBlocks().size() > 2)
		{
			BasicBlock* basicBlock = m_functionBlocks[i]->getBasicBlocks()[1];
			SsaTac* tacHead = basicBlock->getTacHead();
			SsaTac* tacTail = basicBlock->getTacTail();
			if (tacHead != NULL)
			{
				SsaTac* tacHeadHead = new SsaTac();
				tacHeadHead->next = tacHead;
				SsaTac* curTac = tacHeadHead;
				do
				{
					curTac = curTac->next;
					if (curTac->type != TAC_FORMAL)break;
					formalVarList.insert(curTac->first->name);
				} while (curTac != tacTail);
				delete tacHeadHead;
			}

		}
		//变量声明也很简单，先拿数组的变量声明
		vector<SsaTac*>& arrVarVector = m_functionBlocks[i]->getFuncSymTable()->getArrVar();
		for (uint j = 0; j < arrVarVector.size(); j++)
		{
			tacList.push_back(createTacFromSsaTac(arrVarVector[j]));
		}
		//再拿其他变量的变量声明
		unordered_map<string, SsaSymb*>& uName2SsaSymbs = m_functionBlocks[i]->getUName2SsaSymbs();
		unordered_map<string, SsaSymb*>& tName2SsaSymbs = m_functionBlocks[i]->getTName2SsaSymbs();
		for (unordered_map<string, SsaSymb*>::iterator it = uName2SsaSymbs.begin(); it != uName2SsaSymbs.end(); it++)
		{
			if (formalVarList.find(it->second->name) != formalVarList.end())continue;
			SsaTac* varTac = new SsaTac();
			varTac->type = TAC_VAR;
			varTac->first = it->second;
			tacList.push_back(createTacFromSsaTac(varTac));
		}
		for (unordered_map<string, SsaSymb*>::iterator it = tName2SsaSymbs.begin(); it != tName2SsaSymbs.end(); it++)
		{
			SsaTac* varTac = new SsaTac();
			varTac->type = TAC_VAR;
			varTac->first = it->second;
			tacList.push_back(createTacFromSsaTac(varTac));
		}

		vector<BasicBlock*>& basicBlocks = m_functionBlocks[i]->getBasicBlocks();
		vector<vector<uint>> PrintList = m_functionBlocks[i]->getPrintList();
		vector<uint> printOrder;
		printOrder.clear();
		for (uint j = 0; j < PrintList.size(); j++)
		{
			for (uint k = 0; k < PrintList[j].size(); k++)
				printOrder.push_back(PrintList[j][k]);
		}

		printOrder.push_back(basicBlocks.size() - 1);
		for (uint j = 0; j < printOrder.size(); j++)
		{
			uint blockId = printOrder[j];
			SsaTac* tacHead = basicBlocks[blockId]->getTacHead();
			SsaTac* tacTail = basicBlocks[blockId]->getTacTail();
			if (tacHead == NULL)continue;
			SsaTac* tacHeadHead = new SsaTac();
			tacHeadHead->next = tacHead;
			SsaTac* curTac = tacHeadHead;
			do
			{
				curTac = curTac->next;
				tacList.push_back(createTacFromSsaTac(curTac));
			} while (curTac != tacTail);

		}
		string FuncName = m_functionBlocks[i]->m_name;
		addNameToBlockName(FuncName);
		//做完以后还需要加一个end
		tac* endTac = new tac();
		endTac->op = TAC_ENDFUNC;
		tacList.push_back(endTac);
	}

	//建一个空头，用于连接
	tac* emptyHead = new tac();
	tac* curTac = emptyHead;
	for (uint i = 0; i < tacList.size(); i++)
	{
		curTac->next = tacList[i];
		tacList[i]->prev = curTac;
		curTac = tacList[i];
	}
	return emptyHead;
}

void GlobalOptimizer::replaceIfTac(FunctionBlock* functionBlock)
{
    vector<BasicBlock*>& basicBlocks = functionBlock->getBasicBlocks();
    unordered_map<string,SsaSymb*>& uName2SsaSymbs = functionBlock->getUName2SsaSymbs();
    unordered_map<string,SsaSymb*>& tName2SsaSymbs = functionBlock->getTName2SsaSymbs();
    for(uint i = 0;i < basicBlocks.size();i++)
    {
        SsaTac* tacHead = basicBlocks[i]->getTacHead();
        SsaTac* tacTail = basicBlocks[i]->getTacTail();
        if(tacTail == NULL)continue;
        SsaTac* tacTailTail = new SsaTac();
        tacTailTail->prev = tacTail;
        SsaTac* curTac = tacTailTail;
        do
        {
            curTac = curTac->prev;
            if(curTac->type == TAC_IFZ)break;
        }while(curTac != tacHead);
        if(curTac->type != TAC_IFZ)continue;
        if(curTac == tacHead)continue;
        //找到了ifz语句所在地
        //如果是 TAC_NOT 那么再往上窥一条
        if(curTac->prev->type == TAC_NOT &&             //上一条是not语句
            curTac->prev->first == curTac->first &&     //并且和ifz有联系
            curTac->prev->first->useTimes == 1)         //同时该变量
        {
            //t2 = !t1
            //ifz t2 goto C
            //ifne t1 0 goto C
            //将ifz这条命令改变一下
            bool isVarInMap = false;
            bool isPrevVarInMap = false;
            if(uName2SsaSymbs.find(curTac->first->name) != uName2SsaSymbs.end() ||
                tName2SsaSymbs.find(curTac->first->name) != tName2SsaSymbs.end())isVarInMap = true;
            if(uName2SsaSymbs.find(curTac->prev->second->name) != uName2SsaSymbs.end() ||
                tName2SsaSymbs.find(curTac->prev->second->name) != tName2SsaSymbs.end())isPrevVarInMap = true;                
            if(isVarInMap)curTac->prev->type = TAC_UNDEF;//删掉
            if(isVarInMap)
            {
                curTac->prev->first->useTimes--;
                deleteUseSsaTac(curTac->firstPoint);
                if(uName2SsaSymbs.find(curTac->first->name) != uName2SsaSymbs.end())
                    uName2SsaSymbs.erase(curTac->prev->first->name);
                else tName2SsaSymbs.erase(curTac->prev->first->name);
            }
            if(isPrevVarInMap)
            {
                curTac->prev->second->useTimes--;
                deleteUseSsaTac(curTac->prev->secondPoint);
            }
            //删掉这个变量
            
            curTac->type = TAC_IFNE;
            curTac->firstPoint = NULL;
            curTac->secondPoint = NULL;
            curTac->thirdPoint = NULL;
            curTac->first = curTac->prev->second;
            if(isPrevVarInMap)
            {
                addTacToUseListForSsaSymb(curTac,curTac->first,
                curTac->firstPoint);
            }
            curTac->third = new SsaSymb();
            curTac->third->type = SYM_INT;
            curTac->third->value = 0;
            basicBlocks[i]->cleanDirtyTac();
            tacHead = basicBlocks[i]->getTacHead();
        }
        if(curTac == tacHead)continue;
        //此时，curTac的prev已经发生了变化
        //如果是 TAC_NOT 那么再往上窥一条
        if(curTac->prev->type != TAC_EQ && curTac->prev->type != TAC_NE
        && curTac->prev->type != TAC_LT && curTac->prev->type != TAC_LE
        && curTac->prev->type != TAC_GT && curTac->prev->type != TAC_GE)continue;
        //肯定是其中的某一个
        if(curTac->prev->first == curTac->first &&
        curTac->prev->first->useTimes == 1)
        {
            bool isVarInMap = false;
            bool isPrevFirstVarInMap = false;
            bool isPrevSecondVarInMap = false;
            if(uName2SsaSymbs.find(curTac->first->name) != uName2SsaSymbs.end() ||
                tName2SsaSymbs.find(curTac->first->name) != tName2SsaSymbs.end())isVarInMap = true;
            if(uName2SsaSymbs.find(curTac->prev->second->name) != uName2SsaSymbs.end() ||
                tName2SsaSymbs.find(curTac->prev->second->name) != tName2SsaSymbs.end())isPrevFirstVarInMap = true;  
            if(uName2SsaSymbs.find(curTac->prev->second->name) != uName2SsaSymbs.end() ||
                tName2SsaSymbs.find(curTac->prev->second->name) != tName2SsaSymbs.end())isPrevSecondVarInMap = true;                  
            if(isVarInMap)
            {
                curTac->prev->first->useTimes--;
                deleteUseSsaTac(curTac->firstPoint);
                if(uName2SsaSymbs.find(curTac->first->name) != uName2SsaSymbs.end())
                    uName2SsaSymbs.erase(curTac->prev->first->name);
                else tName2SsaSymbs.erase(curTac->prev->first->name);
            }
            if(isPrevFirstVarInMap)
            {
                curTac->prev->second->useTimes--;
                deleteUseSsaTac(curTac->prev->secondPoint);
            }
            if(isPrevSecondVarInMap)
            {
                curTac->prev->third->useTimes--;
                deleteUseSsaTac(curTac->prev->thirdPoint);
            }
            if(curTac->type == TAC_IFZ)
            {
                switch (curTac->prev->type)
                {
                case TAC_EQ:
                    curTac->type = TAC_IFNE;
                    break;
                case TAC_NE:
                    curTac->type = TAC_IFEQ;
                    break;
                case TAC_LT:
                    curTac->type = TAC_IFGE;
                    break;
                case TAC_LE:
                    curTac->type = TAC_IFGT;
                    break;
                case TAC_GT:
                    curTac->type = TAC_IFLE;
                    break;
                case TAC_GE:
                    curTac->type = TAC_IFLT;
                    break;              
                default:
                    break;
                }
            }
            else
            {
                switch (curTac->prev->type)
                {
                case TAC_EQ:
                    curTac->type = TAC_IFEQ;
                    break;
                case TAC_NE:
                    curTac->type = TAC_IFNE;
                    break;
                case TAC_LT:
                    curTac->type = TAC_IFLT;
                    break;
                case TAC_LE:
                    curTac->type = TAC_IFLE;
                    break;
                case TAC_GT:
                    curTac->type = TAC_IFGT;
                    break;
                case TAC_GE:
                    curTac->type = TAC_IFGE;
                    break;              
                default:
                    break;
                }
            }
            curTac->firstPoint = NULL;
            curTac->secondPoint = NULL;
            curTac->thirdPoint = NULL;
            curTac->first = curTac->prev->second;
            curTac->third = curTac->prev->third;
            if(isPrevFirstVarInMap)
            {
                addTacToUseListForSsaSymb(curTac,curTac->first,
                curTac->firstPoint);
            }
            if(isPrevSecondVarInMap)
            {
                addTacToUseListForSsaSymb(curTac,curTac->third,
                curTac->thirdPoint);
            }
            if(isVarInMap)curTac->prev->type = TAC_UNDEF;//删掉
            basicBlocks[i]->cleanDirtyTac();
        }

    }
	//去掉剩下的ifz
	for(uint i = 0;i < basicBlocks.size();i++)
    {
        SsaTac* tacHead = basicBlocks[i]->getTacHead();
        SsaTac* tacTail = basicBlocks[i]->getTacTail();
        if(tacTail == NULL)continue;
        SsaTac* tacHeadHead = new SsaTac();
        tacHeadHead->next = tacHead;
        SsaTac* curTac = tacHeadHead;
		do
		{
			curTac = curTac->next;
			if(curTac->type != TAC_IFZ)continue;
			SsaSymb* varSymb = curTac->first;
			if(varSymb->type == SYM_INT)continue;
			if(uName2SsaSymbs.find(varSymb->name) == uName2SsaSymbs.end()
			&& tName2SsaSymbs.find(varSymb->name) == tName2SsaSymbs.end())continue;
			SsaTac* defPoint = varSymb->defPoint;
			if(defPoint->type == TAC_EQ || defPoint->type == TAC_LT ||
			defPoint->type == TAC_LE || defPoint->type == TAC_GT ||
			defPoint->type == TAC_GE)
			{
				curTac->first->useTimes--;
				deleteUseSsaTac(curTac->firstPoint);
				curTac->first = defPoint->second;
				curTac->third = defPoint->third;
				if(curTac->first->type != SYM_INT && 
				curTac->first->name[0] != 'g')
				{
					addTacToUseListForSsaSymb(curTac,curTac->first,
                	curTac->firstPoint);
				}
				if(curTac->third->type != SYM_INT && 
				curTac->third->name[0] != 'g')
				{
					addTacToUseListForSsaSymb(curTac,curTac->third,
                	curTac->thirdPoint);
				}
			}
			switch(defPoint->type)
			{
			case TAC_EQ:
				curTac->type = TAC_IFNE;
				break;
			case TAC_NE:
				curTac->type = TAC_IFEQ;
				break;
			case TAC_LT:
				curTac->type = TAC_IFGE;
				break;
			case TAC_LE:
				curTac->type = TAC_IFGT;
				break;
			case TAC_GT:
				curTac->type = TAC_IFLE;
				break;
			case TAC_GE:
				curTac->type = TAC_IFLT;
				break;
			default:
				break;
			}

		}while(curTac != tacTail);
		delete tacHeadHead;
	}
}
#endif