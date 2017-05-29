#include <core/FunctionDispatch.h>
#include <parser/Parser.h>
#include <runtime/Runtime.h>
#include <chrono>

using namespace std::chrono;


struct Foo {
	Foo(float t) : v(t) {

	}

	float v;
};

int main() {
	high_resolution_clock clock;
	//std::stringstream ss;
	//ss << std::ifstream("scripts/main.nox").rdbuf();
	//std::string source = ss.str();

	//Scanner scanner(source);
	//Parser parser(scanner.scanTokens());
	//auto ast = parser.parse();
	//
	//Runtime runtime(1024);

	//runtime.registerNativeFunction("print", make_unique<FunctionDispatch<void, std::string>>([&](std::string i) {
	//	std::cout << i << "\n";
	//}));

	//runtime.eval(ast);
	//

	auto start = clock.now();

	//for (int i = 0; i < 1; i++) {
	//	runtime.call("main");
	//}

	auto end = clock.now();
	duration<double> elapsed_seconds = end - start;
	std::cout << "Finished computation in " << elapsed_seconds.count() << " seconds." << "\n";


	enum OpCodes : uint32 {
		// Operations
		ASSIGN, CALL_EXTERN,

		FLOAT_MUL
	};

	float f = 8.25f;

	std::vector<uint32> bytecode = {
		ASSIGN, 0x00, reinterpret_cast<uint32&>(f), ASSIGN, 0x04, FLOAT_MUL, 0x00, 0x00, CALL_EXTERN, 0x00, 0x04, 0x04
	};

	uint32 isp = 0;
	auto& advance = [&]() {
		return bytecode[isp++];
	};
	auto& get = [&]() {
		return bytecode[isp];
	};

	uint8* stackBasePointer = static_cast<uint8*>(malloc(1024));

	start = clock.now();

	while (isp < bytecode.size()) {
		switch (get()) {
		case ASSIGN:
		{
			advance();
			auto dest = stackBasePointer + advance();
			switch (get()) {
			case FLOAT_MUL:
			{
				advance();
				// For float multiplies we only allow stack values to be multiplied so easy time
				auto left = stackBasePointer + advance();
				auto right = stackBasePointer + advance();
				auto result = reinterpret_cast<float&>(*left) * reinterpret_cast<float&>(*right);
				memcpy(dest, &result, 4);
				break;
			}
			default:
			{
				// Just assume its a constant
				auto val = get();
				memcpy(dest, &val, 4);
				advance();
				break;
			}
			}
		}
		break;
		case CALL_EXTERN:
		{
			advance();
			auto functionID = advance();
			
			auto memSize = advance();
			auto memLocation = stackBasePointer + advance();

			auto& print = [](float t) {
				std::cout << t << "\n";
			};
			print(reinterpret_cast<float&>(*memLocation));
		}
		break;
		default:
			assert(false);
		}
	}
	end = clock.now();
	elapsed_seconds = end - start;
	std::cout << "Finished computation in " << elapsed_seconds.count() << " seconds." << "\n";


	free(stackBasePointer);
	std::cin.get();
}

// 0.000408998s

