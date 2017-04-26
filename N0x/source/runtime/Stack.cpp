#include "Stack.h"
#include <iostream>

Stack::Stack(uint32 stackSize) {
	stackPointer = (char*)malloc(stackSize);
	stackEnd = stackPointer;
	stackFrames.push(stackPointer);
	
}

Stack::~Stack() {
	delete stackPointer;
}

uint32 Stack::getCurrentStackFrameOffset() {
	return (uint32)(stackEnd - stackFrames.top());
}

void Stack::dump() {
	for (uint32 i = 0; i < stackEnd - stackPointer; i += 4) {
		std::cout << *(int*)(stackPointer + i) << ", ";
	}

	std::cout << "\n";
}