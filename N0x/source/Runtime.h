#pragma once
#include <core/Definitions.h>
#include <string>
#include <unordered_map>
#include <array>
#include <stack>
#include <core/FunctionDispatch.h>

class Expr;
class Runtime;

class Block {
public:
	Block() = default;
	Block(Block& other) = delete; 
	Block(Block&& other) = default;
	Block& operator=(Block&& other) = default;

	friend class Interpreter;

	void execute(Runtime& runtime) {
		for (auto& it : expressions) {
			it->eval(runtime, *this);
		}
	}

	void defineVariable(std::string identifier, int initial = 0) {
		variables[identifier] = initial;
	}

	int& getVariable(std::string identifier) {
		return variables[identifier];
	}
private:
	std::vector<uptr<Expr>> expressions;
	std::unordered_map<std::string, int> variables;
};

class Runtime {
public:
	friend class Interpreter;

	explicit Runtime() {
		stack = new uint32[255];
		stackFrames.push(stack);
		stackEnd = stack;
	}

	~Runtime() {
		delete stack;
	}

	// Pushes a variable on the stack
	template<class T>
	void push(T var) {
		*(T*)(stackEnd) = var;
		stackEnd += sizeof(T);
	}

	// Get's a variable from an offset to the current stack frame
	template<class T>
	T getFromCurrentStackFrame(uint32 offset) {
		return *(T*)(stackFrames.top() + offset);
	}

	void pushStackFrame() {
		stackFrames.push(stackEnd);
	}

	void popStackFrame() {
		stackEnd = stackFrames.top();
		stackFrames.pop();		
	}

	enum class Registers : std::size_t { RETURN, EAX, EBX, ECX, EDX, EEX, EFX };

	int& getRegister(Registers i) { return registers[(std::size_t)i]; }
	void call(std::string t_identifier) {
		if (functions.find(t_identifier) != functions.end()) {
			functions[t_identifier].execute(*this);
		}
		else if(nativeFunctions.find(t_identifier) != nativeFunctions.end()) {
			nativeFunctions[t_identifier]->invoke(nullptr);
		}
	}

	void mov(int val, Registers r) {
		registers[(std::size_t)r] = val;
	}


	void registerNativeFunction(std::string identifier, std::unique_ptr<FunctionDispatch_Base>&& t_ptr) {
		nativeFunctions[identifier] = std::move(t_ptr);
	}

	uint32* stack;
	std::stack<uint32*> stackFrames;
	uint32* stackEnd;
private:
	uptr<Expr> entryPoint = nullptr;
	std::array<int, 8> registers;
	std::unordered_map<std::string, Block> functions;
	std::unordered_map<std::string, std::unique_ptr<FunctionDispatch_Base>> nativeFunctions;
	
};