#include <core/FunctionDispatch.h>
#include <parser/Parser.h>
#include <runtime/Runtime.h>
#include <chrono>

using namespace std::chrono;


int main() {

	std::stringstream ss;
	ss << std::ifstream("scripts/main.nox").rdbuf();
	std::string source = ss.str();

	Scanner scanner(source);
	Parser parser(scanner.scanTokens());
	auto ast = parser.parse();
	
	Runtime runtime(1024);
	runtime.registerNativeFunction("cpp", make_unique<FunctionDispatch<void>>([&]() {
		std::cout << "Cpp!" << "\n";
	}));

	runtime.registerNativeFunction("print", make_unique<FunctionDispatch<void, int>>([&](int i) {
		std::cout << i << "\n";
	}));


	runtime.eval(ast);
	
	high_resolution_clock clock;
	auto start = clock.now();

	for (int i = 0; i < 60; i++) {
		runtime.call("main");
	}

	auto end = clock.now();
	duration<double> elapsed_seconds = end - start;
	std::cout << "Finished computation in " << elapsed_seconds.count() << " seconds." << "\n";


	std::cin.get();
}


