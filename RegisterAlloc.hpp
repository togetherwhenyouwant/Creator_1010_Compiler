#pragma once
#include <math.h>
unordered_map<string, int> KeepRegisterMap;
void combineTwoMap(unordered_map<string, int>& destination, unordered_map<string, int>& source)
{
	for_each(source.begin(), source.end(), [=, &destination](pair<string, int> pairElem)
	{
		destination.insert(pairElem);
	});
	source.clear();
}

class RegisterStrategy
{
public:
	RegisterStrategy(void)
	{
		this->vRegList.push_back(2);
		this->vRegList.push_back(3);
		this->vRegList.push_back(4);
		this->vRegList.push_back(5);
		this->vRegList.push_back(6);
		this->vRegList.push_back(7);
		this->vRegList.push_back(8);
	}
	~RegisterStrategy(void)
	{
		//	cout << __FILE__ << __LINE__ << endl;
	}
	unordered_map<string, int>& allocFuncblockRegister(FunctionBlock& refFuncblock);

private:
	unordered_map<string, int> RegMalloc;
	vector<int> aBlockHadUsed;
	vector<vector<SsaSymb*>> aVarConflictGraph;
	FunctionBlock* refFuncblock;
	vector<int> vRegList;
	void colorGraph(SsaSymb* shouldBeColor);
	void createConflictGraphForFunction();
	void createConflictGrappForEachVar(SsaSymb* pVariable);
	void makeConflictGraphToRegisterAlloc(vector<SsaSymb*>&);
	vector<int> findBlockForStatementPrev(int distace);
	void liveInBlockOut(int, SsaSymb* pVariable);
	void liveInBlockIn(int, SsaSymb* pVariable);
	void liveAtStatementOut(SsaTac* pStatment, SsaSymb* pVariable);
	void liveAtStatementIn(SsaTac* pStatment, SsaSymb* pVariable);
	vector<SsaSymb*> getConflictSymbFromStatement(SsaTac* pStatement);
	void joinVariable2ConflictGraph(SsaSymb*, vector<SsaSymb*>&);
	void joinSingleVar2Graph(SsaSymb*, SsaSymb*);
	SsaSymb* findMostPotNode(vector<SsaSymb*>& mapForRegHadRecored);
	void getPotOfVar(vector<SsaSymb*>& mapForRegHadRecored, int num, pair<int, SsaSymb*>& oldPair);
	bool isHadUsed(SsaSymb* pVariable);
	void joinPrevConflict(SsaSymb* pvariable, SsaTac* start, int);
	unordered_map<string, uint> getvarValue(void);
	int calEachTacOfVarValue(int value);
	#define TEST
	#ifdef TEST 
	void remove_from_conflict(string var_name)
	{
		this->str_out.push_back(var_name);
		auto iter = find_if(aVarConflictGraph.begin(), aVarConflictGraph.end(), [&](vector<SsaSymb*>ptr) {
			return ptr[0]->name == var_name;
		});
		assert(iter != aVarConflictGraph.end());
		for (int i = 1; i<iter->size(); i++)
		{
			auto iter_temp = find_if(aVarConflictGraph.begin(), aVarConflictGraph.end(), [&](vector<SsaSymb*>ptr) {
				return ptr[0]->name == (*iter)[i]->name;
			});
			assert(iter_temp != aVarConflictGraph.end());

			auto iter_var = find_if(iter_temp->begin(), iter_temp->end(), [&](SsaSymb* ptr) {
				return ptr->name== var_name;
			});
			assert(iter_var != iter_temp->end());

			iter_temp->erase(iter_var);
		}
		iter->resize(1);
	}
	vector<string> str_out;
	void make_out_to_mem(void)
	{
		for_each(this->str_out.begin(), str_out.end(), [&,this](string name) {
			this->RegMalloc[name] = 20;
		});
	}
	void get_out_var_from_graph(void)
	{
		unordered_map<string, uint> keep= getvarValue();
		
		for_each(keep.begin(), keep.end(), [&](pair<string, uint> pair_var) 
		{
			if (pair_var.second == 0)
			{
				remove_from_conflict(pair_var.first);
			}
		});
	}
	#endif
	void sortConflictGraph(void)
	{
		unordered_map<string, uint> keep=getvarValue();
		sort(this->aVarConflictGraph.begin(),aVarConflictGraph.end(),[&](vector<SsaSymb*> &a,vector<SsaSymb*> &b)
		{
			return keep[a[0]->name] > keep[b[0]->name];
		});
	}
	void change_the_register(void)
	{
		vector<vector<string>> reg_to_malloc(100);
		unordered_map<string, uint> statement_value = getvarValue();
		for (auto ptr = statement_value.begin(); ptr != statement_value.end(); ptr++)
		{
			if (ptr->second == 0)
			{
				ptr->second = 1;
			}
		}
		for_each(RegMalloc.begin(), RegMalloc.end(), [&](pair<string, uint> pair_string)
		{
			reg_to_malloc[pair_string.second].push_back(pair_string.first);
		});
		vector<int> reg_to_ref(100);
		for (int i = 0; i < reg_to_malloc.size(); i++)
		{
			int count = 0;
			for (int j = 0; j < reg_to_malloc[i].size(); j++)
			{
				count += statement_value[reg_to_malloc[i][j]];
			}
			reg_to_ref[i] = count;
		}
		vector<pair<int, int>> reg_connect_value;
		for (int i = 0; i < reg_to_ref.size(); i++)
		{
			if (reg_to_ref[i] != 0)
			{
				reg_connect_value.push_back({ i,reg_to_ref[i] });
			}
		}
		sort(reg_connect_value.begin(), reg_connect_value.end(), [&](pair<int, int> a, pair<int, int> b)->bool
		{
			return a.second > b.second;
		});
		unordered_map<int, int> reg_to_new_reg;
		for (int i = 0; i < reg_connect_value.size(); i++)
		{
			reg_to_new_reg[reg_connect_value[i].first] = i + 4;
		}
		/*for_each(this->RegMalloc.begin(), this->RegMalloc.end(), [&](pair<const string, int>& pair_keep)
		{
			pair_keep.second = reg_to_new_reg[pair_keep.second];
		});*/
		for(auto iter=RegMalloc.begin();iter!=RegMalloc.end();iter++)
		{
			iter->second=reg_to_new_reg[iter->second];
		}
	}
	void joinOneVariableToGraph(SsaSymb* symbol)
	{
		for (int i = 0; i < this->aVarConflictGraph.size(); i++)
		{
			if (aVarConflictGraph[i][0] == symbol)
			{
				return;
			}
		}
		this->aVarConflictGraph.push_back({ symbol });
	}
	//vector<BasicBlock*> m_basicBlocks;                          //基本块集合
	BasicBlock* fetchBlockPointerFromID(int id)
	{
		vector<BasicBlock*> BasicBlockSet = this->refFuncblock->m_basicBlocks;
		class findBlock
		{
		private:
			const int id;
		public:
			findBlock(int id) : id(id)
			{
			}
			bool operator()(BasicBlock*& refBlockPoint)
			{
				return this->id == refBlockPoint->m_id;
			}
		};
		BasicBlock* pBlock = *find_if(BasicBlockSet.begin(), BasicBlockSet.end(), findBlock(id));
		return pBlock;
	}
	void pushForSymbExpInt(SsaSymb* pSymb, vector<SsaSymb*>& rVector)
	{
		if (pSymb == nullptr)
		{
			return;
		}
		if ((pSymb->type == SYM_VAR) && pSymb->name[0] != 'g' && isHadUsed(pSymb))
		{
			rVector.push_back(pSymb);
		}
	}
};
unordered_map<string, int>& RegisterStrategy::allocFuncblockRegister(FunctionBlock& refFuncblock)
{
	this->refFuncblock = &refFuncblock;
	this->createConflictGraphForFunction();
	vector<SsaSymb*> mapForRegHadRecored;
	this->get_out_var_from_graph();
	this->makeConflictGraphToRegisterAlloc(mapForRegHadRecored);
	this->change_the_register();
	this->make_out_to_mem();
#ifndef NO_PRINT
	unordered_map<string, uint> varValue = getvarValue();
	unordered_map<string, SsaSymb*>::iterator forEachVar;
	forEachVar = this->refFuncblock->m_uName2SsaSymbs.begin();
	for (; forEachVar != this->refFuncblock->m_uName2SsaSymbs.end(); forEachVar++)
	{
		cout << forEachVar->first << ": R";
		cout << this->RegMalloc[forEachVar->first] << 
		 "  value: " << varValue[forEachVar->first] << endl;
	}

	forEachVar = this->refFuncblock->m_tName2SsaSymbs.begin();
	for (; forEachVar != this->refFuncblock->m_tName2SsaSymbs.end(); forEachVar++)
	{
		cout << forEachVar->first << ": R";
		cout << this->RegMalloc[forEachVar->first] << 
		 "  value: " << varValue[forEachVar->first] << endl;
	}
	fflush(stdout);
#endif
	return this->RegMalloc;
}
void RegisterStrategy::createConflictGraphForFunction(void)
{
	unordered_map<string, SsaSymb*>::iterator forEachVar;
	forEachVar = this->refFuncblock->m_uName2SsaSymbs.begin();
	for (; forEachVar != this->refFuncblock->m_uName2SsaSymbs.end(); forEachVar++)
	{
		createConflictGrappForEachVar(forEachVar->second);
	}

	forEachVar = this->refFuncblock->m_tName2SsaSymbs.begin();
	for (; forEachVar != this->refFuncblock->m_tName2SsaSymbs.end(); forEachVar++)
	{
		createConflictGrappForEachVar(forEachVar->second);
	}
}
void RegisterStrategy::joinPrevConflict(SsaSymb* pvariable, SsaTac* start, int distace)
{
	SsaTac* end = nullptr;
	for_each(this->refFuncblock->m_basicBlocks.begin(), this->refFuncblock->m_basicBlocks.end(), [=, &end](BasicBlock* item)
	{
		if (item->m_id == start->blockId)
		{
			end = item->m_tacHead;
		}
	});
	if (end == nullptr || start == end || start == nullptr)
	{
		return;
	}
	vector<SsaSymb*> keepVar;
	vector<SsaSymb*>tempVar;
	while (start != end->prev)
	{
		tempVar.clear();
		tempVar = getConflictSymbFromStatement(start);
		if (start->type == TAC_INSERT)
		{
			if (tempVar.size() - 1 >= distace)
			{
				keepVar.push_back(tempVar[distace]);
			}
		}
		else
		{
			keepVar.insert(keepVar.end(), tempVar.begin(), tempVar.end());
		}
		start = start->prev;
	}
	this->joinVariable2ConflictGraph(pvariable, keepVar);
}
void RegisterStrategy::createConflictGrappForEachVar(SsaSymb* pVariable)
{

	this->joinOneVariableToGraph(pVariable);
	this->aBlockHadUsed.clear();
	int BlockPrev;
	UseSsaTac* pUseForVar = pVariable->useList->next;
	vector<SsaSymb*>* pVar2FuncSymb = nullptr;
	for (; pUseForVar; pUseForVar = pUseForVar->next)
	{
		pVar2FuncSymb = &pUseForVar->code->functionSymb;
		if (pUseForVar->code->type == TAC_INSERT)
		{

			int displace =
				distance(pVar2FuncSymb->begin(), find(pVar2FuncSymb->begin(), pVar2FuncSymb->end(), pVariable));
			assert(displace >= 0);
			BlockPrev = this->findBlockForStatementPrev(pUseForVar->code->blockId)[displace];
			joinPrevConflict(pVariable, pUseForVar->code, displace);
			this->liveInBlockOut(BlockPrev, pVariable);
			continue;
		}
		this->liveAtStatementIn(pUseForVar->code, pVariable);
	}
}
bool RegisterStrategy::isHadUsed(SsaSymb* pVariable)
{
	if (this->refFuncblock->m_uName2SsaSymbs.find(pVariable->name) != refFuncblock->m_uName2SsaSymbs.end())
	{
		return 1;
	}
	if (this->refFuncblock->m_tName2SsaSymbs.find(pVariable->name) != refFuncblock->m_tName2SsaSymbs.end())
	{
		return 1;
	}
	return 0;

}
void RegisterStrategy::liveInBlockOut(int iBlockID, SsaSymb* pVariable)
{
	class BlockEqID
	{
	private:
		int iBlockID;
	public:
		BlockEqID(int n) : iBlockID(n) { }
		bool operator()(BasicBlock* p)
		{
			return iBlockID == p->m_id;
		}

	};
	if (this->aBlockHadUsed.end() != find(aBlockHadUsed.begin(), aBlockHadUsed.end(), iBlockID))
	{
		return;
	}
	this->aBlockHadUsed.push_back(iBlockID);
	BasicBlock* pBlock =
		*find_if(this->refFuncblock->m_basicBlocks.begin(), refFuncblock->m_basicBlocks.end(), BlockEqID(iBlockID));
	if (!pBlock)
	{
		cout << "error: cannot find block ID:" << iBlockID << "in m_basicBlocks" << endl;
		exit(0);
	}
	if (!pBlock->m_tacHead)
	{
		this->liveInBlockIn(iBlockID, pVariable);
	}
	else
	{
		this->liveAtStatementOut(pBlock->m_tacTail, pVariable);
	}

}
void RegisterStrategy::liveInBlockIn(int iBlockID, SsaSymb* pVariable)
{
	assert(iBlockID >= 0);
	vector<uint>vBlockPrev = this->refFuncblock->m_backwardGraph[iBlockID];
	for_each(vBlockPrev.begin(), vBlockPrev.end(), [=](uint elem)
	{
		liveInBlockOut(elem, pVariable);
	});
}
void RegisterStrategy::liveAtStatementIn(SsaTac* pStatement, SsaSymb* pVariable)
{
	assert(pStatement);
	assert(pVariable);
	BasicBlock* pBlock = fetchBlockPointerFromID(pStatement->blockId);
	SsaTac* ptrBlockFrontTac = pBlock->m_tacHead;
	if (pStatement != ptrBlockFrontTac)
	{
		this->liveAtStatementOut(pStatement->prev, pVariable);
		return;
	}
	vector<int> aBlockPrev = this->findBlockForStatementPrev(pStatement->blockId);
	for (int i = 0; i < aBlockPrev.size(); i++)
	{
		this->liveInBlockOut(aBlockPrev[i], pVariable);
	}
}
vector<int> RegisterStrategy::findBlockForStatementPrev(int distace)
{
	vector<int> vKeep;
	assert(distace < this->refFuncblock->m_backwardGraph.size());
	vector<uint>::iterator it = this->refFuncblock->m_backwardGraph[distace].begin();
	for (; it != this->refFuncblock->m_backwardGraph[distace].end(); it++)
	{
		vKeep.push_back(*it);
	}
	return vKeep;
}
void RegisterStrategy::liveAtStatementOut(SsaTac* pStatement, SsaSymb* pVariable)
{
	assert(pStatement);
	assert(pVariable);
	if (pVariable->defPoint == pStatement)
	{
		return;
	}//lyj_0
	vector<SsaSymb*> aConflictSymb = this->getConflictSymbFromStatement(pStatement);
	auto iterConfSymb = find(aConflictSymb.begin(), aConflictSymb.end(), pVariable);
	if (aConflictSymb.end() != iterConfSymb && pStatement->type != TAC_INSERT)
	{
		aConflictSymb.erase(iterConfSymb);
		this->joinVariable2ConflictGraph(pVariable, aConflictSymb);
	}
	else
	{
		this->joinVariable2ConflictGraph(pVariable, aConflictSymb);
		this->liveAtStatementIn(pStatement, pVariable);
	}
	return;
}
vector<SsaSymb*>  RegisterStrategy::getConflictSymbFromStatement(SsaTac* pStatement)
{
	vector<SsaSymb*> vSymbol;
	if (pStatement->type == TAC_INSERT)
	{
		for_each(pStatement->functionSymb.begin(), pStatement->functionSymb.end(), [=, &vSymbol](SsaSymb* ptr)
		{
			if (isHadUsed(ptr))
			{
				pushForSymbExpInt(ptr, vSymbol);
			}
		});

		pushForSymbExpInt(pStatement->first, vSymbol);
	}
	if (pStatement->type <= TAC_MOD && pStatement->type >= TAC_ADD)
	{
		this->pushForSymbExpInt(pStatement->first, vSymbol);
		this->pushForSymbExpInt(pStatement->second, vSymbol);
		this->pushForSymbExpInt(pStatement->third, vSymbol);
	}
	if (pStatement->type <= TAC_IFGE && pStatement->type >= TAC_IFEQ)
	{
		this->pushForSymbExpInt(pStatement->first, vSymbol);
		this->pushForSymbExpInt(pStatement->third, vSymbol);
	}
	if (pStatement->type >= TAC_NEG && pStatement->type <= TAC_COPY)
	{
		this->pushForSymbExpInt(pStatement->first, vSymbol);
		this->pushForSymbExpInt(pStatement->second, vSymbol);
	}
	if (pStatement->type == TAC_IFZ || pStatement->type == TAC_LEA)
	{
		this->pushForSymbExpInt(pStatement->first, vSymbol);
	}
	if (pStatement->type == TAC_RETURN && pStatement->first)
	{
		this->pushForSymbExpInt(pStatement->first, vSymbol);
	}
	if (pStatement->type == TAC_ACTUAL || pStatement->type == TAC_FORMAL || pStatement->type == TAC_CALL)
	{
		this->pushForSymbExpInt(pStatement->first, vSymbol);
	}
	if (pStatement->type == TAC_ARR_L)
	{
		this->pushForSymbExpInt(pStatement->second, vSymbol);
		this->pushForSymbExpInt(pStatement->third, vSymbol);
	}
	if (pStatement->type == TAC_ARR_R)
	{
		this->pushForSymbExpInt(pStatement->first, vSymbol);
		this->pushForSymbExpInt(pStatement->third, vSymbol);
	}

	return vSymbol;
}
void RegisterStrategy::joinSingleVar2Graph(SsaSymb* left, SsaSymb* right)
{
	//for left to right
	auto iterLeft = find_if(this->aVarConflictGraph.begin(), aVarConflictGraph.end(),
		[=](vector<SsaSymb*> value)-> bool
	{
		return value[0] == left;
	});
	if (iterLeft != aVarConflictGraph.end())
	{
		auto iterRight = find(iterLeft->begin(), iterLeft->end(), right);
		if (iterRight == iterLeft->end())
		{
			iterLeft->push_back(right);
		}
	}
	else
	{
		this->aVarConflictGraph.push_back({ left });
		if (left != right)
		{
			this->aVarConflictGraph.back().push_back(right);
		}
	}
	//for right to left
	auto iterRight = find_if(this->aVarConflictGraph.begin(), aVarConflictGraph.end(),
		[=](vector<SsaSymb*> value)-> bool
	{
		return value[0] == right;
	});
	if (iterRight != aVarConflictGraph.end())
	{
		auto iterLeft = find(iterRight->begin(), iterRight->end(), left);
		if (iterLeft == iterRight->end())
		{
			iterRight->push_back(left);
		}
	}
	else
	{
		this->aVarConflictGraph.push_back({ right });
		if (left != right)
		{
			this->aVarConflictGraph.back().push_back(left);
		}

	}

}
void RegisterStrategy::joinVariable2ConflictGraph(SsaSymb* pSymbol, vector<SsaSymb*>& vSymbol)
{
	auto iterVectorSymb = vSymbol.begin();
	for (; iterVectorSymb != vSymbol.end(); iterVectorSymb++)
	{
		this->joinSingleVar2Graph(pSymbol, *iterVectorSymb);
	}
}
void RegisterStrategy::getPotOfVar(vector<SsaSymb*>& mapForRegHadRecored, int num, pair<int, SsaSymb*>& oldPair)
{
	pair<int, SsaSymb*> pairKeep(0, aVarConflictGraph[num][0]);
	auto iterForSymbol = find(mapForRegHadRecored.begin(), mapForRegHadRecored.end(), aVarConflictGraph[num][0]);
	if (iterForSymbol != mapForRegHadRecored.end())
	{
		return;
	}
	for (int i = 0; i < aVarConflictGraph[num].size(); i++)
	{
		if (mapForRegHadRecored.end() != find(mapForRegHadRecored.begin(), mapForRegHadRecored.end(), aVarConflictGraph[num][i]));
		{
			pairKeep.first++;
		}
	}
	if (oldPair.first < pairKeep.first)
	{
		oldPair = pairKeep;
		return;
	}
	unordered_map<string, uint> var_value = getvarValue();
	if (oldPair.first == pairKeep.first && var_value[pairKeep.second->name] > var_value[oldPair.second->name])
	{
		oldPair = pairKeep;
	}
}
SsaSymb* RegisterStrategy::findMostPotNode(vector<SsaSymb*>& mapForRegHadRecored)
{
	pair<int, SsaSymb*> pairKeep(0, nullptr);
	for (int i = 0; i < this->aVarConflictGraph.size(); i++)
	{
		this->getPotOfVar(mapForRegHadRecored, i, pairKeep);
	}
	return pairKeep.second;
}
void RegisterStrategy::makeConflictGraphToRegisterAlloc(vector<SsaSymb*>& mapForRegHadRecored)
{
	static int counter = 0;
	vector<vector<SsaSymb*>> ::iterator iterConfGraph = this->aVarConflictGraph.begin();
	if (!mapForRegHadRecored.size())
	{
		if (aVarConflictGraph.empty())
		{
			return;
		}
		mapForRegHadRecored.push_back(this->aVarConflictGraph[0][0]);
		this->makeConflictGraphToRegisterAlloc(mapForRegHadRecored);
		colorGraph(mapForRegHadRecored[0]);
		return;
	}
	SsaSymb* keep = this->findMostPotNode(mapForRegHadRecored);
	for (; keep != nullptr; keep = this->findMostPotNode(mapForRegHadRecored))
	{
		counter++;
		mapForRegHadRecored.push_back(keep);
	}
	for (; counter; counter--)
	{
		colorGraph(mapForRegHadRecored[counter]);
	}
}
void RegisterStrategy::colorGraph(SsaSymb* shouldBeColor)
{
	bool keep = true;
	assert(this->RegMalloc.count(shouldBeColor->name) != true);
	int i = 0;
	for (; i < this->aVarConflictGraph.size(); i++)
	{
		if (shouldBeColor == aVarConflictGraph[i][0])
		{
			break;
		}
	}
	assert(i != this->aVarConflictGraph.size());
	unordered_set<int> keepRegMalloc;
	for (int j = 0; j < aVarConflictGraph[i].size(); j++)
	{
		if (this->RegMalloc.count(aVarConflictGraph[i][j]->name))
		{
			keepRegMalloc.insert(this->RegMalloc[aVarConflictGraph[i][j]->name]);
		}
	}
	int k;
	for (k = 4; k <= keepRegMalloc.size() + 4; k++)
	{
		if (!keepRegMalloc.count(k))
		{
			this->RegMalloc[shouldBeColor->name] = k;
			break;
		}
	}
	assert(k <= keepRegMalloc.size() + 4);
}

unordered_map<string, uint> RegisterStrategy::getvarValue(void)
{
	unordered_map<string, uint> varValue;
	varValue.clear();
	unordered_map<string, SsaSymb*>& uName2SsaSymbs = refFuncblock->getUName2SsaSymbs();
	unordered_map<string, SsaSymb*>& tName2SsaSymbs = refFuncblock->getTName2SsaSymbs();
	unordered_map<string, SsaSymb*> name2SsaSymbs;
	name2SsaSymbs.clear();
	unordered_map<string, SsaSymb*>::iterator it;
	//给name2SsaSymbs初始化
	for (it = uName2SsaSymbs.begin(); it != uName2SsaSymbs.end(); it++)
		name2SsaSymbs[it->first] = it->second;
	for (it = tName2SsaSymbs.begin(); it != tName2SsaSymbs.end(); it++)
		name2SsaSymbs[it->first] = it->second;
	//给varValue初始化
	for (it = name2SsaSymbs.begin(); it != name2SsaSymbs.end(); it++)
		varValue[it->first] = 0;
	//对每个变量进行遍历
	for (it = name2SsaSymbs.begin(); it != name2SsaSymbs.end(); it++)
	{
		string varName = it->first;
		SsaSymb* varSymb = it->second;
		UseSsaTac* useTac = varSymb->useList;
		while (useTac->next != NULL)
		{
			SsaTac* codeTac = useTac->next->code;
			int value = codeTac->priority;
			varValue[varName] += calEachTacOfVarValue(value);
			useTac = useTac->next;
		}
	}
	return varValue;
}
int RegisterStrategy::calEachTacOfVarValue(int value)
{
	// return value * 200;
	int varValue = value;
	if(varValue >= 5)varValue = 5;
	return pow(25,varValue);
}