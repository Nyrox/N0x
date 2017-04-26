#pragma once
#include <core/Definitions.h>
#include <core/FunctionDispatch.h>
#include <runtime/ASTVisitor.h>
#include <parser/AST/AST.h>
#include <runtime/Runtime.h>
#include <runtime/Stack.h>
#include <runtime/BoxedValue.h>

#include <unordered_map>
#include <array>


class Runtime : public ASTVisitor {
public:
	Runtime(uint32 stackSize) : stack(stackSize) {}

	virtual void visit(AST::FunctionDeclaration&) override;
	virtual void visit(AST::Variable&) override;
	virtual void visit(AST::VariableDeclaration&) override;
	virtual void visit(AST::Block&) override;
	virtual void visit(AST::BinaryOperation&) override;
	virtual void visit(AST::Constant&) override;
	virtual void visit(AST::FunctionCall&) override;
	virtual void visit(AST::FunctionReturn&) override;
	virtual void visit(AST::Conditional&) override;

	void eval(AST::AbstractSyntaxTree& ast);
	void call(std::string function);

	void registerNativeFunction(std::string identifier, uptr<FunctionDispatch_Base> fun) {
		nativeFunctions[identifier] = std::move(fun);
	}

	BoxedValue getReturnRegister() { return registers[0]; }
	void setReturnRegister(BoxedValue i) { registers[0] = i; }

	enum { NIL = 0, RETURNING } unwindFlag;

	Stack stack;
private:
	std::array<BoxedValue, 1> registers;

	std::stack<std::unordered_map<std::string, uint32>> symbolTable;
	std::unordered_map<std::string, AST::FunctionDeclaration*> noxFunctions;
	std::unordered_map<std::string, uptr<FunctionDispatch_Base>> nativeFunctions;
};