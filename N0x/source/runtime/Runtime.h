#pragma once
#include <core/Definitions.h>
#include <runtime/ASTVisitor.h>
#include <parser/AST/AST.h>

class Runtime : public ASTVisitor {
public:

	virtual void visit(AST::FunctionDeclaration&) override;
	virtual void visit(AST::VariableDeclaration&) override;
	virtual void visit(AST::Block&) override;

	void eval(AST::AbstractSyntaxTree& ast);
	void call(std::string function);

private:

};