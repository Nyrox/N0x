#pragma once
#include <core/Definitions.h>
#include <Scanner.h>

class NosmParser {
public:
	NosmParser(const std::vector<Token>& t_tokens) : tokens(t_tokens) { }

	std::vector<uint32> parseAndCompile();

	std::vector<Token> tokens;
	std::vector<uint32> bytecode;
private:
	uint32 current = 0;
	std::unordered_map<std::string, uint32> labelTable;


	bool endIsReached() const { return current >= tokens.size(); }
	Token peek() const { return tokens.at(current); }
	Token peekNext() const { return tokens.at(current + 1); }
	bool match(uint32 type) const { return match({ type }); };
	bool match(std::initializer_list<uint32> types) const {
		if (current >= tokens.size()) return false;

		for (auto& it : types) {
			if (it == tokens.at(current).type) return true;
		}
		return false;
	}

	Token advance() { current++; return tokens.at(current - 1); }
	Token advance(uint32 type) { return advance({ type }); }
	Token advance(std::initializer_list<uint32> types) {
		if (match(types)) { return advance(); }

		throw std::runtime_error("Parsing error: Malformed code.");
	}
};
