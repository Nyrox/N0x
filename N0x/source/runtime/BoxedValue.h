#pragma once
#include <core/Definitions.h>
#include <cassert>

struct BoxedValue {
	BoxedValue() : type(TYPE_VOID) {}
	explicit BoxedValue(int t_i) { setValue(t_i); }
	explicit BoxedValue(float t_f) { setValue(t_f); }

	/*BoxedValue promote_to_common(BoxedValue o) {
		if (type == o.type)
			return *this;

		if (type == TYPE_VOID)
			assert(false);

		switch (o.type) {
		case TYPE_VOID: assert(false);

		case TYPE_F32: switch (type) {
		case TYPE_U32: return BoxedValue{ (float)d.u32 };
		case TYPE_U64: return BoxedValue{ (float)d.u64 };
		default: assert(false);
		}

		case TYPE_U32: switch (type) {
		case TYPE_F32: return *this;
		case TYPE_U64: return *this;
		default: assert(false);
		}

		case TYPE_U64: switch (type) {
		case TYPE_U32: return BoxedValue{ (uint64_t)d.u32 };
		case TYPE_F32: return *this;
		default: assert(false);
		}
		}
	}*/

	BoxedValue operator+(BoxedValue& other) {
		switch (type) {
		case INT:
			return BoxedValue(value.i + other.value.i);
		case FLOAT:
			return BoxedValue(value.f + other.value.f);
		}

	}

	void setValue(int val) { value.i = val; type = INT; }
	void setValue(float val) { value.f = val; type = FLOAT; }

	union PureValue {
		int32 i32;
		float f32;
	};
private:	
	PureValue value;
	enum Type {
		TYPE_VOID,
		TYPE_I32,
		TYPE_F32,
	} type;

};