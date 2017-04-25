#include "Runtime.h"
#include <iostream>

using namespace AST;

void Runtime::eval(AbstractSyntaxTree& ast) {
	for (auto& it : ast.nodes) {
		it->visit(*this);
	}
}

void Runtime::visit(AST::FunctionDeclaration& node) {
	std::cout << "Declaring function: " << node.identifier << "\n";

	node.block->visit(*this);
}

void Runtime::visit(AST::VariableDeclaration& node) {
	std::cout << "Declaring variable: " << node.identifier << "\n";
}

void Runtime::visit(AST::Block& node) {
	std::cout << "Beginning new block." << "\n";

	for (auto& it : node.statements) {
		it->visit(*this);
	}

	std::cout << "Ending block." << "\n";
}