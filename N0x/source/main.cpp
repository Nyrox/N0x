#include <core/FunctionDispatch.h>
#include <parser/Parser.h>
#include <runtime/Runtime.h>

int main() {

	std::stringstream ss;
	ss << std::ifstream("scripts/main.nox").rdbuf();
	std::string source = ss.str();

	Scanner scanner(source);
	/*Runtime runtime;*/
	Parser parser(scanner.scanTokens());
	auto ast = parser.parse();


	
	Runtime runtime(256);
	runtime.registerNativeFunction("cpp", make_unique<FunctionDispatch<void>>([&]() {
		std::cout << "Cpp!" << "\n";
	}));


	runtime.eval(ast);
	
	
	runtime.call("main");

	std::cin.get();
}


