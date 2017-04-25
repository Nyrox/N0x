#include "ASTNode.h"
#include <runtime/ASTVisitor.h>

using namespace AST;

void Block::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}

void FunctionDeclaration::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}

void VariableDeclaration::visit(ASTVisitor& visitor) {
	visitor.visit(*this);
}
