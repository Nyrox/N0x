#include "expr.h"
#include <Runtime.h>


/* If */
void Condition::eval(Runtime& runtime, Block& scope) {
	condition->eval(runtime, scope);

	if (runtime.getRegister(Runtime::Registers::RETURN)) {
		block.execute(runtime);
	}
	else if (alternate != nullptr) {
		alternate->eval(runtime, scope);
	}
}

/* Block */

void BlockExpr::eval(Runtime& runtime, Block& scope) {
	block.execute(runtime);
}

/* Var Decl */
VarDecl::VarDecl(std::string t_identifier, std::unique_ptr<Expr>&& init) : identifier(t_identifier), initializer(std::move(init)) {

}

void VarDecl::eval(Runtime& runtime, Block& scope) {
	initializer->eval(runtime, scope);
	scope.defineVariable(identifier, runtime.getRegister(Runtime::Registers::RETURN));
}


/* Func Call */
FuncCall::FuncCall(std::string t_identifier) : identifier(t_identifier) {

}

void FuncCall::eval(Runtime& runtime, Block& block) {
	runtime.call(identifier);
}


/* Variable */
Variable::Variable(std::string t_identifier) : identifier(t_identifier) {

}

void Variable::eval(Runtime& runtime, Block& block) {
	runtime.mov(block.getVariable(identifier), Runtime::Registers::RETURN);
}


/* Unary */
Unary::Unary(Token& t_token, uptr<Expr>&& t_operand) : token(t_token), operand(std::move(t_operand)) {

}

void Unary::eval(Runtime& runtime, Block& scope) {
	operand->eval(runtime, scope);
	int value = runtime.getRegister(Runtime::Registers::RETURN);

	if (token.type == MINUS) value = -value;
	else if (token.type == PLUS) value = +value;
	else if (token.type == PRINT) {
		std::cout << std::to_string(value) << "\n";
	}
	else throw std::exception();

	runtime.mov(value, Runtime::Registers::RETURN);
}


/* Binary */
Binary::Binary(uptr<Expr>&& t_left, Token t_token, uptr<Expr>&& t_right) : left(std::move(t_left)), right(std::move(t_right)), token(t_token) { }

void Binary::eval(Runtime& runtime, Block& block) {
	left->eval(runtime, block);
	int left = runtime.getRegister(Runtime::Registers::RETURN);
	right->eval(runtime, block);
	int right = runtime.getRegister(Runtime::Registers::RETURN);

	int out = 0;

	if (token.type == STAR) out = left * right;
	else if (token.type == SLASH) out = left / right;
	else if (token.type == PLUS) out = left + right;
	else if (token.type == MINUS) out = left - right;
	else { throw std::exception(); }

	runtime.mov(out, Runtime::Registers::RETURN);
}


/* Constants */
void Constant::eval(Runtime& runtime, Block& scope) {
	runtime.mov(value, Runtime::Registers::RETURN);
}