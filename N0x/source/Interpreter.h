#include <Scanner.h>
#include <expressions/expr.h>
#include <core/Definitions.h>
#include <Runtime.h>
#include <stack>

class ParsingError : public std::exception {
public:
	ParsingError(std::string error = std::string()) : message(error) {
		#ifdef _DEBUG
			std::cout << message << "\n";
		#endif
	}

	const char* what() const override { return message.c_str(); }
	std::string message;
};


class Interpreter {
public:
	Interpreter(Runtime& runtime, std::vector<Token> t_tokens);

	// Parses a token sequence and loads it into a runtime
	void parse();

	std::vector<Token> tokens;
private:
	uint32 currentStackFrameOffset = 0;
	std::stack<std::unordered_map<std::string, uint32>> symbolTable; 

	int start = 0;
	int current = 0;
	Runtime& runtime; 

	// Rules making up the grammar

	// An expression representing a single l- or rvalue
	uptr<Expr> factor();
	// An expression representing a term made up of factors
	uptr<Expr> term();
	// An expression made up of terms with a return value
	uptr<Expr> expr();
	uptr<Expr> identifier();
	// Temporary instruction to print the following expression

	// Declares a variable
	uptr<Expr> vardecl();
	// Calls a function
	uptr<Expr> funccall();

	uptr<Unary> print();
	void funcdecl();
	void program();
	Block block();

	// Utility functions
	Token peek() {
		return tokens.at(current);
	}

	Token peekNext() {
		return tokens.at(current + 1);
	}

	bool match(TokenType type) {
		return match({ type });
	}

	bool match(std::initializer_list<TokenType> types) {
		if (current >= tokens.size()) return false;

		for (auto& it : types) {
			if (it == tokens.at(current).type) return true;
		}

		return false;
	}

	Token advance() {
		return advance({});
	}

	Token advance(TokenType type) {
		return advance({ type });
	}

	Token advance(std::initializer_list<TokenType> types) {
		current++;
		bool legit = types.size() == 0;
		for (auto& it : types) {
			if (tokens.at(current - 1).type == it) legit = true;
		}
		if (!legit) throw ParsingError("Unexpected symbol.");
		return tokens.at(current - 1);
	}

	Token getCurrent() {
		return tokens.at(current - 1);
	}

	bool endIsReached() {
		return current >= tokens.size();
	}
};
