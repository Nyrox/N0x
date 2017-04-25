#pragma once
#include <Scanner.h>

#include <core/Definitions.h>
#include <string>
#include <vector>

class ASTVisitor;

namespace AST {

	class ASTNode {
	public:
		virtual void visit(ASTVisitor& visitor) = 0;
	};

	class Block : public ASTNode {
	public:
		virtual void visit(ASTVisitor& visitor) override;

		std::vector<uptr<ASTNode>> statements;
	};

	class FunctionDeclaration : public ASTNode {
	public:
		FunctionDeclaration(std::string t_id, uptr<Block> t_block) : identifier(t_id), block(std::move(t_block)) { 	}

		virtual void visit(ASTVisitor& visitor) override;

		std::string identifier;
		uptr<Block> block;
	};

	class FunctionCall : public ASTNode {
	public:
		FunctionCall(std::string t_id) : identifier(t_id) {

		}

		virtual void visit(ASTVisitor& visitor) override;

		std::string identifier;
	};

	class VariableDeclaration : public ASTNode {
	public:
		VariableDeclaration(std::string t_identifier, uptr<ASTNode>&& t_init) : identifier(t_identifier), initializer(std::move(t_init)) {

		}

		virtual void visit(ASTVisitor& visitor) override;

		std::string identifier;
		uptr<ASTNode> initializer;
	};

	class BinaryOperation : public ASTNode {
	public:
		BinaryOperation(uptr<ASTNode>&& t_left, TokenType t_op, uptr<ASTNode>&& t_right) : left(std::move(t_left)), right(std::move(t_right)), op(t_op) {

		}

		virtual void visit(ASTVisitor& visitor) override;


		uptr<ASTNode> left, right;
		TokenType op;
	};

	class Constant : public ASTNode {
	public:
		Constant(int t_value) : value(t_value) {

		}

		virtual void visit(ASTVisitor& visitor) override;

		int value;
	};
}