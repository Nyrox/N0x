#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <experimental/filesystem>
#include <string>
#include <algorithm>
#include <optional>
#include <variant>
#include <tuple>
#include <unordered_map>

enum TokenType {
	// Single character tokens
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	COMMA, DOT, SEMICOLON, COLON, SLASH, STAR, AMPERSAND,


	// One or two character tokens.
	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,
	PLUS, PLUS_PLUS,
	MINUS, MINUS_MINUS,

	// Literals.
	IDENTIFIER, STRING, INT, FLOAT,

	// Keywords.
	AND, CLASS, ELSE, FALSE, FUNC, FOR, IF, NIL, OR,
	PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

	END_OF_FILE
};

struct StringLiteral {
	StringLiteral(std::string str) : string(str) { }
	StringLiteral() = default;

	std::string string;
};

struct IntegerLiteral {
	IntegerLiteral(int i) : value(i) {}
	IntegerLiteral() = default;

	int32_t value;
};

struct FloatLiteral {
	FloatLiteral(float f) : value(f) { }
	FloatLiteral() = default;

	float value;
};

using Literal = std::variant<StringLiteral, IntegerLiteral, FloatLiteral>;


struct Token {

	explicit Token(TokenType t_type, std::string t_lexeme, int t_line) : type(t_type), lexeme(t_lexeme), line(t_line) { }
	// String literal
	explicit Token(TokenType t_type, std::string t_lexeme, int t_line, Literal t_literal) : Token(t_type, t_lexeme, t_line) {
		literal = t_literal;
	}

	std::string getDebugString() {
		return std::to_string(type) + ": " + lexeme + " at line: " + std::to_string(line);
	}

	std::string getStringLiteral() {
		return std::get<StringLiteral>(*literal).string;
	}

	TokenType type;
	std::string lexeme;
	int line;

	std::optional<Literal> literal;
};

class LexicalError : std::exception {
public:
	LexicalError(int t_line, std::string t_error = std::string()) : line(t_line) {
		error = "Lexical error at line: " + std::to_string(line) + ". " + t_error;
	};

	virtual const char* what() const override {
		return error.c_str();
	}

	std::string error;
	int line;
};

class Scanner {
public:
	Scanner(std::string src) : source(src) { }

	std::vector<Token>& scanTokens() {
		while (!endIsReached()) {
			start = current;
			try {
				scanToken();
			}
			catch (LexicalError& error) {
				std::cout << error.what() << "\n";
				hadError = true;
				continue;
			}
		}

		return tokens;
	}

	std::string source;
	std::vector<Token> tokens;
private:
	int start = 0;
	int current = 0;
	int line = 1;
	bool hadError = false;

	void scanToken() {
		char c = advance();
		switch (c) {
		case '(': addToken(LEFT_PAREN); break;
		case ')': addToken(RIGHT_PAREN); break;
		case '{': addToken(LEFT_BRACE); break;
		case '}': addToken(RIGHT_BRACE); break;
		case ',': addToken(COMMA); break;
		case '.': addToken(DOT); break;
		case ';': addToken(SEMICOLON); break;
		case '*': addToken(STAR); break;
			// 1 or 2 length characters
		case '!': addToken(match('=') ? BANG_EQUAL : BANG); break;
		case '=': addToken(match('=') ? EQUAL_EQUAL : EQUAL); break;
		case '<': addToken(match('=') ? LESS_EQUAL : LESS); break;
		case '>': addToken(match('=') ? GREATER_EQUAL : GREATER); break;
		case '-': addToken(match('-') ? MINUS_MINUS : MINUS); break;
		case '+': addToken(match('+') ? PLUS_PLUS : PLUS); break;
			// Comment
		case '/':
			if (match('/')) {
				while (peek() != '\n' && !endIsReached()) advance();
			}
			else {
				addToken(SLASH);
			}
			break;
			// String literals
		case '"': string(); break;

		case ' ':
		case '\r':
		case '\t':
			// Ignore whitespace
			break;
		case '\n':
			line++;
			break;
		default:
			// Digits
			if (isDigit(c)) {
				number();
				break;
			}

			// Identifiers
			if (isAlpha(c)) {
				identifier();
				break;
			}


			// Error unrecognized token
			throw LexicalError(line, "Invalid token");
			break;
		}
	}


	// Returns the next character without consuming it
	char peek() {
		if (current >= source.length()) return '\0';
		return source.at(current);
	}

	// Returns the character after the next one without consuming it
	char peekNext() {
		if (current + 1 >= source.length()) return '\0';
		return source.at(current + 1);
	}

	// Consumes and returns a character
	char advance() {
		current++;
		return source.at(current - 1);
	}

	// Check's whether EOF is reached
	bool endIsReached() const {
		return current >= source.length();
	}

	// Check's if the next character equals expected and if yes consumes it
	bool match(char expected) {
		if (endIsReached()) return false;
		if (source.at(current) != expected) return false;

		current++;
		return true;
	}

	// Return's true on digits
	bool isDigit(char c) {
		return c >= '0' && c <= '9';
	}

	bool isAlpha(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	}

	bool isAlphaNumeric(char c) {
		return isAlpha(c) || isDigit(c);
	}

	// Parses an identifier
	void identifier() {
		while (isAlphaNumeric(peek())) advance();

		// See if the identifier is reserved
		std::string text = source.substr(start, current - start);
		auto it = keywords.find(text);
		TokenType type = IDENTIFIER;
		if (it != keywords.end()) type = keywords.at(text);
		addToken(type, StringLiteral(text));
	}

	// Parses a string
	void string() {
		while (peek() != '"' && !endIsReached()) {
			if (peek() == '\n') line++;
			advance();
		}

		// Unterminated string
		if (endIsReached()) {
			throw LexicalError(line, "Unterminated string.");
			return;
		}

		// The closing ".
		advance();

		// Trim the surrounding quotes
		std::string value = source.substr(start + 1, current - start - 1);
		addToken(STRING, StringLiteral(value));
	}

	// Parses a number
	void number() {
		bool isFloat = false;

		while (isDigit(peek())) advance();

		// Look for a fractional part
		if (peek() == '.' && isDigit(peekNext())) {
			isFloat = true;
			// Consume the "."
			advance();

			while (isDigit(peek())) advance();
		}

		std::string text = source.substr(start, current - start);
		if (isFloat) addToken(FLOAT, FloatLiteral(std::stof(text)));
		else { addToken(INT, IntegerLiteral(std::stoi(text))); }
	}

	// Add's a token without literal
	void addToken(TokenType type) {
		std::string text = source.substr(start, current - start);
		tokens.push_back(Token(type, text, line));
	}

	// Adds a token with literal
	void addToken(TokenType type, Literal literal) {
		std::string text = source.substr(start, current - start);
		tokens.push_back(Token(type, text, line, literal));
	}


	const std::unordered_map<std::string, TokenType> keywords = {
		{ "class", CLASS },
		{ "else", ELSE },
		{ "false", FALSE },
		{ "for", FOR },
		{ "func", FUNC },
		{ "if", IF },
		{ "null", NIL },
		{ "print", PRINT },
		{ "return", RETURN },
		{ "this", THIS },
		{ "true", TRUE },
		{ "var", VAR },
		{ "while", WHILE }
	};
};
