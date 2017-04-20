#pragma once
#include <Scanner.h>
#include <core/Definitions.h>

class Program;

class Expr {
public:
	Expr(Token t_token) : token(t_token) {
		s_desc = desc();
	}

	virtual int eval(Program&) const = 0;
	virtual std::string desc() const {
		return "NULL";
	}

	std::string s_desc;
	Token token;
};

class Program {
public:

	void eval() {
		for (auto& it : expressions) {
			it->eval(*this);
		}
	}

	std::vector<uptr<Expr>> expressions;
	std::unordered_map<std::string, int> vars;
	std::unordered_map<std::string, const class FuncDeclExpr*> funcs;
};

class VarDeclExpr : public Expr {
public:
	VarDeclExpr(Token token, std::string t_identifier, std::unique_ptr<Expr> init = nullptr) : Expr(token), initializer(std::move(init)) {
		identifier = t_identifier;
	}

	int eval(Program& program) const override {
		if (initializer) { program.vars[identifier] = initializer->eval(program); }
		else { program.vars[identifier] = 0; }

		return program.vars[identifier];
	}

	virtual std::string desc() const override { return "Variable Declaration"; }

	uptr<Expr> initializer;
	std::string identifier;
};

class FuncDeclExpr : public Expr {
public:
	FuncDeclExpr(Token token, std::string t_identifier) : Expr(token), identifier(t_identifier) {

	}

	int eval(Program& program) const override {
		program.funcs[identifier] = this;
		return 0;
	}

	void __eval(Program& program) const {
		for (auto& it : expressions) {
			it->eval(program);
		}
	}

	virtual std::string desc() const override { return "Function Declaration"; }

	std::vector<uptr<Expr>> expressions;
	std::string identifier;
};

class FuncCallExpr : public Expr {
public:
	FuncCallExpr(Token token, std::string t_identifier) : Expr(token), identifier(t_identifier) {

	}

	int eval(Program& program) const override {
		program.funcs[identifier]->__eval(program);
		return 0;
	}

	std::string identifier;
};

class IdentifierExpr : public Expr {
public:
	IdentifierExpr(Token token) : Expr(token) {
		identifier = std::get<StringLiteral>(*token.literal).string;
	}

	int eval(Program& program) const override {
		return program.vars[identifier];
	}

	virtual std::string desc() const override { return "Identifier"; }

	std::string identifier;
};

class UnaryExpr : public Expr {
public:
	UnaryExpr(Token token, uptr<Expr>&& t_operand) : Expr(token), operand(std::move(t_operand)) {

	}

	virtual int eval(Program& program) const override {
		switch (token.type) {
		case TokenType::MINUS:
			return -operand->eval(program);
		case TokenType::PLUS:
			return +operand->eval(program);
		case TokenType::PRINT:
			auto result = operand->eval(program);
			std::cout << result << "\n";
			return result;
		}
	}

	virtual std::string desc() const override { return "Unary Expression"; }

	uptr<Expr> operand;
};

class BinaryExpr : public Expr {
public:
	BinaryExpr(uptr<Expr>&& t_left, Token t_token, uptr<Expr>&& t_right) : left(std::move(t_left)), right(std::move(t_right)), Expr(t_token) { }

	virtual int eval(Program& program) const override {
		switch (token.type) {
		case TokenType::STAR:
			return left->eval(program) * right->eval(program);
		case TokenType::SLASH:
			return left->eval(program) / right->eval(program);
		case TokenType::PLUS:
			return left->eval(program) + right->eval(program);
		case TokenType::MINUS:
			return left->eval(program) - right->eval(program);
		default:
			throw std::exception("Unrecognized binary expression.");
		}
	}

	virtual std::string desc() const override { return "Binary Expression"; }

	uptr<Expr> left, right;
};

class NumericExpr : public Expr {
public:
	NumericExpr(Token t_token, int val) : Expr(t_token), value(val) {}

	virtual int eval(Program& program) const override { return value; }
	virtual std::string desc() const override { return "Numeric expression"; }

	int value;
};