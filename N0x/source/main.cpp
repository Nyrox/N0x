#include <Scanner.h>

class Interpreter {
public:
	Interpreter(std::vector<Token> t_tokens) : tokens(t_tokens) { }
	int parse() {
		return expr();
	}

	std::vector<Token> tokens;
private:
	int start = 0;
	int current = 0;

	int factor() {
		if (match({ INT })) return std::get<IntegerLiteral>(*advance().literal).value;
		else if (match({ LEFT_PAREN })) {
			advance();
			int result = expr();
			advance();
			return result;
		}
		else {
			std::cout << "Error invalid factor.";
			std::cin.get();
		}
	}

	int term() {
		int result = factor();

		while (match({ STAR, SLASH })) {
			Token token = advance();
			if (token.type == STAR) { result = result * factor(); }
			else { result = result / factor(); }

			if (endIsReached()) return result;
		}

		return result;
	}

	int expr() {
		int result = term();

		while (match({ PLUS, MINUS })) {
			Token token = advance();
			if (token.type == PLUS) { result = result + term(); }
			else { result = result - term(); }

			if (endIsReached()) { return result; }
		}

		return result;
	}

	bool match(std::initializer_list<TokenType> types) {
		if (current >= tokens.size()) return false;

		for (auto& it : types) {
			if (it == tokens.at(current).type) return true;
		}

		return false;
	}

	Token advance() {
		current++;
		return tokens.at(current - 1);
	}

	bool endIsReached() {
		return current >= tokens.size();
	}
};


int main() {

	std::string source = "(5 + (4 * 3)) * 18 + 22";


	Scanner scanner(source);
	Interpreter interpreter(scanner.scanTokens());

	std::cout << std::to_string(interpreter.parse()) << "\n";
	std::cout << (5 + (4 * 3)) * 18 + 22 << "\n";
	std::cin.get();
}