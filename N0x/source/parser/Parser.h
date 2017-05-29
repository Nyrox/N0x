#pragma once
#include <core/Definitions.h>
#include <Scanner.h>
#include <parser/AST/AST.h>
#include <parser/Exceptions/ParsingError.h>

#include <stack>

class Parser {
public:
	Parser(std::vector<Token> t_tokens);

	AST::AbstractSyntaxTree parse();

	std::vector<Token> tokens;
private:
	AST::AbstractSyntaxTree* ast;

	uint32 currentStackFrameOffset = 0;
	std::stack<std::unordered_map<std::string, uint32>> symbolTable;
	
	int current = 0;

	// Parser util
	Token peek() const;
	Token peekNext() const;
	bool match(TokenType type) const;
	bool match(std::initializer_list<TokenType> types) const;
	Token advance();
	Token advance(TokenType type);
	Token advance(std::initializer_list<TokenType> types);
	bool endIsReached() const;

	// Parser rules
	void program();
	uptr<AST::FunctionDeclaration> funcdecl();
	uptr<AST::ASTNode> funccall();
	uptr<AST::Block> block();
	uptr<AST::ASTNode> statement();
	uptr<AST::ASTNode> whileloop();
	uptr<AST::ASTNode> conditional();
	uptr<AST::ASTNode> varassignment();
	uptr<AST::ASTNode> vardeclaration();
	uptr<AST::ASTNode> expression();
	uptr<AST::ASTNode> term();
	uptr<AST::ASTNode> factor();
	uptr<AST::ASTNode> funcreturn();

};