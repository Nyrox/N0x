#include "Runtime.h"
#include <iostream>

using namespace AST;

void Runtime::eval(AbstractSyntaxTree& ast) {
	for (auto& it : ast.nodes) {
		it->visit(*this);
	}
}

void Runtime::call(std::string identifier) {
	this->noxFunctions[identifier]->visit(*this);
}

void Runtime::visit(AST::FunctionDeclaration& node) {
	std::cout << "Declaring function: " << node.identifier << "\n";

	noxFunctions[node.identifier] = std::move(node.block);
}

void Runtime::visit(AST::FunctionCall& node) {
	std::cout << "Calling function: " << node.identifier << "\n";

	if (noxFunctions.find(node.identifier) != noxFunctions.end()) {
		noxFunctions[node.identifier]->visit(*this);
	}
	else if(nativeFunctions.find(node.identifier) != nativeFunctions.end()) {
		nativeFunctions[node.identifier]->invoke(nullptr);
	}
}

void Runtime::visit(AST::VariableDeclaration& node) {
	std::cout << "Declaring variable: " << node.identifier << "\n";
	
	if (node.initializer) {
		node.initializer->visit(*this);
		stack.push(getReturnRegister());
	}
	else {
		stack.push(0);
	}
}

void Runtime::visit(AST::Block& node) {
	std::cout << "Beginning new block." << "\n";

	for (auto& it : node.statements) {
		it->visit(*this);
	}

	std::cout << "Ending block." << "\n";
}

void Runtime::visit(AST::Constant& node) {
	std::cout << "Encountered constant: " << node.value << "\n";

	setReturnRegister(node.value);
}

void Runtime::visit(AST::BinaryOperation& node) {
	std::cout << "Encountered binary operation: " << node.op << "\n";

	node.left->visit(*this);
	int left = getReturnRegister();

	node.right->visit(*this);
	int right = getReturnRegister();

	switch (node.op) {
	case PLUS:
		setReturnRegister(left + right);
		break;
	case MINUS:
		setReturnRegister(left - right);
		break;
	case SLASH:
		setReturnRegister(left / right);
		break;
	case STAR:
		setReturnRegister(left * right);
		break;
	}
}