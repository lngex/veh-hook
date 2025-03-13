#ifndef HARDWARE_HOOK_H
#define HARDWARE_HOOK_H

// 添加要在此处预编译的标头
#include "framework.h"
#include <thread>
#include <vector>

typedef size_t usize;
// 寄存器
class Register
{
protected:
	usize address;
	usize oldValue;
public:
	Register(usize address) :address(address) {};
	// hook时必须将目标寄存器的值存入oldValue，将address存入目标寄存器
	virtual void hook(CONTEXT* threadContext) = 0;
	// unhook则将目标寄存器还原
	virtual void unHook(CONTEXT* threadContext) = 0;
	~Register() {};
};

class Dr0 :public Register
{
public:
	Dr0(usize address, usize oldValue) :Register(address) {};
	void hook(CONTEXT* threadContext);
	void unHook(CONTEXT* threadContext);
};

class Dr1 :public Register
{
public:
	Dr1(usize address, usize oldValue) :Register(address) {};
	void hook(CONTEXT* threadContext);
	void unHook(CONTEXT* threadContext);
};

class Dr2 :public Register
{
public:
	Dr2(usize address, usize oldValue) :Register(address) {};
	void hook(CONTEXT* threadContext);
	void unHook(CONTEXT* threadContext);
};

class Dr3 :public Register
{
public:
	Dr3(usize address, usize oldValue) :Register(address) {};
	void hook(CONTEXT* threadContext);
	void unHook(CONTEXT* threadContext);
};

class Dr7 :public Register
{
public:
	Dr7(usize address, usize oldValue) :Register(address) {};
	void hook(CONTEXT* threadContext);
	void unHook(CONTEXT* threadContext);
};

class Hook
{
private:
	// 寄存器
	std::vector<Register*> regists;
	// 标志寄存器
	Dr7 dr7;
	// 向量句柄
	PVOID handle;
	// 函数异常处理函数
	PVECTORED_EXCEPTION_HANDLER func;
	// 不允许HOOK的线程id
	std::vector<DWORD> threadIds;
public:
	bool status;
	Hook(std::vector<Register*> regists, Dr7 dr7, PVECTORED_EXCEPTION_HANDLER func, std::vector<DWORD> threadIds) :regists(regists), dr7(dr7), func(func), threadIds(threadIds) {
		this->status = false;
		this->handle = NULL;
	};
	// 开始HOOK
	bool begin();
	// 结束HOOK
	void end();
	~Hook() {
		for (Register* reg : regists)
		{
			delete reg;
		}
	};
};
#endif //PCH_H
