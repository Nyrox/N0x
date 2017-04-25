#pragma once
#include <parser/AST/ASTNode.h>

class ASTVisitor {
public:
	virtual void visit(AST::Block&) = 0;
	virtual void visit(AST::FunctionDeclaration&) = 0;
	virtual void visit(AST::VariableDeclaration&) = 0;
	virtual void visit(AST::BinaryOperation&) = 0;
	virtual void visit(AST::Constant&) = 0;
	virtual void visit(AST::FunctionCall&) = 0;
};
