#pragma once
#include <core/Definitions.h>
#include <string>
#include <vector>

class ASTVisitor;

namespace AST {

	struct ASTNode {
		virtual void visit(ASTVisitor& visitor) = 0;
	};

	struct Block {
		virtual void visit(ASTVisitor& visitor);

		std::vector<uptr<ASTNode>> statements;
	};

	struct FunctionDeclaration : public ASTNode {
		FunctionDeclaration(std::string t_id, uptr<Block> t_block) : identifier(t_id), block(std::move(t_block)) { 	}

		virtual void visit(ASTVisitor& visitor);

		std::string identifier;
		uptr<Block> block;
	};

	struct VariableDeclaration : public ASTNode {
		VariableDeclaration(std::string t_identifier, uptr<ASTNode>&& t_init) : identifier(t_identifier), initializer(std::move(t_init)) {

		}

		virtual void visit(ASTVisitor& visitor);

		std::string identifier;
		uptr<ASTNode> initializer;
	};

}