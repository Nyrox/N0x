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
	// params
	advance(RIGHT_PAREN);

	return make_unique<FunctionDeclaration>(identifier.getStringLiteral(), block());
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
	default:
		throw ParsingError("Unexpected symbol: " + peek().lexeme);
		break;
	}
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

	while (match({ PLUS, MINUS })) {
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
	}


	throw ParsingError("Unexpected symbol: " + peek().lexeme);
}

uptr<ASTNode> Parser::funccall() {
	Token token = advance(IDENTIFIER);

	advance(LEFT_PAREN);
	advance(RIGHT_PAREN);

	return make_unique<FunctionCall>(token.getStringLiteral());
}