#include "Stack.h"
#include <iostream>

SStack::SStack(uint32 stackSize) {
	stackPointer = (char*)malloc(stackSize);
	stackEnd = stackPointer;
	stackFrames.push(stackPointer);
	
}

SStack::~SStack() {
	delete stackPointer;
}

uint32 SStack::getCurrentStackFrameOffset() {
	return (uint32)(stackEnd - stackFrames.top());
}

void SStack::dump() {
	for (uint32 i = 0; i < stackEnd - stackPointer; i += 4) {
		std::cout << *(int*)(stackPointer + i) << ", ";
	}

	std::cout << "\n";
}