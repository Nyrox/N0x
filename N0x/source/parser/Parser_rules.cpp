#include "Parser.h"

using namespace AST;

void Parser::program() {
	while (!endIsReached()) {
		ast->nodes.push_back(funcdecl());
	}
}

uptr<FunctionDeclaration> Parser::funcdecl() {
	Token funcToken = advance(FUNC);
	Token identifier = advance(IDENTIFIER);

	advance(LEFT_PAREN);
	
	auto out = make_unique<FunctionDeclaration>(identifier.getStringLiteral(), nullptr);

	while (!endIsReached() && !match(RIGHT_PAREN)) {
		Token param = advance(IDENTIFIER);
		out->params.push_back(param.getStringLiteral());

		if (match(COMMA)) advance();
	}

	advance(RIGHT_PAREN);

	out->block = block();

	return std::move(out);
}

uptr<ASTNode> Parser::funccall() {
	Token token = advance(IDENTIFIER);

	advance(LEFT_PAREN);
	
	auto out = make_unique<FunctionCall>(token.getStringLiteral());

	while (!endIsReached() && !match(RIGHT_PAREN)) {
		out->params.push_back(expression());

		if (match(COMMA)) advance();
	}
	
	advance(RIGHT_PAREN);

	return std::move(out);
}

uptr<ASTNode> Parser::funcreturn() {
	advance(RETURN);

	return make_unique<FunctionReturn>(expression());
}

uptr<Block> Parser::block() {
	advance(LEFT_BRACE);

	auto out = make_unique<Block>();

	while (!endIsReached() && !match(RIGHT_BRACE)) {
		out->statements.push_back(statement());
	}

	advance(RIGHT_BRACE);
	return std::move(out);
}

uptr<ASTNode> Parser::statement() {
	switch (peek().type) {
	case VAR:
		return vardeclaration();
		break;
	case IDENTIFIER:
		return expression();
		break;
	case RETURN:
		return funcreturn();
		break;
	case IF:
		return conditional();
		break;
	default:
		throw ParsingError("Unexpected symbol: " + peek().lexeme);
		break;
	}
}

uptr<ASTNode> Parser::conditional() {
	advance(IF);

	advance(LEFT_PAREN);
	auto condition = expression();
	advance(RIGHT_PAREN);
	
	auto consequent = block();
	uptr<ASTNode> alternate = nullptr;

	if (match(ELSE)) {
		advance(ELSE);
		if (match(IF)) {
			alternate = conditional();
		}
		else {
			alternate = block();
		}
	}

	return make_unique<Conditional>(std::move(condition), std::move(consequent), std::move(alternate));
}

uptr<ASTNode> Parser::vardeclaration() {
	advance(VAR);
	Token identifier = advance(IDENTIFIER);

	uptr<ASTNode> init = nullptr;
	if (match(EQUAL)) {
		advance(EQUAL);
		init = expression();
	}

	return make_unique<VariableDeclaration>(identifier.getStringLiteral(), std::move(init));
}

uptr<ASTNode> Parser::expression() {
	auto node = term();

	while (match({ PLUS, MINUS, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL })) {
		Token token = advance();
		node = make_unique<BinaryOperation>(std::move(node), token.type, term());
	}

	return std::move(node);
}

uptr<ASTNode> Parser::term() {
	auto node = factor();

	while (match({ STAR, SLASH })) {
		Token token = advance();
		node = make_unique<BinaryOperation>(std::move(node), token.type, factor());
	}

	return std::move(node);
}

uptr<ASTNode> Parser::factor() {
	if (match({ INT })) {
		Token token = advance();
		return make_unique<Constant>(token.getIntLiteral());
	}
	else if (match(IDENTIFIER)) {
		if (peekNext().type == LEFT_PAREN) {
			return funccall();
		}
		else {
			return make_unique<Variable>(advance().getStringLiteral());
		}
	}


	throw ParsingError("Unexpected symbol: " + peek().lexeme);
}

