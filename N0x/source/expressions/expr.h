#pragma once
#include <Scanner.h>
#include <core/Definitions.h>
#include <Runtime.h>


/* 
Defines an expression in the AST which is executable. 
Does NOT represent a definition
*/
class Expr {
public:
	Expr() { }

	virtual void eval(Runtime&, Block&) = 0;
};

class BlockExpr : public Expr {
public:
	BlockExpr(Block&& t_block) : block(std::move(t_block)) { }

	void eval(Runtime& runtime, Block& scope) override;

	Block block;
};

class Condition : public Expr {
public:
	Condition(uptr<Expr>&& t_condition, Block&& t_block, uptr<Expr>&& t_alternate = nullptr) : alternate(std::move(t_alternate)), condition(std::move(t_condition)), block(std::move(t_block)) { }

	void eval(Runtime& runtime, Block& scope) override;

	uptr<Expr> condition = nullptr;
	Block block;
	uptr<Expr> alternate = nullptr;
};

class VarDecl : public Expr {
public:
	VarDecl(std::string t_identifier, std::unique_ptr<Expr>&& init = nullptr);

	void eval(Runtime& runtime, Block& scope) override;

	uptr<Expr> initializer = nullptr;;
	std::string identifier;
};

class FuncCall : public Expr {
public:
	FuncCall(std::string t_identifier);

	void eval(Runtime&, Block&) override;

	std::string identifier;
};

class Variable : public Expr {
public:
	Variable(std::string t_identifier);
	void eval(Runtime& runtime, Block& block);

	std::string identifier;
};

class Unary : public Expr {
public:
	Unary(Token& t_token, uptr<Expr>&& t_operand);

	void eval(Runtime& runtime, Block& scope) override;

	Token token;
	uptr<Expr> operand;
};

class Binary : public Expr {
public:
	Binary(uptr<Expr>&& t_left, Token t_token, uptr<Expr>&& t_right);

	void eval(Runtime&, Block&) override;

	Token token;
	uptr<Expr> left, right;
};

class Constant : public Expr {
public:
	Constant(int val) : value(val) {}

	void eval(Runtime&, Block&) override;

	int value;
};