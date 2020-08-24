#ifndef GLOBALPOLICYEXECUTOR_HPP
#define GLOBALPOLICYEXECUTOR_HPP
#include "functionBlock.hpp"
#include "funcPropertyGetter.hpp"
//全局优化策略器
//优化器基本功能
//1. 告知全局优化器是否有优化
//2. 启动优化器进行优化

//优化所需要的信息
//1. 各级符号表（全局[变量+数组]、局部[变量+数组]、临时）
//2. 基本块（SSA代码）和控制流信息

//优化器能做的事情
//1. 删除或者修改代码
//2. 删除和添加基本块
//3. 修改控制流信息
//4. 修改符号表
class GlobalPolicyExecutor
{
protected:
    FunctionBlock* m_block;
    string m_name;
public:
    GlobalPolicyExecutor();
    ~GlobalPolicyExecutor();

    virtual void printInfoOfOptimizer(void) = 0;        //打印优化器的信息
    //true 表示有优化的机会，false表示没有优化的机会
    virtual bool runOptimizer(FunctionBlock* block,
        FuncPropertyGetter* funcPropertyGetter) = 0;//运行优化器
    string getOptimizerName(void){return m_name;};
};

GlobalPolicyExecutor::GlobalPolicyExecutor()
{

}

GlobalPolicyExecutor::~GlobalPolicyExecutor()
{
    
}


#endif