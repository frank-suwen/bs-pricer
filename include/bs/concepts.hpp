#pragma once
#include <type_traits>

// Accepts all arithmetic types EXCEPT bool (bool wreaks havoc in numeric kernels).
namespace bs {

template <class T>
concept Arithmetic = 
    std::is_arithmetic_v<T> &&
    !std::is_same_v<std::remove_cv_t<T>, bool>;

} // namespace bs