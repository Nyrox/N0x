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
		std::vector<std::string> params;
	};

	class FunctionCall : public ASTNode {
	public:
		FunctionCall(std::string t_id) : identifier(t_id) {

		}

		virtual void visit(ASTVisitor& visitor) override;

		std::string identifier;
		std::vector<uptr<ASTNode>> params;
	};

	class Variable : public ASTNode {
	public:
		Variable(std::string t_identifier) : identifier(t_identifier) { }

		virtual void visit(ASTVisitor& visitor) override;

		std::string identifier;
	};

	class FunctionReturn : public ASTNode {
	public:
		FunctionReturn(uptr<ASTNode>&& t_expr) : expression(std::move(t_expr)) {

		}

		virtual void visit(ASTVisitor& visitor) override;

		uptr<ASTNode> expression;
	};

	class VariableDeclaration : public ASTNode {
	public:
		VariableDeclaration(std::string t_identifier, uptr<ASTNode>&& t_init) : identifier(t_identifier), initializer(std::move(t_init)) {

		}

		virtual void visit(ASTVisitor& visitor) override;

		std::string identifier;
		uptr<ASTNode> initializer;
	};

	class Conditional : public ASTNode {
	public:
		Conditional(uptr<ASTNode>&& t_condition, uptr<ASTNode>&& t_consequent, uptr<ASTNode>&& t_alternate)
			: condition(std::move(t_condition)), consequent(std::move(t_consequent)), alternate(std::move(t_alternate)) { }
	
		virtual void visit(ASTVisitor& visitor) override;

		uptr<ASTNode> consequent, alternate, condition;
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