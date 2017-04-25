#pragma once
#include <core/Definitions.h>
#include <core/FunctionDispatch.h>
#include <runtime/ASTVisitor.h>
#include <parser/AST/AST.h>
#include <runtime/Runtime.h>
#include <runtime/Stack.h>

#include <unordered_map>
#include <array>


class Runtime : public ASTVisitor {
public:
	Runtime(uint32 stackSize) : stack(stackSize) {}

	virtual void visit(AST::FunctionDeclaration&) override;
	virtual void visit(AST::VariableDeclaration&) override;
	virtual void visit(AST::Block&) override;
	virtual void visit(AST::BinaryOperation&) override;
	virtual void visit(AST::Constant&) override;
	virtual void visit(AST::FunctionCall&) override;

	void eval(AST::AbstractSyntaxTree& ast);
	void call(std::string function);

	
	int getReturnRegister() { return registers[0]; }
	void setReturnRegister(int i) { registers[0] = i; }

	Stack stack;
private:
	std::array<int, 1> registers;

	std::unordered_map<std::string, uptr<AST::Block>> noxFunctions;
	std::unordered_map<std::string, uptr<FunctionDispatch_Base>> nativeFunctions;
};