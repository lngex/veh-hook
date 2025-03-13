#include "pch.h"
#include <thread>
#include	"HardwareHook.h"
#include "TlHelp32.h"

void Dr0::hook(CONTEXT* threadContext) {
	this->oldValue = threadContext->Dr0;
	threadContext->Dr0 = this->address;
}
void Dr0::unHook(CONTEXT* threadContext) {
	threadContext->Dr0 = this->oldValue;
}

void Dr1::hook(CONTEXT* threadContext) {
	this->oldValue = threadContext->Dr1;
	threadContext->Dr1 = this->address;
}
void Dr1::unHook(CONTEXT* threadContext) {
	threadContext->Dr1 = this->oldValue;
}

void Dr2::hook(CONTEXT* threadContext) {
	this->oldValue = threadContext->Dr2;
	threadContext->Dr2 = this->address;
}
void Dr2::unHook(CONTEXT* threadContext) {
	threadContext->Dr2 = this->oldValue;
}

void Dr3::hook(CONTEXT* threadContext) {
	this->oldValue = threadContext->Dr3;
	threadContext->Dr3 = this->address;
}
void Dr3::unHook(CONTEXT* threadContext) {
	threadContext->Dr3 = this->oldValue;
}

void Dr7::hook(CONTEXT* threadContext) {
	this->oldValue = threadContext->Dr7;
	threadContext->Dr7 = this->address;
}
void Dr7::unHook(CONTEXT* threadContext) {
	threadContext->Dr7 = this->oldValue;
}


// 开始HOOK
bool Hook::begin() {
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	// 获取系统中所有线程的快照
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	// 初始化结构体大小
	te32.dwSize = sizeof(THREADENTRY32);
	// 检索第一个线程的信息，以便进行迭代
	if (!Thread32First(hThreadSnap, &te32)) {
		CloseHandle(hThreadSnap);
		return false;
	}
	if (this->handle == NULL)
	{
		// 添加回调函数
		this->handle = AddVectoredExceptionHandler(0, this->func);
	}
	DWORD currentProcessId = GetCurrentProcessId();
	do {
		// 如果该线程属于目标进程，并且线程id不是排除的线程，则进行hook
		if (te32.th32OwnerProcessID == currentProcessId 
			&& std::find(this->threadIds.begin(), this->threadIds.end(), te32.th32ThreadID)==this->threadIds.end()
			&& GetCurrentThreadId() != te32.th32ThreadID) {
			HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
			if (threadHandle == NULL)
			{
				// 线程句柄无效则不打开
				break;
			}
			// 挂起线程
			SuspendThread(threadHandle);
			// 获取上下文
			CONTEXT context;
			context.ContextFlags = CONTEXT_ALL;
			if (GetThreadContext(threadHandle, &context))
			{
				for (auto dr : this->regists) {
					if (this->status)
					{
						dr->unHook(&context);
					}
					else
					{
						dr->hook(&context);
					}
				}
				if (this->status) {
					this->dr7.unHook(&context);
				}
				else {
					this->dr7.hook(&context);
				}
				SetThreadContext(threadHandle, &context);
			}
			ResumeThread(threadHandle);
			CloseHandle(threadHandle);
		}
	} while (Thread32Next(hThreadSnap, &te32));
	// 关闭快照句柄
	CloseHandle(hThreadSnap);
	this->status = !this->status;
	return true;
}

// 结束HOOK
void Hook::end() {
	if (this->status)
	{
		// 移除寄存器标志
		this->begin();
	}
	// 移除回调
	if (this->handle != NULL)
	{
		RemoveVectoredExceptionHandler(this->handle);
		this->handle = NULL;
	}
}