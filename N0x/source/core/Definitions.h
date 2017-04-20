#pragma once
#include <memory>

template<class T>
using uptr = std::unique_ptr<T>;
using std::make_unique;
