#pragma once
#include <core/Definitions.h>
#include <vector>
#include <parser/AST/ASTNode.h>

namespace AST {

	struct AbstractSyntaxTree {
		std::vector<uptr<ASTNode>> nodes;
	};

}