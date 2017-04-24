#pragma once
#include <core/Definitions.h>
#include <functional>

class FunctionDispatch_Base {
public:
	virtual void invoke(void*) = 0;
};

template<class T, class... P>
class FunctionDispatch : public FunctionDispatch_Base {
public:
	FunctionDispatch(std::function<T(P...)> t_ptr) : ptr(t_ptr) { }



	virtual void invoke(void* mem) override {
		invoke<sizeof...(P)>(mem);
	}

private:
	template<int pc>
	void invoke(void* mem) {
		ptr();
	}

	template<>
	void invoke<1>(void* mem) {
		using t1 = NthTypeOf<0, P...>;
		ptr(*(t1*)(mem));
	}

	template<>
	void invoke<2>(void* mem) {
		using t1 = NthTypeOf<0, P...>;
		using t2 = NthTypeOf<1, P...>;

		ptr(*(t1*)(mem), *((t2*)(mem)+sizeof(t1)));
	}

	template<>
	void invoke<3>(void* mem) {
		using t1 = NthTypeOf<0, P...>;
		using t2 = NthTypeOf<1, P...>;
		using t3 = NthTypeOf<2, P...>;

		ptr(*(t1*)(mem), *((t2*)(mem)+sizeof(t1)), *((t3*)(mem)+sizeof(t1) + sizeof(t2)));
	}

	std::function<T(P...)> ptr;
};
