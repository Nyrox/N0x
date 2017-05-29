#pragma once
#include <core/Definitions.h>
#include <cassert>
#include <string>

struct BoxedValue {
	union PureValue {
		int32 i32;
		float f32;
	};

	BoxedValue() : type(TYPE_VOID) {}
	BoxedValue(const BoxedValue&) = default;
	BoxedValue(BoxedValue&&) = default;
	BoxedValue& operator=(const BoxedValue&) = default;
	BoxedValue& operator=(BoxedValue&&) = default;

	explicit BoxedValue(int t_i) { setValue(t_i); }
	explicit BoxedValue(float t_f) { setValue(t_f); }

	BoxedValue promote_to_common(BoxedValue& o) {
		if (type == o.type) return *this;

		if (o.type == TYPE_VOID) assert(false);
	
		switch (o.type) {
		case TYPE_F32:
			if (type == TYPE_I32) return BoxedValue((float)value.i32);
			else return *this;
		case TYPE_I32:
			return *this;
		}
	}

	operator bool() const {
		switch (type) {
		case TYPE_I32:
			return value.i32;
		case TYPE_F32:
			return value.f32;
		}
	}

	operator std::string() const {
		switch (type) {
		case TYPE_I32:
			return std::to_string(value.i32);
		case TYPE_F32:
			return std::to_string(value.f32);
		}

		assert(false);
	}

	BoxedValue operator+(BoxedValue& right) {
		const auto left = promote_to_common(right);
		right = right.promote_to_common(*this);

		switch (left.type) {
		case TYPE_I32:
			return BoxedValue(left.value.i32 + right.value.i32);
		case TYPE_F32:
			return BoxedValue(left.value.f32 + right.value.f32);
		}
	}

	BoxedValue operator-(BoxedValue& right) {
		const auto left = promote_to_common(right);
		right = right.promote_to_common(*this);

		switch (left.type) {
		case TYPE_I32:
			return BoxedValue(left.value.i32 - right.value.i32);
		case TYPE_F32:
			return BoxedValue(left.value.f32 - right.value.f32);
		}
	}

	BoxedValue operator*(BoxedValue& right) {
		const auto left = promote_to_common(right);
		right = right.promote_to_common(*this);

		switch (left.type) {
		case TYPE_I32:
			return BoxedValue(left.value.i32 * right.value.i32);
		case TYPE_F32:
			return BoxedValue(left.value.f32 * right.value.f32);
		}
	}

	BoxedValue operator/(BoxedValue& right) {
		const auto left = promote_to_common(right);
		right = right.promote_to_common(*this);

		switch (left.type) {
		case TYPE_I32:
			return BoxedValue(left.value.i32 / right.value.i32);
		case TYPE_F32:
			return BoxedValue(left.value.f32 / right.value.f32);
		}
	}

	BoxedValue operator<(BoxedValue& right) {
		const auto left = promote_to_common(right);
		right = right.promote_to_common(*this);

		switch (left.type) {
		case TYPE_I32:
			return BoxedValue(left.value.i32 < right.value.i32);
		case TYPE_F32:
			return BoxedValue(left.value.f32 < right.value.f32);
		}
	}

	BoxedValue operator>(BoxedValue& right) {
		const auto left = promote_to_common(right);
		right = right.promote_to_common(*this);

		switch (left.type) {
		case TYPE_I32:
			return BoxedValue(left.value.i32 > right.value.i32);
		case TYPE_F32:
			return BoxedValue(left.value.f32 > right.value.f32);
		}
	}

	BoxedValue operator<=(BoxedValue& right) {
		const auto left = promote_to_common(right);
		right = right.promote_to_common(*this);

		switch (left.type) {
		case TYPE_I32:
			return BoxedValue(left.value.i32 <= right.value.i32);
		case TYPE_F32:
			return BoxedValue(left.value.f32 <= right.value.f32);
		}
	}

	BoxedValue operator>=(BoxedValue& right) {
		const auto left = promote_to_common(right);
		right = right.promote_to_common(*this);

		switch (left.type) {
		case TYPE_I32:
			return BoxedValue(left.value.i32 >= right.value.i32);
		case TYPE_F32:
			return BoxedValue(left.value.f32 >= right.value.f32);
		}
	}


	PureValue getPureValue() { return value; }

	void setValue(int val) { value.i32 = val; type = TYPE_I32; }
	void setValue(float val) { value.f32 = val; type = TYPE_F32; }

	
private:	
	PureValue value;
	enum Type {
		TYPE_VOID,
		TYPE_I32,
		TYPE_F32,
	} type;

};