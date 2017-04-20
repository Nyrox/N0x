#pragma once
#include <core/Definitions.h>
#include <string>
#include <unordered_map>
#include <array>

class Expr;

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

	enum class Registers : std::size_t { RETURN, EAX };

	int& getRegister(Registers i) { return registers[(std::size_t)i]; }
	void execute() {
		functions["main"].execute(*this);
	}

	void call(std::string t_identifier) {
		functions[t_identifier].execute(*this);
	}

	void mov(int val, Registers r) {
		registers[(std::size_t)r] = val;
	}

	
protected:
	

private:	
	uptr<Expr> entryPoint = nullptr;
	std::array<int, 8> registers;
	std::unordered_map<std::string, Block> functions;
};