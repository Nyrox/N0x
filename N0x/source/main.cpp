#include <Interpreter.h>
#include <core/FunctionDispatch.h>

int main() {

	std::stringstream ss;
	ss << std::ifstream("scripts/main.nox").rdbuf();
	std::string source = ss.str();

	Scanner scanner(source);
	Runtime runtime;
	Interpreter interpreter(runtime, scanner.scanTokens());
	interpreter.parse();

	runtime.registerNativeFunction("cpp", make_unique<FunctionDispatch<void>>([&]() { 
		std::cout << "Cpp!" << "\n";
	}));

	runtime.call("main");


	std::cin.get();
}


