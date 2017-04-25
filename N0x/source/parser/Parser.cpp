#include "Parser.h"

using namespace AST;

Parser::Parser(std::vector<Token> t_tokens) : tokens(t_tokens) {

}

AbstractSyntaxTree Parser::parse() {
	AbstractSyntaxTree out;
	this->ast = &out;

	program();

	this->ast = nullptr;
	return out;
}



// Util
bool Parser::endIsReached() const {
	return current >= tokens.size();
}

Token Parser::peek() const {
	return tokens.at(current);
}

Token Parser::peekNext() const {
	return tokens.at(current + 1);
}

bool Parser::match(TokenType type) const {
	return match({ type });
}

bool Parser::match(std::initializer_list<TokenType> types) const {
	if (current >= tokens.size()) return false;
	
	for (auto& it : types) {
		if (it == tokens.at(current).type) return true;
	}

	return false;
}

Token Parser::advance() {
	current++;
	return tokens.at(current - 1);
}

Token Parser::advance(TokenType type) {
	return advance({ type });
}

Token Parser::advance(std::initializer_list<TokenType> types) {
	if (match(types)) {
		return advance();
	}

	throw ParsingError("Unexpected symbol: " + peek().lexeme);
}