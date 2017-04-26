#include "Runtime.h"
#include <iostream>

using namespace AST;

void Runtime::eval(AbstractSyntaxTree& ast) {
	for (auto& it : ast.nodes) {
		it->visit(*this);
	}
}

void Runtime::call(std::string identifier) {
	FunctionCall(identifier).visit(*this);
}

void Runtime::visit(AST::FunctionDeclaration& node) {
	noxFunctions[node.identifier] = &node;
}

void Runtime::visit(AST::FunctionCall& node) {
	if (noxFunctions.find(node.identifier) != noxFunctions.end()) {
		auto& func = noxFunctions[node.identifier];

		if (func->params.size() != node.params.size()) {
			throw "Function mismatch";
		}

		// Buffer all the parameters
		std::vector<BoxedValue> params;
		for (auto& it : node.params) {
			it->visit(*this);
			params.push_back(getReturnRegister());
		}

		symbolTable.push({});
		stack.pushStackFrame();

		for (int i = 0; i < node.params.size(); i++) {
			symbolTable.top()[func->params[i]] = stack.getCurrentStackFrameOffset();
			stack.push<BoxedValue>(params[i]);
		}

		func->block->visit(*this);
		if (unwindFlag == RETURNING) unwindFlag = NIL;

		symbolTable.pop();
		stack.popStackFrame();
	}
	else if (nativeFunctions.find(node.identifier) != nativeFunctions.end()) {
		auto& func = nativeFunctions[node.identifier];

		if (func->getParamCount() != node.params.size()) {
			throw "Function mismatch";
		}

		std::vector<BoxedValue::PureValue> params;
		for (auto& it : node.params) {
			it->visit(*this);
			params.push_back(getReturnRegister());
		}
		
		nativeFunctions[node.identifier]->invoke((void*)params.data());
	}
}

void Runtime::visit(AST::FunctionReturn& node) {
	node.expression->visit(*this);
	unwindFlag = RETURNING;
}

void Runtime::visit(AST::Conditional& node) {
	node.condition->visit(*this);

	if (getReturnRegister()) {
		node.consequent->visit(*this);
	}
	else {
		node.alternate->visit(*this);
	}
}

void Runtime::visit(AST::Variable& node) {
	setReturnRegister(*stack.getRelativeToStackFrame<int>(symbolTable.top()[node.identifier]));
}

void Runtime::visit(AST::VariableDeclaration& node) {
	symbolTable.top()[node.identifier] = stack.getCurrentStackFrameOffset();

	if (node.initializer) {
		node.initializer->visit(*this);
		stack.push(getReturnRegister());
		//std::cout << "Declaring variable " << node.identifier << " with: " << getReturnRegister() << "\n";
	}
	else {
		stack.push(0);
	}

	
}

void Runtime::visit(AST::Block& node) {
	for (auto& it : node.statements) {
		if (unwindFlag == RETURNING) return;
		it->visit(*this);
	}
}

void Runtime::visit(AST::Constant& node) {
	setReturnRegister(node.value);
}

void Runtime::visit(AST::BinaryOperation& node) {
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
	case LESS:
		setReturnRegister(left < right);
		break;
	case GREATER:
		setReturnRegister(left > right);
		break;
	case GREATER_EQUAL:
		setReturnRegister(left >= right);
		break;
	case LESS_EQUAL:
		setReturnRegister(left <= right);
		break;
	}
}