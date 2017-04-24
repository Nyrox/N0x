#include "Interpreter.h"
#include <Runtime.h>

Interpreter::Interpreter(Runtime& t_runtime, std::vector<Token> t_tokens) : runtime(t_runtime), tokens(t_tokens) { }

void Interpreter::parse() {
	program();
}



void Interpreter::program() {
	while (!endIsReached()) {
		funcdecl();
	}

}

void Interpreter::funcdecl() {
	Token token = advance(FUNC);
	Token identifier = advance(IDENTIFIER);

	advance(LEFT_PAREN);
	advance(RIGHT_PAREN);

	runtime.functions[std::get<StringLiteral>(*identifier.literal).string] = std::move(block());
}

uptr<Expr> Interpreter::condition() {
	advance(IF);

	advance(LEFT_PAREN);
	auto c = expr();
	advance(RIGHT_PAREN);
	auto b = block();

	uptr<Expr> alternate = nullptr;
	if (match(ELSE)) {
		advance(ELSE);

		if (match(IF)) {
			alternate = condition();
		}
		else {
			alternate = make_unique<BlockExpr>(block());
		}
	}

	return make_unique<Condition>(std::move(c), std::move(b), std::move(alternate));
}

Block Interpreter::block() {
	Token brace = advance(LEFT_BRACE);
	Block out;

	while (!endIsReached() && !match(RIGHT_BRACE)) {
		switch (peek().type) {
		case VAR:
			out.expressions.push_back(vardecl());
			break;
		case PRINT:
			out.expressions.push_back(print());
			break;
		case IDENTIFIER:
			out.expressions.push_back(expr());
			break;
		case IF:
			out.expressions.push_back(condition());
			break;
		default:
			throw ParsingError("Unexpected symbol: " + peek().lexeme);
		}
	}

	advance(RIGHT_BRACE);
	return out;
}

uptr<Expr> Interpreter::expr() {
	auto node = term();

	while (match({ PLUS, MINUS })) {
		Token token = advance();
		node = make_unique<Binary>(std::move(node), token, std::move(term()));
		if (endIsReached()) { return node; }
	}

	return node;
}

uptr<Expr> Interpreter::term() {
	auto node = factor();

	while (match({ STAR, SLASH })) {
		Token token = advance();
		node = make_unique<Binary>(std::move(node), token, std::move(factor()));
		if (endIsReached()) return node;
	}

	return node;
}

uptr<Expr> Interpreter::factor() {
	if (match({ INT })) {
		Token token = advance(INT);
		return make_unique<Constant>(std::get<IntegerLiteral>(*token.literal).value);
	}
	else if (match({ LEFT_PAREN })) {
		advance(LEFT_PAREN);
		auto node = expr();
		advance(RIGHT_PAREN);
		return node;
	}
	else if (match({ PLUS, MINUS })) {
		Token token = advance({ PLUS, MINUS });
		return make_unique<Unary>(token, std::move(factor()));
	}
	else if (match(IDENTIFIER)) {
		if (peekNext().type == LEFT_PAREN) {
			return funccall();
		}
		else {
			Token token = advance(IDENTIFIER);
			return make_unique<Variable>(std::get<StringLiteral>(*token.literal).string);
		}
	}
	else {
		throw ParsingError("Expected factor, found: " + getCurrent().lexeme);
	}
}

uptr<Unary> Interpreter::print() {
	Token token = advance(PRINT);

	return make_unique<Unary>(token, expr());
}

uptr<Expr> Interpreter::funccall() {
	Token token = advance(IDENTIFIER);

	advance(LEFT_PAREN);
	advance(RIGHT_PAREN);

	return make_unique<FuncCall>(std::get<StringLiteral>(*token.literal).string);
}

uptr<Expr> Interpreter::vardecl() {
	Token token = advance(VAR);

	if (!match(IDENTIFIER)) { throw ParsingError("Expected identifier. Found: " + getCurrent().lexeme); }
	auto identifier = std::get<StringLiteral>(*advance().literal).string;

	uptr<Expr> initializer = nullptr;
	if (match({ EQUAL })) {
		advance();
		initializer = expr();
	}

	return make_unique<VarDecl>(identifier, std::move(initializer));
}



