#pragma once
#include <memory>

template<class T>
using uptr = std::unique_ptr<T>;
using std::make_unique;

using int32 = int32_t;
using int16 = int16_t;
using int8 = int8_t;

using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8 = uint8_t;

/* Constructs a tuple from a variadic template parameters pack and returns the T for the index N in the pack */
template<int N, typename... Ts> using NthTypeOf =
typename std::tuple_element<N, std::tuple<Ts...>>::type;
