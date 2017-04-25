#include "Stack.h"

Stack::Stack(uint32 stackSize) {
	stackPointer = (char*)malloc(stackSize);
	stackEnd = stackPointer;
	stackFrames.push(stackPointer);
}

Stack::~Stack() {
	delete stackPointer;
}