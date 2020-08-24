#include<set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "frontEndInput.hpp"
#include <vector>
class ReplaceGlobalByLocal
{
	/*
		1.传入函数头
		2.构建调用图
		3.构建被调图
		4.从被调图构建全局变量传播
		5.从主调图（main）计算可封装变量集合
		6.对每个变量进行封装
	*/
public:
	//1.
	ReplaceGlobalByLocal(TAC* header)
	{
		m_refTacHeader = header;
	};
	~ReplaceGlobalByLocal(void) {};
private:
	TAC* m_refTacHeader;
public:
	//调度
	void makeGlobalToLocal(void)
	{
		//拿到链表头
		TAC* TacListHeader = m_refTacHeader;
		//拿到函数开始和结尾节点
		vector<pair<TAC*, TAC*>> AllFuncsBeginNodeAndEndNode =
			splitFuncTac(TacListHeader);
		//构建函数直接调用图
		unordered_map<string, set<string>> FuncDirectiveCallGraph =
			createCallFuncGraph(AllFuncsBeginNodeAndEndNode);
		//从main函数开始构建函数深度调用图
		string StartFunction = "main";
		unordered_map<string, set<string>>& FuncDeepthCallGraph = FuncDirectiveCallGraph;
		createDeepthCallFuncGraph(FuncDeepthCallGraph, StartFunction);

		//构建函数可修改变量
		//左值右值有交叉
		unordered_map<string, pair<set<SYMB*>, set<SYMB*>>> VarDefAndRefInFunction;
		findGlobalVar(AllFuncsBeginNodeAndEndNode, VarDefAndRefInFunction);
		//区分开了左值和右值
		makeFuncVarClear(VarDefAndRefInFunction);
		//仅仅保留了可以被局部化的全局变量
		findVarCanBeMove(VarDefAndRefInFunction, FuncDeepthCallGraph);
		//构建变量使用链
		unordered_map<string, unordered_map<SYMB*, unordered_set<TAC*>>> VarUseList =
			findRefList(AllFuncsBeginNodeAndEndNode);
		//函数返回语句
		unordered_map<string, unordered_set<TAC*>> FuncReturnStatement =
			findRetList(AllFuncsBeginNodeAndEndNode);
		//替代全局变量
		replaceGlobal(
			VarDefAndRefInFunction,
			AllFuncsBeginNodeAndEndNode,
			VarUseList,
			FuncReturnStatement);
	}
	//2.构建调用图
	vector<pair<TAC*, TAC*>> splitFuncTac(TAC* pTacHeader)
	{
		vector<pair<TAC*, TAC*>> vKeepFunc;
		while (pTacHeader != nullptr && pTacHeader->op != TAC_FUNC )
		{
			pTacHeader = pTacHeader->next;
		}
		while (pTacHeader != nullptr && pTacHeader->op == TAC_FUNC )
		{
			pair<TAC*, TAC*> pairBeginAndEnd;
			pairBeginAndEnd.first = pTacHeader;
			while (pTacHeader != nullptr && pTacHeader->op != TAC_ENDFUNC )
			{
				pTacHeader = pTacHeader->next;
			}
			assert(pTacHeader != nullptr);
			pairBeginAndEnd.second = pTacHeader;

			vKeepFunc.push_back(pairBeginAndEnd);
			pTacHeader = pTacHeader->next;
		}

		return vKeepFunc;
	}
	unordered_map<string, set<string>> createCallFuncGraph(vector<pair<TAC*, TAC*>> vBeginAndEnd)
	{
		vector<pair<string, set<string>>> CallFuncGraph;

		for (int i = 0; i < vBeginAndEnd.size(); i++)
		{
			pair<string, set<string>> CallerAndCalledNmae;
			CallerAndCalledNmae.first = vBeginAndEnd[i].first->a->name;
			while (vBeginAndEnd[i].first != nullptr && vBeginAndEnd[i].first != vBeginAndEnd[i].second)
			{
				if (vBeginAndEnd[i].first->op == TAC_CALL)
				{
					assert(vBeginAndEnd[i].first->b != nullptr);
					assert(vBeginAndEnd[i].first->b->type == SYM_FUNC);
					CallerAndCalledNmae.second.insert(vBeginAndEnd[i].first->b->name);
				}
				vBeginAndEnd[i].first = vBeginAndEnd[i].first->next;
			}
			CallFuncGraph.push_back(CallerAndCalledNmae);
		}
		return makeVectorToMap(CallFuncGraph);
	}

	//从调图构建全局变量传播

	unordered_map<string, set<string>>  makeVectorToMap(vector<pair<string, set<string>>>& vector)
	{
		unordered_map<string, set<string>> map;
		for (int i = 0; i < vector.size(); i++)
		{
			map[vector[i].first] = vector[i].second;
		}
		return map;
	}
	set<string> createDeepthCallFuncGraph(unordered_map<string, set<string>>& CallFuncGraph, string strFuncName)
	{
		set<string> pSet = CallFuncGraph[strFuncName];
		set<string> setFuncCall;
		for (auto iterSet = pSet.begin(); iterSet != pSet.end(); iterSet++)
		{
			if (strFuncName != *iterSet)
			{
				set<string> setTemp = createDeepthCallFuncGraph(CallFuncGraph, *iterSet);
				setFuncCall.insert(setTemp.begin(), setTemp.end());
			}
		}
		pSet.insert(setFuncCall.begin(), setFuncCall.end());

		return pSet;
	}
	//建立变量传播
	bool isAnGlobalVar(SYMB* symbol)
	{
		return symbol->type == SYM_VAR && symbol->name[0] == 'g';
	}
	pair<SYMB*, vector<SYMB*>> makeSymbToLR(SYMB* a, SYMB* b, SYMB* c, pair<SYMB*, vector<SYMB*>>& pairLeftAndRight)
	{
		if (a != nullptr && isAnGlobalVar(a))
		{
			pairLeftAndRight.first = a;
		}
		if (b != nullptr && isAnGlobalVar(b))
		{
			pairLeftAndRight.second.push_back(b);
		}
		if (c != nullptr && isAnGlobalVar(c))
		{
			pairLeftAndRight.second.push_back(c);
		}
		return pairLeftAndRight;
	}
	pair<SYMB*, vector<SYMB*>>findGlobalVarInStatement(TAC* pTac)
	{
		pair<SYMB*, vector<SYMB*>> pairLeftAndRight;
		if (pTac->op >= TAC_ADD && pTac->op <= TAC_MOD)
		{
			return makeSymbToLR(pTac->a, pTac->b, pTac->c, pairLeftAndRight);
		}
		if (pTac->op >= TAC_NEG && pTac->op <= TAC_COPY)
		{
			return makeSymbToLR(pTac->a, pTac->b, nullptr, pairLeftAndRight);
		}
		if (pTac->op >= TAC_IFEQ && pTac->op <= TAC_IFGE)
		{
			return makeSymbToLR(nullptr, pTac->a, pTac->c, pairLeftAndRight);
		}
		if (pTac->op == TAC_IFZ)
		{
			return makeSymbToLR(nullptr, pTac->a, nullptr, pairLeftAndRight);
		}
		if (pTac->op == TAC_ACTUAL)
		{
			return makeSymbToLR(nullptr, pTac->a, nullptr, pairLeftAndRight);
		}
		if (pTac->op == TAC_CALL)
		{
			return makeSymbToLR(nullptr, pTac->a, nullptr, pairLeftAndRight);
		}
		if (pTac->op == TAC_RETURN)
		{
			return makeSymbToLR(nullptr, pTac->a, nullptr, pairLeftAndRight);
		}
		if (pTac->op == TAC_ARR_L)
		{
			return makeSymbToLR(nullptr, pTac->b, pTac->c, pairLeftAndRight);
		}
		if (pTac->op == TAC_ARR_R)
		{
			return makeSymbToLR(pTac->a, pTac->c, nullptr, pairLeftAndRight);
		}
		if (pTac->op == TAC_LEA)
		{
			return makeSymbToLR(pTac->a, pTac->c, nullptr, pairLeftAndRight);
		}
		return pairLeftAndRight;
	}
	void findGlobalVar(vector<pair<TAC*, TAC*>>& vFuncSection, unordered_map<string, pair<set<SYMB*>, set<SYMB*>>>& pairVarUsed)
	{
		for (int i = 0; i < vFuncSection.size(); i++)
		{
			assert(vFuncSection[i].first->op == TAC_FUNC);
			for (TAC* pBegin = vFuncSection[i].first; pBegin != vFuncSection[i].second; pBegin = pBegin->next)
			{
				pair<SYMB*, vector<SYMB*>> pairStatement = findGlobalVarInStatement(pBegin);
				for (auto it = pairStatement.second.begin(); it != pairStatement.second.end(); it++)
				{
					if (*it != nullptr)
					{
						pairVarUsed[vFuncSection[i].first->a->name].first.insert(*it);
					}
				}
				if (pairStatement.first != nullptr)
				{
					pairVarUsed[vFuncSection[i].first->a->name].second.insert(pairStatement.first);
				}
				
			}
		}
	}
	void makeFuncVarClear(unordered_map<string, pair<set<SYMB*>, set<SYMB*>>>& umapVarUsed)
	{
		for (auto it_I = umapVarUsed.begin(); it_I != umapVarUsed.end(); it_I++)
		{
			for (auto it_J = it_I->second.first.begin(); it_J != it_I->second.first.end();)
			{
				if (it_I->second.second.count(*it_J))
				{
					it_J = it_I->second.first.erase(it_J);
				}
				else
				{
					it_J++;
				}
			}
		}
	}
	void findVarCanBeMove(unordered_map<string, pair<set<SYMB*>, set<SYMB*>>>& umapVarUsed, unordered_map<string, set<string>>& CallFuncGraph)
	{
		unordered_map<string, pair<set<SYMB*>, set<SYMB*>>> umapVarMove;
		for (auto it_I = CallFuncGraph.begin(); it_I != CallFuncGraph.end(); it_I++)
		{
			set<SYMB*> NotMoved;
			for (auto it_J = it_I->second.begin(); it_J != it_I->second.end(); it_J++)
			{
				NotMoved.insert(umapVarUsed[*it_J].second.begin(), umapVarUsed[*it_J].second.end());
			}
			for (auto it_J = NotMoved.begin(); it_J != NotMoved.end(); it_J++)
			{
				umapVarUsed[it_I->first].second.erase(*it_J);
				umapVarUsed[it_I->first].first.erase(*it_J);
			}
			//umapVarUsed[it_I->first].second.erase(NotMoved.begin(), NotMoved.end());
			//umapVarUsed[it_I->first].first.erase(NotMoved.begin(), NotMoved.end());
			
		}
	}
	unordered_map<string, unordered_map<SYMB*, unordered_set<TAC*>>>  findRefList(vector<pair<TAC*, TAC*>>& vFuncSection)
	{
		unordered_map<string,unordered_map<SYMB*, unordered_set<TAC*>>>  umapVarToStatement;
		for (int i = 0; i < vFuncSection.size(); i++)
		{
			assert(vFuncSection[i].first->op == TAC_FUNC);
			for (auto it = vFuncSection[i].first; it != vFuncSection[i].second; it = it->next)
			{
				if (it->a != nullptr && it->a->type == SYM_VAR && it->a->name[0] == 'g')
				{
					umapVarToStatement[vFuncSection[i].first->a->name][it->a].insert(it);
				}
				if (it->b != nullptr && it->b->type == SYM_VAR && it->b->name[0] == 'g')
				{
					umapVarToStatement[vFuncSection[i].first->a->name][it->b].insert(it);
				}
				if (it->c != nullptr && it->c->type == SYM_VAR && it->c->name[0] == 'g')
				{
					umapVarToStatement[vFuncSection[i].first->a->name][it->c].insert(it);
				}
			}

		}
		return umapVarToStatement;
	}
	unordered_map<string, unordered_set<TAC*>>findRetList(vector<pair<TAC*, TAC*>>& vFuncSection)
	{
		unordered_map<string, unordered_set<TAC*>> umapFuncToRet;
		for (int i = 0; i < vFuncSection.size(); i++)
		{
			for (auto it = vFuncSection[i].first; it != vFuncSection[i].second; it = it->next)
			{
				if (it->op == TAC_RETURN)
				{
					umapFuncToRet[vFuncSection[i].first->a->name].insert(it);
				}
				if (it->next != nullptr && it->next->op == TAC_ENDFUNC && umapFuncToRet[vFuncSection[i].first->a->name].size()==0)
				{
					umapFuncToRet[vFuncSection[i].first->a->name].insert(it->next);
				}
			}
		}

		return umapFuncToRet;
	}
	void replaceItWithTemp(SYMB* TempVar, SYMB* GlobalVar, unordered_set<TAC*>& UseList)
	{
		for (auto it = UseList.begin(); it != UseList.end(); it++)
		{
			if ((*it)->a == GlobalVar)
			{
				(*it)->a = TempVar;
				continue;
			}
			if ((*it)->b == GlobalVar)
			{
				(*it)->b = TempVar;
				continue;
			}
			if ((*it)->c == GlobalVar)
			{
				(*it)->c = TempVar;
				continue;
			}
		}
	}
	void joinTacNextDecl(TAC* begin, TAC* insert)
	{
		assert(begin->op == TAC_FUNC);
		begin = begin->next->next;
		while (begin->op == TAC_VAR || begin->op == TAC_ARR || begin->op == TAC_FORMAL)
		{
			begin = begin->next;
		}
		begin = begin->prev;

		insert->next = begin->next;
		begin->next = insert;
		if (insert->next != nullptr)
		{
			insert->next->prev = insert;
		}
		insert->prev = begin;
	}
	void joinTacBeforeIt(TAC* ret, TAC* before)
	{
		assert(ret != nullptr && before != nullptr);
		before->prev = ret->prev;
		if (ret->prev != nullptr)
		{
			ret->prev->next = before;
		}

		before->next = ret;
		ret->prev = before;
	}
	void joinTacBeforeThem(unordered_set<TAC*>& ret, TAC* before)
	{

		for (auto it = ret.begin(); it != ret.end(); it++)
		{
			TAC* pTac = create_a_node(before->a, before->b, before->c, before->op);
			joinTacBeforeIt(*it, pTac);
		}
	}
	void replaceGlobal(unordered_map<string, pair<set<SYMB*>, set<SYMB*>>>& umapVarUsed,
		vector<pair<TAC*, TAC*>>& vFuncSection, unordered_map<string, unordered_map<SYMB*, unordered_set<TAC*>>>&umapVarToStatement,
		unordered_map<string, unordered_set<TAC*>>& umapFuncToRet)
	{
		for (int i = 0; i < vFuncSection.size(); i++)
		{
			unordered_set<TAC*>& FuncReturn = umapFuncToRet[vFuncSection[i].first->a->name];
			set<SYMB*>& VarUse = umapVarUsed[vFuncSection[i].first->a->name].first;
			for (auto it = VarUse.begin(); it != VarUse.end(); it++)
			{
				SYMB* TempVar = create_var_symb(get_temp_var_name());
				sprintf(TempVar->name, "u%d", count_for_rename++);
				replaceItWithTemp(TempVar, *it, umapVarToStatement[vFuncSection[i].first->a->name][*it]);
				TAC* TempAssign = create_a_node(TempVar, *it, nullptr, TAC_COPY);
				TAC* TempDecl = create_a_node(TempVar, nullptr, nullptr, TAC_VAR);
				joinTacNextDecl(vFuncSection[i].first, TempDecl);
				joinTacNextDecl(vFuncSection[i].first, TempAssign);
			}

			set<SYMB*>& VarDef = umapVarUsed[vFuncSection[i].first->a->name].second;
			for (auto it = VarDef.begin(); it != VarDef.end(); it++)
			{
				SYMB* TempVar = create_var_symb(get_temp_var_name());
				sprintf(TempVar->name, "u%d", count_for_rename++);
				replaceItWithTemp(TempVar, *it, umapVarToStatement[vFuncSection[i].first->a->name][*it]);
				TAC* TempGL = create_a_node(TempVar, *it, nullptr, TAC_COPY);
				TAC* TempDecl = create_a_node(TempVar, nullptr, nullptr, TAC_VAR);
				TAC* TempLG = create_a_node(*it, TempVar, nullptr, TAC_COPY);
				joinTacNextDecl(vFuncSection[i].first, TempDecl);
				joinTacNextDecl(vFuncSection[i].first, TempGL);
				joinTacBeforeThem(umapFuncToRet[vFuncSection[i].first->a->name], TempLG);
			}
		}
	}
};
