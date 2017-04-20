#include <Interpreter.h>


int main() {
	std::stringstream ss;
	ss << std::ifstream("scripts/main.nox").rdbuf();
	std::string source = ss.str();

	Scanner scanner(source);
	Runtime runtime;
	Interpreter interpreter(runtime, scanner.scanTokens());
	interpreter.parse();

	runtime.call("main");

	std::cin.get();
}