#include <Scanner.h>

class ParsingError : public std::exception {
public:
	ParsingError(std::string error = std::string()) : message(error) { }

	const char* what() const override { return message.c_str(); }

	std::string message;
};

template<class T>
using uptr = std::unique_ptr<T>;
using std::make_unique;

class Program;

class Expr {
public:
	Expr(Token t_token) : token(t_token) { }

	virtual int eval(Program&) const = 0;

	Token token;
};

class Program {
public:

	void eval() {
		for (auto& it : expressions) {
			it->eval(*this);
		}
	}

	std::vector<uptr<Expr>> expressions;
	std::unordered_map<std::string, int> vars;
};

class VarDeclExpr : public Expr {
public:
	VarDeclExpr(Token token, std::string t_identifier, std::unique_ptr<Expr> init = nullptr) : Expr(token), initializer(std::move(init)) {
		identifier = t_identifier;
	}

	int eval(Program& program) const override {
		if (initializer) { program.vars[identifier] = initializer->eval(program); }
		else { program.vars[identifier] = 0; }

		return program.vars[identifier];
	}

	uptr<Expr> initializer;
	std::string identifier;
};

class IdentifierExpr : public Expr {
public:
	IdentifierExpr(Token token) : Expr(token) { 
		identifier = std::get<StringLiteral>(*token.literal).string;
	}
	
	int eval(Program& program) const override {
		return program.vars[identifier];
	}

	std::string identifier;
};

class UnaryExpr : public Expr {
public:
	UnaryExpr(Token token, uptr<Expr>&& t_operand) : Expr(token), operand(std::move(t_operand)) {

	}

	virtual int eval(Program& program) const override {
		switch (token.type) {
		case TokenType::MINUS:
			return -operand->eval(program);
		case TokenType::PLUS:
			return +operand->eval(program);
		case TokenType::PRINT:
			auto result = operand->eval(program);
			std::cout << result << "\n";
			return result;
		}
	}

	uptr<Expr> operand;
};

class BinaryExpr : public Expr {
public:
	BinaryExpr(uptr<Expr>&& t_left, Token t_token, uptr<Expr>&& t_right) : left(std::move(t_left)), right(std::move(t_right)), Expr(t_token) { }

	virtual int eval(Program& program) const override {
		switch (token.type) {
		case TokenType::STAR:
			return left->eval(program) * right->eval(program);
		case TokenType::SLASH:
			return left->eval(program) / right->eval(program);
		case TokenType::PLUS:
			return left->eval(program) + right->eval(program);
		case TokenType::MINUS:
			return left->eval(program) - right->eval(program);
		default:
			throw std::exception("Unrecognized binary expression.");
		}
	}

	uptr<Expr> left, right;
};

class NumericExpr : public Expr {
public:
	NumericExpr(Token t_token, int val) : Expr(t_token), value(val) {}

	virtual int eval(Program& program) const override { return value; }
	int value;
};

class Interpreter {
public:
	Interpreter(std::vector<Token> t_tokens) : tokens(t_tokens) { }
	uptr<Program> parse() {
		return std::move(program());
	}

	std::vector<Token> tokens;
private:
	int start = 0;
	int current = 0;

	uptr<Expr> factor() {
		if (match({ INT })) {
			Token token = advance(INT);
			return make_unique<NumericExpr>(token, std::get<IntegerLiteral>(*token.literal).value);
		}
		else if (match({ LEFT_PAREN })) {
			advance(LEFT_PAREN);
			auto node = expr();
			advance(RIGHT_PAREN);
			return node;
		}
		else if (match({ PLUS, MINUS })) {
			Token token = advance({ PLUS, MINUS });
			return make_unique<UnaryExpr>(token, std::move(factor()));
		}
		else if (match(IDENTIFIER)) {
			Token token = advance();
			return make_unique<IdentifierExpr>(token);
		}
	}

	uptr<Expr> term() {
		auto node = factor();

		while (match({ STAR, SLASH })) {
			Token token = advance();			
			node = make_unique<BinaryExpr>(std::move(node), token, std::move(factor()));
			if (endIsReached()) return node;
		}

		return node;
	}

	uptr<Expr> expr() {
		auto node = term();

		while (match({ PLUS, MINUS })) {
			Token token = advance();			
			node = make_unique<BinaryExpr>(std::move(node), token, std::move(term()));
			if (endIsReached()) { return node; }
		}

		return node;
	}

	uptr<Expr> identifier() {
		Token token = advance(IDENTIFIER);
		return make_unique<IdentifierExpr>(token);
	}

	uptr<Expr> print() {
		Token token = advance(PRINT);
		auto node = expr();

		return make_unique<UnaryExpr>(token, std::move(node));
	}

	uptr<Expr> vardecl() {
		Token token = advance(VAR);
		
		if (!match(IDENTIFIER)) { throw ParsingError("Expected identifier. Found: " + std::to_string(token.type)); }
		auto node = std::get<StringLiteral>(*advance().literal).string;

		uptr<Expr> initializer = nullptr;
		if (match({ EQUAL })) {
			advance();
			initializer = expr();
		}

		return make_unique<VarDeclExpr>(token, std::move(node), std::move(initializer));
	}

	uptr<Program> program() {
		auto program = make_unique<Program>();

		while (!endIsReached()) {
			Token token = peek();
			
			switch (token.type) {
			case TokenType::PRINT:
				program->expressions.push_back(std::move(print()));
				break;
			case TokenType::VAR:
				program->expressions.push_back(std::move(vardecl()));
				break;
			}
		}

		return std::move(program);
	}

	Token peek() {
		return tokens.at(current);
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
		return advance({ });
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




int main() {
	std::stringstream ss;
	ss << std::ifstream("scripts/main.nox").rdbuf();
	std::string source = ss.str();

	Scanner scanner(source);
	Interpreter interpreter(scanner.scanTokens());
	auto program = interpreter.parse();
	program->eval();

	std::cin.get();
}