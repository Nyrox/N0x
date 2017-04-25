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

	return nullptr;
}