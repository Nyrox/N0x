#pragma once
#include <core/Definitions.h>
#include <stack>

class SStack {
public:
	SStack(uint32 stackSize);
	~SStack();

	template<class T>
	void push(T val) {
		*(T*)(stackEnd) = val;
		stackEnd += sizeof(T);
	}

	void pushStackFrame() {
		stackFrames.push(stackEnd);
	}

	void popStackFrame() {
		stackFrames.pop();
		stackEnd = stackFrames.top();
	}

	template<class T>
	T* getRelativeToStackFrame(uint32 offset) {
		return (T*)(stackFrames.top() + offset);
	}

	void dump();
	uint32 getCurrentStackFrameOffset();
	
private:
	char* stackPointer;
	char* stackEnd;
	std::stack<char*> stackFrames;

};
