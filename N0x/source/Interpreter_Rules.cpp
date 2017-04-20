#include "Interpreter.h"

Interpreter::Interpreter(std::vector<Token> t_tokens) : tokens(t_tokens) { }

uptr<Program> Interpreter::parse() {
	return std::move(program());
}

uptr<Program> Interpreter::program() {
	auto program = make_unique<Program>();

	while (!endIsReached()) {
		Token token = peek();

		switch (token.type) {
		case TokenType::PRINT:
			program->expressions.push_back(std::move(print()));
			break;
		case TokenType::VAR:
			program->expressions.push_back(std::move(vardecl()));
			break;
		case TokenType::FUNC:
			program->expressions.push_back(std::move(funcdecl()));
			break;
		case TokenType::IDENTIFIER:
			program->expressions.push_back(std::move(expr()));
			break;
		default:
			throw ParsingError("Unexpected token: " + token.lexeme);
		}
	}

	return std::move(program);
}

uptr<Expr> Interpreter::print() {
	Token token = advance(PRINT);
	auto node = expr();

	return make_unique<UnaryExpr>(token, std::move(node));
}

uptr<Expr> Interpreter::funcdecl() {
	Token token = advance(FUNC);
	Token identifier = advance(IDENTIFIER);

	advance(LEFT_PAREN);
	advance(RIGHT_PAREN);
	advance(LEFT_BRACE);

	uptr<FuncDeclExpr> func = make_unique<FuncDeclExpr>(token, std::get<StringLiteral>(*identifier.literal).string);

	while (!endIsReached() && !match(RIGHT_BRACE)) {
		Token token = peek();

		switch (token.type) {
		case TokenType::VAR:
			func->expressions.push_back(std::move(vardecl()));
			break;
		case TokenType::PRINT:
			func->expressions.push_back(std::move(print()));
			break;
		case TokenType::IDENTIFIER:
			func->expressions.push_back(std::move(expr()));
			break;
		default:
			throw ParsingError("Unexpected token: " + token.lexeme);
		}
	}

	advance(RIGHT_BRACE);

	return func;
}

uptr<Expr> Interpreter::funccall() {
	Token token = advance(IDENTIFIER);

	advance(LEFT_PAREN);
	advance(RIGHT_PAREN);

	return make_unique<FuncCallExpr>(token, std::get<StringLiteral>(*token.literal).string);
}

uptr<Expr> Interpreter::vardecl() {
	Token token = advance(VAR);

	if (!match(IDENTIFIER)) { throw ParsingError("Expected identifier. Found: " + getCurrent().lexeme); }
	auto node = std::get<StringLiteral>(*advance().literal).string;

	uptr<Expr> initializer = nullptr;
	if (match({ EQUAL })) {
		advance();
		initializer = expr();
	}

	return make_unique<VarDeclExpr>(token, std::move(node), std::move(initializer));
}

uptr<Expr> Interpreter::identifier() {
	Token token = advance(IDENTIFIER);
	return make_unique<IdentifierExpr>(token);
}

uptr<Expr> Interpreter::expr() {
	auto node = term();

	while (match({ PLUS, MINUS })) {
		Token token = advance();
		node = make_unique<BinaryExpr>(std::move(node), token, std::move(term()));
		if (endIsReached()) { return node; }
	}

	return node;
}

uptr<Expr> Interpreter::term() {
	auto node = factor();

	while (match({ STAR, SLASH })) {
		Token token = advance();
		node = make_unique<BinaryExpr>(std::move(node), token, std::move(factor()));
		if (endIsReached()) return node;
	}

	return node;
}

uptr<Expr> Interpreter::factor() {
	if (match({ INT })) {
		Token token = advance(INT);
		return make_unique<NumericExpr>(token, std::get<IntegerLiteral>(*token.literal).value);
	}
	else if (match({ LEFT_PAREN })) {
		advance(LEFT_PAREN);
		auto node = expr();
		advance(RIGHT_PAREN);
		return node;
	}
	else if (match({ PLUS, MINUS })) {
		Token token = advance({ PLUS, MINUS });
		return make_unique<UnaryExpr>(token, std::move(factor()));
	}
	else if (match(IDENTIFIER)) {
		if (peekNext().type == LEFT_PAREN) {
			return funccall();
		}
		else {
			Token token = advance(IDENTIFIER);
			return make_unique<IdentifierExpr>(token);
		}
	}
	else {
		throw ParsingError("Expected factor, found: " + getCurrent().lexeme);
	}
}