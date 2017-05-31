#include <core/FunctionDispatch.h>
#include <parser/Parser.h>
#include <runtime/Runtime.h>
#include <chrono>

using namespace std::chrono;

enum StackInitFlags : uint32 {
	ZERO, UNDEFINED
};


struct StackFrame {
	uint8* basePointer = nullptr;
	uint8* endPointer = nullptr;
	uint8* lookAheadPointer = nullptr;
};

class Stack {
public:
	Stack(uint32 t_stackSize) : stackSize(t_stackSize) {
		data = (uint8*)malloc(stackSize);
		stackFrames.emplace_back();
		stackFrames.back().basePointer = data;
		stackFrames.back().endPointer = data;
		stackFrames.back().lookAheadPointer = data;
	}
	~Stack() {
		free(data);
	}

	void pushBytes(uint32 byteCount, StackInitFlags initFlag) {
		StackFrame& current = stackFrames.back();

		if (initFlag == ZERO) {
			memset(current.endPointer, 0, byteCount);
		}

		current.endPointer += byteCount;
		current.lookAheadPointer += byteCount;
	}

	void pushParams(uint32 byteCount, void* source) {
		StackFrame& current = stackFrames.back();

		memcpy(current.lookAheadPointer, source, byteCount);
		current.lookAheadPointer += byteCount;
	}

	void pushStackFrame() {
		stackFrames.emplace_back();
		StackFrame& current = stackFrames.at(stackFrames.size() - 2);
		StackFrame& next = stackFrames.back();

		next.basePointer = current.endPointer;
		next.endPointer = current.lookAheadPointer;
		next.lookAheadPointer = next.endPointer;
	}

	void popStackFrame() {
		stackFrames.pop_back();
		stackFrames.back().lookAheadPointer = stackFrames.back().endPointer;
	}

	template<class T = uint8>
	T* getFromCurrent(uint32 advance) const {
		return (T*)(stackFrames.back().basePointer + advance);
	}

	template<class T>
	void dumpStack() const {
		for (int i = 0; i <= stackSize - sizeof(T); i += sizeof(T)) {
			std::cout << (T*)(data)[i] << "  -  ";
			if (i / sizeof(T) % 4 == 0) std::cout << "\n";
		}
	}

	uint8* data;
	uint32 stackSize;
private:
	std::vector<StackFrame> stackFrames;
};

int main() {
	high_resolution_clock clock;
	

	//Scanner scanner(source);
	//scanner.keywords = {
	//	{ "class", CLASS },
	//	{ "else", ELSE },
	//	{ "false", FALSE },
	//	{ "for", FOR },
	//	{ "func", FUNC },
	//	{ "if", IF },
	//	{ "null", NIL },
	//	{ "return", RETURN },
	//	{ "this", THIS },
	//	{ "true", TRUE },
	//	{ "var", VAR },
	//	{ "while", WHILE }
	//};

	//Parser parser(scanner.scanTokens());
	//auto ast = parser.parse();
	//
	//Runtime runtime(1024);

	//runtime.registerNativeFunction("print", make_unique<FunctionDispatch<void, std::string>>([&](std::string i) {
	//	std::cout << i << "\n";
	//}));

	//runtime.eval(ast);
	//


	//for (int i = 0; i < 1; i++) {
	//	runtime.call("main");
	//}

	enum OpCodes : uint32 {
		// Operations
		MOV, JMP_IF,
		ILESS,

		// Descriptors
		SPTR, REG,
		CONST,

		PUSH_STACK, PUSH_PARAM, CALL, CALL_EXTERN, RETURN,

		// Arithmetic
		IADD,
		FMUL,

		LABEL = 19247152125125,
	};

	std::stringstream ss;
	ss << std::ifstream("scripts/compiled.nosm").rdbuf();
	std::string source = ss.str();
	Scanner scanner(source);
	scanner.keywords = {
		{ "MOV", MOV },
		{ "PUSH_STACK", PUSH_STACK },
		{ "UNDEFINED", UNDEFINED },
		{ "SPTR", SPTR },
		{ "CONST", CONST },
		{ "IADD", IADD },
		{ "LESS", LESS },
		{ "JMP_IF", JMP_IF },
		{ "PUSH_PARAM", PUSH_PARAM },
		{ "CALL", CALL },
		{ "CALL_EXT", CALL_EXTERN },
		{ "RET", RETURN }
	};

	auto tokens = scanner.scanTokens();
	

	enum Registers : uint32 {
		EAX, 

		// Stores the result of boolean operations
		BOOL,

		REGISTER_COUNT
	};


	float f = 8.25f;

	std::vector<uint32> bytecode = {
		// Fib
		LABEL, 0x01,
		// Allocate stack variables
		PUSH_STACK, 0x12, UNDEFINED,
		// Initialize stack variables
		MOV, SPTR, 0x04, CONST, 1,
		MOV, SPTR, 0x08, CONST, 1,
		MOV, SPTR, 0x0C, CONST, 2,

		// Start loop
		LABEL, 0x02,
		MOV, SPTR, 0x10, SPTR, 0x04,
		IADD, SPTR, 0x04, SPTR, 0x08,
		MOV, SPTR, 0x08, SPTR, 0x10,
		IADD, SPTR, 0x0C, CONST, 1,
		ILESS, SPTR, 0x0C, SPTR, 0x00,
		JMP_IF, REG, BOOL, 0x02,

		MOV, REG, EAX, SPTR, 0x04,
		RETURN,

		// main
		LABEL, 0x00,
		PUSH_STACK, 0x04, UNDEFINED,
		MOV, SPTR, 0x00, CONST, 60,

		PUSH_PARAM, 0x04, SPTR, 0x00,
		CALL, 1,

		PUSH_PARAM, 0x04, REG, EAX,
		CALL_EXTERN, 0
	};

	std::vector<uint32> labelTable(255);
	for (uint32 i = 0; i < bytecode.size(); i++) {
		if (bytecode.at(i) == LABEL) {
			labelTable[bytecode.at(i+1)] = i + 2;
		}
	}


	std::array<uint32, REGISTER_COUNT> registers;

	uint32 isp = labelTable[0];
	auto& advance = [&]() -> uint32& {
		return bytecode[isp++];
	};
	auto& get = [&]() -> uint32& {
		return bytecode[isp];
	};

	auto start = clock.now();



	Stack stack(2048);
	std::stack<uint32> callstack;

	while (isp < bytecode.size()) {
		switch (advance()) {
		case MOV:
		{
			uint8* dest = nullptr;
			
			switch (advance()) {
			case SPTR:
				dest = stack.getFromCurrent(advance());
				break;
			case REG:
				dest = (uint8*)(&registers[advance()]);
				break;
			default:
				throw std::runtime_error("Fatal execution error: Malformed code.");
			}

			switch (advance()) {
			case SPTR:
			{
				uint8* src = stack.getFromCurrent(advance());
				memcpy(dest, src, 4);
			}
			break;
			case CONST:
				memcpy(dest, &advance(), 4);
				break;
			default:
				throw std::runtime_error("Fatal execution error: Malformed code.");
			}

			break;
		}
		case IADD:
		{
			void* dest = nullptr;

			switch (advance()) {
			case SPTR:
				dest = stack.getFromCurrent(advance());
				break;
			default:
				throw std::runtime_error("Fatal execution error: Malformed code.");
			}

			switch (advance()) {
			case SPTR:
			{
				uint32* left = (uint32*)dest;
				*left = *left + *stack.getFromCurrent<uint32>(advance());
			}
			break;
			case CONST:
			{
				uint32* left = (uint32*)dest;
				*left = *left + advance();
			}
			break;
			default:
				throw std::runtime_error("Fatal execution error: Malformed code.");
			}
		}
		break;
		case ILESS:
		{
			void* dest = nullptr;

			switch (advance()) {
			case SPTR:
				dest = stack.getFromCurrent(advance());
				break;
			default:
				throw std::runtime_error("Fatal execution error: Malformed code.");
			}

			switch (advance()) {
			case SPTR:
			{
				uint32* right = stack.getFromCurrent<uint32>(advance());
				registers[BOOL] = *(uint32*)(dest) < *right;
				break;
			}
			case CONST:
			{
				uint32 right = advance();
				registers[BOOL] = *(uint32*)(dest) < right;
			}
			break;
			default:
				throw std::runtime_error("Fatal execution error: Malformed code.");
			}
		}
		break;
		case JMP_IF:
		{
			bool jmp = false;

			switch (advance()) {
			case REG:
				jmp = registers[advance()];
				break;
			default:
				throw std::runtime_error("Fatal execution error: Malformed code.");
			}

			// Early exit
			if (!jmp) {
				isp += 1;
				break;
			}

			auto labelIsp = labelTable[advance()];
			isp = labelIsp;
			break;
		}
		break;
		case LABEL:
		{
			isp += 1;
			break;
		}
		case PUSH_PARAM:
		{
			uint32 byteCount = advance();
			void* src = nullptr;

			switch (advance()) {
			case SPTR:
				src = stack.getFromCurrent(advance());
				break;
			case REG:
				src = &registers[advance()];
				break;
			default:
				throw std::runtime_error("Fatal execution error: Malformed code.");
			}

			stack.pushParams(byteCount, src);
		}
		break;
		case PUSH_STACK:
		{
			uint32 byteCount = advance();
			StackInitFlags initFlag = static_cast<StackInitFlags>(advance());

			stack.pushBytes(byteCount, initFlag);
		}
		break;
		case CALL_EXTERN:
		{
			auto functionID = advance();
			stack.pushStackFrame();

			auto& print = [](uint32 t) {
				std::cout << t << "\n";
			};
			print(*(uint32*)(stack.getFromCurrent(0)));

			stack.popStackFrame();
		}
		break;
		case CALL:
		{
			auto functionID = advance();
			stack.pushStackFrame();
			callstack.push(isp);

			isp = labelTable[functionID];
		}
		break;
		case RETURN:
			stack.popStackFrame();
			isp = callstack.top();
			callstack.pop();
		break;
		default:
			stack.dumpStack<uint32>();
			assert(false);
		}
	}
	auto end = clock.now();
	auto elapsed_seconds = end - start;
	std::cout << "Finished computation in " << elapsed_seconds.count() << " seconds." << "\n";


	std::cin.get();
}

// 0.000408998s

