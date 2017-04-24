#include "Runtime.h"
#include <expressions/expr.h>

void Block::execute(Runtime& runtime) {
	for (auto& it : expressions) {
		it->eval(runtime, *this);
	}
}