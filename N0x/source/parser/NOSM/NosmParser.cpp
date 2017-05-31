#include "NosmParser.h"

std::vector<uint32> NosmParser::parseAndCompile() {
	while (!endIsReached()) {
		switch (advance().type) {
		case TokenType::DOT:
		{
			auto identifier = advance(IDENTIFIER);
			labelTable[identifier.getStringLiteral()] = bytecode.size();
		}
			break;
		default:
			throw std::runtime_error("Fatal error while parsing: Malformed code.");
		}
	}


	return bytecode;
}