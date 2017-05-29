#pragma once
#include <core/Definitions.h>
#include <functional>
#include <runtime/BoxedValue.h>
#include <vector>

class FunctionDispatch_Base {
public:
	virtual void invoke(std::vector<BoxedValue> value) = 0;
	virtual uint32 getParamCount() const = 0;
};

template<class T, class... P>
class FunctionDispatch : public FunctionDispatch_Base {
public:
	FunctionDispatch(std::function<T(P...)> t_ptr) : ptr(t_ptr) { }

	virtual uint32 getParamCount() const override {
		return sizeof...(P);
	}

	virtual void invoke(std::vector<BoxedValue> params) override {
		invoke<sizeof...(P)>(params);
	}

private:
	template<int pc>
	void invoke(std::vector<BoxedValue> params) {
		ptr();
	}

	template<>
	void invoke<1>(std::vector<BoxedValue> params) {
		using t1 = NthTypeOf<0, P...>;
		ptr((t1)params[0]);
	}

	template<>
	void invoke<2>(std::vector<BoxedValue> params) {
		using t1 = NthTypeOf<0, P...>;
		using t2 = NthTypeOf<1, P...>;

		ptr((t1)params[0], (t2)params[1]);
	}

	template<>
	void invoke<3>(std::vector<BoxedValue> params) {
		using t1 = NthTypeOf<0, P...>;
		using t2 = NthTypeOf<1, P...>;
		using t3 = NthTypeOf<2, P...>;

		ptr((t1)params[0], (t2)params[1], (t3)params[2]);
	}

	std::function<T(P...)> ptr;
};
