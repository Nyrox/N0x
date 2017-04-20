#include <Scanner.h>
#include <expressions/expr.h>
#include <core/Definitions.h>

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
	Interpreter(std::vector<Token> t_tokens);

	
	uptr<Program> parse();

	std::vector<Token> tokens;
private:
	int start = 0;
	int current = 0;
	
	// Rules making up the grammar

	// An expression representing a single l- or rvalue
	uptr<Expr> factor();
	// An expression representing a term made up of factors
	uptr<Expr> term();
	// An expression made up of terms with a return value
	uptr<Expr> expr();
	uptr<Expr> identifier();
	// Temporary instruction to print the following expression
	uptr<Expr> print();
	// Declares a variable
	uptr<Expr> vardecl();
	// Calls a function
	uptr<Expr> funccall();
	// Declares a function
	uptr<Expr> funcdecl();
	// Main entry rule
	uptr<Program> program();


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
