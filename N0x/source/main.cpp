#include <Interpreter.h>


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