#include "ASTNode.h"
#include <runtime/ASTVisitor.h>

using namespace AST;

void Block::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}

void FunctionDeclaration::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}

void FunctionCall::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}

void FunctionReturn::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}

void Conditional::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}

void Variable::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}

void VariableDeclaration::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}

void BinaryOperation::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}

void Constant::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}